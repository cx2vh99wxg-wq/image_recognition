module dialate
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
binary_morphology #(.U_COL(U_COL), .U_ROW(U_ROW), .EROSION(1'b0)) u_filter (
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


module binary_morphology #(
    parameter U_COL = 640,
    parameter U_ROW = 480,
    parameter EROSION = 1'b0
) (
    input clk, rst_n,
    input in_vs, in_hs, in_de, in_data,
    input [23:0] in_rgb,
    input [9:0] in_x, in_y,
    output out_vs, out_hs, out_de, out_data,
    output [23:0] out_rgb,
    output [9:0] out_x, out_y
);
    wire [8:0] taps;
    reg [2:0] row_result;
    reg result;

    matrix_3x3_1bit #(.COL(U_COL), .ROW(U_ROW)) u_matrix (
        .clk(clk), .rst_n(rst_n), .valid_in(in_de), .din(in_data),
        .matrix_11(taps[0]), .matrix_12(taps[1]), .matrix_13(taps[2]),
        .matrix_21(taps[3]), .matrix_22(taps[4]), .matrix_23(taps[5]),
        .matrix_31(taps[6]), .matrix_32(taps[7]), .matrix_33(taps[8])
    );

    integer row;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            row_result <= 3'd0;
            result <= 1'b0;
        end else begin
            for (row = 0; row < 3; row = row + 1)
                row_result[row] <= EROSION ? (&taps[row*3 +: 3]) : (|taps[row*3 +: 3]);
            result <= EROSION ? (&row_result) : (|row_result);
        end
    end

    assign out_data = result;
    isp_pixel_delay #(.CYCLES(3)) u_metadata (
        .clk(clk), .rst_n(rst_n),
        .pixel_in({in_vs, in_hs, in_de, in_x, in_y, in_rgb}),
        .pixel_out({out_vs, out_hs, out_de, out_x, out_y, out_rgb})
    );
endmodule

module isp_pixel_delay #(parameter CYCLES = 3) (
    input clk, rst_n,
    input [46:0] pixel_in,
    output [46:0] pixel_out
);
    reg [47*CYCLES-1:0] history;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            history <= {47*CYCLES{1'b0}};
        else
            history <= {history[47*(CYCLES-1)-1:0], pixel_in};
    end
    assign pixel_out = history[47*CYCLES-1 -: 47];
endmodule
