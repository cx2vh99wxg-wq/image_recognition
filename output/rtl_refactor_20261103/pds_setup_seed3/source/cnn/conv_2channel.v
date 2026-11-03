// 2026-11-03 注释校订。分别计算两组 3×3 乘积，再按本模块的累加宽度与顺序合成输出。
module conv_2channel
#(
    parameter                           IMG_HDISP                 = 11'd640   ,
    parameter                           IMG_VDISP                 = 11'd480   ,
    parameter                           core_witdh                = 4'd8
)
(
    input                               clk                        ,
    input                               rst_n                      ,

    input  wire                         per_img_vsync              ,
    input  wire                         per_img_href               ,
    input  wire        [   7: 0]        conv_Data_in_1               ,
    input  wire        [   7: 0]        conv_Data_in_2               ,

    output reg                          post_img_vsync             ,
    output reg                          post_img_href              ,
    output reg         [   7: 0]        conv_Data_out              ,

    input        [core_witdh: 0]      conv_coef_1_1                ,
    input        [core_witdh: 0]      conv_coef_2_1                ,
    input        [core_witdh: 0]      conv_coef_3_1                ,
    input        [core_witdh: 0]      conv_coef_4_1                ,
    input        [core_witdh: 0]      conv_coef_5_1                ,
    input        [core_witdh: 0]      conv_coef_6_1                ,
    input        [core_witdh: 0]      conv_coef_7_1                ,
    input        [core_witdh: 0]      conv_coef_8_1                ,
    input        [core_witdh: 0]      conv_coef_9_1                ,

    input        [core_witdh: 0]      conv_coef_1_2                ,
    input        [core_witdh: 0]      conv_coef_2_2                ,
    input        [core_witdh: 0]      conv_coef_3_2                ,
    input        [core_witdh: 0]      conv_coef_4_2                ,
    input        [core_witdh: 0]      conv_coef_5_2                ,
    input        [core_witdh: 0]      conv_coef_6_2                ,
    input        [core_witdh: 0]      conv_coef_7_2                ,
    input        [core_witdh: 0]      conv_coef_8_2                ,
    input        [core_witdh: 0]      conv_coef_9_2                ,

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
    .per_img_Data                       (conv_Data_in_1              ),

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

wire            [7:0]           conv1_2_matrix_p11;
wire            [7:0]           conv1_2_matrix_p12;
wire            [7:0]           conv1_2_matrix_p13;
wire            [7:0]           conv1_2_matrix_p21;
wire            [7:0]           conv1_2_matrix_p22;
wire            [7:0]           conv1_2_matrix_p23;
wire            [7:0]           conv1_2_matrix_p31;
wire            [7:0]           conv1_2_matrix_p32;
wire            [7:0]           conv1_2_matrix_p33;

Matrix_Generate_3X3_Buf
#(
    .DATA_WIDTH                         (8                         ),
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 )
)
u_Matrix_Generate_3X3_8Bit_2
(

    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),

    .per_frame_vsync                    (per_img_vsync             ),
    .per_frame_href                     (per_img_href              ),
    .per_img_Data                       (conv_Data_in_2              ),

    .matrix_frame_vsync                 (          ),
    .matrix_frame_href                  (          ),
    .matrix_p11                         (conv1_2_matrix_p11                ),
    .matrix_p12                         (conv1_2_matrix_p12                ),
    .matrix_p13                         (conv1_2_matrix_p13                ),
    .matrix_p21                         (conv1_2_matrix_p21                ),
    .matrix_p22                         (conv1_2_matrix_p22                ),
    .matrix_p23                         (conv1_2_matrix_p23                ),
    .matrix_p31                         (conv1_2_matrix_p31                ),
    .matrix_p32                         (conv1_2_matrix_p32                ),
    .matrix_p33                         (conv1_2_matrix_p33                )
);

wire [8+core_witdh:0] conv1_core_1_mult_g11;
wire [8+core_witdh:0] conv1_core_1_mult_g21;
wire [8+core_witdh:0] conv1_core_1_mult_g31;
wire [8+core_witdh:0] conv1_core_1_mult_g12;
wire [8+core_witdh:0] conv1_core_1_mult_g22;
wire [8+core_witdh:0] conv1_core_1_mult_g32;
wire [8+core_witdh:0] conv1_core_1_mult_g13;
wire [8+core_witdh:0] conv1_core_1_mult_g23;
wire [8+core_witdh:0] conv1_core_1_mult_g33;
conv_kernel_products #(
    .COEF_BITS(core_witdh + 1),
    .PRODUCT_BITS(8+core_witdh + 1)
) u_products_1 (
    .clk(clk),
    .pixels({conv1_1_matrix_p33, conv1_1_matrix_p32, conv1_1_matrix_p31, conv1_1_matrix_p23, conv1_1_matrix_p22, conv1_1_matrix_p21, conv1_1_matrix_p13, conv1_1_matrix_p12, conv1_1_matrix_p11}),
    .coefficients({conv_coef_9_1, conv_coef_8_1, conv_coef_7_1, conv_coef_6_1, conv_coef_5_1, conv_coef_4_1, conv_coef_3_1, conv_coef_2_1, conv_coef_1_1}),
    .products({conv1_core_1_mult_g33, conv1_core_1_mult_g23, conv1_core_1_mult_g13, conv1_core_1_mult_g32, conv1_core_1_mult_g22, conv1_core_1_mult_g12, conv1_core_1_mult_g31, conv1_core_1_mult_g21, conv1_core_1_mult_g11})
);

