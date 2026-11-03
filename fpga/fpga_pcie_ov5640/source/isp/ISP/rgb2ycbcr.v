// 2026-11-03 注释校订。按可配置系数转换 RGB。像素、坐标和同步标志通过三级无复位侧带流水线。
`timescale 1ns / 1ps

module	rgb2ycbcr(
    input                            pixelclk   ,
	input                            rst_n      ,
    input          [23:0]            i_rgb      ,
    input                            i_hsync    ,
    input                            i_vsync    ,
    input                            i_de       ,
    input                            i_de0      ,
	input			[9:0]			x,
	input			[9:0]			y,

    output            [23:0]         o_rgb      ,
	output			   [9:0]		o_x,
	output				[9:0]		o_y,
    output            [23:0]         o_ycbcr    ,
    output                           o_hsync    ,
    output                           o_vsync    ,
    output                           o_de0      ,
    output                           o_de
);

parameter    para_0183_10b = 10'd47     ;
parameter    para_0614_10b = 10'd157    ;
parameter    para_0062_10b = 10'd16     ;
parameter    para_0101_10b = 10'd26     ;
parameter    para_0338_10b = 10'd86     ;
parameter    para_0439_10b = 10'd112    ;
parameter    para_0399_10b = 10'd102    ;
parameter    para_0040_10b = 10'd10     ;
parameter    para_16_18b   = 18'd4096   ;
parameter    para_128_18b  = 18'd32768  ;

wire [7 : 0]        i_r_8b                  ;
wire [7 : 0]        i_g_8b                  ;
wire [7 : 0]        i_b_8b                  ;
wire [7 : 0]        o_y_8b                  ;
wire [7 : 0]        o_cb_8b                 ;
wire [7 : 0]        o_cr_8b                 ;
reg  [17: 0]        mult_r_for_y_18b        ;
reg  [17: 0]        mult_r_for_cb_18b       ;
reg  [17: 0]        mult_r_for_cr_18b       ;
reg  [17: 0]        mult_g_for_y_18b        ;
reg  [17: 0]        mult_g_for_cb_18b       ;
reg  [17: 0]        mult_g_for_cr_18b       ;
reg  [17: 0]        mult_b_for_y_18b        ;
reg  [17: 0]        mult_b_for_cb_18b       ;
reg  [17: 0]        mult_b_for_cr_18b       ;
reg  [17: 0]        add_y_0_18b             ;
reg  [17: 0]        add_cb_0_18b            ;
reg  [17: 0]        add_cr_0_18b            ;
reg  [17: 0]        add_y_1_18b             ;
reg  [17: 0]        add_cb_1_18b            ;
reg  [17: 0]        add_cr_1_18b            ;
reg  [17: 0]        result_y_18b            ;
reg  [17: 0]        result_cb_18b           ;
reg  [17: 0]        result_cr_18b           ;
reg  [15:0]         y_tmp                   ;
reg  [15:0]         cb_tmp                  ;
reg  [15:0]         cr_tmp                  ;
assign i_r_8b  = i_rgb[23:16];
assign i_g_8b  = i_rgb[15:8];
assign i_b_8b  = i_rgb[7:0];
assign o_ycbcr = {o_y_8b,o_cb_8b,o_cr_8b};
assign o_y_8b  = y_tmp[15 : 8];
assign o_cb_8b =cb_tmp[15 : 8];
assign o_cr_8b =cr_tmp[15 : 8];

always @(posedge   pixelclk)begin
    mult_r_for_y_18b  <= i_r_8b * para_0183_10b;
    mult_r_for_cb_18b <= i_r_8b * para_0101_10b;
    mult_r_for_cr_18b <= i_r_8b * para_0439_10b;
end

always @(posedge   pixelclk)begin
    mult_g_for_y_18b  <= i_g_8b * para_0614_10b;
    mult_g_for_cb_18b <= i_g_8b * para_0338_10b;
    mult_g_for_cr_18b <= i_g_8b * para_0399_10b;
end

always @(posedge   pixelclk)begin
    mult_b_for_y_18b  <= i_b_8b * para_0062_10b;
    mult_b_for_cb_18b <= i_b_8b * para_0439_10b;
    mult_b_for_cr_18b <= i_b_8b * para_0040_10b;
end

always @(posedge   pixelclk)begin
    add_y_0_18b  <= mult_r_for_y_18b  + mult_g_for_y_18b;
    add_y_1_18b  <= mult_b_for_y_18b  + para_16_18b;

    add_cb_0_18b <= mult_b_for_cb_18b + para_128_18b;
    add_cb_1_18b <= mult_r_for_cb_18b + mult_g_for_cb_18b;

    add_cr_0_18b <= mult_r_for_cr_18b + para_128_18b;
    add_cr_1_18b <= mult_g_for_cr_18b + mult_b_for_cr_18b;
end

always @(posedge   pixelclk)begin
    y_tmp  <= add_y_0_18b  + add_y_1_18b;
    cb_tmp <= add_cb_0_18b - add_cb_1_18b ;
    cr_tmp <= add_cr_0_18b - add_cr_1_18b ;
end

reg [143:0] video_history;
always @(posedge pixelclk)
    video_history <= {video_history[95:0], i_hsync, i_vsync, i_de, i_de0, x, y, i_rgb};
assign {o_hsync, o_vsync, o_de, o_de0, o_x, o_y, o_rgb} = video_history[143:96];

endmodule
