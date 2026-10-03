module spi_ram_crtl
(
	input	wire				wr_clk		,
	input	wire				rst_n		,
	input	wire				spi_cs		,
	
	input	wire	[1:0]		cmd			,
	input	wire				tx_done		,
	input	wire	[7:0]		wr_data		,
	input	wire				rx_valid	,
	input	wire	[3:0]			select_rearview	,	// GPIO接收到的一位值，作为寄存器4的读取值
	output	reg		[7:0]		rd_data		,
	output	wire	[7:0]		mem0_out	,	// 引出第0个地址的寄存器
	output	wire	[7:0]		mem1_out	,	// 引出第1个地址的寄存器
	output	wire	[7:0]		mem2_out	,	// 引出第2个地址的寄存器
	output	wire	[7:0]		mem3_out	,	// 引出第3个地址的寄存器
	output	wire	[7:0]		mem4_out		// 引出第4个地址的寄存器
);

//--------------------状态机定义--------------------
localparam IDLE         = 3'd0;
localparam RCV_ADDR_H   = 3'd1;  // 接收地址高字节
localparam RCV_ADDR_L   = 3'd2;  // 接收地址低字节
localparam WRITE_DATA   = 3'd3;  // 写数据状态
localparam READ_ADDR_H  = 3'd4;  // 读操作-接收地址高字节
localparam READ_ADDR_L  = 3'd5;  // 读操作-接收地址低字节
localparam READ_DATA    = 3'd6;  // 读数据状态

//--------------------wire-------------------------
wire		rx_pose	;	//接收的上升沿
wire		rx_nege	;	//接收的下降沿
wire		tx_pose	;	//发送的上升沿
wire		tx_nege	;	//发送的下降沿

//--------------------reg--------------------------
reg	[2:0]	state			/* synthesis PAP_MARK_DEBUG="true" */;
reg	[1:0]	cmd_reg			;
reg			spi_cs_reg		;	
reg			rx_valid_d0		;
reg			rx_valid_d1		;
reg			tx_done_d0		;
reg			tx_done_d1		;
reg	[7:0]	wr_data_reg		;
reg	[7:0]	addr_high		/* synthesis PAP_MARK_DEBUG="true" */;  // 保存地址高字节
reg			ram_wr_en		/* synthesis PAP_MARK_DEBUG="true" */;	//1写 0读
reg	[15:0]	ram_addr		/* synthesis PAP_MARK_DEBUG="true" */;  // 扩展到16位地址
reg	[1:0]	byte_cnt		/* synthesis PAP_MARK_DEBUG="true" */;  // 字节计数器,防止重复写入
reg [7:0]   read_addr_high  /* synthesis PAP_MARK_DEBUG="true" */;  // 读操作保存地址高字节

//-------------------assign------------------------
assign	rx_pose = rx_valid_d0 && ~rx_valid_d1	;
assign	rx_nege = ~rx_valid_d0 && rx_valid_d1	;
assign	tx_pose	= tx_done_d0 && ~tx_done_d1		;
assign	tx_nege	= ~tx_done_d0 && tx_done_d1		;

//-------------------always-------------------------
//打拍寄存
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

//数据寄存
always@(negedge wr_clk or negedge rst_n)	begin
	if(!rst_n)
		wr_data_reg	<=	8'd0;
	else if(spi_cs_reg)
		wr_data_reg	<=	8'hff;
	else
		wr_data_reg	<=	wr_data;
end

//====================状态机 - 写操作支持指定地址====================
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
		// 片选无效时复位到IDLE状态，但保持地址不变
		state		<=	IDLE;
		ram_wr_en	<=	1'd0;
		byte_cnt	<=	2'd0;
	end
	else begin
		case(state)
			IDLE: begin
				ram_wr_en <= 1'd0;
				byte_cnt  <= 2'd0;
				if(rx_pose && cmd_reg == 2'b01) begin
					// 写命令: 接收第1个字节(地址高字节)
					addr_high	<=	wr_data_reg;
					byte_cnt	<=	2'd1;
					state		<=	RCV_ADDR_H;
				end
				else if(rx_pose && cmd_reg == 2'b10) begin
					// 读命令: 接收第1个字节(地址高字节)
					read_addr_high <= wr_data_reg;
					byte_cnt	<=	2'd1;
					state		<=	READ_ADDR_H;
				end
			end
			
			RCV_ADDR_H: begin
				// 写操作-等待接收第2个字节(地址低字节)
				if(rx_pose && byte_cnt == 2'd1) begin
					ram_addr	<=	{addr_high, wr_data_reg};  // 组合成16位地址
					byte_cnt	<=	2'd2;
					state		<=	RCV_ADDR_L;
				end
			end
			
			RCV_ADDR_L: begin
				// 写操作-等待接收第3个字节(数据)并写入
				if(rx_pose && byte_cnt == 2'd2) begin
					ram_wr_en	<=	1'd1;
					byte_cnt	<=	2'd3;
					state		<=	WRITE_DATA;
				end
			end
			
			WRITE_DATA: begin
				// 写入完成后立即关闭写使能并回到IDLE
				ram_wr_en	<=	1'd0;
				state		<=	IDLE;
				// 单次写入模式,写完立即结束
			end
			
			READ_ADDR_H: begin
				// 读操作-等待接收第2个字节(地址低字节)
				if(rx_pose && byte_cnt == 2'd1) begin
					ram_addr	<=	{read_addr_high, wr_data_reg};  // 组合成16位地址
					byte_cnt	<=	2'd2;
					state		<=	READ_ADDR_L;
				end
			end
			
			READ_ADDR_L: begin
				// 读操作-地址设置完成，等待下一个时钟周期进入读数据状态
				if(byte_cnt == 2'd2) begin
					state		<=	READ_DATA;
				end
			end
			
			READ_DATA: begin
				// 读操作:从指定地址读取数据，保持地址不变
				// 读取完成后回到IDLE状态
				if(tx_nege) begin
					state <= IDLE;
				end
			end
			
			default: state <= IDLE;
		endcase
	end
end

//====================内存数组定义====================
reg [7:0] mem [4:0];  // 5个8位的内存单元(0-4)

// 内存读写逻辑 - 统一使用negedge wr_clk与状态机保持一致
always@(negedge wr_clk or negedge rst_n) begin
	if(!rst_n) begin
		mem[0] <= 8'd0;
		mem[1] <= 8'd0;
		mem[2] <= 8'd0;
		mem[3] <= 8'd0;
		mem[4] <= 8'd0;
		rd_data <= 8'd0;
	end
	else begin
		// 写操作 - 修正地址范围检查
		if(ram_wr_en && ram_addr[15:0] < 16'd5) begin
			mem[ram_addr[2:0]] <= wr_data_reg;  // 使用[2:0]确保只访问0-4
		end
		
		// 读操作 - 修正地址范围检查
		if(ram_addr[15:0] < 16'd6) begin
			rd_data <= {4'd0, select_rearview};  // 片选无效时输出select_rearview值
		end
		else begin
			rd_data <= 8'hFF;  // 超出范围返回0xFF
		end
	end
end
// 将第0到4个地址的寄存器引出
assign mem0_out = mem[0];
assign mem1_out = mem[1];
assign mem2_out = mem[2];
assign mem3_out = mem[3];
assign mem4_out = mem[4];
endmodule

