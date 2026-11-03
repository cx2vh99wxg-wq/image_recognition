// 2026-11-03: cycle-preserving structure refactor.
module spi_ram_crtl
(
	input	wire				wr_clk		,
	input	wire				rst_n		,
	input	wire				spi_cs		,
	
	input	wire	[1:0]		cmd			,
	input	wire				tx_done		,
	input	wire	[7:0]		wr_data		,
	input	wire				rx_valid	,
	input	wire	[3:0]			select_rearview	,	// 寄存器访问：GPIO接收到的一位值，作为寄存器4的读取值
	output	reg		[7:0]		rd_data		,
	output	wire	[7:0]		mem0_out	,	// 寄存器访问：引出第0个地址的寄存器
	output	wire	[7:0]		mem1_out	,	// 寄存器访问：引出第1个地址的寄存器
	output	wire	[7:0]		mem2_out	,	// 寄存器访问：引出第2个地址的寄存器
	output	wire	[7:0]		mem3_out	,	// 寄存器访问：引出第3个地址的寄存器
	output	wire	[7:0]		mem4_out,
    output  wire    [7:0]        mem5_out,   // 寄存器访问：lane/traffic permissions + heartbeat
    output  wire    [7:0]        mem6_out    // 寄存器访问：A5 = declare known physical low-gear reference
);

// 状态机定义。
localparam IDLE         = 3'd0;
localparam RCV_ADDR_H   = 3'd1;  // 读入访问地址的高八位。
localparam RCV_ADDR_L   = 3'd2;  // 读入访问地址的低八位。
localparam WRITE_DATA   = 3'd3;  // 状态机处于数据写入阶段。
localparam READ_ADDR_H  = 3'd4;  // 寄存器访问：读操作-接收地址高字节
localparam READ_ADDR_L  = 3'd5;  // 寄存器访问：读操作-接收地址低字节
localparam READ_DATA    = 3'd6;  // 状态机处于数据读取阶段。

// wire。
wire		rx_pose	;	// 接收阶段检测到的上升沿。
wire		rx_nege	;	// 接收阶段检测到的下降沿。
wire		tx_pose	;	// 发送阶段检测到的上升沿。
wire		tx_nege	;	// 发送阶段检测到的下降沿。

// reg。
reg	[2:0]	state			/* synthesis PAP_MARK_DEBUG="true" */;
reg	[1:0]	cmd_reg			;
reg			spi_cs_reg		;	
reg			rx_valid_d0		;
reg			rx_valid_d1		;
reg			tx_done_d0		;
reg			tx_done_d1		;
reg	[7:0]	wr_data_reg		;
reg	[7:0]	addr_high		/* synthesis PAP_MARK_DEBUG="true" */;  // 锁存当前地址的高八位。
reg			ram_wr_en		/* synthesis PAP_MARK_DEBUG="true" */;	// 编码 1 为写，编码 0 为读。
reg	[15:0]	ram_addr		/* synthesis PAP_MARK_DEBUG="true" */;  // 寄存器访问：扩展到16位地址
reg	[1:0]	byte_cnt		/* synthesis PAP_MARK_DEBUG="true" */;  // 寄存器访问：字节计数器,防止重复写入
reg [7:0]   read_addr_high  /* synthesis PAP_MARK_DEBUG="true" */;  // 寄存器访问：读操作保存地址高字节

// assign。
assign	rx_pose = rx_valid_d0 && ~rx_valid_d1	;
assign	rx_nege = ~rx_valid_d0 && rx_valid_d1	;
assign	tx_pose	= tx_done_d0 && ~tx_done_d1		;
assign	tx_nege	= ~tx_done_d0 && tx_done_d1		;

// always。
// 用时钟寄存对齐信号。
always@(negedge wr_clk or negedge rst_n)	begin
	if(!rst_n) begin
		rx_valid_d0	<=	1'd0;
		rx_valid_d1	<=	1'd0;
		tx_done_d0	<=	1'd0;
		tx_done_d1	<=	1'd0;
		spi_cs_reg	<=	1'd1;
		cmd_reg		<=	2'd0;
	end
	else begin
		rx_valid_d0	<=	rx_valid	;
		rx_valid_d1	<=	rx_valid_d0 ;
		tx_done_d0	<=	tx_done     ;
		tx_done_d1	<=	tx_done_d0  ;
		spi_cs_reg	<=	spi_cs		;
		cmd_reg		<=	cmd			;
	end
end

// 锁存当前输入数据。
always@(negedge wr_clk or negedge rst_n)	begin
	if(!rst_n)
		wr_data_reg	<=	8'd0;
	else if(spi_cs_reg)
		wr_data_reg	<=	8'hff;
	else
		wr_data_reg	<=	wr_data;
