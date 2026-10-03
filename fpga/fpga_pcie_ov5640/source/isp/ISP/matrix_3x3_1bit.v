module matrix_3x3_1bit
#(
parameter COL = 640,
parameter ROW = 480
)
(
	input clk,
    input rst_n,
    input valid_in,//输入数据有效信号
    input din,     //输入的图像数据，将一帧的数据从左到右，然后从上到下依次输入
	
	output reg matrix_11, 
	output reg matrix_12, 
	output reg matrix_13, 
	output reg matrix_21, 
	output reg matrix_22, 
	output reg matrix_23, 
	output reg matrix_31, 
	output reg matrix_32, 
	output reg matrix_33 
	
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

assign wr_en_1 = (row_cnt < ROW - 1) ? valid_in : 1'b0; //不写最后1行
assign rd_en_1 = (row_cnt > 0) ? valid_in : 1'b0; //从第1行开始读
assign wr_en_2 = (row_cnt < ROW - 2) ? valid_in : 1'b0; //不写最后2行
assign rd_en_2 = (row_cnt > 1) ? valid_in : 1'b0; //从第2行开始读
/*
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
/*
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

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        {matrix_11, matrix_12, matrix_13} <= {1'b0, 1'b0, 1'b0};
        {matrix_21, matrix_22, matrix_23} <= {1'b0, 1'b0, 1'b0};
        {matrix_31, matrix_32, matrix_33} <= {1'b0, 1'b0, 1'b0};
    end
    //------------------------------------------------------------------------- 第1排矩阵
    else if(row_cnt == 0)begin
        if(col_cnt == 0) begin        //第1个矩阵
            {matrix_11, matrix_12, matrix_13} <= {dout_r2, dout_r2, dout_r2};
            {matrix_21, matrix_22, matrix_23} <= {dout_r2, dout_r2, dout_r2};
            {matrix_31, matrix_32, matrix_33} <= {dout_r2, dout_r2, dout_r2};
        end
        else begin                    //剩余矩阵
            {matrix_11, matrix_12, matrix_13} <= {matrix_12, matrix_13, dout_r2};
            {matrix_21, matrix_22, matrix_23} <= {matrix_22, matrix_23, dout_r2};
            {matrix_31, matrix_32, matrix_33} <= {matrix_32, matrix_33, dout_r2};
        end
    end
    //------------------------------------------------------------------------- 第2排矩阵
    else if(row_cnt == 1)begin
        if(col_cnt == 0) begin        //第1个矩阵
            {matrix_11, matrix_12, matrix_13} <= {dout_r1, dout_r1, dout_r1};
            {matrix_21, matrix_22, matrix_23} <= {dout_r1, dout_r1, dout_r1};
            {matrix_31, matrix_32, matrix_33} <= {dout_r2, dout_r2, dout_r2};
        end
        else begin                    //剩余矩阵
            {matrix_11, matrix_12, matrix_13} <= {matrix_12, matrix_13, dout_r1};
            {matrix_21, matrix_22, matrix_23} <= {matrix_22, matrix_23, dout_r1};
            {matrix_31, matrix_32, matrix_33} <= {matrix_32, matrix_33, dout_r2};
        end
    end
    //------------------------------------------------------------------------- 剩余矩阵
    else begin
        if(col_cnt == 0) begin        //第1个矩阵
            {matrix_11, matrix_12, matrix_13} <= {dout_r0, dout_r0, dout_r0};
            {matrix_21, matrix_22, matrix_23} <= {dout_r1, dout_r1, dout_r1};
            {matrix_31, matrix_32, matrix_33} <= {dout_r2, dout_r2, dout_r2};
        end
        else begin                    //剩余矩阵
            {matrix_11, matrix_12, matrix_13} <= {matrix_12, matrix_13, dout_r0};
            {matrix_21, matrix_22, matrix_23} <= {matrix_22, matrix_23, dout_r1};
            {matrix_31, matrix_32, matrix_33} <= {matrix_32, matrix_33, dout_r2};
        end
    end
end

endmodule