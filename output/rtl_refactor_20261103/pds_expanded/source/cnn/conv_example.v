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
// 卷积流水线：Last modified Date: 2024/10/17 11:14:31
// 参数记录为 Last Version: V1.0。
// 卷积流水线：Descriptions:
// ── 卷积流水线 ──
// 卷积流水线：Created by: 黄煜宏
// 参数记录为 Created date: 2024/10/17 11:14:31。
// 卷积流水线：mail : 1807092357@qq.com
// 参数记录为 Version: V1.0。
// 卷积流水线：TEXT NAME: conv_example.v
// 卷积流水线：PATH: C:\Users\huangyuhong\Desktop\cnn_juanji\user\src\conv_example.v
// 卷积流水线：Descriptions:
// ── 卷积流水线 ──
// ── 卷积流水线 ──
// ── 卷积流水线 ──

module conv_example
#(
    parameter                           IMG_HDISP                 = 11'd640   ,
    parameter                           IMG_VDISP                 = 11'd480   ,
    parameter                           core_witdh                = 4'd8  
)
(
    input  wire                         clk                        ,
    input  wire                         rst_n                      ,
    
    // 卷积流水线：Image data prepared to be processed
    input  wire                         per_img_vsync              ,// 卷积流水线：Prepared Image data vsync valid signal
    input  wire                         per_img_href               ,// 卷积流水线：Prepared Image data href vaild signal
    input  wire        [   7: 0]        conv_Data_in               ,// 卷积流水线：Prepared Image brightness input
    
    // 卷积流水线：Image data has been processed
    output reg                          post_img_vsync             ,// 卷积流水线：processed Image data vsync valid signal
    output reg                          post_img_href              ,// 卷积流水线：processed Image data href vaild signal
    output reg         [   7: 0]        conv_Data_out              ,// 卷积流水线：processed Image brightness output

    // 卷积核系数及其偏置。
    input        [core_witdh: 0]      conv_coef_1                ,
    input        [core_witdh: 0]      conv_coef_2                ,
    input        [core_witdh: 0]      conv_coef_3                ,
    input        [core_witdh: 0]      conv_coef_4                ,
    input        [core_witdh: 0]      conv_coef_5                ,
    input        [core_witdh: 0]      conv_coef_6                ,
    input        [core_witdh: 0]      conv_coef_7                ,
    input        [core_witdh: 0]      conv_coef_8                ,
    input        [core_witdh: 0]      conv_coef_9                ,

    input        [8+ 3+core_witdh: 0]   conv_bias_0      // 卷积流水线：位宽还可以再高，但是没有必要了

);
// 给第一个通道。
// 卷积流水线：Generate 8Bit 3X3 Matrix
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
    // 卷积流水线：global clock & reset
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    
    // 卷积流水线：Image data prepared to be processed
    .per_frame_vsync                    (per_img_vsync             ),// 卷积流水线：Prepared Image data vsync valid signal
    .per_frame_href                     (per_img_href              ),// 卷积流水线：Prepared Image data href vaild signal
    .per_img_Data                       (conv_Data_in              ),// 卷积流水线：Prepared Image brightness input
    // 卷积流水线：Image data has been processed
    .matrix_frame_vsync                 (matrix_img_vsync          ),// 卷积流水线：processed Image data vsync valid signal
    .matrix_frame_href                  (matrix_img_href           ),// 卷积流水线：processed Image data href vaild signal
    .matrix_p11                         (conv1_1_matrix_p11                ),// 参数记录为 3X3 Matrix output。
    .matrix_p12                         (conv1_1_matrix_p12                ),
    .matrix_p13                         (conv1_1_matrix_p13                ),
    .matrix_p21                         (conv1_1_matrix_p21                ),
    .matrix_p22                         (conv1_1_matrix_p22                ),
    .matrix_p23                         (conv1_1_matrix_p23                ),
    .matrix_p31                         (conv1_1_matrix_p31                ),
    .matrix_p32                         (conv1_1_matrix_p32                ),
    .matrix_p33                         (conv1_1_matrix_p33                ) 
);                                                          