end

// 状态机 - 写操作支持指定地址。
always@(negedge wr_clk or negedge rst_n) begin
	if(!rst_n) begin
		state		<=	IDLE;
		ram_addr	<=	16'd0;
		ram_wr_en	<=	1'd0;
		addr_high	<=	8'd0;
		read_addr_high <= 8'd0;
		byte_cnt	<=	2'd0;
	end
	else if(spi_cs_reg) begin
		// 寄存器访问：片选无效时复位到IDLE状态，但保持地址不变
		state		<=	IDLE;
		ram_wr_en	<=	1'd0;
		byte_cnt	<=	2'd0;
	end
	else begin
		case(state)
            IDLE: begin
                ram_wr_en <= 1'b0;
                byte_cnt <= 2'd0;
                // The first valid byte is the high address for either command.
                if (rx_pose) begin
                    case (cmd_reg)
                        2'b01: begin
                            addr_high <= wr_data_reg;
                            state <= RCV_ADDR_H;
                            byte_cnt <= 2'd1;
                        end
                        2'b10: begin
                            read_addr_high <= wr_data_reg;
                            state <= READ_ADDR_H;
                            byte_cnt <= 2'd1;
                        end
                        default: ;
                    endcase
                end
            end

            RCV_ADDR_H, READ_ADDR_H: begin
                // Keep the separate high-byte registers and debug state codes.
                if (rx_pose && byte_cnt == 2'd1) begin
                    ram_addr <= {(state == RCV_ADDR_H ? addr_high : read_addr_high),
                                 wr_data_reg};
                    byte_cnt <= 2'd2;
                    state <= (state == RCV_ADDR_H) ? RCV_ADDR_L : READ_ADDR_L;
                end
            end

            RCV_ADDR_L: begin
                if (rx_pose && byte_cnt == 2'd2) begin
                    // The bank sees this enable on the following falling edge.
                    ram_wr_en <= 1'b1;
                    byte_cnt <= 2'd3;
                    state <= WRITE_DATA;
                end
            end
            WRITE_DATA: begin
                ram_wr_en <= 1'b0;
                state <= IDLE;
            end

            READ_ADDR_L: begin
                if (byte_cnt == 2'd2)
                    state <= READ_DATA;
            end
            READ_DATA: begin
                if (tx_nege)
                    state <= IDLE;
            end

			default: state <= IDLE;
		endcase
	end
end

// 内存数组定义。
reg [7:0] mem [5:0];
// 寄存器访问：Keep the original motion/permission bank and its decode unchanged.
// 寄存器访问：The added calibration register uses a constant address rather than
// 寄存器访问：expanding the 400 MHz indexed-write decoder for every existing register.
reg [7:0] gear_reference;
always @(negedge wr_clk or negedge rst_n) begin
    if (!rst_n) gear_reference <= 8'd0;
    else if (ram_wr_en && ram_addr == 16'd6) gear_reference <= wr_data_reg;
end

// 寄存器访问：内存读写逻辑 - 统一使用negedge wr_clk与状态机保持一致
always@(negedge wr_clk or negedge rst_n) begin
	if(!rst_n) begin
		// 寄存器访问：Active-low remote contacts must be released at FPGA reset.
		mem[0] <= 8'd1;
		mem[1] <= 8'd1;
		mem[2] <= 8'd1;
		mem[3] <= 8'd1;
		mem[4] <= 8'd1;
		mem[5] <= 8'd0; // 寄存器访问：no valid lane metadata: legacy manual behavior
		rd_data <= 8'd0;
	end
	else begin
		// 寄存器访问：写操作 - 修正地址范围检查
		if(ram_wr_en && ram_addr[15:0] < 16'd6) begin
			mem[ram_addr[2:0]] <= wr_data_reg;  // 使用[2:0]保证只访问0-4
		end
		
		// 寄存器访问：读操作 - 修正地址范围检查
		if(ram_addr[15:0] < 16'd7) begin
			rd_data <= {4'd0, select_rearview};  // 寄存器访问：片选无效时输出select_rearview值
		end
		else begin
			rd_data <= 8'hFF;  // 寄存器访问：超出范围返回0xFF
		end
	end
end
// 寄存器访问：将第0到4个地址的寄存器引出
assign mem0_out = mem[0];
assign mem1_out = mem[1];
assign mem2_out = mem[2];
assign mem3_out = mem[3];
assign mem4_out = mem[4];
assign mem5_out = mem[5];
assign mem6_out = gear_reference;
endmodule

