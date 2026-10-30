//四线spi
module	fspi_slave_m_imu
(
    input wire [511:0] imu_snapshot,
	input	wire			spi_clk			,
	input	wire			spi_cs			,
	inout	wire	[3:0]	spi_data		,
	input	wire			rst_n			,
	input	wire	[3:0]		select_rearview	,	//从顶层传入的select_rearview信号
    input   wire            auto_brake_enable,
    input   wire [5:0]      speed_status_async,
    input   wire [15:0]     control_status_async,

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

// The proven SMDT RX-only transaction supplies opcode 80 automatically.
// Byte 0 remains the exact legacy repeated HMI nibble. A four-byte read adds
// A7, a coherent gear snapshot, and CRC8. No addressed-81 read is required.
(* ASYNC_REG="TRUE" *) reg [5:0] speed_meta,speed_sync;
reg [7:0] extended_index;
reg [511:0] imu_latched;
reg [3:0] extended_nibble;
reg [3:0] hmi_snapshot;
reg [7:0] speed_snapshot,speed_crc;
(* ASYNC_REG="TRUE" *) reg [15:0] control_meta,control_sync;
reg [15:0] control_snapshot;
reg [7:0] control_crc;
reg extended_active;
function [2:0] decode_gear(input [2:0] g);
    case(g)
      3'b000,3'b010:decode_gear=1;
      3'b001,3'b110:decode_gear=2;
      3'b011,3'b100:decode_gear=3;
      default:decode_gear=0;
    endcase
endfunction
wire [2:0] sampled_gear=decode_gear(speed_sync[2:0]);
wire [7:0] sampled_speed={1'b1,(speed_sync[5]&&sampled_gear!=0),
                        speed_sync[4],speed_sync[3],1'b0,sampled_gear};
function [7:0] speed_crc8(input [7:0] value);
    reg [7:0] c;integer k;
    begin
      c=8'ha7;
      for(k=0;k<8;k=k+1)c=c[7]?(c<<1)^8'h07:c<<1;
      c=c^value;
      for(k=0;k<8;k=k+1)c=c[7]?(c<<1)^8'h07:c<<1;
      speed_crc8=c;
    end
endfunction
function [7:0] crc_byte(input [7:0] c0,input [7:0] value);
    reg [7:0] c;integer k;
    begin
      c=c0^value;
      for(k=0;k<8;k=k+1)c=c[7]?(c<<1)^8'h07:c<<1;
      crc_byte=c;
    end
endfunction
wire [7:0] sampled_control_crc=crc_byte(crc_byte(crc_byte(0,8'hd7),control_sync[7:0]),control_sync[15:8]);
always @(posedge spi_clk or negedge rst_n) begin
    if(!rst_n) begin speed_meta<=0;speed_sync<=0;control_meta<=0;control_sync<=0;end
    else begin speed_meta<=speed_status_async;speed_sync<=speed_meta;control_meta<=control_status_async;control_sync<=control_meta;end
end
always @(negedge spi_clk or negedge rst_n or posedge spi_cs) begin
    if(!rst_n||spi_cs) begin
      extended_index<=0;extended_nibble<=4'hf;extended_active<=0;
      hmi_snapshot<=0;speed_snapshot<=0;speed_crc<=0;imu_latched<=0;
      control_snapshot<=0;control_crc<=0;
    end else if(cmd_reg==2'b10) begin
      extended_active<=1;
      if(extended_index<144)extended_index<=extended_index+1'b1;
      case(extended_index)
        0:begin
          extended_nibble<=select_rearview;hmi_snapshot<=select_rearview;
          imu_latched<=imu_snapshot;
          speed_snapshot<=sampled_speed;speed_crc<=speed_crc8(sampled_speed);
          control_snapshot<=control_sync;control_crc<=sampled_control_crc;
        end
        1:extended_nibble<=hmi_snapshot;
        2:extended_nibble<=4'ha;
        3:extended_nibble<=4'h7;
        4:extended_nibble<=speed_snapshot[7:4];
        5:extended_nibble<=speed_snapshot[3:0];
        6:extended_nibble<=speed_crc[7:4];
        7:extended_nibble<=speed_crc[3:0];
        // Optional bytes 4..7 expose logical contacts and gates. The first
        // four bytes and legacy one-byte reads remain byte-for-byte intact.
        8:extended_nibble<=4'hd;
        9:extended_nibble<=4'h7;
        10:extended_nibble<=control_snapshot[7:4];
        11:extended_nibble<=control_snapshot[3:0];
        12:extended_nibble<=control_snapshot[15:12];
        13:extended_nibble<=control_snapshot[11:8];
        14:extended_nibble<=control_crc[7:4];
        15:extended_nibble<=control_crc[3:0];
        default:extended_nibble<=extended_index<144 ?
          imu_latched[511-(extended_index-16)*4 -: 4] : 4'hf;
      endcase
    end
end

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
                   ((!spi_cs && (cmd == 2'b10) && extended_active) ? extended_nibble :
                   ((!spi_cs && (cmd == 2'b10)) ? {select_rearview} : 4'bzzzz));
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
