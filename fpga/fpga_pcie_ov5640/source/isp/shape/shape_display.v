//****************************************Copyright (c)***********************************//
//原子哥在线教学平台：www.yuanzige.com
//技术支持：www.openedv.com
//淘宝店铺：http://openedv.taobao.com
//关注微信公众平台微信号："正点原子"，免费获取ZYNQ & FPGA & STM32 & LINUX资料。
//版权所有，盗版必究。
//Copyright(C) 正点原子 2018-2028
//All rights reserved
//----------------------------------------------------------------------------------------
// File name:           digital_recognition
// Last modified Date:  2020/05/04 9:19:08
// Last Version:        V1.0
// Descriptions:        数字特征识别模块
//                      
//----------------------------------------------------------------------------------------
// Created by:          正点原子
// Created date:        2019/05/04 9:19:08
// Version:             V1.0
// Descriptions:        The original version
//
//----------------------------------------------------------------------------------------
//****************************************************************************************//

module shape_display #(
    parameter NUM_ROW =  1 ,
    parameter NUM_COL =  2 ,
    parameter NUM_WIDTH = (NUM_ROW*NUM_COL<<2)-1
)(
    //module clock
    input                    clk              ,  //时钟信号
    input                    rst_n            ,  //复位信号（低有效）

    //image data interface
    input      [9:0]        xpos             ,  //横坐标
    input      [9:0]        ypos             ,  //纵坐标
	input	   [7:0]	    i_rgb		,
	input					i_vsync		,	
	input					i_hsync		,	
	input					i_de		,	

	input					en_r		,
	input					en_b		,
	input					en_y		,
    

    //project border ram interface
    input      [10:0]        row_border_data  ,  //行边界ram读数据
    output reg [10:0]        row_border_addr  ,  //行边界ram读地址
    input      [10:0]        col_border_data  ,  //列边界ram读数据
    output reg [10:0]        col_border_addr  ,  //列边界ram读地址
	
	output					o_vs		,	
	output					o_hs		,	
	output					o_de		,	
	output  [7:0]           color_rgb   ,  //输出图像数据
	output  [9:0]			o_x			,
	output	[9:0]			o_y			,
	output	[10:0]     		o_top		,
	output	[10:0]     		o_bottom	,
	output	[10:0]			o_left		,
	output	[10:0]     		o_right		,
    //user interface
    input      [ 1:0]        frame_cnt        ,  //当前帧
    input                    project_done_flag,  //投影完成标志
    input      [ 3:0]        num_col          ,  //采集到的数字列数
    input      [ 3:0]        num_row          ,  //采集到的数字行数
	output     [15:0]		 o_shape
);

//localparam define
localparam NUM_TOTAL = NUM_ROW * NUM_COL - 1'b1; // 需识别的数字共个数，始于0

//reg define
reg  [10:0]        col_border_l                    ;  //左边界
reg  [10:0]        col_border_r                    ;  //右边界
reg  [10:0]        row_border_low                  ;  //下边界
reg  [10:0]        row_border_high                 ;  //上边界
reg  [16:0]        row_border_low_t                ;
reg  [16:0]        row_border_high_t               ;  

