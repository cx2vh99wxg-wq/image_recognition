// 2026-11-03 注释校订。在寄存器时钟下降沿解析读写。完整地址锁存后，已排队的写入仍在下一拍提交。
module spi_ram_crtl
(
	input	wire				wr_clk		,
	input	wire				rst_n		,
	input	wire				spi_cs		,

	input	wire	[1:0]		cmd			,
	input	wire				tx_done		,
	input	wire	[7:0]		wr_data		,
	input	wire				rx_valid	,
	input	wire	[3:0]			select_rearview	,
	output	reg		[7:0]		rd_data		,
	output	wire	[7:0]		mem0_out	,
	output	wire	[7:0]		mem1_out	,
	output	wire	[7:0]		mem2_out	,
	output	wire	[7:0]		mem3_out	,
	output	wire	[7:0]		mem4_out,
    output  wire    [7:0]        mem5_out,
    output  wire    [7:0]        mem6_out
);

localparam IDLE         = 3'd0;
localparam RCV_ADDR_H   = 3'd1;
localparam RCV_ADDR_L   = 3'd2;
localparam WRITE_DATA   = 3'd3;
localparam READ_ADDR_H  = 3'd4;
localparam READ_ADDR_L  = 3'd5;
localparam READ_DATA    = 3'd6;

wire		rx_pose	;
wire		rx_nege	;
wire		tx_pose	;
wire		tx_nege	;

reg	[2:0]	state			/* synthesis PAP_MARK_DEBUG="true" */;
reg	[1:0]	cmd_reg			;
reg			spi_cs_reg		;
reg			rx_valid_d0		;
reg			rx_valid_d1		;
reg			tx_done_d0		;
reg			tx_done_d1		;
reg	[7:0]	wr_data_reg		;
reg	[7:0]	addr_high		/* synthesis PAP_MARK_DEBUG="true" */;
reg			ram_wr_en		/* synthesis PAP_MARK_DEBUG="true" */;
reg	[15:0]	ram_addr		/* synthesis PAP_MARK_DEBUG="true" */;
reg	[1:0]	byte_cnt		/* synthesis PAP_MARK_DEBUG="true" */;
reg [7:0]   read_addr_high  /* synthesis PAP_MARK_DEBUG="true" */;

reg [7:0] transaction_high;
reg transaction_high_zero;
reg address_in_bank, address_is_gear, address_read_valid;

assign	rx_pose = rx_valid_d0 && ~rx_valid_d1	;
assign	rx_nege = ~rx_valid_d0 && rx_valid_d1	;
assign	tx_pose	= tx_done_d0 && ~tx_done_d1		;
assign	tx_nege	= ~tx_done_d0 && tx_done_d1		;

// 下降沿各过程读取更新前的寄存器值，片选撤销与已排队写入可在同拍出现。
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

always@(negedge wr_clk or negedge rst_n)	begin
	if(!rst_n)
		wr_data_reg	<=	8'd0;
	else if(spi_cs_reg)
		wr_data_reg	<=	8'hff;
	else
		wr_data_reg	<=	wr_data;
end

always@(negedge wr_clk or negedge rst_n) begin
	if(!rst_n) begin
		state		<=	IDLE;
		ram_addr	<=	16'd0;
		ram_wr_en	<=	1'd0;
		addr_high	<=	8'd0;
		read_addr_high <= 8'd0;
        transaction_high <= 8'd0;
        transaction_high_zero <= 1'b1;
        address_in_bank <= 1'b1;
        address_is_gear <= 1'b0;
        address_read_valid <= 1'b1;
		byte_cnt	<=	2'd0;
	end
	else if(spi_cs_reg) begin

		state		<=	IDLE;
		ram_wr_en	<=	1'd0;
		byte_cnt	<=	2'd0;
	end
	else begin
		case(state)
            IDLE: begin
                ram_wr_en <= 1'b0;
                byte_cnt <= 2'd0;

                if (rx_pose) begin
                    case (cmd_reg)
                        2'b01: begin
                            addr_high <= wr_data_reg;
                            transaction_high <= wr_data_reg;
                            transaction_high_zero <= (wr_data_reg == 8'd0);
                            state <= RCV_ADDR_H;
                            byte_cnt <= 2'd1;
                        end
                        2'b10: begin
                            read_addr_high <= wr_data_reg;
                            transaction_high <= wr_data_reg;
                            transaction_high_zero <= (wr_data_reg == 8'd0);
                            state <= READ_ADDR_H;
                            byte_cnt <= 2'd1;
                        end
                        default: ;
                    endcase
                end
            end

            RCV_ADDR_H, READ_ADDR_H: begin

                if (rx_pose && byte_cnt == 2'd1) begin
                    ram_addr <= {transaction_high, wr_data_reg};

                    address_in_bank <= transaction_high_zero && wr_data_reg < 8'd6;
                    address_is_gear <= transaction_high_zero && wr_data_reg == 8'd6;
                    address_read_valid <= transaction_high_zero && wr_data_reg < 8'd7;
                    byte_cnt <= 2'd2;
                    state <= (state == RCV_ADDR_H) ? RCV_ADDR_L : READ_ADDR_L;
                end
            end

            RCV_ADDR_L: begin
                if (rx_pose && byte_cnt == 2'd2) begin

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

reg [7:0] mem [5:0];

reg [7:0] gear_reference;
always @(negedge wr_clk or negedge rst_n) begin
    if (!rst_n) gear_reference <= 8'd0;
    else if (ram_wr_en && address_is_gear) gear_reference <= wr_data_reg;
end

always@(negedge wr_clk or negedge rst_n) begin
	if(!rst_n) begin

		mem[0] <= 8'd1;
		mem[1] <= 8'd1;
		mem[2] <= 8'd1;
		mem[3] <= 8'd1;
		mem[4] <= 8'd1;
		mem[5] <= 8'd0;
		rd_data <= 8'd0;
	end
	else begin

		if(ram_wr_en && address_in_bank) begin
			// 地址范围在锁存时预解码。写数据取提交边沿前已保存的字节。
			mem[ram_addr[2:0]] <= wr_data_reg;
		end

		if(address_read_valid) begin
			rd_data <= {4'd0, select_rearview};
		end
		else begin
			rd_data <= 8'hFF;
		end
	end
end

assign mem0_out = mem[0];
assign mem1_out = mem[1];
assign mem2_out = mem[2];
assign mem3_out = mem[3];
assign mem4_out = mem[4];
assign mem5_out = mem[5];
assign mem6_out = gear_reference;
endmodule

