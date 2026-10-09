//======================================================================
// fspi_slave.v — 四线 QUAD SPI 从机物理层（【人员 C · 控制与 FPGA】）
//
// 职责：把 4 线 QUAD SPI 的比特流组装成字节（rx_data / rx_data_valid），
//       并从首字节解码出读/写命令（cmd）。发送侧按下降沿把读回数据逐半字节
//       送回收发器，并产生 tx_data_valid 翻转脉冲供上层 spi_ram_crtl 做时序。
//
// 命令编码（首字节，由 {高半字节, 低半字节} 拼接）：
//   0x80 → cmd=2'b10（读）
//   0x00 → cmd=2'b01（写）
// 注意：旧版注释曾把「2'b10:写 / 2'b01:读」标反，实际语义以 spi_ram_crtl.v
// 为准（2'b01=写 / 2'b10=读），本文件已修正。
//
// 端口与 fspi_slave_top.v / image_pcie_capture.v 保持一致，未改动。
//======================================================================
module fspi_slave
(
    input   wire            spi_clk         ,
    input   wire            spi_cs          ,
    inout   wire    [3:0]   spi_data        ,
    input   wire            rst_n           ,
    input   wire    [3:0]   select_rearview ,   // 顶层传入（历史沿用，实为 cnn_level）
    // user
    output  wire    [1:0]   cmd             ,   // 2'b01:写  2'b10:读
    output  wire    [7:0]   rx_data         ,
    output  wire            rx_data_valid   ,

    input   wire    [7:0]   tx_data         ,
    output  wire            tx_data_valid
);

//----------------- reg 定义 -----------------
reg         rx_nibble_sel ;   // 接收半字节选择：0=高半字节，1=低半字节
reg [7:0]   rx_byte       ;
reg         rx_valid_r    ;
reg [1:0]   cmd_r         ;
reg         cmd_locked    ;   // 首字节命令解码后锁定，避免与数据字节误判

//----------------- assign -----------------
// 片选拉低且为读操作时，把 select_rearview 驱动到总线（读回值）
assign  spi_data       = (!spi_cs && (cmd == 2'b10)) ? {select_rearview} : 4'bzzzz;
assign  tx_data_valid  = tx_valid_r;
assign  cmd            = cmd_r;
assign  rx_data        = rx_byte;
assign  rx_data_valid  = rx_valid_r;

//----------------- 接收：上升沿采样 4 bit，两拍拼一个字节 -----------------
always @(posedge spi_clk or negedge rst_n) begin
    if ((rst_n == 1'b0) || spi_cs) begin
        rx_nibble_sel   <= 1'd0 ;
        rx_byte         <= 8'hff;
    end
    else if (!spi_cs) begin
        if (!rx_nibble_sel) begin
            rx_byte[7:4]    <= spi_data;
            rx_nibble_sel   <= ~rx_nibble_sel;
        end
        else begin
            rx_byte[3:0]    <= spi_data;
            rx_nibble_sel   <= ~rx_nibble_sel;
        end
    end
end

//----------------- 命令字节解码：首字节锁定，cs 到来时清零 ----------------
always @(posedge spi_clk or negedge rst_n or posedge spi_cs) begin
    if ((rst_n == 1'b0) || spi_cs) begin
        cmd_locked  <= 1'd0;
        cmd_r       <= 2'd0;
    end
    else if (!spi_cs && rx_nibble_sel) begin
        if (!cmd_locked) begin
            if ({rx_byte[7:4], spi_data} == 8'h80)
                cmd_r   <= 2'b10;      // 读
            else if ({rx_byte[7:4], spi_data} == 8'h00)
                cmd_r   <= 2'b01;      // 写
            cmd_locked  <= 1'd1;
        end
    end
end

//----------------- 接收完成脉冲：写命令时每字节拉高一拍 -----------------
always @(posedge spi_clk or negedge rst_n) begin
    if ((rst_n == 1'b0) || spi_cs)
        rx_valid_r  <= 1'd0;
    else if (!spi_cs) begin
        if (rx_nibble_sel && cmd_r == 2'b01)
            rx_valid_r  <= 1'd1;
        else
            rx_valid_r  <= 1'd0;
    end
end

//----------------- 发送：下降沿逐半字节翻转 tx_data_valid（时序脉冲） -----
reg [3:0]   tx_nibble  ;
reg         tx_valid_r ;

always @(negedge spi_clk or negedge rst_n) begin
    if ((rst_n == 1'b0) || spi_cs) begin
        tx_nibble   <= 4'hf;
        tx_valid_r  <= 1'd0;
    end
    else if (!spi_cs) begin
        if (cmd_r == 2'b10) begin
            if (!tx_valid_r) begin
                tx_nibble   <= tx_data[7:4];
                tx_valid_r  <= ~tx_valid_r;
            end
            else begin
                tx_nibble   <= tx_data[3:0];
                tx_valid_r  <= ~tx_valid_r;
            end
        end
    end
end

endmodule
