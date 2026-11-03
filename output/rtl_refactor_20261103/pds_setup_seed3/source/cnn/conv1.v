// 2026-11-03 注释校订。第 1 层卷积。卷积支路与 RGBY 对齐支路并行推进，各自保留原取样级。
`timescale 1ns / 1ps

module conv1
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
    input  wire        [   7: 0]        conv1_1_Data_in            ,
    input  wire        [   7: 0]        conv1_2_Data_in            ,

    output wire                         post_img_vsync             ,
    output wire                         post_img_href              ,
    output wire        [   7: 0]        conv1_1_Data_out           ,
    output wire        [   7: 0]        conv1_2_Data_out           ,
    output wire        [   7: 0]        conv1_3_Data_out           ,
    output wire        [   7: 0]        conv1_4_Data_out

);

conv_example_1_1
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv1_1      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (conv1_1_Data_in           ),
    .post_img_vsync                     (     post_img_vsync       ),
    .post_img_href                      (     post_img_href        ),
    .conv_Data_out                      (conv1_1_Data_out          ),

    .conv_coef_1                        (     61                   ),
    .conv_coef_2                        (     21                   ),
    .conv_coef_3                        (     84                  ),
    .conv_coef_4                        (     93                   ),
    .conv_coef_5                        (     16                   ),
    .conv_coef_6                        (     15                  ),
    .conv_coef_7                        (     30                  ),
    .conv_coef_8                        (     6                   ),
    .conv_coef_9                        (     108                  ),

    .conv_bias_0                        (       265384                   )
);

conv_example
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv1_2      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (conv1_1_Data_in           ),
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),
    .conv_Data_out                      (conv1_2_Data_out          ),

    .conv_coef_1                        (        54                  ),
    .conv_coef_2                        (        32                  ),
    .conv_coef_3                        (        74                  ),
    .conv_coef_4                        (        88                  ),
    .conv_coef_5                        (        54                  ),
    .conv_coef_6                        (        27                  ),
    .conv_coef_7                        (        18                  ),
    .conv_coef_8                        (        12                  ),
    .conv_coef_9                        (        97                  ),

    .conv_bias_0                        (         208080                 )
);

conv_example
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv1_3      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (conv1_2_Data_in           ),
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),
    .conv_Data_out                      (conv1_3_Data_out          ),

    .conv_coef_1                        (         0                 ),
    .conv_coef_2                        (         0                 ),
    .conv_coef_3                        (         0                 ),
    .conv_coef_4                        (         0                 ),
    .conv_coef_5                        (         0                 ),
    .conv_coef_6                        (         0                 ),
    .conv_coef_7                        (         0                 ),
    .conv_coef_8                        (         0                 ),
    .conv_coef_9                        (         0                 ),

    .conv_bias_0                        (         0                 )
);

conv_example
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv1_4      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (conv1_2_Data_in           ),
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),
    .conv_Data_out                      (conv1_4_Data_out          ),

    .conv_coef_1                        (         0                 ),
    .conv_coef_2                        (         0                 ),
    .conv_coef_3                        (         0                 ),
    .conv_coef_4                        (         0                 ),
    .conv_coef_5                        (         0                 ),
    .conv_coef_6                        (         0                 ),
    .conv_coef_7                        (         0                 ),
    .conv_coef_8                        (         0                 ),
    .conv_coef_9                        (         0                 ),

    .conv_bias_0                        (         0                 )
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