// 1拍。
// 对第一个通道卷积。
// ── 卷积流水线 ──
// 卷积流水线：第一个通道的第一个卷积核//
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g11 = 8'd76;
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g12 = 8'd126;
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g13 = 8'd76;
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g21 = 8'd126;
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g22 = 8'd209;
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g23 = 8'd126;
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g31 = 8'd76;
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g32 = 8'd126;
// 停用示例，仅供对照：wire signed [core_witdh:0] conv1_core_1_g33 = 8'd76;
// 卷积流水线：localparam conv1_core_1_pianzhi = -128 ;


// 2026-11-03: all nine matrix taps share one registered multiplier bank.
wire [10+core_witdh:0] conv1_core_1_mult_g11;
wire [10+core_witdh:0] conv1_core_1_mult_g21;
wire [10+core_witdh:0] conv1_core_1_mult_g31;
wire [10+core_witdh:0] conv1_core_1_mult_g12;
wire [10+core_witdh:0] conv1_core_1_mult_g22;
wire [10+core_witdh:0] conv1_core_1_mult_g32;
wire [10+core_witdh:0] conv1_core_1_mult_g13;
wire [10+core_witdh:0] conv1_core_1_mult_g23;
wire [10+core_witdh:0] conv1_core_1_mult_g33;
conv_kernel_products #(
    .COEF_BITS(core_witdh + 1),
    .PRODUCT_BITS(10+core_witdh + 1)
) u_products_1 (
    .clk(clk),
    .pixels({conv1_1_matrix_p33, conv1_1_matrix_p32, conv1_1_matrix_p31, conv1_1_matrix_p23, conv1_1_matrix_p22, conv1_1_matrix_p21, conv1_1_matrix_p13, conv1_1_matrix_p12, conv1_1_matrix_p11}),
    .coefficients({conv_coef_9, conv_coef_8, conv_coef_7, conv_coef_6, conv_coef_5, conv_coef_4, conv_coef_3, conv_coef_2, conv_coef_1}),
    .products({conv1_core_1_mult_g33, conv1_core_1_mult_g23, conv1_core_1_mult_g13, conv1_core_1_mult_g32, conv1_core_1_mult_g22, conv1_core_1_mult_g12, conv1_core_1_mult_g31, conv1_core_1_mult_g21, conv1_core_1_mult_g11})
);

// ── 卷积流水线 ──
// 2拍。
reg             [10+2+core_witdh:0]          conv1_core_1_weight1;
reg             [10+2+core_witdh:0]          conv1_core_1_weight2;
reg             [10+2+core_witdh:0]          conv1_core_1_weight3;

always @(posedge clk)
begin
    conv1_core_1_weight1 <= conv1_core_1_mult_g11 + conv1_core_1_mult_g21 + conv1_core_1_mult_g31;
    conv1_core_1_weight2 <= conv1_core_1_mult_g12 + conv1_core_1_mult_g22 + conv1_core_1_mult_g32;
    conv1_core_1_weight3 <= conv1_core_1_mult_g13 + conv1_core_1_mult_g23 + conv1_core_1_mult_g33;

end
// ── 卷积流水线 ──
// 3拍。
reg    signed        [8+3+core_witdh:0]          conv1_core_1_weight_sum;

always @(posedge clk)
begin
    conv1_core_1_weight_sum   <= conv1_core_1_weight1 + conv1_core_1_weight2 + conv1_core_1_weight3+conv_bias_0;
end

// 4拍。
// 激活函数。
reg             [7:0]          sum;

always @(posedge clk)
begin
    if(conv1_core_1_weight_sum<=0) conv_Data_out<=8'd0;
    else 
    conv_Data_out      <= conv1_core_1_weight_sum[core_witdh+7:core_witdh]+conv1_core_1_weight_sum[core_witdh-1];
end


// 延时。
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

module conv_kernel_products #(
    parameter COEF_BITS = 9,
    parameter PRODUCT_BITS = 19
) (
    input wire clk,
    input wire [71:0] pixels,
    input wire [9*COEF_BITS-1:0] coefficients,
    output wire [9*PRODUCT_BITS-1:0] products
);
    genvar tap;
    generate
        for (tap = 0; tap < 9; tap = tap + 1) begin : kernel_tap
            reg [PRODUCT_BITS-1:0] product;
            always @(posedge clk)
                product <= pixels[tap*8 +: 8] * coefficients[tap*COEF_BITS +: COEF_BITS];
            assign products[tap*PRODUCT_BITS +: PRODUCT_BITS] = product;
        end
    endgenerate
endmodule
