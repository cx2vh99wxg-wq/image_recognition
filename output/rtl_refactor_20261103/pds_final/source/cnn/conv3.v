`timescale 1ns / 1ps

module conv3
#(
    parameter   [10:0]  IMG_HDISP   = 11'd640,                      // 参数记录为 640*480。
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
    // 卷积流水线：Image data prepared to be processed
    input  wire                         per_img_vsync              ,// 卷积流水线：Prepared Image data vsync valid signal
    input  wire                         per_img_href               ,// 卷积流水线：Prepared Image data href vaild signal
    input  wire        [   7: 0]        conv3_1_Data_in            ,// 卷积流水线：Prepared Image brightness input
    input  wire        [   7: 0]        conv3_2_Data_in            ,
    input  wire        [   7: 0]        conv3_3_Data_in            ,
    input  wire        [   7: 0]        conv3_4_Data_in            ,


    // 卷积流水线：Image data has been processed
    output wire                         post_img_vsync             ,// 卷积流水线：processed Image data vsync valid signal
    output wire                         post_img_href              ,// 卷积流水线：processed Image data href vaild signal
    output wire        [   7: 0]        conv3_1_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv3_2_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv3_3_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv3_4_Data_out            // 卷积流水线：processed Image brightness output
    
);
// ── 卷积流水线 ──

conv_example
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                ) 
)
conv1_1      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (conv3_1_Data_in           ),
    .conv_Data_out                      (conv3_1_Data_out          ), 
    .post_img_vsync                     (     post_img_vsync       ),
    .post_img_href                      (     post_img_href        ),
// 本节给出卷积核系数。
    .conv_coef_1                        (     0                   ),
    .conv_coef_2                        (     0                   ),
    .conv_coef_3                        (     0                  ),
    .conv_coef_4                        (     0                   ),
    .conv_coef_5                        (     0                   ),
    .conv_coef_6                        (     0                   ),
    .conv_coef_7                        (     0                  ),
    .conv_coef_8                        (     0                   ),
    .conv_coef_9                        (     0                  ),
// 本节给出运算所用的偏置系数。
    .conv_bias_0                        (       0                   ) 
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
    .conv_Data_in                       (conv3_2_Data_in           ),
    .conv_Data_out                      (conv3_2_Data_out          ), 
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),
// 本节给出卷积核系数。
    .conv_coef_1                        (        0                 ),
    .conv_coef_2                        (        0                 ),
    .conv_coef_3                        (        0                 ),
    .conv_coef_4                        (        0                 ),
    .conv_coef_5                        (        0                 ),
    .conv_coef_6                        (        0                 ),
    .conv_coef_7                        (        0                 ),
    .conv_coef_8                        (        0                 ),
    .conv_coef_9                        (        0                 ),
// 本节给出运算所用的偏置系数。
    .conv_bias_0                        (         0                 ) 
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
    .conv_Data_in                       (conv3_3_Data_in           ),
    .conv_Data_out                      (conv3_3_Data_out          ), 
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),
// 本节给出卷积核系数。
    .conv_coef_1                        (         0                ),
    .conv_coef_2                        (         0                ),
    .conv_coef_3                        (         0                ),
    .conv_coef_4                        (         0                ),
    .conv_coef_5                        (         0                ),
    .conv_coef_6                        (         0                ),
    .conv_coef_7                        (         0                ),
    .conv_coef_8                        (         0                ),
    .conv_coef_9                        (         0                ),
// 本节给出运算所用的偏置系数。
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
    .conv_Data_in                       (conv3_4_Data_in           ),
    .conv_Data_out                      (conv3_4_Data_out          ), 
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),
// 本节给出卷积核系数。
    .conv_coef_1                        (         0                ),
    .conv_coef_2                        (         0                ),
    .conv_coef_3                        (         0                ),
    .conv_coef_4                        (         0                ),
    .conv_coef_5                        (         0                ),
    .conv_coef_6                        (         0                ),
    .conv_coef_7                        (         0                ),
    .conv_coef_8                        (         0                ),
    .conv_coef_9                        (         0                ),
// 本节给出运算所用的偏置系数。
    .conv_bias_0                        (         0                 ) 
);

// rgb Y ，延迟。

// 2026-11-03: one shared four-channel delay connection per convolution layer.
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