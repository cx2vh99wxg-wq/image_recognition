//======================================================================
// spi_ram_crtl.v — FSPI 字节流 → 寄存器读写解析（【人员 C · 控制与 FPGA】）
//
// 把 fspi_slave 传来的字节流解析成 5 个 8-bit 寄存器 mem[0..4] 的读写：
//   - 写帧（cmd=2'b01）：[命令/地址高=0x00][地址低][数据]，共 3 字节；
//   - 读帧（cmd=2'b10，历史保留）：[地址高][地址低] 后由上层回读。
//   地址只取低字节低 3 位（0..4）。
//
// 读回语义（历史保留，主机当前不触发读命令）：
//   ram_addr < 6  → rd_data = {4'd0, select_rearview}
//   否则          → rd_data = 0xFF
//
// 相对旧版的改进：删除了与状态机线性推进完全冗余的 byte_cnt 计数器
// （每个状态只由前一状态进入，byte_cnt 的条件恒为真），逻辑等价。
//======================================================================
module spi_ram_crtl
(
    input   wire            wr_clk          ,
    input   wire            rst_n           ,
    input   wire            spi_cs          ,

    input   wire    [1:0]   cmd             ,
    input   wire            tx_done         ,
    input   wire    [7:0]   wr_data         ,
    input   wire            rx_valid        ,
    input   wire    [3:0]   select_rearview ,   // 作为寄存器读回值（历史沿用）
    output  reg     [7:0]   rd_data         ,
    output  wire    [7:0]   mem0_out        ,   // 第 0 个地址寄存器
    output  wire    [7:0]   mem1_out        ,   // 第 1 个地址寄存器
    output  wire    [7:0]   mem2_out        ,   // 第 2 个地址寄存器
    output  wire    [7:0]   mem3_out        ,   // 第 3 个地址寄存器
    output  wire    [7:0]   mem4_out            // 第 4 个地址寄存器
);

//-------------------- 状态机定义 --------------------
localparam IDLE        = 3'd0;
localparam RCV_ADDR_H  = 3'd1;  // 写：接收地址高字节（实际为命令 0x00）
localparam RCV_ADDR_L  = 3'd2;  // 写：接收地址低字节
localparam WRITE_DATA  = 3'd3;  // 写：写数据
localparam READ_ADDR_H = 3'd4;  // 读：接收地址高字节（历史保留）
localparam READ_ADDR_L = 3'd5;  // 读：接收地址低字节
localparam READ_DATA   = 3'd6;  // 读：回读数据

//-------------------- wire（边沿检测：pose=上升沿，nege=下降沿） --------
wire    rx_pose;   // 接收上升沿
wire    rx_nege;   // 接收下降沿
wire    tx_pose;   // 发送上升沿
wire    tx_nege;   // 发送下降沿

//-------------------- reg ---------------------------
reg [2:0]   state           /* synthesis PAP_MARK_DEBUG="true" */;
reg [1:0]   cmd_r           ;
reg         cs_r            ;
reg         rx_valid_d0     ;
reg         rx_valid_d1     ;
reg         tx_done_d0      ;
reg         tx_done_d1      ;
reg [7:0]   wr_data_r       ;
reg [7:0]   addr_high       /* synthesis PAP_MARK_DEBUG="true" */;  // 写地址高字节
reg         ram_wr_en       /* synthesis PAP_MARK_DEBUG="true" */;  // 1 写 0 读
reg [15:0]  ram_addr        /* synthesis PAP_MARK_DEBUG="true" */;  // 16 位地址
reg [7:0]   read_addr_high  /* synthesis PAP_MARK_DEBUG="true" */;  // 读地址高字节

//------------------- assign ------------------------
assign  rx_pose = rx_valid_d0 && ~rx_valid_d1 ;
assign  rx_nege = ~rx_valid_d0 && rx_valid_d1 ;
assign  tx_pose = tx_done_d0 && ~tx_done_d1   ;
assign  tx_nege = ~tx_done_d0 && tx_done_d1   ;