reg     signed        [8+2+core_witdh:0]          conv1_core_1_weight1;
reg     signed        [8+2+core_witdh:0]          conv1_core_1_weight2;
reg     signed        [8+2+core_witdh:0]          conv1_core_1_weight3;

always @(posedge clk)
begin
    conv1_core_1_weight1 <= conv1_core_1_mult_g11 + conv1_core_1_mult_g21 + conv1_core_1_mult_g31;
    conv1_core_1_weight2 <= conv1_core_1_mult_g12 + conv1_core_1_mult_g22 + conv1_core_1_mult_g32;
    conv1_core_1_weight3 <= conv1_core_1_mult_g13 + conv1_core_1_mult_g23 + conv1_core_1_mult_g33;

end

wire [8+core_witdh:0] conv1_core_2_mult_g11;
wire [8+core_witdh:0] conv1_core_2_mult_g21;
wire [8+core_witdh:0] conv1_core_2_mult_g31;
wire [8+core_witdh:0] conv1_core_2_mult_g12;
wire [8+core_witdh:0] conv1_core_2_mult_g22;
wire [8+core_witdh:0] conv1_core_2_mult_g32;
wire [8+core_witdh:0] conv1_core_2_mult_g13;
wire [8+core_witdh:0] conv1_core_2_mult_g23;
wire [8+core_witdh:0] conv1_core_2_mult_g33;
conv_kernel_products #(
    .COEF_BITS(core_witdh + 1),
    .PRODUCT_BITS(8+core_witdh + 1)
) u_products_2 (
    .clk(clk),
    .pixels({conv1_2_matrix_p33, conv1_2_matrix_p32, conv1_2_matrix_p31, conv1_2_matrix_p23, conv1_2_matrix_p22, conv1_2_matrix_p21, conv1_2_matrix_p13, conv1_2_matrix_p12, conv1_2_matrix_p11}),
    .coefficients({conv_coef_9_2, conv_coef_8_2, conv_coef_7_2, conv_coef_6_2, conv_coef_5_2, conv_coef_4_2, conv_coef_3_2, conv_coef_2_2, conv_coef_1_2}),
    .products({conv1_core_2_mult_g33, conv1_core_2_mult_g23, conv1_core_2_mult_g13, conv1_core_2_mult_g32, conv1_core_2_mult_g22, conv1_core_2_mult_g12, conv1_core_2_mult_g31, conv1_core_2_mult_g21, conv1_core_2_mult_g11})
);

reg             [8+2+core_witdh:0]          conv1_core_2_weight1;
reg             [8+2+core_witdh:0]          conv1_core_2_weight2;
reg             [8+2+core_witdh:0]          conv1_core_2_weight3;

always @(posedge clk)
begin
    conv1_core_2_weight1 <= conv1_core_2_mult_g11 + conv1_core_2_mult_g21 + conv1_core_2_mult_g31;
    conv1_core_2_weight2 <= conv1_core_2_mult_g12 + conv1_core_2_mult_g22 + conv1_core_2_mult_g32;
    conv1_core_2_weight3 <= conv1_core_2_mult_g13 + conv1_core_2_mult_g23 + conv1_core_2_mult_g33;

end

reg     signed        [8+4+core_witdh:0]          conv1_core_1_weight_sum_add_conv1_core_2_weight_sum;

always @(posedge clk)
begin
    conv1_core_1_weight_sum_add_conv1_core_2_weight_sum   <= conv1_core_1_weight1 + conv1_core_1_weight2 + conv1_core_1_weight3 + conv1_core_2_weight1 + conv1_core_2_weight2 + conv1_core_2_weight3+conv_bias_0;
end

reg             [7:0]          sum;

always @(posedge clk)
begin
    if(conv1_core_1_weight_sum_add_conv1_core_2_weight_sum<=0) conv_Data_out<=8'd0;
    else
    conv_Data_out      <= conv1_core_1_weight_sum_add_conv1_core_2_weight_sum[core_witdh+7:core_witdh]+conv1_core_1_weight_sum_add_conv1_core_2_weight_sum[core_witdh-1];
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