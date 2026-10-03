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

reg [3:0] tens_cond [0:MAX_CONDITIONS-1];
reg [3:0] ones_cond [0:MAX_CONDITIONS-1];
reg [2:0] color_en_cond [0:MAX_CONDITIONS-1];
reg [15:0] code_cond [0:MAX_CONDITIONS-1];
reg [15:0] shape_en_cond[0:MAX_CONDITIONS-1];
reg [15:0] choose_char_out;
// 初始化条件数组
always@(*) begin
	//限速标志
    //speed15
    tens_cond[0] = 4'd1;  ones_cond[0] = 4'd5;  color_en_cond[0] = 3'b100; shape_en_cond[0] = 16'b11111111_11111111; code_cond[0] = 16'd1;
    //speed30
    tens_cond[1] = 4'd3;  ones_cond[1] = 4'd0;  color_en_cond[1] = 3'b100; shape_en_cond[1] = 16'b11111111_11111111; code_cond[1] = 16'd2;
    //speed40
    tens_cond[2] = 4'd4;  ones_cond[2] = 4'd0;  color_en_cond[2] = 3'b100; shape_en_cond[2] = 16'b11111111_11111111; code_cond[2] = 16'd3;
	 //speed50
    tens_cond[3] = 4'd5;  ones_cond[3] = 4'd0;  color_en_cond[3] = 3'b100; shape_en_cond[3] = 16'b11111111_11111111; code_cond[3] = 16'd4;
    //speed60
    tens_cond[4] = 4'd6;  ones_cond[4] = 4'd0;  color_en_cond[4] = 3'b100; shape_en_cond[4] = 16'b11111111_11111111; code_cond[4] = 16'd5;
	
	//指示标志
	//直行
	tens_cond[5] = 4'd15;  ones_cond[5] = 4'd15;  color_en_cond[5] = 3'b001; shape_en_cond[5] = 16'd1; code_cond[5] = 16'd6;
	//左转
	tens_cond[6] = 4'd15;  ones_cond[6] = 4'd15;  color_en_cond[6] = 3'b001; shape_en_cond[6] = 16'd2; code_cond[6] = 16'd7;
	//右转
	tens_cond[7] = 4'd15;  ones_cond[7] = 4'd15;  color_en_cond[7] = 3'b001; shape_en_cond[7] = 16'd3; code_cond[7] = 16'd8;
	//直行或左转
	tens_cond[8] = 4'd15;  ones_cond[8] = 4'd15;  color_en_cond[8] = 3'b001; shape_en_cond[8] = 16'd4; code_cond[8] = 16'd9;
	//直行或右转
	tens_cond[9] = 4'd15;  ones_cond[9] = 4'd15;  color_en_cond[9] = 3'b001; shape_en_cond[9] = 16'd5; code_cond[9] = 16'd10;
	//停车位
	tens_cond[10] = 4'd15;  ones_cond[10] = 4'd15;  color_en_cond[10] = 3'b001; shape_en_cond[10] = 16'd6; code_cond[10] = 16'd11;
	//人行道
	tens_cond[11] = 4'd15;  ones_cond[11] = 4'd15;  color_en_cond[11] = 3'b001; shape_en_cond[11] = 16'd7; code_cond[11] = 16'd12;
	//环岛行驶
	tens_cond[12] = 4'd15;  ones_cond[12] = 4'd15;  color_en_cond[12] = 3'b001; shape_en_cond[12] = 16'd8; code_cond[12] = 16'd13;
	//掉头
	tens_cond[13] = 4'd15;  ones_cond[13] = 4'd15;  color_en_cond[13] = 3'b001; shape_en_cond[13] = 16'd9; code_cond[13] = 16'd14;
	//单行路
	tens_cond[14] = 4'd15;  ones_cond[14] = 4'd15;  color_en_cond[14] = 3'b001; shape_en_cond[14] = 16'd10; code_cond[14] = 16'd15;
	
	//警告标志
	//交叉路口
	tens_cond[15] = 4'd15;  ones_cond[15] = 4'd15;  color_en_cond[15] = 3'b010; shape_en_cond[15] = 16'd11; code_cond[15] = 16'd16;
	//双向交通
	tens_cond[16] = 4'd15;  ones_cond[16] = 4'd15;  color_en_cond[16] = 3'b010; shape_en_cond[16] = 16'd12; code_cond[16] = 16'd17;
	//注意危险
	tens_cond[17] = 4'd15;  ones_cond[17] = 4'd15;  color_en_cond[17] = 3'b010; shape_en_cond[17] = 16'd13; code_cond[17] = 16'd18;
	//注意行人
	tens_cond[18] = 4'd15;  ones_cond[18] = 4'd15;  color_en_cond[18] = 3'b010; shape_en_cond[18] = 16'd14; code_cond[18] = 16'd19;
	//注意儿童
	tens_cond[19] = 4'd15;  ones_cond[19] = 4'd15;  color_en_cond[19] = 3'b010; shape_en_cond[19] = 16'd15; code_cond[19] = 16'd20;
	//注意野生动物
	tens_cond[20] = 4'd15;  ones_cond[20] = 4'd15;  color_en_cond[20] = 3'b010; shape_en_cond[20] = 16'd16; code_cond[20] = 16'd21;
	//注意牲畜
	tens_cond[21] = 4'd15;  ones_cond[21] = 4'd15;  color_en_cond[21] = 3'b010; shape_en_cond[21] = 16'd17; code_cond[21] = 16'd22;
	//左急转
	tens_cond[22] = 4'd15;  ones_cond[22] = 4'd15;  color_en_cond[22] = 3'b010; shape_en_cond[22] = 16'd18; code_cond[22] = 16'd23;
	//右急转
	tens_cond[23] = 4'd15;  ones_cond[23] = 4'd15;  color_en_cond[23] = 3'b010; shape_en_cond[23] = 16'd19; code_cond[23] = 16'd24;
	
	//禁令标志
	//禁止驶入
	tens_cond[24] = 4'd15;  ones_cond[24] = 4'd15;  color_en_cond[24] = 3'b100; shape_en_cond[24] = 16'd20; code_cond[24] = 16'd25;
	//禁止车辆停放
	tens_cond[25] = 4'd15;  ones_cond[25] = 4'd15;  color_en_cond[25] = 3'b100; shape_en_cond[25] = 16'd21; code_cond[25] = 16'd26;
	//施工
	tens_cond[26] = 4'd15;  ones_cond[26] = 4'd15;  color_en_cond[26] = 3'b100; shape_en_cond[26] = 16'd22; code_cond[26] = 16'd27;
	//停车让行
	tens_cond[27] = 4'd15;  ones_cond[27] = 4'd15;  color_en_cond[27] = 3'b100; shape_en_cond[27] = 16'd23; code_cond[27] = 16'd28;
	//减速让行
	tens_cond[28] = 4'd15;  ones_cond[28] = 4'd15;  color_en_cond[28] = 3'b100; shape_en_cond[28] = 16'd24; code_cond[28] = 16'd29;
	//禁止直行
	tens_cond[29] = 4'd15;  ones_cond[29] = 4'd15;  color_en_cond[29] = 3'b100; shape_en_cond[29] = 16'd25; code_cond[29] = 16'd30;
	//禁止右转
	tens_cond[30] = 4'd15;  ones_cond[30] = 4'd15;  color_en_cond[30] = 3'b100; shape_en_cond[30] = 16'd26; code_cond[30] = 16'd31;
	//禁止左右转
	tens_cond[31] = 4'd15;  ones_cond[31] = 4'd15;  color_en_cond[31] = 3'b100; shape_en_cond[31] = 16'd27; code_cond[31] = 16'd32;
	//禁止掉头
	tens_cond[32] = 4'd15;  ones_cond[32] = 4'd15;  color_en_cond[32] = 3'b100; shape_en_cond[32] = 16'd28; code_cond[32] = 16'd33;
	//禁止鸣笛
	tens_cond[33] = 4'd15;  ones_cond[33] = 4'd15;  color_en_cond[33] = 3'b100; shape_en_cond[33] = 16'd29; code_cond[33] = 16'd34;
	//会车让行
	tens_cond[34] = 4'd15;  ones_cond[34] = 4'd15;  color_en_cond[34] = 3'b100; shape_en_cond[34] = 16'd30; code_cond[34] = 16'd35;
	//speed70
	tens_cond[35] = 4'd7;  ones_cond[35] = 4'd0;  color_en_cond[35] = 3'b100; shape_en_cond[35] = 16'b11111111_11111111; code_cond[35] = 16'd36;
	//speed80
	tens_cond[36] = 4'd8;  ones_cond[36] = 4'd0;  color_en_cond[36] = 3'b100; shape_en_cond[36] = 16'b11111111_11111111; code_cond[36] = 16'd37;
	//speed20
	tens_cond[37] = 4'd2;  ones_cond[37] = 4'd0;  color_en_cond[37] = 3'b100; shape_en_cond[37] = 16'b11111111_11111111; code_cond[37] = 16'd38;
	//禁止左转
	tens_cond[38] = 4'd15;  ones_cond[38] = 4'd15;  color_en_cond[38] = 3'b100; shape_en_cond[38] = 16'd31; code_cond[38] = 16'd39;
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
			if(color_en_cond[i] == color_en && tens_cond[i] == tens_digit && ones_cond[i] == ones_digit)begin
				digit_reg <= code_cond[i];
				digit_en <= 1'd1;
			end
            else if(color_en_cond[i] == color_en && shape_en_cond[i] == shape_en)
                choose_char_out <= code_cond[i];
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