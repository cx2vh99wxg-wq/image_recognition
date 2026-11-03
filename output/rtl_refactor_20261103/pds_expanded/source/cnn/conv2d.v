`timescale 1ns / 1ps
// VSCODE PLUG-IN。
// ── 卷积流水线 ──
// 卷积流水线：IDE : VSCODE
// 参数记录为 VSCODE plug-in version: Verilog-Hdl-Format-2.8.20240817。
// VSCODE plug-in author : Jiang Percy
// ── 卷积流水线 ──
//****************************************Copyright (c)***********************************//
// Copyright(C)            company
// All rights reserved     
// 卷积流水线：File name:
// 卷积流水线：Last modified Date: 2024/10/19 14:54:32
// 参数记录为 Last Version: V1.0。
// 卷积流水线：Descriptions:
// ── 卷积流水线 ──
// 卷积流水线：Created by: 黄煜宏
// 参数记录为 Created date: 2024/10/19 14:54:32。
// 卷积流水线：mail : 1807092357@qq.com
// 参数记录为 Version: V1.0。
// 参数记录为 TEXT NAME: cinv2d.v。
// 卷积流水线：PATH: C:\Users\huangyuhong\Desktop\cnn_juanji\user\src\cinv2d.v
// 卷积流水线：Descriptions:
// ── 卷积流水线 ──
// ── 卷积流水线 ──
// ── 卷积流水线 ──

module conv2d
#(
    parameter   [10:0]  IMG_HDISP   = 11'd640,                      // 参数记录为 640*480。
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


// Pair the two active coefficients, retaining the bitwise merge for unknown levels.
wire [25:0] active_coefficients =
    (level_reg == 6'd8) ? {13'd6224, 13'd3842} :
    (level_reg == 6'd7) ? {13'd5896, 13'd3638} :
    (level_reg == 6'd6) ? {13'd5568, 13'd3433} :
    (level_reg == 6'd5) ? {13'd5241, 13'd3229} :
    (level_reg == 6'd4) ? {13'd4913, 13'd3024} :
    (level_reg == 6'd3) ? {13'd4587, 13'd2820} :
    (level_reg == 6'd2) ? {13'd4259, 13'd2614} :
    (level_reg == 6'd1) ? {13'd3931, 13'd2409} :
    26'd0;
assign conv_coef_1 = active_coefficients[25:13];
assign conv_coef_2 = 0;
assign conv_coef_3 = 0;
assign conv_coef_4 = active_coefficients[12:0];

// 1拍。
always @(posedge clk)
begin
    conv1_core_1_mult_g11 <=conv2d_1_Data *conv_coef_1 ;
    conv1_core_1_mult_g21 <=conv2d_2_Data *conv_coef_2 ;
    conv1_core_1_mult_g31 <=conv2d_3_Data *conv_coef_3 ;
    conv1_core_1_mult_g12 <=conv2d_4_Data *conv_coef_4 ;
end  

// 2拍。
reg  [8+2+3+core_witdh:0] conv1_core_2_mult_g11;
always @(posedge clk)
conv1_core_2_mult_g11<=conv1_core_1_mult_g11+conv1_core_1_mult_g21+conv1_core_1_mult_g31+conv1_core_1_mult_g12;

// 3拍。

always @(posedge clk)
begin
    if(conv1_core_2_mult_g11[core_witdh+13:core_witdh]>=1023) conv2d_data_out<=1023;
    else conv2d_data_out<=conv1_core_2_mult_g11[core_witdh+9:core_witdh];
end

// 延时。
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

// ── 卷积流水线 ──
// 2026-11-03: align one RGBY bus through the three legacy reset stages.
reg [31:0] pixel_stage1, pixel_stage2;
always @(posedge clk)
    pixel_stage1 <= {per_img_r, per_img_g, per_img_b, per_img_Y};

always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        pixel_stage2 <= 32'd0;
    else
        pixel_stage2 <= pixel_stage1;
end

always @(posedge clk)
    {post_img_r, post_img_g, post_img_b, post_img_Y} <= pixel_stage2;

endmodule