//------------------- 打拍寄存 ---------------------
always @(negedge wr_clk or negedge rst_n) begin
    if (!rst_n) begin
        rx_valid_d0 <= 1'd0;
        rx_valid_d1 <= 1'd0;
        tx_done_d0  <= 1'd0;
        tx_done_d1  <= 1'd0;
        cs_r        <= 1'd1;
        cmd_r       <= 2'd0;
    end
    else begin
        rx_valid_d0 <= rx_valid    ;
        rx_valid_d1 <= rx_valid_d0 ;
        tx_done_d0  <= tx_done     ;
        tx_done_d1  <= tx_done_d0  ;
        cs_r        <= spi_cs      ;
        cmd_r       <= cmd         ;
    end
end

//------------------- 数据寄存 ---------------------
always @(negedge wr_clk or negedge rst_n) begin
    if (!rst_n)
        wr_data_r <= 8'd0;
    else if (cs_r)
        wr_data_r <= 8'hff;
    else
        wr_data_r <= wr_data;
end

//==================== 状态机（写支持指定地址） ====================
always @(negedge wr_clk or negedge rst_n) begin
    if (!rst_n) begin
        state          <= IDLE;
        ram_addr       <= 16'd0;
        ram_wr_en      <= 1'd0;
        addr_high      <= 8'd0;
        read_addr_high <= 8'd0;
    end
    else if (cs_r) begin
        // 片选无效时复位到 IDLE，地址保持
        state     <= IDLE;
        ram_wr_en <= 1'd0;
    end
    else begin
        case (state)
            IDLE: begin
                ram_wr_en <= 1'd0;
                if (rx_pose && cmd_r == 2'b01) begin
                    // 写命令：第 1 字节（命令/地址高）
                    addr_high <= wr_data_r;
                    state     <= RCV_ADDR_H;
                end
                else if (rx_pose && cmd_r == 2'b10) begin
                    // 读命令：第 1 字节（地址高）
                    read_addr_high <= wr_data_r;
                    state          <= READ_ADDR_H;
                end
            end

            RCV_ADDR_H: begin
                // 写：第 2 字节（地址低）
                if (rx_pose) begin
                    ram_addr <= {addr_high, wr_data_r};
                    state    <= RCV_ADDR_L;
                end
            end

            RCV_ADDR_L: begin
                // 写：第 3 字节（数据）
                if (rx_pose) begin
                    ram_wr_en <= 1'd1;
                    state     <= WRITE_DATA;
                end
            end

            WRITE_DATA: begin
                // 写完成后立即关写使能并回 IDLE
                ram_wr_en <= 1'd0;
                state     <= IDLE;
            end

            READ_ADDR_H: begin
                // 读：第 2 字节（地址低）
                if (rx_pose) begin
                    ram_addr <= {read_addr_high, wr_data_r};
                    state    <= READ_ADDR_L;
                end
            end

            READ_ADDR_L: begin
                // 读：地址设置完成，进入读数据状态
                state <= READ_DATA;
            end

            READ_DATA: begin
                // 读：tx_nege 时回 IDLE
                if (tx_nege)
                    state <= IDLE;
            end

            default: state <= IDLE;
        endcase
    end
end

//==================== 内存数组（5 个 8-bit 寄存器） ====================
reg [7:0] mem [4:0];

always @(negedge wr_clk or negedge rst_n) begin
    if (!rst_n) begin
        mem[0]  <= 8'd0;
        mem[1]  <= 8'd0;
        mem[2]  <= 8'd0;
        mem[3]  <= 8'd0;
        mem[4]  <= 8'd0;
        rd_data <= 8'd0;
    end
    else begin
        // 写：地址范围检查（仅 0..4）
        if (ram_wr_en && ram_addr[15:0] < 16'd5)
            mem[ram_addr[2:0]] <= wr_data_r;

        // 读：地址 < 6 输出 select_rearview，否则 0xFF
        if (ram_addr[15:0] < 16'd6)
            rd_data <= {4'd0, select_rearview};
        else
            rd_data <= 8'hFF;
    end
end

// 引出第 0..4 个地址寄存器
assign mem0_out = mem[0];
assign mem1_out = mem[1];
assign mem2_out = mem[2];
assign mem3_out = mem[3];
assign mem4_out = mem[4];

endmodule
