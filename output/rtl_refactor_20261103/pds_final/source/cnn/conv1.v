`timescale 1ns / 1ps

module conv1
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
    input  wire        [   7: 0]        conv1_1_Data_in            ,// 卷积流水线：Prepared Image brightness input
    input  wire        [   7: 0]        conv1_2_Data_in            ,
    
    // 卷积流水线：Image data has been processed
    output wire                         post_img_vsync             ,// 卷积流水线：processed Image data vsync valid signal
    output wire                         post_img_href              ,// 卷积流水线：processed Image data href vaild signal
    output wire        [   7: 0]        conv1_1_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv1_2_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv1_3_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv1_4_Data_out            // 卷积流水线：processed Image brightness output
    
);
// ── 卷积流水线 ──

// 参数记录为 ///////////2048///////////////。
// 参数记录为 core_witdh = 11。
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
// 本节给出卷积核系数。
    .conv_coef_1                        (     61                   ),
    .conv_coef_2                        (     21                   ),
    .conv_coef_3                        (     84                  ),
    .conv_coef_4                        (     93                   ),
    .conv_coef_5                        (     16                   ),
    .conv_coef_6                        (     15                  ),
    .conv_coef_7                        (     30                  ),
    .conv_coef_8                        (     6                   ),
    .conv_coef_9                        (     108                  ),
// 本节给出运算所用的偏置系数。
    .conv_bias_0                        (       265384                   ) 
);

// 参数记录为 ///////////2048///////////////。
// 参数记录为 core_witdh = 10。
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
// 本节给出卷积核系数。
    .conv_coef_1                        (        54                  ),
    .conv_coef_2                        (        32                  ),
    .conv_coef_3                        (        74                  ),
    .conv_coef_4                        (        88                  ),
    .conv_coef_5                        (        54                  ),
    .conv_coef_6                        (        27                  ),
    .conv_coef_7                        (        18                  ),
    .conv_coef_8                        (        12                  ),
    .conv_coef_9                        (        97                  ),
// 本节给出运算所用的偏置系数。
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
// 本节给出卷积核系数。
    .conv_coef_1                        (         0                 ),
    .conv_coef_2                        (         0                 ),
    .conv_coef_3                        (         0                 ),
    .conv_coef_4                        (         0                 ),
    .conv_coef_5                        (         0                 ),
    .conv_coef_6                        (         0                 ),
    .conv_coef_7                        (         0                 ),
    .conv_coef_8                        (         0                 ),
    .conv_coef_9                        (         0                 ),
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
    .conv_Data_in                       (conv1_2_Data_in           ),
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),
    .conv_Data_out                      (conv1_4_Data_out          ), 
// 本节给出卷积核系数。
    .conv_coef_1                        (         0                 ),
    .conv_coef_2                        (         0                 ),
    .conv_coef_3                        (         0                 ),
    .conv_coef_4                        (         0                 ),
    .conv_coef_5                        (         0                 ),
    .conv_coef_6                        (         0                 ),
    .conv_coef_7                        (         0                 ),
    .conv_coef_8                        (         0                 ),
    .conv_coef_9                        (         0                 ),
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