// 2026-11-03 注释校订。先排序三行，再对行极值和中值做列归约，最后选取邻域中值。
module middle_filter
#(
	parameter U_COL = 640,
	parameter U_ROW = 480
)
(
	input clk,
	input rst_n,

	input in_vs,
	input in_hs,
	input in_de,
	input [7:0] in_data,
    input [23:0] in_rgb,

    input [9:0] in_x,
    input [9:0] in_y,

	output out_vs,
	output out_hs,
	output out_de,
	output [7:0] out_data,
	output [23:0] out_rgb,

    output [9:0] out_x,
    output [9:0] out_y
);

wire [7:0] matrix_11 ;
wire [7:0] matrix_12 ;
wire [7:0] matrix_13 ;
wire [7:0] matrix_21 ;
wire [7:0] matrix_22 ;
wire [7:0] matrix_23 ;
wire [7:0] matrix_31 ;
wire [7:0] matrix_32 ;
wire [7:0] matrix_33 ;

matrix_3x3_8bit #(
	.COL  (U_COL),
	.ROW  (U_ROW)
)u_matrix_3x3
(
	.clk      (clk),
	.rst_n    (rst_n),
	.valid_in (in_de),
	.din      (in_data),

	.matrix_11(matrix_11),
	.matrix_12(matrix_12),
	.matrix_13(matrix_13),
	.matrix_21(matrix_21),
	.matrix_22(matrix_22),
	.matrix_23(matrix_23),
	.matrix_31(matrix_31),
	.matrix_32(matrix_32),
	.matrix_33(matrix_33)
);

wire [71:0] taps = {matrix_33, matrix_32, matrix_31, matrix_23, matrix_22,
                     matrix_21, matrix_13, matrix_12, matrix_11};
wire [23:0] row_max, row_mid, row_min;
genvar row;
generate
    for (row = 0; row < 3; row = row + 1) begin : sort_row
        sort u_sort (
            .clk(clk), .rst_n(rst_n),
            .data1(taps[row*24 +: 8]),
            .data2(taps[row*24+8 +: 8]),
            .data3(taps[row*24+16 +: 8]),
            .max_data(row_max[row*8 +: 8]),
            .mid_data(row_mid[row*8 +: 8]),
            .min_data(row_min[row*8 +: 8])
        );
    end
endgenerate

wire [7:0] min_max_data;
wire [7:0] mid_mid_data;
wire [7:0] max_min_data;
wire [7:0] median_data;

assign out_data = median_data;

sort u4
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (row_min[0 +: 8]              ),
    .data2                  (row_min[8 +: 8]              ),
    .data3                  (row_min[16 +: 8]              ),
    .max_data               (min_max_data           ),
    .mid_data               (                       ),
    .min_data               (                       )
);

sort u5
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (row_mid[0 +: 8]              ),
    .data2                  (row_mid[8 +: 8]              ),
    .data3                  (row_mid[16 +: 8]              ),
    .max_data               (                       ),
    .mid_data               (mid_mid_data           ),
    .min_data               (                       )
);

sort u6
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (row_max[0 +: 8]              ),
    .data2                  (row_max[8 +: 8]              ),
    .data3                  (row_max[16 +: 8]              ),
    .max_data               (                       ),
    .mid_data               (                       ),
    .min_data               (max_min_data           )
);

sort u7
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (max_min_data           ),
    .data2                  (mid_mid_data           ),
    .data3                  (min_max_data           ),
    .max_data               (                       ),
    .mid_data               (median_data            ),
    .min_data               (                       )
);

isp_pixel_delay #(.CYCLES(4)) u_metadata (
    .clk(clk), .rst_n(rst_n),
    .pixel_in({in_vs, in_hs, in_de, in_x, in_y, in_rgb}),
    .pixel_out({out_vs, out_hs, out_de, out_x, out_y, out_rgb})
);
endmodule
