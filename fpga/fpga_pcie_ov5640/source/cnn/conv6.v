`timescale 1ns / 1ps

module conv6
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
    input  wire        [   7: 0]        conv6_1_Data_in            ,//  Prepared Image brightness input
    input  wire        [   7: 0]        conv6_2_Data_in            ,
    input  wire        [   7: 0]        conv6_3_Data_in            ,
    input  wire        [   7: 0]        conv6_4_Data_in            ,
    input  wire        [   7: 0]        conv6_5_Data_in            ,
    input  wire        [   7: 0]        conv6_6_Data_in            ,
    input  wire        [   7: 0]        conv6_7_Data_in            ,
    input  wire        [   7: 0]        conv6_8_Data_in            ,


    //  Image data has been processed
    output wire                         post_img_vsync             ,//  processed Image data vsync valid signal
    output wire                         post_img_href              ,//  processed Image data href vaild  signal
    output wire        [   7: 0]        conv6_1_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv6_2_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv6_3_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv6_4_Data_out            //  processed Image brightness output
    
);
//----------------------------------------------------------------------
//wire [7:0] conv6_1_Data;
//wire [7:0] conv6_2_Data;
//wire [7:0] conv6_3_Data;
//wire [7:0] conv6_4_Data;
//wire [7:0] conv6_5_Data;
//wire [7:0] conv6_6_Data;
//wire [7:0] conv6_7_Data;
//wire [7:0] conv6_8_Data;
//
//assign conv6_1_Data_out=conv6_1_Data+conv6_2_Data;
//assign conv6_2_Data_out=conv6_3_Data+conv6_4_Data;
//assign conv6_3_Data_out=conv6_5_Data+conv6_6_Data;
//assign conv6_4_Data_out=conv6_7_Data+conv6_8_Data;

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
//卷积核参数1
    .conv_coef_1_1                        (0                         ),
    .conv_coef_2_1                        (0                         ),
    .conv_coef_3_1                        (0                         ),
    .conv_coef_4_1                        (0                         ),
    .conv_coef_5_1                        (0                         ),
    .conv_coef_6_1                        (0                         ),
    .conv_coef_7_1                        (0                         ),
    .conv_coef_8_1                        (0                         ),
    .conv_coef_9_1                        (0                         ),
//卷积核参数2
    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),
//偏置参数
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
//卷积核参数1
    .conv_coef_1_1                        (0                         ),
    .conv_coef_2_1                        (0                         ),
    .conv_coef_3_1                        (0                         ),
    .conv_coef_4_1                        (0                         ),
    .conv_coef_5_1                        (0                         ),
    .conv_coef_6_1                        (0                         ),
    .conv_coef_7_1                        (0                         ),
    .conv_coef_8_1                        (0                         ),
    .conv_coef_9_1                        (0                         ),
//卷积核参数2
    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),
//偏置参数
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
//卷积核参数1
    .conv_coef_1_1                      (0                       ),
    .conv_coef_2_1                      (0                       ),
    .conv_coef_3_1                      (0                       ),
    .conv_coef_4_1                      (0                       ),
    .conv_coef_5_1                      (0                       ),
    .conv_coef_6_1                      (0                       ),
    .conv_coef_7_1                      (0                       ),
    .conv_coef_8_1                      (0                       ),
    .conv_coef_9_1                      (0                       ),
//卷积核参数2
    .conv_coef_1_2                      (0                         ),
    .conv_coef_2_2                      (0                         ),
    .conv_coef_3_2                      (0                         ),
    .conv_coef_4_2                      (0                         ),
    .conv_coef_5_2                      (0                         ),
    .conv_coef_6_2                      (0                         ),
    .conv_coef_7_2                      (0                         ),
    .conv_coef_8_2                      (0                         ),
    .conv_coef_9_2                      (0                         ),
//偏置参数
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
//卷积核参数1
    .conv_coef_1_1                        (103                            ),
    .conv_coef_2_1                        (40                             ),
    .conv_coef_3_1                        (205                            ),
    .conv_coef_4_1                        (197                            ),
    .conv_coef_5_1                        (180                            ),
    .conv_coef_6_1                        (130                            ),
    .conv_coef_7_1                        (197                            ),
    .conv_coef_8_1                        (   140                         ),
    .conv_coef_9_1                        (  137                          ),
//卷积核参数2
    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),
//偏置参数
    .conv_bias_0                        (75061                         ) 
);

//conv_example
//#(  .IMG_HDISP (IMG_HDISP ),
//    .IMG_VDISP                          (IMG_VDISP                 ),
//    .core_witdh                         (core_witdh                ) 
//)
//conv1_1      (
//    .clk                                (clk                       ),
//    .rst_n                              (rst_n                     ),
//    .per_img_vsync                      (per_img_vsync             ),
//    .per_img_href                       (per_img_href              ),
//    .conv_Data_in                       (conv6_1_Data_in           ),
//    .conv_Data_out                      (conv6_1_Data          ),
//    .post_img_vsync                     (post_img_vsync            ),
//    .post_img_href                      (post_img_href             ),
////卷积核参数
//    .conv_coef_1                        (0                       ),
//    .conv_coef_2                        (0                       ),
//    .conv_coef_3                        (0                       ),
//    .conv_coef_4                        (0                       ),
//    .conv_coef_5                        (0                       ),
//    .conv_coef_6                        (0                       ),
//    .conv_coef_7                        (0                       ),
//    .conv_coef_8                        (0                       ),
//    .conv_coef_9                        (0                       ),
////偏置参数
//    .conv_bias_0                        (0                         ) 
//);
//
//conv_example
//#(  .IMG_HDISP (IMG_HDISP ),
//    .IMG_VDISP                          (IMG_VDISP                 ),
//    .core_witdh                         (core_witdh                ) 
//)
//conv1_2      (
//    .clk                                (clk                       ),
//    .rst_n                              (rst_n                     ),
//    .per_img_vsync                      (per_img_vsync             ),
//    .per_img_href                       (per_img_href              ),
//    .conv_Data_in                       (conv6_2_Data_in           ),
//    .conv_Data_out                      (conv6_2_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (0                      ),
//    .conv_coef_2                        (0                      ),
//    .conv_coef_3                        (0                      ),
//    .conv_coef_4                        (0                      ),
//    .conv_coef_5                        (0                      ),
//    .conv_coef_6                        (0                      ),
//    .conv_coef_7                        (0                      ),
//    .conv_coef_8                        (0                      ),
//    .conv_coef_9                        (0                      ),
////偏置参数
//    .conv_bias_0                        (0                         ) 
//);
//
//conv_example
//#(  .IMG_HDISP (IMG_HDISP ),
//    .IMG_VDISP                          (IMG_VDISP                 ),
//    .core_witdh                         (core_witdh                ) 
//)
//conv1_3      (
//    .clk                                (clk                       ),
//    .rst_n                              (rst_n                     ),
//    .per_img_vsync                      (per_img_vsync             ),
//    .per_img_href                       (per_img_href              ),
//    .conv_Data_in                       (conv6_3_Data_in           ),
//    .conv_Data_out                      (conv6_3_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (0                       ),
//    .conv_coef_2                        (0                       ),
//    .conv_coef_3                        (0                       ),
//    .conv_coef_4                        (0                       ),
//    .conv_coef_5                        (0                       ),
//    .conv_coef_6                        (0                       ),
//    .conv_coef_7                        (0                       ),
//    .conv_coef_8                        (0                       ),
//    .conv_coef_9                        (0                       ),
////偏置参数
//    .conv_bias_0                        (0                         ) 
//);
//
//conv_example
//#(  .IMG_HDISP (IMG_HDISP ),
//    .IMG_VDISP                          (IMG_VDISP                 ),
//    .core_witdh                         (core_witdh                ) 
//)
//conv1_4      (
//    .clk                                (clk                       ),
//    .rst_n                              (rst_n                     ),
//    .per_img_vsync                      (per_img_vsync             ),
//    .per_img_href                       (per_img_href              ),
//    .conv_Data_in                       (conv6_4_Data_in           ),
//    .conv_Data_out                      (conv6_4_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (0                       ),
//    .conv_coef_2                        (0                       ),
//    .conv_coef_3                        (0                       ),
//    .conv_coef_4                        (0                       ),
//    .conv_coef_5                        (0                       ),
//    .conv_coef_6                        (0                       ),
//    .conv_coef_7                        (0                       ),
//    .conv_coef_8                        (0                       ),
//    .conv_coef_9                        (0                       ),
////偏置参数
//    .conv_bias_0                        (0                         ) 
//);
//conv_example
//#(  .IMG_HDISP (IMG_HDISP ),
//    .IMG_VDISP                          (IMG_VDISP                 ),
//    .core_witdh                         (core_witdh                ) 
//)
//conv1_5      (
//    .clk                                (clk                       ),
//    .rst_n                              (rst_n                     ),
//    .per_img_vsync                      (per_img_vsync             ),
//    .per_img_href                       (per_img_href              ),
//    .conv_Data_in                       (conv6_5_Data_in           ),
//    .conv_Data_out                      (conv6_5_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (0                      ),
//    .conv_coef_2                        (0                      ),
//    .conv_coef_3                        (0                      ),
//    .conv_coef_4                        (0                      ),
//    .conv_coef_5                        (0                      ),
//    .conv_coef_6                        (0                      ),
//    .conv_coef_7                        (0                      ),
//    .conv_coef_8                        (0                      ),
//    .conv_coef_9                        (0                      ),
////偏置参数
//    .conv_bias_0                        (0                         ) 
//);
//conv_example
//#(  .IMG_HDISP (IMG_HDISP ),
//    .IMG_VDISP                          (IMG_VDISP                 ),
//    .core_witdh                         (core_witdh                ) 
//)
//conv1_6      (
//    .clk                                (clk                       ),
//    .rst_n                              (rst_n                     ),
//    .per_img_vsync                      (per_img_vsync             ),
//    .per_img_href                       (per_img_href              ),
//    .conv_Data_in                       (conv6_6_Data_in           ),
//    .conv_Data_out                      (conv6_6_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (0                       ),
//    .conv_coef_2                        (0                       ),
//    .conv_coef_3                        (0                       ),
//    .conv_coef_4                        (0                       ),
//    .conv_coef_5                        (0                       ),
//    .conv_coef_6                        (0                       ),
//    .conv_coef_7                        (0                       ),
//    .conv_coef_8                        (0                       ),
//    .conv_coef_9                        (0                       ),
////偏置参数
//    .conv_bias_0                        (0                         ) 
//);
//conv_example
//#(  .IMG_HDISP (IMG_HDISP ),
//    .IMG_VDISP                          (IMG_VDISP                 ),
//    .core_witdh                         (core_witdh                ) 
//)
//conv1_7      (
//    .clk                                (clk                       ),
//    .rst_n                              (rst_n                     ),
//    .per_img_vsync                      (per_img_vsync             ),
//    .per_img_href                       (per_img_href              ),
//    .conv_Data_in                       (conv6_7_Data_in           ),
//    .conv_Data_out                      (conv6_7_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数    
//    .conv_coef_1                        (103                       ),
//    .conv_coef_2                        (40                       ),
//    .conv_coef_3                        (205                       ),
//    .conv_coef_4                        (197                       ),
//    .conv_coef_5                        (180                       ),
//    .conv_coef_6                        (130                       ),
//    .conv_coef_7                        (197                       ),
//    .conv_coef_8                        (   140                    ),
//    .conv_coef_9                        (  137                     ),
////偏置参数
//    .conv_bias_0                        (    -2350                     ) 
//);
//conv_example
//#(  .IMG_HDISP (IMG_HDISP ),
//    .IMG_VDISP                          (IMG_VDISP                 ),
//    .core_witdh                         (core_witdh                ) 
//)
//conv1_8      (
//    .clk                                (clk                       ),
//    .rst_n                              (rst_n                     ),
//    .per_img_vsync                      (per_img_vsync             ),
//    .per_img_href                       (per_img_href              ),
//    .conv_Data_in                       (conv6_8_Data_in           ),
//    .conv_Data_out                      (conv6_8_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (0                      ),
//    .conv_coef_2                        (0                      ),
//    .conv_coef_3                        (0                      ),
//    .conv_coef_4                        (0                      ),
//    .conv_coef_5                        (0                      ),
//    .conv_coef_6                        (0                      ),
//    .conv_coef_7                        (0                      ),
//    .conv_coef_8                        (0                      ),
//    .conv_coef_9                        (0                      ),
////偏置参数
//    .conv_bias_0                        (75098                        ) 
//);

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