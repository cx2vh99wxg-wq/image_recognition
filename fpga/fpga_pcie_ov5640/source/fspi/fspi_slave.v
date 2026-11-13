// 2026-11-03 注释校订。共用四线 SPI 传输引擎。输入在上升沿采样，返回半字节在下降沿更新。
module	fspi_slave
(
	input	wire			spi_clk			,
	input	wire			spi_cs			,
	inout	wire	[3:0]	spi_data		,
	input	wire			rst_n			,
	input	wire	[3:0]		select_rearview	,
    input   wire            auto_brake_enable,
    input   wire [5:0]      speed_status_async,
    input   wire [15:0]     control_status_async,

	output	wire	[1:0]	cmd				,
	output	wire	[7:0]	rx_data			,
	output	wire			rx_data_valid	,

	input	wire	[7:0]	tx_data			,
	output	wire			tx_data_valid

);

    wire status_tx_started;
    fspi_transport #(.HAS_IMU(1'b0)) u_transport (
        .spi_clk(spi_clk),
        .spi_cs(spi_cs),
        .spi_data(spi_data),
        .rst_n(rst_n),
        .select_rearview(select_rearview),
        .camera_view_async(4'd0), .wheel_report_async(32'd0),
        .auto_brake_enable(auto_brake_enable),
        .speed_status_async(speed_status_async),
        .control_status_async(control_status_async),
        .cmd(cmd),
        .rx_data(rx_data),
        .rx_data_valid(rx_data_valid),
        .tx_data(tx_data),
        .tx_data_valid(tx_data_valid),
        .imu_snapshot(512'd0), .status_tx_started_dbg(status_tx_started)
    );
endmodule

module fspi_transport #(
    parameter HAS_IMU = 1'b0,
    parameter HAS_CAMERA_VIEW = 1'b0,
    parameter HAS_WHEEL_REPORT = 1'b0
) (
    input wire [511:0] imu_snapshot,
    output wire status_tx_started_dbg,
	input	wire			spi_clk			,
	input	wire			spi_cs			,
	inout	wire	[3:0]	spi_data		,
	input	wire			rst_n			,
	input	wire	[3:0]		select_rearview	,
    input wire [3:0] camera_view_async,
    input wire [31:0] wheel_report_async,
    input   wire            auto_brake_enable,
    input   wire [5:0]      speed_status_async,
    input   wire [15:0]     control_status_async,

	output	wire	[1:0]	cmd				,
	output	wire	[7:0]	rx_data			,
	output	wire			rx_data_valid	,

	input	wire	[7:0]	tx_data			,
	output	wire			tx_data_valid

);

reg			recv_flag	;
reg	[7:0]	recv_data	;
reg			recv_valid	;
reg	[1:0]	cmd_reg		;
reg			cmd_lock	;
reg [3:0] tx_data_reg;
reg tx_valid_reg;

(* ASYNC_REG = "TRUE" *) reg auto_enable_meta;
(* ASYNC_REG = "TRUE" *) reg auto_enable_sync;
// 地址状态读按六个半字节接收命令与地址，随后锁存返回字节。
reg [23:0] status_header;
reg [2:0] status_nibble_count;
reg status_read_ready;
reg [7:0] status_read_data;
reg status_tx_started;
assign status_tx_started_dbg = status_tx_started;
reg [3:0] status_tx_nibble;
wire [23:0] next_status_header = {status_header[19:0], spi_data};

(* ASYNC_REG="TRUE" *) reg [5:0] speed_meta,speed_sync;
localparam EXT_INDEX_WIDTH = HAS_IMU ? 8 : 5;
localparam [EXT_INDEX_WIDTH-1:0] EXT_END = HAS_IMU ? 144 : HAS_WHEEL_REPORT ? 26 : HAS_CAMERA_VIEW ? 18 : 16;
reg [EXT_INDEX_WIDTH-1:0] extended_index;
reg [511:0] imu_latched;
reg [3:0] extended_nibble;
reg [3:0] hmi_snapshot;
reg [7:0] speed_snapshot,speed_crc;
(* ASYNC_REG="TRUE" *) reg [15:0] control_meta,control_sync;
reg [15:0] control_snapshot;
reg [7:0] control_crc;
(* ASYNC_REG = "TRUE" *) reg [3:0] camera_view_meta, camera_view_sync;
reg [3:0] camera_view_previous, camera_view_snapshot;
(* ASYNC_REG = "TRUE" *) reg [31:0] wheel_meta, wheel_sync;
reg [31:0] wheel_previous, wheel_snapshot;
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
always @(posedge spi_clk or negedge rst_n) begin
    if (!rst_n) begin
      camera_view_meta <= 0;
      camera_view_sync <= 0;
      camera_view_previous <= 0;
      wheel_meta <= 0;
      wheel_sync <= 0;
      wheel_previous <= 0;
    end else begin
      camera_view_meta <= camera_view_async;
      camera_view_sync <= camera_view_meta;
      camera_view_previous <= camera_view_sync;
      wheel_meta <= wheel_report_async;
      wheel_sync <= wheel_meta;
      wheel_previous <= wheel_sync;
    end
end

// S appends a view byte then a signed-by-CRC wheel packet. M keeps its IMU tail.
wire [3:0] extended_tail = HAS_IMU && extended_index < EXT_END ?
    imu_latched[511-(extended_index-16)*4 -: 4] :
    HAS_CAMERA_VIEW && extended_index < 18 ? camera_view_snapshot :
    HAS_WHEEL_REPORT && extended_index < EXT_END ?
    wheel_snapshot[31-(extended_index-18)*4 -: 4] : 4'hf;
always @(negedge spi_clk or negedge rst_n or posedge spi_cs) begin
    if(!rst_n||spi_cs) begin
      extended_index<=0;extended_nibble<=4'hf;extended_active<=0;
      hmi_snapshot<=0;speed_snapshot<=0;speed_crc<=0;imu_latched<=0;
      control_snapshot<=0;control_crc<=0;
      camera_view_snapshot<=0;
      wheel_snapshot<=0;
    end else if(cmd_reg==2'b10) begin
      extended_active<=1;
      if (extended_index < EXT_END)extended_index<=extended_index+1'b1;
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

        8:extended_nibble<=4'hd;
        9:extended_nibble<=4'h7;
        10:extended_nibble<=control_snapshot[7:4];
        11:extended_nibble<=control_snapshot[3:0];
        12:extended_nibble<=control_snapshot[15:12];
        13:extended_nibble<=control_snapshot[11:8];
        14:begin
          extended_nibble<=control_crc[7:4];
          camera_view_snapshot <= camera_view_sync == camera_view_previous ? camera_view_sync : 4'd0;
          wheel_snapshot <= wheel_sync == wheel_previous ? wheel_sync : 32'd0;
        end
        15:extended_nibble<=control_crc[3:0];
        default:extended_nibble<=extended_tail;
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

assign 	spi_data = (!spi_cs && status_tx_started) ? status_tx_nibble :
                   ((!spi_cs && (cmd == 2'b10) && extended_active) ? extended_nibble :
                   ((!spi_cs && (cmd == 2'b10)) ? {select_rearview} : 4'bzzzz));
assign	tx_data_valid	=	tx_valid_reg;
assign	cmd	=	cmd_reg;
assign	rx_data	=	recv_data;
assign	rx_data_valid	=	recv_valid;

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
