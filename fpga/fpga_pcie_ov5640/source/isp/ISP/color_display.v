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

module color_display #(
    parameter NUM_ROW =  1 ,
    parameter NUM_COL =  2 ,
    parameter NUM_WIDTH = (NUM_ROW*NUM_COL<<2)-1
)(
    //module clock
    input                    clk              ,  //时钟信号
    input                    rst_n            ,  //复位信号（低有效）

    //image data interface
    input                    monoc            ,  //单色图像像素数据
    input                    monoc_fall       ,  //图像数据变化
    input      [9:0]        xpos             ,  //横坐标
    input      [9:0]        ypos             ,  //纵坐标
	input	[23:0]			i_rgb		,
	input					i_vsync		,	
	input					i_hsync		,	
	input					i_de		,		
	input					red_en		,
	input					blue_en		,
	input					yellow_en	,
    

    //project border ram interface
    input      [10:0]        row_border_data  ,  //行边界ram读数据
    output reg [10:0]        row_border_addr  ,  //行边界ram读地址
    input      [10:0]        col_border_data  ,  //列边界ram读数据
    output reg [10:0]        col_border_addr  ,  //列边界ram读地址
	
	output					o_vs		,	
	output					o_hs	,	
	output					o_de		,	
	output  [23:0]        color_rgb        ,  //输出图像数据
	output  [9:0]			o_x			,
	output	[9:0]			o_y			,
	output	[10:0]     		o_top		,
	output	[10:0]			o_left		,
    //user interface
    input      [ 1:0]        frame_cnt        ,  //当前帧
    input                    project_done_flag,  //投影完成标志
    input      [ 3:0]        num_col          ,  //采集到的数字列数
    input      [ 3:0]        num_row            //采集到的数字行数
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
assign o_left = col_border_l;
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


reg [23:0] rgb_r;
reg vs_r;
reg hs_r;
reg de_r;
reg [9:0]x_d;
reg	[9:0]y_d;
//输出边界和图像
always @(posedge clk or negedge rst_n) begin
    if(!rst_n)
        rgb_r <= 24'h000000;
	else if(red_en)begin
		if(row_area[row_cnt] && ( xpos == col_border_l|| xpos == col_border_r ||
            xpos == (col_border_l - 1) || xpos == (col_border_r-1)))
			rgb_r <= 24'hf80000; //左右竖直边界线
		else if(col_area[col_cnt] && (ypos == row_border_high || ypos== row_border_low ||
            ypos==( row_border_high - 1) || ypos== (row_border_low + 1)))
			rgb_r <= 24'hf80000; //上下水平边界线
		else 
			rgb_r <= i_rgb;
	end
	
	else if(blue_en)begin
		if(row_area[row_cnt] && ( xpos == col_border_l|| xpos == col_border_r ||
            xpos == (col_border_l - 1) || xpos == (col_border_r-1)))
			rgb_r <= 24'h0000f8; //左右竖直边界线
		else if(col_area[col_cnt] && (ypos == row_border_high || ypos== row_border_low ||
            ypos==( row_border_high - 1) || ypos== (row_border_low + 1)))
			rgb_r <= 24'h0000f8; //上下水平边界线
		else 
			rgb_r <= i_rgb;
	end
	
	else if(yellow_en)begin
		if(row_area[row_cnt] && ( xpos == col_border_l|| xpos == col_border_r ||
            xpos == (col_border_l - 1) || xpos == (col_border_r-1)))
			rgb_r <= 24'hffff00; //左右竖直边界线
		else if(col_area[col_cnt] && (ypos == row_border_high || ypos== row_border_low ||
            ypos==( row_border_high - 1) || ypos== (row_border_low + 1)))
			rgb_r <= 24'hffff00; //上下水平边界线
		else 
			rgb_r <= i_rgb;
	end
    else 
        rgb_r <= i_rgb;
end
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