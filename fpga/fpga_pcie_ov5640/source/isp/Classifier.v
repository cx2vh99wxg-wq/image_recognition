// 2026-11-03 注释校订。按颜色、数字和形状匹配规则。数字结果优先，重复匹配沿用最后命中的规则。
module Classifier(
    input   wire        clk,
    input   wire        rst_n,
    input   wire        red_en,
    input   wire        blue_en,
    input   wire        yellow_en,
    input   wire [7:0]  digit,
	input   wire [15:0]	shape_en,

    output  wire        o_red_en,
    output  wire        o_blue_en,
    output  wire        o_yellow_en,
    output  wire  [15:0]  char_en
);

localparam MAX_CONDITIONS = 39;
wire [2:0] color_en = {red_en , yellow_en , blue_en};

reg [42:0] rules [0:MAX_CONDITIONS-1];
reg [15:0] choose_char_out;

always@(*) begin

    rules[0] = {4'd1, 4'd5, 3'b100, 16'b11111111_11111111, 16'd1};

    rules[1] = {4'd3, 4'd0, 3'b100, 16'b11111111_11111111, 16'd2};

    rules[2] = {4'd4, 4'd0, 3'b100, 16'b11111111_11111111, 16'd3};

    rules[3] = {4'd5, 4'd0, 3'b100, 16'b11111111_11111111, 16'd4};

    rules[4] = {4'd6, 4'd0, 3'b100, 16'b11111111_11111111, 16'd5};

	rules[5] = {4'd15, 4'd15, 3'b001, 16'd1, 16'd6};

	rules[6] = {4'd15, 4'd15, 3'b001, 16'd2, 16'd7};

	rules[7] = {4'd15, 4'd15, 3'b001, 16'd3, 16'd8};

	rules[8] = {4'd15, 4'd15, 3'b001, 16'd4, 16'd9};

	rules[9] = {4'd15, 4'd15, 3'b001, 16'd5, 16'd10};

	rules[10] = {4'd15, 4'd15, 3'b001, 16'd6, 16'd11};

	rules[11] = {4'd15, 4'd15, 3'b001, 16'd7, 16'd12};

	rules[12] = {4'd15, 4'd15, 3'b001, 16'd8, 16'd13};

	rules[13] = {4'd15, 4'd15, 3'b001, 16'd9, 16'd14};

	rules[14] = {4'd15, 4'd15, 3'b001, 16'd10, 16'd15};

	rules[15] = {4'd15, 4'd15, 3'b010, 16'd11, 16'd16};

	rules[16] = {4'd15, 4'd15, 3'b010, 16'd12, 16'd17};

	rules[17] = {4'd15, 4'd15, 3'b010, 16'd13, 16'd18};

	rules[18] = {4'd15, 4'd15, 3'b010, 16'd14, 16'd19};

	rules[19] = {4'd15, 4'd15, 3'b010, 16'd15, 16'd20};

	rules[20] = {4'd15, 4'd15, 3'b010, 16'd16, 16'd21};

	rules[21] = {4'd15, 4'd15, 3'b010, 16'd17, 16'd22};

	rules[22] = {4'd15, 4'd15, 3'b010, 16'd18, 16'd23};

	rules[23] = {4'd15, 4'd15, 3'b010, 16'd19, 16'd24};

	rules[24] = {4'd15, 4'd15, 3'b100, 16'd20, 16'd25};

	rules[25] = {4'd15, 4'd15, 3'b100, 16'd21, 16'd26};

	rules[26] = {4'd15, 4'd15, 3'b100, 16'd22, 16'd27};

	rules[27] = {4'd15, 4'd15, 3'b100, 16'd23, 16'd28};

	rules[28] = {4'd15, 4'd15, 3'b100, 16'd24, 16'd29};

	rules[29] = {4'd15, 4'd15, 3'b100, 16'd25, 16'd30};

	rules[30] = {4'd15, 4'd15, 3'b100, 16'd26, 16'd31};

	rules[31] = {4'd15, 4'd15, 3'b100, 16'd27, 16'd32};

	rules[32] = {4'd15, 4'd15, 3'b100, 16'd28, 16'd33};

	rules[33] = {4'd15, 4'd15, 3'b100, 16'd29, 16'd34};

	rules[34] = {4'd15, 4'd15, 3'b100, 16'd30, 16'd35};

	rules[35] = {4'd7, 4'd0, 3'b100, 16'b11111111_11111111, 16'd36};

	rules[36] = {4'd8, 4'd0, 3'b100, 16'b11111111_11111111, 16'd37};

	rules[37] = {4'd2, 4'd0, 3'b100, 16'b11111111_11111111, 16'd38};

	rules[38] = {4'd15, 4'd15, 3'b100, 16'd31, 16'd39};
end

wire [3:0] tens_digit = digit[7:4];
wire [3:0] ones_digit = digit[3:0];
reg [15:0] digit_reg;
reg digit_en;

integer i;
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        choose_char_out <= 16'd0;
		digit_en <= 1'd0;
		digit_reg <= 16'd0;
    end else begin
        choose_char_out <= 16'd0;
		digit_reg <= 16'd0;
		digit_en <= 1'd0;
        for (i = 0; i < MAX_CONDITIONS; i = i + 1) begin
			if(rules[i][42:32] == {tens_digit, ones_digit, color_en})begin
				digit_reg <= rules[i][15:0];
				digit_en <= 1'd1;
			end
            else if(rules[i][34:16] == {color_en, shape_en})
                choose_char_out <= rules[i][15:0];
        end
    end
end

assign char_en = (digit_en == 1)?digit_reg:choose_char_out;

reg red_en_r, blue_en_r, yellow_en_r;
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        red_en_r    <= 0;
        blue_en_r   <= 0;
        yellow_en_r <= 0;
    end else begin
        red_en_r    <= red_en;
        blue_en_r   <= blue_en;
        yellow_en_r <= yellow_en;
    end
end

assign o_red_en    = red_en_r;
assign o_blue_en   = blue_en_r;
assign o_yellow_en = yellow_en_r;

endmodule