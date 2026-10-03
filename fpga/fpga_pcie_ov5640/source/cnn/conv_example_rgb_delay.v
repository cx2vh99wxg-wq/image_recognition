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
// Last modified Date:     2024/10/17 11:14:31
// Last Version:           V1.0
// Descriptions:           
//----------------------------------------------------------------------------------------
// Created by:             黄煜宏
// Created date:           2024/10/17 11:14:31
// mail      :             1807092357@qq.com
// Version:                V1.0
// TEXT NAME:              conv_example.v
// PATH:                   C:\Users\huangyuhong\Desktop\cnn_juanji\user\src\conv_example.v
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//

module conv_example_rgb_delay
#(
    parameter                           IMG_HDISP                 = 11'd640   ,
    parameter                           IMG_VDISP                 = 11'd480   ,
    parameter                           core_witdh                = 4'd8  
)
(
    input  wire                         clk                        ,
    input  wire                         rst_n                      ,
    
    //  Image data prepared to be processed
    input  wire                         per_img_vsync              ,//  Prepared Image data vsync valid signal
    input  wire                         per_img_href               ,//  Prepared Image data href vaild  signal
    input  wire        [   7: 0]        conv_Data_in               ,//  Prepared Image brightness input
    
    //  Image data has been processed
    output reg                          post_img_vsync             ,//  processed Image data vsync valid signal
    output reg                          post_img_href              ,//  processed Image data href vaild  signal
    output reg         [   7: 0]        conv_Data_out              //  processed Image brightness output  

);
////////////////给第一个通道/////////////////////////////////////////
//  Generate 8Bit 3X3 Matrix
wire                            matrix_img_vsync;
wire                            matrix_img_href;
wire            [7:0]           conv1_1_matrix_p11;
wire            [7:0]           conv1_1_matrix_p12;
wire            [7:0]           conv1_1_matrix_p13;
wire            [7:0]           conv1_1_matrix_p21;
wire            [7:0]           conv1_1_matrix_p22;
wire            [7:0]           conv1_1_matrix_p23;
wire            [7:0]           conv1_1_matrix_p31; 
wire            [7:0]           conv1_1_matrix_p32;
wire            [7:0]           conv1_1_matrix_p33;

Matrix_Generate_3X3_Buf
#(
    .DATA_WIDTH                         (8                         ),
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ) 
)
u_Matrix_Generate_3X3_8Bit
(
    //  global clock & reset
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    
    //  Image data prepared to be processed
    .per_frame_vsync                    (per_img_vsync             ),//  Prepared Image data vsync valid signal
    .per_frame_href                     (per_img_href              ),//  Prepared Image data href vaild  signal
    .per_img_Data                       (conv_Data_in              ),//  Prepared Image brightness input
    //  Image data has been processed
    .matrix_frame_vsync                 (matrix_img_vsync          ),//  processed Image data vsync valid signal
    .matrix_frame_href                  (matrix_img_href           ),//  processed Image data href vaild  signal
    .matrix_p11                         (conv1_1_matrix_p11                ),//  3X3 Matrix output
    .matrix_p12                         (conv1_1_matrix_p12                ),
    .matrix_p13                         (conv1_1_matrix_p13                ),
    .matrix_p21                         (conv1_1_matrix_p21                ),
    .matrix_p22                         (conv1_1_matrix_p22                ),
    .matrix_p23                         (conv1_1_matrix_p23                ),
    .matrix_p31                         (conv1_1_matrix_p31                ),
    .matrix_p32                         (conv1_1_matrix_p32                ),
    .matrix_p33                         (conv1_1_matrix_p33                ) 
);                                                          


//////////1拍////////////////
////////////////////对第一个通道卷积///////////////////////////////////
//----------------------------------------------------------------------
//第一个通道的第一个卷积核//
//wire signed [core_witdh:0] conv1_core_1_g11 = 8'd76;              
//wire signed [core_witdh:0] conv1_core_1_g12 = 8'd126;
//wire signed [core_witdh:0] conv1_core_1_g13 = 8'd76;
//wire signed [core_witdh:0] conv1_core_1_g21 = 8'd126;
//wire signed [core_witdh:0] conv1_core_1_g22 = 8'd209;
//wire signed [core_witdh:0] conv1_core_1_g23 = 8'd126;
//wire signed [core_witdh:0] conv1_core_1_g31 = 8'd76;
//wire signed [core_witdh:0] conv1_core_1_g32 = 8'd126;
//wire signed [core_witdh:0] conv1_core_1_g33 = 8'd76;
//localparam conv1_core_1_pianzhi = -128 ;


reg             [8+core_witdh:0]          conv1_core_1_mult_g11;
reg             [8+core_witdh:0]          conv1_core_1_mult_g21;
reg             [8+core_witdh:0]          conv1_core_1_mult_g31;
reg             [8+core_witdh:0]          conv1_core_1_mult_g12;
reg             [8+core_witdh:0]          conv1_core_1_mult_g22;
reg             [8+core_witdh:0]          conv1_core_1_mult_g32;
reg             [8+core_witdh:0]          conv1_core_1_mult_g13;
reg             [8+core_witdh:0]          conv1_core_1_mult_g23;
reg             [8+core_witdh:0]          conv1_core_1_mult_g33;

always @(posedge clk)
begin
    conv1_core_1_mult_g22 <= conv1_1_matrix_p22;

end         

//----------------------------------------------------------------------
//////////2拍//////////////////////////////////////////
reg             [8+2+core_witdh:0]          conv1_core_1_weight1;
reg             [8+2+core_witdh:0]          conv1_core_1_weight2;
reg             [8+2+core_witdh:0]          conv1_core_1_weight3;

always @(posedge clk)
begin
    conv1_core_1_weight1 <= 0;
    conv1_core_1_weight2 <= conv1_core_1_mult_g22 ;
    conv1_core_1_weight3 <= 0;

end
//----------------------------------------------------------------------
/////////////3拍//////////////////////////
reg            [8+3+core_witdh:0]          conv1_core_1_weight_sum;

always @(posedge clk)
begin
    conv1_core_1_weight_sum   <= conv1_core_1_weight2 ;
end

//////////////////4拍/////////////////////////////////
///////////////激活函数//////////////////////////////
reg             [7:0]          sum;

always @(posedge clk)
begin
    conv_Data_out      <= conv1_core_1_weight_sum[7:0];
end


////////////////延时//////////////////////////////////////////
localparam C_CLK_LATENCY = 4;

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
        matrix_img_vsync_r1 <= {matrix_img_vsync_r1[C_CLK_LATENCY-2:0],matrix_img_vsync};
        matrix_img_href_r1  <= {matrix_img_href_r1[C_CLK_LATENCY-2:0],matrix_img_href};
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
endmodule