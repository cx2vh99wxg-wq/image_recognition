`timescale 1ns / 1ps
//****************************************VSCODE PLUG-IN**********************************//
//----------------------------------------------------------------------------------------
// IDE :                   VSCODE     
// VSCODE plug-in version: Verilog-Hdl-Format-2.8.20240817
// VSCODE plug-in author : Jiang Percy
//----------------------------------------------------------------------------------------
//****************************************Copyright (c)***********************************//
// Copyright(C)            company
// All rights reserved     
// File name:              
// Last modified Date:     2024/10/19 14:54:32
// Last Version:           V1.0
// Descriptions:           
//----------------------------------------------------------------------------------------
// Created by:             黄煜宏
// Created date:           2024/10/19 14:54:32
// mail      :             1807092357@qq.com
// Version:                V1.0
// TEXT NAME:              cinv2d.v
// PATH:                   C:\Users\huangyuhong\Desktop\cnn_juanji\user\src\cinv2d.v
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//

module conv2d
#(
    parameter   [10:0]  IMG_HDISP   = 11'd640,                      //  640*480
    parameter   [10:0]  IMG_VDISP   = 11'd480,
    parameter                           core_witdh                = 4'd11  
)
(
    input                               clk                        ,
    input                               rst_n                      ,
    input              [   7: 0]        per_img_r                  ,
    input              [   7: 0]        per_img_g                  ,
    input              [   7: 0]        per_img_b                  ,
    output reg         [   7: 0]        post_img_r                 ,
    output reg         [   7: 0]        post_img_g                 ,
    output reg         [   7: 0]        post_img_b                 ,

    input                               per_img_vsync              ,
    input                               per_img_href               ,
    input              [   7: 0]        per_img_Y                  ,

    output reg                          post_img_vsync             ,
    output reg                          post_img_href              ,
    output reg         [   7: 0]        post_img_Y                 ,

    input              [   7: 0]        conv2d_1_Data              ,
    input              [   7: 0]        conv2d_2_Data              ,
    input              [   7: 0]        conv2d_3_Data              ,
    input              [   7: 0]        conv2d_4_Data              ,

    output reg   [   9: 0]        conv2d_data_out     ,

    input [5:0] level         
);

reg             [8+core_witdh+3:0]          conv1_core_1_mult_g11;
reg             [8+core_witdh+3:0]          conv1_core_1_mult_g21;
reg             [8+core_witdh+3:0]          conv1_core_1_mult_g31;
reg             [8+core_witdh+3:0]          conv1_core_1_mult_g12;

wire  [core_witdh+3:0] conv_coef_1;
wire  [core_witdh+3:0] conv_coef_2;
wire  [core_witdh+3:0] conv_coef_3;
wire  [core_witdh+3:0] conv_coef_4;

reg [5:0]level_reg;
    always @(posedge clk or negedge rst_n)           
        begin                                        
            if(!rst_n)                               
              level_reg<=0;                                                                     
            else                                    
              level_reg<=level;                                                                                
        end                                          


    assign                              conv_coef_1               = (level_reg==8)?6224:(level_reg==7)?5896:(level_reg==6)?5568:(level_reg==5)?5241:(level_reg==4)?4913:(level_reg==3)?4587:(level_reg==2)?4259:(level_reg==1)?3931:(level_reg==0)?0:0;
    assign                              conv_coef_2               = (level_reg==8)?   0:(level_reg==7)?   0:(level_reg==6)?   0:(level_reg==5)?   0:(level_reg==4)?   0:(level_reg==3)?   0:(level_reg==2)?   0:(level_reg==1)?   0:(level_reg==0)?0:0;
    assign                              conv_coef_3               = (level_reg==8)?   0:(level_reg==7)?   0:(level_reg==6)?   0:(level_reg==5)?   0:(level_reg==4)?   0:(level_reg==3)?   0:(level_reg==2)?   0:(level_reg==1)?   0:(level_reg==0)?0:0;
    assign                              conv_coef_4               = (level_reg==8)?3842:(level_reg==7)?3638:(level_reg==6)?3433:(level_reg==5)?3229:(level_reg==4)?3024:(level_reg==3)?2820:(level_reg==2)?2614:(level_reg==1)?2409:(level_reg==0)?0:0;

///////////////1拍/////////////////////
always @(posedge clk)
begin
    conv1_core_1_mult_g11 <=conv2d_1_Data *conv_coef_1 ;
    conv1_core_1_mult_g21 <=conv2d_2_Data *conv_coef_2 ;
    conv1_core_1_mult_g31 <=conv2d_3_Data *conv_coef_3 ;
    conv1_core_1_mult_g12 <=conv2d_4_Data *conv_coef_4 ;
end  

//////////////////////2拍///////////////////
reg  [8+2+3+core_witdh:0] conv1_core_2_mult_g11;
always @(posedge clk)
conv1_core_2_mult_g11<=conv1_core_1_mult_g11+conv1_core_1_mult_g21+conv1_core_1_mult_g31+conv1_core_1_mult_g12;

////////////////////3拍////////////////////////

always @(posedge clk)
begin
    if(conv1_core_2_mult_g11[core_witdh+13:core_witdh]>=1023) conv2d_data_out<=1023;
    else conv2d_data_out<=conv1_core_2_mult_g11[core_witdh+9:core_witdh];
end

////////////////延时//////////////////////////////////////////
localparam C_CLK_LATENCY = 3;

reg     [C_CLK_LATENCY-1:0]     matrix_img_vsync_r1;
reg     [C_CLK_LATENCY-1:0]     matrix_img_href_r1;

always @(posedge clk or negedge rst_n)
begin
    if(!rst_n)
    begin
        matrix_img_vsync_r1 <= {C_CLK_LATENCY{1'b0}};
        matrix_img_href_r1  <= {C_CLK_LATENCY{1'b0}};
    end
    else
    begin
        matrix_img_vsync_r1 <= {matrix_img_vsync_r1[C_CLK_LATENCY-2:0],per_img_vsync};
        matrix_img_href_r1  <=  {matrix_img_href_r1[C_CLK_LATENCY-2:0],per_img_href };
    end
end

always @(posedge clk or negedge rst_n)
begin
    if(!rst_n)
    begin
        post_img_vsync <= 1'b0;
        post_img_href  <= 1'b0;
    end
    else
    begin
        post_img_vsync <= matrix_img_vsync_r1[C_CLK_LATENCY-2];
        post_img_href  <=  matrix_img_href_r1[C_CLK_LATENCY-2];
    end
end

////////////////////////////////////////////////////////
reg [7:0] post_img_r_r1;
reg [7:0] post_img_g_r1;
reg [7:0] post_img_b_r1;
reg [7:0] post_img_Y_r1;

always @(posedge clk )
begin
    begin
        post_img_r_r1 <= per_img_r;
        post_img_g_r1 <= per_img_g;
        post_img_b_r1 <= per_img_b;
        post_img_Y_r1 <= per_img_Y;
    end
end

reg [7:0] post_img_r_r2;
reg [7:0] post_img_g_r2;
reg [7:0] post_img_b_r2;
reg [7:0] post_img_Y_r2;

always @(posedge clk or negedge rst_n)
begin
    if(!rst_n)
    begin
        post_img_r_r2 <= 8'h00;
        post_img_g_r2 <= 8'h00;
        post_img_b_r2 <= 8'h00;
        post_img_Y_r2 <= 8'h00;
    end
    else
    begin
        post_img_r_r2 <= post_img_r_r1;
        post_img_g_r2 <= post_img_g_r1;
        post_img_b_r2 <= post_img_b_r1;
        post_img_Y_r2 <= post_img_Y_r1;
    end
end

always @(posedge clk)
begin
    post_img_r <= post_img_r_r2;
    post_img_g <= post_img_g_r2;
    post_img_b <= post_img_b_r2;
    post_img_Y <= post_img_Y_r2;
end


endmodule