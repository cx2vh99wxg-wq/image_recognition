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

// 2026-11-03: share the neighborhood and pixel alignment pipeline.
binary_morphology #(.U_COL(U_COL), .U_ROW(U_ROW), .EROSION(1'b1)) u_filter (
    .clk(clk),
    .rst_n(rst_n),
    .in_vs(in_vs),
    .in_hs(in_hs),
    .in_de(in_de),
    .in_data(in_data),
    .in_rgb(in_rgb),
    .in_x(in_x),
    .in_y(in_y),
    .out_vs(out_vs),
    .out_hs(out_hs),
    .out_de(out_de),
    .out_data(out_data),
    .out_rgb(out_rgb),
    .out_x(out_x),
    .out_y(out_y)
);
endmodule


module isp_morphology_pair #(
    parameter U_COL = 640,
    parameter U_ROW = 480,
    parameter ERODE_FIRST = 1'b0
) (
    input clk, rst_n,
    input [47:0] pixel_in,
    output [47:0] pixel_out
);
    wire [47:0] stages [0:2];
    assign stages[0] = pixel_in;
    assign pixel_out = stages[2];
    genvar stage;
    generate
        for (stage = 0; stage < 2; stage = stage + 1) begin : morphology_stage
            binary_morphology #(
                .U_COL(U_COL), .U_ROW(U_ROW), .EROSION(stage == 0 ? ERODE_FIRST : !ERODE_FIRST)
            ) u_filter (
                .clk(clk), .rst_n(rst_n),
                .in_vs(stages[stage][47]), .in_hs(stages[stage][46]), .in_de(stages[stage][45]),
                .in_data(stages[stage][44]), .in_x(stages[stage][43:34]), .in_y(stages[stage][33:24]),
                .in_rgb(stages[stage][23:0]),
                .out_vs(stages[stage+1][47]), .out_hs(stages[stage+1][46]), .out_de(stages[stage+1][45]),
                .out_data(stages[stage+1][44]), .out_x(stages[stage+1][43:34]), .out_y(stages[stage+1][33:24]),
                .out_rgb(stages[stage+1][23:0])
            );
        end
    endgenerate
endmodule
