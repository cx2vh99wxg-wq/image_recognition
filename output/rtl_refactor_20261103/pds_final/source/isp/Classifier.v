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

localparam MAX_CONDITIONS = 39; // 支持最多n个条件（可扩展）
wire [2:0] color_en = {red_en , yellow_en , blue_en};

// 2026-11-03: keep both match keys and the result together in each rule.
reg [42:0] rules [0:MAX_CONDITIONS-1];
reg [15:0] choose_char_out;
// 初始化条件数组
always@(*) begin
	//限速标志
    //speed15
    rules[0] = {4'd1, 4'd5, 3'b100, 16'b11111111_11111111, 16'd1};
    //speed30
    rules[1] = {4'd3, 4'd0, 3'b100, 16'b11111111_11111111, 16'd2};
    //speed40
    rules[2] = {4'd4, 4'd0, 3'b100, 16'b11111111_11111111, 16'd3};
	 //speed50
    rules[3] = {4'd5, 4'd0, 3'b100, 16'b11111111_11111111, 16'd4};
    //speed60
    rules[4] = {4'd6, 4'd0, 3'b100, 16'b11111111_11111111, 16'd5};
	
	//指示标志
	//直行
	rules[5] = {4'd15, 4'd15, 3'b001, 16'd1, 16'd6};
	//左转
	rules[6] = {4'd15, 4'd15, 3'b001, 16'd2, 16'd7};
	//右转
	rules[7] = {4'd15, 4'd15, 3'b001, 16'd3, 16'd8};
	//直行或左转
	rules[8] = {4'd15, 4'd15, 3'b001, 16'd4, 16'd9};
	//直行或右转
	rules[9] = {4'd15, 4'd15, 3'b001, 16'd5, 16'd10};
	//停车位
	rules[10] = {4'd15, 4'd15, 3'b001, 16'd6, 16'd11};
	//人行道
	rules[11] = {4'd15, 4'd15, 3'b001, 16'd7, 16'd12};
	//环岛行驶
	rules[12] = {4'd15, 4'd15, 3'b001, 16'd8, 16'd13};
	//掉头
	rules[13] = {4'd15, 4'd15, 3'b001, 16'd9, 16'd14};
	//单行路
	rules[14] = {4'd15, 4'd15, 3'b001, 16'd10, 16'd15};
	
	//警告标志
	//交叉路口
	rules[15] = {4'd15, 4'd15, 3'b010, 16'd11, 16'd16};
	//双向交通
	rules[16] = {4'd15, 4'd15, 3'b010, 16'd12, 16'd17};
	//注意危险
	rules[17] = {4'd15, 4'd15, 3'b010, 16'd13, 16'd18};
	//注意行人
	rules[18] = {4'd15, 4'd15, 3'b010, 16'd14, 16'd19};
	//注意儿童
	rules[19] = {4'd15, 4'd15, 3'b010, 16'd15, 16'd20};
	//注意野生动物
	rules[20] = {4'd15, 4'd15, 3'b010, 16'd16, 16'd21};
	//注意牲畜
	rules[21] = {4'd15, 4'd15, 3'b010, 16'd17, 16'd22};
	//左急转
	rules[22] = {4'd15, 4'd15, 3'b010, 16'd18, 16'd23};
	//右急转
	rules[23] = {4'd15, 4'd15, 3'b010, 16'd19, 16'd24};
	
	//禁令标志
	//禁止驶入
	rules[24] = {4'd15, 4'd15, 3'b100, 16'd20, 16'd25};
	//禁止车辆停放
	rules[25] = {4'd15, 4'd15, 3'b100, 16'd21, 16'd26};
	//施工
	rules[26] = {4'd15, 4'd15, 3'b100, 16'd22, 16'd27};
	//停车让行
	rules[27] = {4'd15, 4'd15, 3'b100, 16'd23, 16'd28};
	//减速让行
	rules[28] = {4'd15, 4'd15, 3'b100, 16'd24, 16'd29};
	//禁止直行
	rules[29] = {4'd15, 4'd15, 3'b100, 16'd25, 16'd30};
	//禁止右转
	rules[30] = {4'd15, 4'd15, 3'b100, 16'd26, 16'd31};
	//禁止左右转
	rules[31] = {4'd15, 4'd15, 3'b100, 16'd27, 16'd32};
	//禁止掉头
	rules[32] = {4'd15, 4'd15, 3'b100, 16'd28, 16'd33};
	//禁止鸣笛
	rules[33] = {4'd15, 4'd15, 3'b100, 16'd29, 16'd34};
	//会车让行
	rules[34] = {4'd15, 4'd15, 3'b100, 16'd30, 16'd35};
	//speed70
	rules[35] = {4'd7, 4'd0, 3'b100, 16'b11111111_11111111, 16'd36};
	//speed80
	rules[36] = {4'd8, 4'd0, 3'b100, 16'b11111111_11111111, 16'd37};
	//speed20
	rules[37] = {4'd2, 4'd0, 3'b100, 16'b11111111_11111111, 16'd38};
	//禁止左转
	rules[38] = {4'd15, 4'd15, 3'b100, 16'd31, 16'd39};
end

// 提取十位和个位
wire [3:0] tens_digit = digit[7:4];
wire [3:0] ones_digit = digit[3:0];
reg [15:0] digit_reg;
reg digit_en;
// 输出逻辑
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
// 使能信号打拍
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