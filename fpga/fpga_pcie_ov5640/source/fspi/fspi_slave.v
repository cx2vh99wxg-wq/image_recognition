//四线spi
module	fspi_slave
(
	input	wire			spi_clk			,
	input	wire			spi_cs			,
	inout	wire	[3:0]	spi_data		,
	input	wire			rst_n			,
	input	wire	[3:0]		select_rearview	,	//从顶层传入的select_rearview信号
	
	//user	
	output	wire	[1:0]	cmd				,	//2'b10:写   2'b01:读
	output	wire	[7:0]	rx_data			,
	output	wire			rx_data_valid	,
	
	input	wire	[7:0]	tx_data			,
	output	wire			tx_data_valid	

);
//-----------------reg_define---------------------
reg			recv_flag	;
reg	[7:0]	recv_data	;
reg			recv_valid	;
reg	[1:0]	cmd_reg		;
reg			cmd_lock	;	//确保不会被数据误判操作


//-----------------assign---------------------
//当片选拉低，且是写操作的时候数据发送给到RK。
assign 	spi_data = (!spi_cs && (cmd == 2'b10)) ? {select_rearview} : 4'bzzzz;
assign	tx_data_valid	=	tx_valid_reg;
assign	cmd	=	cmd_reg;
assign	rx_data	=	recv_data;
assign	rx_data_valid	=	recv_valid;
//-------------------接收部分----------------------------
always@(posedge spi_clk or negedge rst_n) begin
	if ((rst_n == 1'b0) || spi_cs) 
	begin	
		recv_flag	<=	1'd0	;
		recv_data	<=	8'hff	;
	end
	else	if(!spi_cs)
	begin
		if(!recv_flag)
		begin
			recv_data[7:4]	<=	spi_data;
			recv_flag		<=	~recv_flag;
		end
		else	if(recv_flag)
		begin
			recv_data[3:0]	<=	spi_data;
			recv_flag		<=	~recv_flag;
		end
	end
end

//cmd_lock 需要锁住  避免和数据冲突，需要在下次cs到来置0
always@(posedge spi_clk or negedge rst_n or posedge spi_cs) begin
	if ((rst_n == 1'b0) || spi_cs) 
	begin
		cmd_lock	<=	1'd0;
		cmd_reg		<=	2'd0;
	end
	else	if(!spi_cs && recv_flag)
	begin
		if(!cmd_lock)
		begin
			if({recv_data[7:4],spi_data}==8'h80)
				cmd_reg	<=	2'b10;
			else	if({recv_data[7:4],spi_data}==8'h00)
				cmd_reg	<=	2'b01;
			cmd_lock	<=	1'd1;
		end
	end
end

//接收完成
always@(posedge spi_clk or negedge rst_n) begin
	if ((rst_n == 1'b0) || spi_cs) 
		recv_valid	<=	1'd0;
	else	if(!spi_cs)
	begin
		if(recv_flag && cmd_reg==2'b01)
			recv_valid	<=	1'd1;
		else
			recv_valid	<=	1'd0;
	end
end

//--------------------发送部分-------------------------
reg	[3:0]	tx_data_reg		;
reg			tx_valid_reg	;	
//下降沿发送
always@(negedge spi_clk or negedge rst_n) begin
	if ((rst_n == 1'b0) || spi_cs) 
	begin
		tx_data_reg		<=	4'hf;
		tx_valid_reg	<=	1'd0;
	end
	else	if(!spi_cs)
	begin
		if(cmd_reg==2'b10)
		begin
			if(!tx_valid_reg)
			begin
				tx_data_reg		<=	tx_data[7:4];
				tx_valid_reg	<=	~tx_valid_reg;
			end
			else	if(tx_valid_reg)
			begin
				tx_data_reg		<=	tx_data[3:0];
				tx_valid_reg	<=	~tx_valid_reg;
			end
		end
	end
end




endmodule