// 2026-11-03 注释校订。单路 3×3 卷积。乘积、行和、加偏置与激活各占一个寄存级。
`timescale 1ns / 1ps

// VSCODE plug-in author : Jiang Percy

//****************************************Copyright (c)***********************************//
// Copyright(C)            company
// All rights reserved

// 卷积流水线：Last modified Date: 2024/10/17 11:14:31

// 卷积流水线：Created by: 黄煜宏
// 参数记录为 Created date: 2024/10/17 11:14:31。
// 卷积流水线：mail : 1807092357@qq.com

module conv_example
#(
    parameter                           IMG_HDISP                 = 11'd640   ,
    parameter                           IMG_VDISP                 = 11'd480   ,
    parameter                           core_witdh                = 4'd8
)
(
    input  wire                         clk                        ,
    input  wire                         rst_n                      ,

    input  wire                         per_img_vsync              ,
    input  wire                         per_img_href               ,
    input  wire        [   7: 0]        conv_Data_in               ,

    output reg                          post_img_vsync             ,
    output reg                          post_img_href              ,
    output reg         [   7: 0]        conv_Data_out              ,

    input        [core_witdh: 0]      conv_coef_1                ,
    input        [core_witdh: 0]      conv_coef_2                ,
    input        [core_witdh: 0]      conv_coef_3                ,
    input        [core_witdh: 0]      conv_coef_4                ,
    input        [core_witdh: 0]      conv_coef_5                ,
    input        [core_witdh: 0]      conv_coef_6                ,
    input        [core_witdh: 0]      conv_coef_7                ,
    input        [core_witdh: 0]      conv_coef_8                ,
    input        [core_witdh: 0]      conv_coef_9                ,

    input        [8+ 3+core_witdh: 0]   conv_bias_0

);

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

    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),

    .per_frame_vsync                    (per_img_vsync             ),
    .per_frame_href                     (per_img_href              ),
    .per_img_Data                       (conv_Data_in              ),

    .matrix_frame_vsync                 (matrix_img_vsync          ),
    .matrix_frame_href                  (matrix_img_href           ),
    .matrix_p11                         (conv1_1_matrix_p11                ),
    .matrix_p12                         (conv1_1_matrix_p12                ),
    .matrix_p13                         (conv1_1_matrix_p13                ),
    .matrix_p21                         (conv1_1_matrix_p21                ),
    .matrix_p22                         (conv1_1_matrix_p22                ),
    .matrix_p23                         (conv1_1_matrix_p23                ),
    .matrix_p31                         (conv1_1_matrix_p31                ),
    .matrix_p32                         (conv1_1_matrix_p32                ),
    .matrix_p33                         (conv1_1_matrix_p33                )
);

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

reg             [10+2+core_witdh:0]          conv1_core_1_weight1;
reg             [10+2+core_witdh:0]          conv1_core_1_weight2;
reg             [10+2+core_witdh:0]          conv1_core_1_weight3;

always @(posedge clk)
begin
    conv1_core_1_weight1 <= conv1_core_1_mult_g11 + conv1_core_1_mult_g21 + conv1_core_1_mult_g31;
    conv1_core_1_weight2 <= conv1_core_1_mult_g12 + conv1_core_1_mult_g22 + conv1_core_1_mult_g32;
    conv1_core_1_weight3 <= conv1_core_1_mult_g13 + conv1_core_1_mult_g23 + conv1_core_1_mult_g33;

end

reg    signed        [8+3+core_witdh:0]          conv1_core_1_weight_sum;

always @(posedge clk)
begin
    conv1_core_1_weight_sum   <= conv1_core_1_weight1 + conv1_core_1_weight2 + conv1_core_1_weight3+conv_bias_0;
end

reg             [7:0]          sum;

always @(posedge clk)
begin
    if(conv1_core_1_weight_sum<=0) conv_Data_out<=8'd0;
    else
    conv_Data_out      <= conv1_core_1_weight_sum[core_witdh+7:core_witdh]+conv1_core_1_weight_sum[core_witdh-1];
end

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
