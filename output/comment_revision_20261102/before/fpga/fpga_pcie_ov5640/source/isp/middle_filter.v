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

wire [7:0] max_data1;
wire [7:0] mid_data1;
wire [7:0] min_data1;

sort u1
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (matrix_11              ), 
    .data2                  (matrix_12              ), 
    .data3                  (matrix_13              ),
    .max_data               (max_data1              ),
    .mid_data               (mid_data1              ),
    .min_data               (min_data1              )
);

wire [7:0] max_data2;
wire [7:0] mid_data2;
wire [7:0] min_data2;

//第2行
sort u2
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (matrix_21              ),
    .data2                  (matrix_22              ),
    .data3                  (matrix_23              ),
    .max_data               (max_data2              ),
    .mid_data               (mid_data2              ),
    .min_data               (min_data2              )
);

wire [7:0] max_data3;
wire [7:0] mid_data3;
wire [7:0] min_data3;

//第3行
sort u3
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (matrix_31              ),
    .data2                  (matrix_32              ),
    .data3                  (matrix_33              ),
    .max_data               (max_data3              ),
    .mid_data               (mid_data3              ),
    .min_data               (min_data3              )
);

//三行的最小值取最大值
//三行的中间值取中间值
//三行的最大值取最小值，clk2
//---------------------------------------------------
//min-max
wire [7:0] min_max_data;
wire [7:0] mid_mid_data;
wire [7:0] max_min_data;
wire [7:0] median_data;

assign out_data = median_data;

sort u4
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (min_data1              ),
    .data2                  (min_data2              ),
    .data3                  (min_data3              ),
    .max_data               (min_max_data           ),
    .mid_data               (                       ),
    .min_data               (                       )
);

//mid-mid
sort u5
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (mid_data1              ),
    .data2                  (mid_data2              ),
    .data3                  (mid_data3              ),
    .max_data               (                       ),
    .mid_data               (mid_mid_data           ),
    .min_data               (                       )
);

//max-min
sort u6
(
    .clk                    (clk                    ),
    .rst_n                  (rst_n                  ),
    .data1                  (max_data1              ), 
    .data2                  (max_data2              ), 
    .data3                  (max_data3              ),
    .max_data               (                       ),
    .mid_data               (                       ),
    .min_data               (max_min_data           )
);

//前面的三个值再取中间值，clk3
//---------------------------------------------------
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

reg [3:0] Y_vs_r;
reg [3:0] Y_hs_r;
reg [3:0] Y_de_r;
reg [39:0] Y_x_r;
reg [39:0] Y_y_r;

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        Y_vs_r    <= 4'b0;
        Y_hs_r    <= 4'b0;
        Y_de_r    <= 4'b0;
        Y_x_r     <= 40'b0;
        Y_y_r     <= 40'b0;
    end
    else begin  
        Y_vs_r    <= {Y_vs_r[2:0],    in_vs};
        Y_hs_r    <= {Y_hs_r[2:0],    in_hs};
        Y_de_r    <= {Y_de_r[2:0],    in_de};
        Y_x_r     <= {Y_x_r[29:0],    in_x };
        Y_y_r     <= {Y_y_r[29:0],    in_y };
    end
end

reg [23:0] in_rgb_r0;
reg [23:0] in_rgb_r1;
reg [23:0] in_rgb_r2;
reg [23:0] in_rgb_r3;

always @(posedge clk or negedge rst_n)begin
	if(!rst_n)begin
		in_rgb_r0    <= 23'b0;
		in_rgb_r1    <= 23'b0;
		in_rgb_r2    <= 23'b0;
		in_rgb_r3    <= 23'b0;
	end
	else begin
		in_rgb_r0    <= in_rgb;
		in_rgb_r1    <= in_rgb_r0;
		in_rgb_r2    <= in_rgb_r1;
		in_rgb_r3    <= in_rgb_r2;
	end
end

assign out_vs    = Y_vs_r[3];
assign out_hs    = Y_hs_r[3];
assign out_de    = Y_de_r[3];
assign out_x     = Y_x_r[39:30];
assign out_y     = Y_y_r[39:30];
assign out_rgb   = in_rgb_r3;

endmodule