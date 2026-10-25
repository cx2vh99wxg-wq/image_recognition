//四线spi
module	fspi_slave
(
	input	wire			spi_clk			,
	input	wire			spi_cs			,
	inout	wire	[3:0]	spi_data		,
	input	wire			rst_n			,
	input	wire	[3:0]		select_rearview	,	//从顶层传入的select_rearview信号
    input   wire            auto_brake_enable,

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
reg [3:0] tx_data_reg;
reg tx_valid_reg;

// Dedicated addressed status read. Legacy 00 writes and 80 reads below keep
// their existing behavior; only 81 00 05 selects the new switch register.
(* ASYNC_REG = "TRUE" *) reg auto_enable_meta;
(* ASYNC_REG = "TRUE" *) reg auto_enable_sync;
reg [23:0] status_header;
reg [2:0] status_nibble_count;
reg status_read_ready;
reg [7:0] status_read_data;
reg status_tx_started;
reg [3:0] status_tx_nibble;
wire [23:0] next_status_header = {status_header[19:0], spi_data};

always @(posedge spi_clk or negedge rst_n) begin
    if (!rst_n) begin
        auto_enable_meta <= 1'b0;
        auto_enable_sync <= 1'b0;
    end else begin
        auto_enable_meta <= auto_brake_enable;
        auto_enable_sync <= auto_enable_meta;
    end
end

// Six quad-clock samples collect opcode and a 16-bit address. Latch one
// response byte so an enable transition cannot tear its two output nibbles.
always @(posedge spi_clk or negedge rst_n or posedge spi_cs) begin
    if (!rst_n || spi_cs) begin
        status_header <= 24'd0;
        status_nibble_count <= 3'd0;
        status_read_ready <= 1'b0;
        status_read_data <= 8'hff;
    end else if (status_nibble_count < 3'd6) begin
        status_header <= next_status_header;
        status_nibble_count <= status_nibble_count + 3'd1;
        if (status_nibble_count == 3'd5 && next_status_header[23:16] == 8'h81) begin
            status_read_ready <= 1'b1;
            status_read_data <= (next_status_header[15:0] == 16'h0005) ?
                                (auto_enable_sync ? 8'ha1 : 8'ha0) : 8'hff;
        end
    end
end

// SPI mode 3: change the outgoing nibble on the falling edge, sample it on
// the rising edge. Keep the bus released during every command/address edge.
always @(negedge spi_clk or negedge rst_n or posedge spi_cs) begin
    if (!rst_n || spi_cs) begin
        status_tx_started <= 1'b0;
        status_tx_nibble <= 4'hf;
    end else if (status_read_ready) begin
        if (!status_tx_started) begin
            status_tx_nibble <= status_read_data[7:4];
            status_tx_started <= 1'b1;
        end else begin
            status_tx_nibble <= status_read_data[3:0];
        end
    end
end


//-----------------assign---------------------
//当片选拉低，且是写操作的时候数据发送给到RK。
assign 	spi_data = (!spi_cs && status_tx_started) ? status_tx_nibble :
                   ((!spi_cs && (cmd == 2'b10)) ? {select_rearview} : 4'bzzzz);
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
