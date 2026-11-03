module matrix_3x3_1bit
#(
parameter COL = 640,
parameter ROW = 480
)
(
	input clk,
    input rst_n,
    input valid_in,// 指示输入数据是否处于有效区间。
    input din,     // 输入的图像数据，将一帧的数据从左到右，随后从上到下依次输入
	
	output wire matrix_11,
	output wire matrix_12,
	output wire matrix_13,
	output wire matrix_21,
	output wire matrix_22,
	output wire matrix_23,
	output wire matrix_31,
	output wire matrix_32,
	output wire matrix_33
	
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

wire q_1;
wire q_2;

wire dout_r2;
wire dout_r1;
wire dout_r0;

assign dout_r2 = din;
assign dout_r1 = q_1;
assign dout_r0 = q_2;

wire wr_en_1;
wire rd_en_1;
wire wr_en_2;
wire rd_en_2;

assign wr_en_1 = (row_cnt < ROW - 1) ? valid_in : 1'b0; // 不写末尾1行
assign rd_en_1 = (row_cnt > 0) ? valid_in : 1'b0; // 读取起点设在第一行。
assign wr_en_2 = (row_cnt < ROW - 2) ? valid_in : 1'b0; // 不写末尾2行
assign rd_en_2 = (row_cnt > 1) ? valid_in : 1'b0; // 读取起点设在第二行。
/* 停用示例，仅供对照：
	FIFO_SC_1bit_Top u1_FIFO_SC_1bit_Top(
		.data(din), //input [7:0] Data
		.clock(clk), //input Clk
		.wrreq(wr_en_1), //input WrEn
		.rdreq(rd_en_1), //input RdEn
		.aclr(~rst_n), //input Reset
		.q(q_1), //output [7:0] Q
		.empty(), //output Empty
		.full() //output Full
	);
*/

	FIFO_SC_1bit_Top u1_FIFO_SC_1bit_Top(
		.wr_data(din), // 停用示例，仅供对照：input [7:0] Data
		.clk(clk), // 停用示例，仅供对照：input Clk
		.wr_en(wr_en_1), // 停用示例，仅供对照：input WrEn
		.rd_en(rd_en_1), // 停用示例，仅供对照：input RdEn
		.rst(~rst_n), // 停用示例，仅供对照：input Reset
		.rd_data(q_1), // 停用示例，仅供对照：output [7:0] Q
		.rd_empty(), // 停用示例，仅供对照：output Empty
		.wr_full(), // 停用示例，仅供对照：output Full
        .almost_full(),
        .almost_empty()
	);
/* 停用示例，仅供对照：
	FIFO_SC_1bit_Top u2_FIFO_SC_1bit_Top(
		.data(din), //input [7:0] Data
		.clock(clk), //input Clk
		.wrreq(wr_en_2), //input WrEn
		.rdreq(rd_en_2), //input RdEn
		.aclr(~rst_n), //input Reset
		.q(q_2), //output [7:0] Q
		.empty(), //output Empty
		.full() //output Full
	);
*/
	FIFO_SC_1bit_Top u2_FIFO_SC_1bit_Top(
		.wr_data(din), // 停用示例，仅供对照：input [7:0] Data
		.clk(clk), // 停用示例，仅供对照：input Clk
		.wr_en(wr_en_2), // 停用示例，仅供对照：input WrEn
		.rd_en(rd_en_2), // 停用示例，仅供对照：input RdEn
		.rst(~rst_n), // 停用示例，仅供对照：input Reset
		.rd_data(q_2), // 停用示例，仅供对照：output [7:0] Q
		.rd_empty(), // 停用示例，仅供对照：output Empty
		.wr_full(), // 停用示例，仅供对照：output Full
        .almost_full(),
        .almost_empty()
	);

// 2026-11-03: share border extension and the three-row shift stage.
isp_window_border #(.PIXEL_BITS(1)) u_border (
    .clk(clk), .rst_n(rst_n), .row_index(row_cnt), .column_index(col_cnt),
    .current_pixel(dout_r2), .previous_line(dout_r1), .earlier_line(dout_r0),
    .top_row({matrix_11, matrix_12, matrix_13}),
    .middle_row({matrix_21, matrix_22, matrix_23}),
    .bottom_row({matrix_31, matrix_32, matrix_33})
);
endmodule
