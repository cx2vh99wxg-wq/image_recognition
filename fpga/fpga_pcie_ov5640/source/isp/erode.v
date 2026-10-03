module erode
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
	input in_data,
    input [23:0] in_rgb,

	input [9:0] in_x,
	input [9:0] in_y,

	output out_vs,
    output out_hs,	
	output out_de,
	output out_data,
    output [23:0] out_rgb,
	
	output [9:0] out_x,
	output [9:0] out_y

);

wire erode_matrix_11 ;
wire erode_matrix_12 ;
wire erode_matrix_13 ;
wire erode_matrix_21 ;
wire erode_matrix_22 ;
wire erode_matrix_23 ;
wire erode_matrix_31 ;
wire erode_matrix_32 ;
wire erode_matrix_33 ;

reg erode_1;
reg erode_2;
reg erode_3;
reg erode;

matrix_3x3_1bit //delay 1 clk
#(
    .COL                    (U_COL),
    .ROW                    (U_ROW)
)
matrix_3x3_erode
(
    .clk                    (clk     	 	),
    .rst_n                  (rst_n    		),
    .valid_in               (in_de  		),
    .din                    (in_data		),
    .matrix_11              (erode_matrix_11),
    .matrix_12              (erode_matrix_12),
    .matrix_13              (erode_matrix_13),
    .matrix_21              (erode_matrix_21),
    .matrix_22              (erode_matrix_22),
    .matrix_23              (erode_matrix_23),
    .matrix_31              (erode_matrix_31),
    .matrix_32              (erode_matrix_32),
    .matrix_33              (erode_matrix_33)
);

always @ (posedge clk or negedge rst_n)begin
    if(!rst_n)begin
        erode_1 <= 'd0;
        erode_2 <= 'd0;
        erode_3 <= 'd0;
    end
    else begin
        erode_1 <= erode_matrix_11 && erode_matrix_12 && erode_matrix_13;
        erode_2 <= erode_matrix_21 && erode_matrix_22 && erode_matrix_23;
        erode_3 <= erode_matrix_31 && erode_matrix_32 && erode_matrix_33;
    end
end

always @(posedge clk or negedge rst_n)begin
    if(!rst_n)begin
        erode <= 'd0;
    end
    else begin
        erode <= erode_1 && erode_2 && erode_3;
    end
end

assign out_data = erode ? 1'b1 : 1'b0;

reg [2:0] diff_vs_r;
reg [2:0] diff_hs_r;
reg [2:0] diff_de_r;
reg [29:0] diff_x_r; 
reg [29:0] diff_y_r;

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        diff_vs_r    <= 3'b0;
        diff_hs_r    <= 3'b0;
        diff_de_r    <= 3'b0;
        diff_x_r     <= 30'b0;
        diff_y_r     <= 30'b0;
    end
    else begin 
        diff_vs_r    <= {diff_vs_r[1:0],    in_vs};
        diff_hs_r    <= {diff_hs_r[1:0],    in_hs}; 
        diff_de_r    <= {diff_de_r[1:0],    in_de};
        diff_x_r     <= {diff_x_r[19:0],    in_x };
        diff_y_r     <= {diff_y_r[19:0],    in_y };
    end
end

reg [23:0] in_rgb_r0;
reg [23:0] in_rgb_r1;
reg [23:0] in_rgb_r2;

always @(posedge clk or negedge rst_n)begin
	if(!rst_n)begin
		in_rgb_r0    <= 23'b0;
		in_rgb_r1    <= 23'b0;
		in_rgb_r2    <= 23'b0;
	end
	else begin
		in_rgb_r0    <= in_rgb;
		in_rgb_r1    <= in_rgb_r0;
		in_rgb_r2    <= in_rgb_r1;
	end
end

assign out_vs    = diff_vs_r[2];
assign out_hs    = diff_hs_r[2];
assign out_de    = diff_de_r[2];
assign out_x     = diff_x_r[29:20];
assign out_y     = diff_y_r[29:20];
assign out_rgb   = in_rgb_r2;

endmodule