reg                row_area [NUM_ROW - 1'b1:0]     ;  // 行区域
reg                col_area [NUM_TOTAL     :0]     ;  // 列区域
reg  [ 3:0]        row_cnt,row_cnt_t               ;  //数字列计数
reg  [ 3:0]        col_cnt,col_cnt_t               ;  //数字行计数
reg  [11:0]        cent_y_t                        ;

reg                row_d0,row_d1                   ;
reg                col_d0,col_d1                   ;
reg                row_chg_d0,row_chg_d1,row_chg_d2;
reg                row_chg_d3                      ;
reg                col_chg_d0,col_chg_d1,col_chg_d2;
reg  [ 7:0]        real_num_total                  ;  //被测数字总数


reg  [10:0]        cent_y                          ;  //被测数字的中间横坐标

//wire define
wire        y_flag_fall ;
wire        col_chg     ;
wire        row_chg     ;
wire        feature_deal;  //数字特征检测有效信号              

//*****************************************************
//**                    main code
//*****************************************************
assign row_chg = row_d0 ^ row_d1;
assign col_chg = col_d0 ^ col_d1;
assign feature_deal = project_done_flag && frame_cnt == 2'd2; // 处理特征
assign o_top = row_border_high;
assign o_bottom = row_border_low;
assign o_left = col_border_l;
assign o_right = col_border_r;
//实际采集到的数字总数
always @(*) begin
    if(project_done_flag)
        real_num_total = num_col * num_row;
end


//检测行变化
always @(posedge clk) begin
    if(project_done_flag) begin
        row_cnt_t <= row_cnt;
        row_d1    <= row_d0 ;
        if(row_cnt_t != row_cnt)
            row_d0 <= ~row_d0;
    end
    else begin
        row_d0 <= 1'b1;
        row_d1 <= 1'b1;
        row_cnt_t <= 4'hf;
    end
end

//获取数字的行边界
always @(posedge clk) begin
    if(row_chg)
        row_border_addr <= (row_cnt << 1'b1) + 1'b1;
    else
        row_border_addr <= row_cnt << 1'b1;
end

always @(posedge clk) begin
    if(row_border_addr[0])
        row_border_low <= row_border_data;
    else
        row_border_high <= row_border_data;
end

always @(posedge clk) begin
    row_chg_d0 <= row_chg;
    row_chg_d1 <= row_chg_d0;
    row_chg_d2 <= row_chg_d1;
    row_chg_d3 <= row_chg_d2;
end

//检测列变化
always @(posedge clk) begin
    if(project_done_flag) begin
        col_cnt_t <= col_cnt;
        col_d1    <= col_d0;
        if(col_cnt_t != col_cnt)
            col_d0 <= ~col_d0;
    end
    else begin
        col_d0 <= 1'b1;
        col_d1 <= 1'b1;
        col_cnt_t <= 4'hf;
    end
end

//获取单个数字的列边界
always @(posedge clk) begin
    if(col_chg)
        col_border_addr <= (col_cnt << 1'b1) + 1'b1;
    else
        col_border_addr <= col_cnt << 1'b1;
end

always @(posedge clk) begin
    if(col_border_addr[0])
        col_border_r <= col_border_data;
    else
        col_border_l <= col_border_data;
end

always @(posedge clk) begin
    col_chg_d0 <= col_chg;
    col_chg_d1 <= col_chg_d0;
    col_chg_d2 <= col_chg_d1;
end


//行区域
always @(*) begin
    row_area[row_cnt] = ypos >= row_border_high && ypos <= row_border_low;
end

//列区域
always @(*) begin
    col_area[col_cnt] = xpos >= col_border_l   && xpos <= col_border_r;
end
//确定col_cnt
always @(posedge clk) begin
    if(project_done_flag) begin
        if(row_area[row_cnt] && xpos == col_border_r)
            col_cnt <= col_cnt == num_col - 1'b1 ? 'd0 : col_cnt + 1'b1;
    end
    else
        col_cnt <= 4'd0;
end

//确定row_cnt
always @(posedge clk) begin
    if(project_done_flag) begin
        if(ypos == row_border_low + 1'b1)
            row_cnt <= row_cnt == num_row - 1'b1 ? 'd0 : row_cnt + 1'b1;
    end
    else
        row_cnt <= 12'd0;
end


reg [7:0] rgb_r;
reg vs_r;
reg hs_r;
reg de_r;
reg [9:0]x_d;
reg	[9:0]y_d;
//输出边界和图像
always @(posedge clk or negedge rst_n) begin
    if(!rst_n)
        rgb_r <= 8'h0;
	else if(row_area[row_cnt] && ( xpos == col_border_l|| xpos == col_border_r ||
        xpos == (col_border_l - 1) || xpos == (col_border_r+1)))
		rgb_r <= 8'h0; //左右竖直边界线
	else if(col_area[col_cnt] && (ypos == row_border_high || ypos== row_border_low ||
        ypos==( row_border_high - 1) || ypos== (row_border_low + 1)))
		rgb_r <= 8'h0; //上下水平边界线
	else 
		rgb_r <= i_rgb;	
end

//匹配
wire [10:0] high;
wire [10:0] width;
wire [10:0] mid_w;
wire [10:0] mid_h;

assign high  = row_border_low - row_border_high + 1;
assign width = col_border_r   - col_border_l    + 1;
assign mid_w = (col_border_l     + col_border_r  ) >> 1;
assign mid_h = (row_border_high  + row_border_low) >> 1;

wire [10:0] mid_wl;
wire [10:0] mid_wr;
wire [10:0] mid_hh;
wire [10:0] mid_hl;

assign mid_wl = (col_border_l + mid_w) >> 1;
assign mid_wr = (mid_w + col_border_r) >> 1;

assign mid_hh = (row_border_high + mid_h) >> 1;
assign mid_hl = (mid_h +  row_border_low) >> 1;

wire [10:0] mid_wwz;
wire [10:0] mid_wwf;
wire [10:0] mid_hhz;
wire [10:0] mid_hhf;

assign mid_wwz = mid_w + (width >> 3);
assign mid_wwf = mid_w - (width >> 3);

assign mid_hhz = mid_h + (high >> 3);
assign mid_hhf = mid_h - (high >> 3);

wire [10:0] mid_wwz16;
wire [10:0] mid_wwf16;
assign mid_wwz16 = mid_w + (width >> 4);
assign mid_wwf16 = mid_w - (width >> 4);

reg [10:0] cnt_x0;
reg [10:0] cnt_x1;
reg [10:0] cnt_x2;
reg [10:0] cnt_x3;
reg [10:0] cnt_x4;
reg [10:0] cnt_x5;
reg [10:0] cnt_x6;
reg [10:0] cnt_x7;
reg [10:0] cnt_x8;

reg [10:0] cnt_y0;
reg [10:0] cnt_y1;
reg [10:0] cnt_y2;
reg [10:0] cnt_y3;
reg [10:0] cnt_y4;
reg [10:0] cnt_y5;
reg [10:0] cnt_y6;

wire pos_vs;

assign  pos_vs = i_vsync && (~vs_r);

always @(posedge clk or negedge rst_n) begin
	if(!rst_n) begin
		cnt_x0 <= 11'b0;
		cnt_x1 <= 11'b0;
		cnt_x2 <= 11'b0;
		cnt_x3 <= 11'b0;
		cnt_x4 <= 11'b0;
		cnt_x5 <= 11'b0;
		cnt_x6 <= 11'b0;
		cnt_x7 <= 11'b0;
		cnt_x8 <= 11'b0;	
		
		cnt_y0 <= 11'b0;
		cnt_y1 <= 11'b0;
		cnt_y2 <= 11'b0;
		cnt_y3 <= 11'b0;
		cnt_y4 <= 11'b0;
		cnt_y5 <= 11'b0;
		cnt_y6 <= 11'b0;
	end
	else if((row_area[row_cnt]) && (xpos == (col_border_l+2)) && (i_rgb == 0)) begin
		cnt_x0 <= cnt_x0 + 1;
	end
	else if((row_area[row_cnt]) && (xpos == mid_wl) && (i_rgb == 0)) begin
		cnt_x1 <= cnt_x1 + 1;
	end
	else if((row_area[row_cnt]) && (xpos == mid_wwf) && (i_rgb == 0)) begin
		cnt_x2 <= cnt_x2 + 1;
	end
	else if((row_area[row_cnt]) && (xpos == mid_wwf16) && (i_rgb == 0)) begin
		cnt_x3 <= cnt_x3 + 1;
	end	
	else if((row_area[row_cnt]) && (xpos == mid_w) && (i_rgb == 0)) begin
		cnt_x4 <= cnt_x4 + 1;
	end
	else if((row_area[row_cnt]) && (xpos == mid_wwz16) && (i_rgb == 0)) begin
		cnt_x5 <= cnt_x5 + 1;
	end	
	else if((row_area[row_cnt]) && (xpos == mid_wwz) && (i_rgb == 0)) begin
		cnt_x6 <= cnt_x6 + 1;
	end
	else if((row_area[row_cnt]) && (xpos == mid_wr) && (i_rgb == 0)) begin
		cnt_x7 <= cnt_x7 + 1;
	end
	else if((row_area[row_cnt]) && (xpos == (col_border_r-2)) && (i_rgb == 0)) begin
		cnt_x8 <= cnt_x8 + 1;
	end
	else if((col_area[col_cnt]) && (ypos == (row_border_high+2)) && (i_rgb == 0)) begin
		cnt_y0 <= cnt_y0 + 1;
	end
	else if((col_area[col_cnt]) && (ypos == mid_hh) && (i_rgb == 0)) begin
		cnt_y1 <= cnt_y1 + 1;
	end
	else if((col_area[col_cnt]) && (ypos == mid_hhf) && (i_rgb == 0)) begin
		cnt_y2 <= cnt_y2 + 1;
	end
	else if((col_area[col_cnt]) && (ypos == mid_h) && (i_rgb == 0)) begin
		cnt_y3 <= cnt_y3 + 1;
	end
	else if((col_area[col_cnt]) && (ypos == mid_hhz) && (i_rgb == 0)) begin
		cnt_y4 <= cnt_y4 + 1;
	end
	else if((col_area[col_cnt]) && (ypos == mid_hl) && (i_rgb == 0)) begin
		cnt_y5 <= cnt_y5 + 1;
	end
	else if((col_area[col_cnt]) && (ypos == row_border_low-2) && (i_rgb == 0)) begin
		cnt_y6 <= cnt_y6 + 1;
	end
	else if(pos_vs) begin
		cnt_x0 <= 11'b0;
		cnt_x1 <= 11'b0;
		cnt_x2 <= 11'b0;
		cnt_x3 <= 11'b0;
		cnt_x4 <= 11'b0;
		cnt_x5 <= 11'b0;
		cnt_x6 <= 11'b0;
		cnt_x7 <= 11'b0;
		cnt_x8 <= 11'b0;
		
		cnt_y0 <= 11'b0;
		cnt_y1 <= 11'b0;
		cnt_y2 <= 11'b0;
		cnt_y3 <= 11'b0;
		cnt_y4 <= 11'b0;
		cnt_y5 <= 11'b0;
		cnt_y6 <= 11'b0;
	end
	else begin
		cnt_x0 <= cnt_x0;
		cnt_x1 <= cnt_x1;
		cnt_x2 <= cnt_x2;
		cnt_x3 <= cnt_x3;
		cnt_x4 <= cnt_x4;
		cnt_x5 <= cnt_x5;
		cnt_x6 <= cnt_x6;	
		cnt_x7 <= cnt_x7;
		cnt_x8 <= cnt_x8;
		
		cnt_y0 <= cnt_y0;
		cnt_y1 <= cnt_y1;
		cnt_y2 <= cnt_y2;
		cnt_y3 <= cnt_y3;
		cnt_y4 <= cnt_y4;
		cnt_y5 <= cnt_y5;
		cnt_y6 <= cnt_y6;
	end
end

wire x_0;
wire x_1;
wire x_2;
wire x_3;
wire x_4;
wire x_5;
wire x_6;
wire x_7;
wire x_8;

wire y_0;
wire y_1;
wire y_2;
wire y_3;
wire y_4;
wire y_5;
wire y_6;

assign x_0 = (cnt_x0 >= (high >> 1)) ? 1 : 0;
assign x_1 = (cnt_x1 >= (high >> 1)) ? 1 : 0;
assign x_2 = (cnt_x2 >= (high >> 1)) ? 1 : 0;
assign x_3 = (cnt_x3 >= (high >> 1)) ? 1 : 0;
assign x_4 = (cnt_x4 >= (high >> 1)) ? 1 : 0;
assign x_5 = (cnt_x5 >= (high >> 1)) ? 1 : 0;
assign x_6 = (cnt_x6 >= (high >> 1)) ? 1 : 0;
assign x_7 = (cnt_x7 >= (high >> 1)) ? 1 : 0;
assign x_8 = (cnt_x8 >= (high >> 1)) ? 1 : 0;

assign y_0 = (cnt_y0 >= (width >> 1)) ? 1 : 0;
assign y_1 = (cnt_y1 >= (width >> 1)) ? 1 : 0;
assign y_2 = (cnt_y2 >= (width >> 1)) ? 1 : 0;
assign y_3 = (cnt_y3 >= (width >> 1)) ? 1 : 0;
assign y_4 = (cnt_y4 >= (width >> 1)) ? 1 : 0;
assign y_5 = (cnt_y5 >= (width >> 1)) ? 1 : 0;
assign y_6 = (cnt_y6 >= (width >> 1)) ? 1 : 0;

reg [15:0] shape;
reg [15:0] shape_r;
reg f_rst = 1;

always @(posedge clk or negedge rst_n) begin
	if(!rst_n) begin
		shape <= 0;
		f_rst <= 0;
	end
	else if (f_rst) begin
		shape <= 0;
		f_rst <= 0;
	end
	else begin
	case ({en_r, en_b, en_y, x_0, x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8, y_0, y_1, y_2, y_3, y_4, y_5, y_6})
		19'b010_011101110_0111110: shape <= 1; //直行
		19'b010_011100110_0111110: shape <= 1; //直行
		19'b010_011111010_0101110: shape <= 2; //左转
		19'b010_010111110_0101110: shape <= 3; //右转
		19'b010_011110010_0111010: shape <= 4; //直行和左转
		19'b010_010011110_0111010: shape <= 5; //直行和右转
		19'b010_010111110_1011111: shape <= 6; //停车位
		19'b010_010111111_1011111: shape <= 6; //停车位
		19'b010_110111110_1011111: shape <= 6; //停车位	
		19'b010_110111111_1011111: shape <= 6; //停车位
		19'b010_110011110_1011111: shape <= 6; //停车位	
		19'b010_110011111_1011111: shape <= 6; //停车位
		19'b010_010111110_0011110: shape <= 6; //停车位
		19'b010_010111111_0011110: shape <= 6; //停车位
		19'b010_110111110_0011110: shape <= 6; //停车位	
		19'b010_110111111_0011110: shape <= 6; //停车位
		19'b010_110011110_0011110: shape <= 6; //停车位	
		19'b010_110011111_0011110: shape <= 6; //停车位
		19'b010_011000110_1111001: shape <= 7; //人行道
		19'b010_011000111_1111001: shape <= 7; //人行道
		19'b010_111000110_1111001: shape <= 7; //人行道
		19'b010_111000111_1111001: shape <= 7; //人行道	
		19'b010_010000010_1111001: shape <= 7; //人行道
		19'b010_011000110_0111010: shape <= 7; //人行道
		19'b010_011000111_0111010: shape <= 7; //人行道
		19'b010_111000110_0111010: shape <= 7; //人行道
		19'b010_111000111_0111010: shape <= 7; //人行道	
		19'b010_010000010_0111010: shape <= 7; //人行道
		19'b010_011111110_0111110: shape <= 8; //环岛行驶
		19'b010_010111010_1111111: shape <= 9; //掉头
		19'b010_010111011_1111111: shape <= 9; //掉头
		19'b010_110111010_1111111: shape <= 9; //掉头
		19'b010_110111011_1111111: shape <= 9; //掉头
		19'b010_010000010_1001111: shape <= 10; //单行路
		19'b010_010000011_1001111: shape <= 10; //单行路
		19'b010_110000010_1001111: shape <= 10; //单行路
		19'b010_110000011_1001111: shape <= 10; //单行路
		19'b010_111000011_1001111: shape <= 10; //单行路
		19'b010_011000011_1001111: shape <= 10; //单行路

		19'b001_001011100_0000011: shape <= 11; //交叉路口
		19'b001_001010100_0000001: shape <= 12; //双向交通
		19'b001_001010000_0000001: shape <= 12; //双向交通		
		19'b001_001101100_0000011: shape <= 13; //注意危险
		19'b001_000100000_0000001: shape <= 14; //注意行人
		19'b001_000000000_0000001: shape <= 14; //注意行人
		19'b001_000100000_0000000: shape <= 14; //注意行人
		19'b001_000010000_0000001: shape <= 15; //注意儿童
		19'b001_001111000_0000001: shape <= 17; //注意牲畜
		19'b001_000011000_0000001: shape <= 17; //注意牲畜
		19'b001_001011000_0000011: shape <= 18; //左急转
		19'b001_001111000_0000011: shape <= 18; //左急转
		19'b001_000110100_0000011: shape <= 19; //右急转
		19'b001_000111100_0000011: shape <= 19; //右急转
		
		
		
		19'b100_011111110_0110110: shape <= 20; //禁止驶入
		19'b100_011111110_0111110: shape <= 21; //禁止车辆停放
		19'b100_011111110_0001111: shape <= 22; //施工
		19'b100_001110010_0011010: shape <= 23; //停车让行
		19'b100_001110010_0010010: shape <= 23; //停车让行
		19'b100_011110010_0011010: shape <= 23; //停车让行
		19'b100_011110010_0010010: shape <= 23; //停车让行
		19'b100_000000000_1000000: shape <= 24; //减速让行
		19'b100_000011000_0010000: shape <= 25; //禁止直行
		19'b100_000010000_0010000: shape <= 25; //禁止直行
		19'b100_001001100_0111000: shape <= 26; //禁止右转
		19'b100_001001100_0011000: shape <= 26; //禁止右转
		19'b100_010010010_0011000: shape <= 27; //禁止左右转
		19'b100_010010010_0010000: shape <= 27; //禁止左右转
		19'b100_010011000_0010000: shape <= 27; //禁止左右转
		19'b100_000011010_0010000: shape <= 27; //禁止左右转
		19'b100_001100100_0100100: shape <= 28; //禁止掉头
		19'b100_010000000_0001000: shape <= 29; //禁止鸣笛
		19'b100_001000000_0000000: shape <= 30; //会车让行
		19'b100_001000100_0011000: shape <= 31; //禁止左转
		
		//额外添加
		
		default: shape <= {8{2'b10}}; //错误
	endcase
	end
end

always @(posedge clk or negedge rst_n) begin
	if(!rst_n) begin
		shape_r <= 0;
	end
	else if(shape != {8{2'b10}}) begin
		shape_r <= shape;
	end
	else if(shape == {8{2'b10}}) begin
		shape_r <= shape_r;
	end
end

assign o_shape = shape_r;

//同步
always@(posedge clk or negedge rst_n)begin
	if(!rst_n)begin
		vs_r <= 0;
		hs_r <= 0;
		de_r <= 0;
		x_d <= 9'd0;
		y_d <= 9'd0;
	end
	else begin
		vs_r <= i_vsync;
		hs_r <= i_hsync;
		de_r <= i_de;
		x_d <= xpos;
		y_d <= ypos;
	end
end
assign color_rgb = rgb_r;
assign o_vs		 = vs_r;
assign o_hs		 = hs_r;
assign o_de      = de_r;
assign o_x       = x_d;
assign o_y		 = y_d;

endmodule