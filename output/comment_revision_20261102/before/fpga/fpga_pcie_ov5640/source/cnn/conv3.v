`timescale 1ns / 1ps

module conv3
#(
    parameter   [10:0]  IMG_HDISP   = 11'd640,                      //  640*480
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
    //  Image data prepared to be processed
    input  wire                         per_img_vsync              ,//  Prepared Image data vsync valid signal
    input  wire                         per_img_href               ,//  Prepared Image data href vaild  signal
    input  wire        [   7: 0]        conv3_1_Data_in            ,//  Prepared Image brightness input
    input  wire        [   7: 0]        conv3_2_Data_in            ,
    input  wire        [   7: 0]        conv3_3_Data_in            ,
    input  wire        [   7: 0]        conv3_4_Data_in            ,


    //  Image data has been processed
    output wire                         post_img_vsync             ,//  processed Image data vsync valid signal
    output wire                         post_img_href              ,//  processed Image data href vaild  signal
    output wire        [   7: 0]        conv3_1_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv3_2_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv3_3_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv3_4_Data_out            //  processed Image brightness output
    
);
//----------------------------------------------------------------------

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
//卷积核参数
    .conv_coef_1                        (     0                   ),
    .conv_coef_2                        (     0                   ),
    .conv_coef_3                        (     0                  ),
    .conv_coef_4                        (     0                   ),
    .conv_coef_5                        (     0                   ),
    .conv_coef_6                        (     0                   ),
    .conv_coef_7                        (     0                  ),
    .conv_coef_8                        (     0                   ),
    .conv_coef_9                        (     0                  ),
//偏置参数
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
//卷积核参数
    .conv_coef_1                        (        0                 ),
    .conv_coef_2                        (        0                 ),
    .conv_coef_3                        (        0                 ),
    .conv_coef_4                        (        0                 ),
    .conv_coef_5                        (        0                 ),
    .conv_coef_6                        (        0                 ),
    .conv_coef_7                        (        0                 ),
    .conv_coef_8                        (        0                 ),
    .conv_coef_9                        (        0                 ),
//偏置参数
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
//卷积核参数
    .conv_coef_1                        (         0                ),
    .conv_coef_2                        (         0                ),
    .conv_coef_3                        (         0                ),
    .conv_coef_4                        (         0                ),
    .conv_coef_5                        (         0                ),
    .conv_coef_6                        (         0                ),
    .conv_coef_7                        (         0                ),
    .conv_coef_8                        (         0                ),
    .conv_coef_9                        (         0                ),
//偏置参数
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
//卷积核参数
    .conv_coef_1                        (         0                ),
    .conv_coef_2                        (         0                ),
    .conv_coef_3                        (         0                ),
    .conv_coef_4                        (         0                ),
    .conv_coef_5                        (         0                ),
    .conv_coef_6                        (         0                ),
    .conv_coef_7                        (         0                ),
    .conv_coef_8                        (         0                ),
    .conv_coef_9                        (         0                ),
//偏置参数
    .conv_bias_0                        (         0                 ) 
);

///////////////rgb Y ，延迟/////////////////
conv_example_rgb_delay
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                ) 
)
conv1_r      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (per_img_r                 ),
    .post_img_vsync                     (                          ),
    .post_img_href                      (                          ),
    .conv_Data_out                      (post_img_r                ) 
);

conv_example_rgb_delay
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                ) 
)
conv1_g      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (per_img_g                 ),
    .post_img_vsync                     (                          ),
    .post_img_href                      (                          ),
    .conv_Data_out                      (post_img_g                ) 
);

conv_example_rgb_delay
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                ) 
)
conv1_b      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (per_img_b                 ),
    .post_img_vsync                     (                          ),
    .post_img_href                      (                          ),
    .conv_Data_out                      (post_img_b                ) 
);

conv_example_rgb_delay
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                ) 
)
conv1_Y      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in                       (per_img_Y                 ),
    .post_img_vsync                     (                          ),
    .post_img_href                      (                          ),
    .conv_Data_out                      (post_img_Y                ) 
);
endmodule