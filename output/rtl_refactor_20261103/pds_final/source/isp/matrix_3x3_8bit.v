module matrix_3x3_8bit
#(
parameter COL = 640,
parameter ROW = 480
)
(
	input clk,
    input rst_n,
    input valid_in,//输入数据有效信号
    input [7:0] din,     //输入的图像数据，将一帧的数据从左到右，然后从上到下依次输入
	
	output wire [7:0] matrix_11,
	output wire [7:0] matrix_12,
	output wire [7:0] matrix_13,
	output wire [7:0] matrix_21,
	output wire [7:0] matrix_22,
	output wire [7:0] matrix_23,
	output wire [7:0] matrix_31,
	output wire [7:0] matrix_32,
	output wire [7:0] matrix_33
	
);

reg [15:0] col_cnt;
reg [15:0] row_cnt;

always @(posedge clk or negedge rst_n)
    if(rst_n == 1'b0)
        col_cnt             <=          11'd0;
    else if(col_cnt == COL-1 && valid_in == 1'b1)
        col_cnt             <=          11'd0;
    else if(valid_in == 1'b1)
        col_cnt             <=          col_cnt + 1'b1;
    else
        col_cnt             <=          col_cnt;

always @(posedge clk or negedge rst_n)
    if(rst_n == 1'b0)
        row_cnt             <=          11'd0;
    else if(row_cnt == ROW-1 && col_cnt == COL-1 && valid_in == 1'b1)
        row_cnt             <=          11'd0;
    else if(col_cnt == COL-1 && valid_in == 1'b1) 
        row_cnt             <=          row_cnt + 1'b1;

wire [7:0] q_1;
wire [7:0] q_2;

wire [7:0] dout_r2;
wire [7:0] dout_r1;
wire [7:0] dout_r0;

assign dout_r2 = din;
assign dout_r1 = q_1;
assign dout_r0 = q_2;

wire wr_en_1;
wire rd_en_1;
wire wr_en_2;
wire rd_en_2;

assign wr_en_1 = (row_cnt < ROW - 1) ? valid_in : 1'b0; //不写最后1行
assign rd_en_1 = (row_cnt > 0) ? valid_in : 1'b0; //从第1行开始读
assign wr_en_2 = (row_cnt < ROW - 2) ? valid_in : 1'b0; //不写最后2行
assign rd_en_2 = (row_cnt > 1) ? valid_in : 1'b0; //从第2行开始读

	FIFO_SC_8bit_Top u1_FIFO_SC_8bit_Top(
		.wr_data(din), //input [7:0] Data
		.clk(clk), //input Clk
		.wr_en(wr_en_1), //input WrEn
		.rd_en(rd_en_1), //input RdEn
		.rst(~rst_n), //input Reset
		.rd_data(q_1), //output [7:0] Q
		.rd_empty(), //output Empty
		.wr_full(), //output Full
        .almost_full(),
        .almost_empty()
	);

	FIFO_SC_8bit_Top u2_FIFO_SC_8bit_Top(
		.wr_data(din), //input [7:0] Data
		.clk(clk), //input Clk
		.wr_en(wr_en_2), //input WrEn
		.rd_en(rd_en_2), //input RdEn
		.rst(~rst_n), //input Reset
		.rd_data(q_2), //output [7:0] Q
		.rd_empty(), //output Empty
		.wr_full(), //output Full
        .almost_full(),
        .almost_empty()
	);

// 2026-11-03: share border extension and the three-row shift stage.
isp_window_border #(.PIXEL_BITS(8)) u_border (
    .clk(clk), .rst_n(rst_n), .row_index(row_cnt), .column_index(col_cnt),
    .current_pixel(dout_r2), .previous_line(dout_r1), .earlier_line(dout_r0),
    .top_row({matrix_11, matrix_12, matrix_13}),
    .middle_row({matrix_21, matrix_22, matrix_23}),
    .bottom_row({matrix_31, matrix_32, matrix_33})
);
endmodule


module isp_window_border #(parameter PIXEL_BITS = 8) (
    input clk, rst_n,
    input [15:0] row_index, column_index,
    input [PIXEL_BITS-1:0] current_pixel, previous_line, earlier_line,
    output reg [3*PIXEL_BITS-1:0] top_row, middle_row, bottom_row
);
    reg [PIXEL_BITS-1:0] top_pixel, middle_pixel;

    // Repeat the nearest available line at the top border.
    always @* begin
        if (row_index == 0) begin
            top_pixel = current_pixel;
            middle_pixel = current_pixel;
        end else if (row_index == 1) begin
            top_pixel = previous_line;
            middle_pixel = previous_line;
        end else begin
            top_pixel = earlier_line;
            middle_pixel = previous_line;
        end
    end

    // The window advances on every clock, including invalid pixels.
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            top_row <= {3*PIXEL_BITS{1'b0}};
            middle_row <= {3*PIXEL_BITS{1'b0}};
            bottom_row <= {3*PIXEL_BITS{1'b0}};
        end else if (column_index == 0) begin
            top_row <= {3{top_pixel}};
            middle_row <= {3{middle_pixel}};
            bottom_row <= {3{current_pixel}};
        end else begin
            top_row <= {top_row[2*PIXEL_BITS-1:0], top_pixel};
            middle_row <= {middle_row[2*PIXEL_BITS-1:0], middle_pixel};
            bottom_row <= {bottom_row[2*PIXEL_BITS-1:0], current_pixel};
        end
    end
endmodule
