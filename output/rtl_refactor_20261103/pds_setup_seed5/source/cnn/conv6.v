// 2026-11-03 注释校订。第 6 层卷积。卷积支路与 RGBY 对齐支路并行推进，各自保留原取样级。
`timescale 1ns / 1ps

module conv6
#(
    parameter   [10:0]  IMG_HDISP   = 11'd640,
    parameter   [10:0]  IMG_VDISP   = 11'd480,
    parameter                           core_witdh                = 4'd8
)
(
    input  wire                         clk                        ,
    input  wire                         rst_n                      ,

    input              [   7: 0]        per_img_r                  ,
    input              [   7: 0]        per_img_g                  ,
    input              [   7: 0]        per_img_b                  ,
    output             [   7: 0]        post_img_r                 ,
    output             [   7: 0]        post_img_g                 ,
    output             [   7: 0]        post_img_b                 ,
    input              [   7: 0]        per_img_Y                  ,
    output             [   7: 0]        post_img_Y                 ,

    input  wire                         per_img_vsync              ,
    input  wire                         per_img_href               ,
    input  wire        [   7: 0]        conv6_1_Data_in            ,
    input  wire        [   7: 0]        conv6_2_Data_in            ,
    input  wire        [   7: 0]        conv6_3_Data_in            ,
    input  wire        [   7: 0]        conv6_4_Data_in            ,
    input  wire        [   7: 0]        conv6_5_Data_in            ,
    input  wire        [   7: 0]        conv6_6_Data_in            ,
    input  wire        [   7: 0]        conv6_7_Data_in            ,
    input  wire        [   7: 0]        conv6_8_Data_in            ,

    output wire                         post_img_vsync             ,
    output wire                         post_img_href              ,
    output wire        [   7: 0]        conv6_1_Data_out           ,
    output wire        [   7: 0]        conv6_2_Data_out           ,
    output wire        [   7: 0]        conv6_3_Data_out           ,
    output wire        [   7: 0]        conv6_4_Data_out

);

conv_2channel
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv1_1      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in_1                     (conv6_1_Data_in           ),
    .conv_Data_in_2                     (conv6_2_Data_in           ),
    .conv_Data_out                      (conv6_1_Data_out          ),
    .post_img_vsync                     (post_img_vsync            ),
    .post_img_href                      (post_img_href             ),

    .conv_coef_1_1                        (0                         ),
    .conv_coef_2_1                        (0                         ),
    .conv_coef_3_1                        (0                         ),
    .conv_coef_4_1                        (0                         ),
    .conv_coef_5_1                        (0                         ),
    .conv_coef_6_1                        (0                         ),
    .conv_coef_7_1                        (0                         ),
    .conv_coef_8_1                        (0                         ),
    .conv_coef_9_1                        (0                         ),

    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),

    .conv_bias_0                        (0                         )
);

conv_2channel
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv1_2      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in_1                     (conv6_3_Data_in           ),
    .conv_Data_in_2                     (conv6_4_Data_in           ),
    .conv_Data_out                      (conv6_2_Data_out          ),
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),

    .conv_coef_1_1                        (0                         ),
    .conv_coef_2_1                        (0                         ),
    .conv_coef_3_1                        (0                         ),
    .conv_coef_4_1                        (0                         ),
    .conv_coef_5_1                        (0                         ),
    .conv_coef_6_1                        (0                         ),
    .conv_coef_7_1                        (0                         ),
    .conv_coef_8_1                        (0                         ),
    .conv_coef_9_1                        (0                         ),

    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),

    .conv_bias_0                        (0                         )
);

conv_2channel
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv1_3      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in_1                     (conv6_5_Data_in           ),
    .conv_Data_in_2                     (conv6_6_Data_in           ),
    .conv_Data_out                      (conv6_3_Data_out          ),
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),

    .conv_coef_1_1                      (0                       ),
    .conv_coef_2_1                      (0                       ),
    .conv_coef_3_1                      (0                       ),
    .conv_coef_4_1                      (0                       ),
    .conv_coef_5_1                      (0                       ),
    .conv_coef_6_1                      (0                       ),
    .conv_coef_7_1                      (0                       ),
    .conv_coef_8_1                      (0                       ),
    .conv_coef_9_1                      (0                       ),

    .conv_coef_1_2                      (0                         ),
    .conv_coef_2_2                      (0                         ),
    .conv_coef_3_2                      (0                         ),
    .conv_coef_4_2                      (0                         ),
    .conv_coef_5_2                      (0                         ),
    .conv_coef_6_2                      (0                         ),
    .conv_coef_7_2                      (0                         ),
    .conv_coef_8_2                      (0                         ),
    .conv_coef_9_2                      (0                         ),

    .conv_bias_0                        (0                    )
);

conv_2channel
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv1_4      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in_1                     (conv6_7_Data_in           ),
    .conv_Data_in_2                     (conv6_8_Data_in           ),
    .conv_Data_out                      (conv6_4_Data_out          ),
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),

    .conv_coef_1_1                        (103                            ),
    .conv_coef_2_1                        (40                             ),
    .conv_coef_3_1                        (205                            ),
    .conv_coef_4_1                        (197                            ),
    .conv_coef_5_1                        (180                            ),
    .conv_coef_6_1                        (130                            ),
    .conv_coef_7_1                        (197                            ),
    .conv_coef_8_1                        (   140                         ),
    .conv_coef_9_1                        (  137                          ),

    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),

    .conv_bias_0                        (75061                         )
);

conv_rgb_y_alignment #(
    .IMG_HDISP(IMG_HDISP),
    .IMG_VDISP(IMG_VDISP),
    .core_witdh(core_witdh)
) u_rgb_y_alignment (
    .clk(clk),
    .rst_n(rst_n),
    .per_img_vsync(per_img_vsync),
    .per_img_href(per_img_href),
    .pixels_in({per_img_r, per_img_g, per_img_b, per_img_Y}),
    .pixels_out({post_img_r, post_img_g, post_img_b, post_img_Y}),
    .post_img_vsync(),
    .post_img_href()
);

endmodule