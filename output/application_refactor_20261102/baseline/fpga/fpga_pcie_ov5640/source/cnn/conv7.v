`timescale 1ns / 1ps

module conv7
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
    input  wire        [   7: 0]        conv7_1_Data_in            ,//  Prepared Image brightness input
    input  wire        [   7: 0]        conv7_2_Data_in            ,
    input  wire        [   7: 0]        conv7_3_Data_in            ,
    input  wire        [   7: 0]        conv7_4_Data_in            ,
    input  wire        [   7: 0]        conv7_5_Data_in            ,
    input  wire        [   7: 0]        conv7_6_Data_in            ,
    input  wire        [   7: 0]        conv7_7_Data_in            ,
    input  wire        [   7: 0]        conv7_8_Data_in            ,


    //  Image data has been processed
    output wire                         post_img_vsync             ,//  processed Image data vsync valid signal
    output wire                         post_img_href              ,//  processed Image data href vaild  signal
    output wire        [   7: 0]        conv7_1_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv7_2_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv7_3_Data_out           ,//  processed Image brightness output
    output wire        [   7: 0]        conv7_4_Data_out            //  processed Image brightness output
    
);
//----------------------------------------------------------------------


conv_2channel_7
#(  .IMG_HDISP (IMG_HDISP ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                ) 
)
conv1_1      (
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv_Data_in_1                     (conv7_1_Data_in           ),
    .conv_Data_in_2                     (conv7_2_Data_in           ),
    .conv_Data_out                      (conv7_1_Data_out          ),
    .post_img_vsync                     (post_img_vsync            ),
    .post_img_href                      (post_img_href             ),
//卷积核参数1
    .conv_coef_1_1                      (182                      ),
    .conv_coef_2_1                      (418                       ),
    .conv_coef_3_1                      (108                      ),
    .conv_coef_4_1                      (392                       ),
    .conv_coef_5_1                      (680                       ),
    .conv_coef_6_1                      (395                       ),
    .conv_coef_7_1                      (141                      ),
    .conv_coef_8_1                      (371                       ),
    .conv_coef_9_1                      (115                      ),
//卷积核参数2
    .conv_coef_1_2                      (285                      ),
    .conv_coef_2_2                      (349                       ),
    .conv_coef_3_2                      (274                      ),
    .conv_coef_4_2                      (352                       ),
    .conv_coef_5_2                      (690                       ),
    .conv_coef_6_2                      (285                       ),
    .conv_coef_7_2                      (242                      ),
    .conv_coef_8_2                      (312                       ),
    .conv_coef_9_2                      (271                      ),
//偏置参数
    .conv_bias_0                        (158500                    ) 
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
    .conv_Data_in_1                     (conv7_3_Data_in           ),
    .conv_Data_in_2                     (conv7_4_Data_in           ),
    .conv_Data_out                      (conv7_2_Data_out          ),
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
    .conv_Data_in_1                     (conv7_5_Data_in           ),
    .conv_Data_in_2                     (conv7_6_Data_in           ),
    .conv_Data_out                      (conv7_3_Data_out          ),
    .post_img_vsync                     (            ),
    .post_img_href                      (            ),
//卷积核参数1
    .conv_coef_1_1                      (0                      ),
    .conv_coef_2_1                      (0                      ),
    .conv_coef_3_1                      (0                      ),
    .conv_coef_4_1                      (0                      ),
    .conv_coef_5_1                      (0                      ),
    .conv_coef_6_1                      (0                      ),
    .conv_coef_7_1                      (0                      ),
    .conv_coef_8_1                      (0                      ),
    .conv_coef_9_1                      (0                      ),
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
    .conv_bias_0                        (0                     ) 
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
    .conv_Data_in_1                     (conv7_7_Data_in           ),
    .conv_Data_in_2                     (conv7_8_Data_in           ),
    .conv_Data_out                      (conv7_4_Data_out          ),
    .post_img_vsync                     (                          ),
    .post_img_href                      (                          ),
//卷积核参数1
    .conv_coef_1_1                      (0                         ),
    .conv_coef_2_1                      (0                         ),
    .conv_coef_3_1                      (0                         ),
    .conv_coef_4_1                      (0                         ),
    .conv_coef_5_1                      (0                         ),
    .conv_coef_6_1                      (0                         ),
    .conv_coef_7_1                      (0                         ),
    .conv_coef_8_1                      (0                         ),
    .conv_coef_9_1                      (0                         ),
//卷积核参数2
    .conv_coef_1_2                      (56                        ),
    .conv_coef_2_2                      (162                       ),
    .conv_coef_3_2                      (66                        ),
    .conv_coef_4_2                      (271                       ),
    .conv_coef_5_2                      (329                       ),
    .conv_coef_6_2                      (157                       ),
    .conv_coef_7_2                      (75                        ),
    .conv_coef_8_2                      (183                       ),
    .conv_coef_9_2                      (60                        ),
//偏置参数
    .conv_bias_0                        (208739                    ) 
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
//    .conv_Data_in                       (conv7_1_Data_in           ),
//    .conv_Data_out                      (conv7_1_Data          ),
//    .post_img_vsync                     (post_img_vsync            ),
//    .post_img_href                      (post_img_href             ),
////卷积核参数
//    .conv_coef_1                        (76                        ),
//    .conv_coef_2                        (126                       ),
//    .conv_coef_3                        (76                        ),
//    .conv_coef_4                        (126                       ),
//    .conv_coef_5                        (209                       ),
//    .conv_coef_6                        (126                       ),
//    .conv_coef_7                        (76                        ),
//    .conv_coef_8                        (126                       ),
//    .conv_coef_9                        (76                        ),
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
//    .conv_Data_in                       (conv7_2_Data_in           ),
//    .conv_Data_out                      (conv7_2_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (76                        ),
//    .conv_coef_2                        (126                       ),
//    .conv_coef_3                        (76                        ),
//    .conv_coef_4                        (126                       ),
//    .conv_coef_5                        (209                       ),
//    .conv_coef_6                        (126                       ),
//    .conv_coef_7                        (76                        ),
//    .conv_coef_8                        (126                       ),
//    .conv_coef_9                        (76                        ),
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
//    .conv_Data_in                       (conv7_3_Data_in           ),
//    .conv_Data_out                      (conv7_3_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (76                        ),
//    .conv_coef_2                        (126                       ),
//    .conv_coef_3                        (76                        ),
//    .conv_coef_4                        (126                       ),
//    .conv_coef_5                        (209                       ),
//    .conv_coef_6                        (126                       ),
//    .conv_coef_7                        (76                        ),
//    .conv_coef_8                        (126                       ),
//    .conv_coef_9                        (76                        ),
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
//    .conv_Data_in                       (conv7_4_Data_in           ),
//    .conv_Data_out                      (conv7_4_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (76                        ),
//    .conv_coef_2                        (126                       ),
//    .conv_coef_3                        (76                        ),
//    .conv_coef_4                        (126                       ),
//    .conv_coef_5                        (209                       ),
//    .conv_coef_6                        (126                       ),
//    .conv_coef_7                        (76                        ),
//    .conv_coef_8                        (126                       ),
//    .conv_coef_9                        (76                        ),
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
//    .conv_Data_in                       (conv7_5_Data_in           ),
//    .conv_Data_out                      (conv7_5_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (76                        ),
//    .conv_coef_2                        (126                       ),
//    .conv_coef_3                        (76                        ),
//    .conv_coef_4                        (126                       ),
//    .conv_coef_5                        (209                       ),
//    .conv_coef_6                        (126                       ),
//    .conv_coef_7                        (76                        ),
//    .conv_coef_8                        (126                       ),
//    .conv_coef_9                        (76                        ),
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
//    .conv_Data_in                       (conv7_6_Data_in           ),
//    .conv_Data_out                      (conv7_6_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (76                        ),
//    .conv_coef_2                        (126                       ),
//    .conv_coef_3                        (76                        ),
//    .conv_coef_4                        (126                       ),
//    .conv_coef_5                        (209                       ),
//    .conv_coef_6                        (126                       ),
//    .conv_coef_7                        (76                        ),
//    .conv_coef_8                        (126                       ),
//    .conv_coef_9                        (76                        ),
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
//    .conv_Data_in                       (conv7_7_Data_in           ),
//    .conv_Data_out                      (conv7_7_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数    
//    .conv_coef_1                        (76                        ),
//    .conv_coef_2                        (126                       ),
//    .conv_coef_3                        (76                        ),
//    .conv_coef_4                        (126                       ),
//    .conv_coef_5                        (209                       ),
//    .conv_coef_6                        (126                       ),
//    .conv_coef_7                        (76                        ),
//    .conv_coef_8                        (126                       ),
//    .conv_coef_9                        (76                        ),
////偏置参数
//    .conv_bias_0                        (0                         ) 
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
//    .conv_Data_in                       (conv7_8_Data_in           ),
//    .conv_Data_out                      (conv7_8_Data          ),
//    .post_img_vsync                     (                          ),
//    .post_img_href                      (                          ),
////卷积核参数
//    .conv_coef_1                        (76                        ),
//    .conv_coef_2                        (126                       ),
//    .conv_coef_3                        (76                        ),
//    .conv_coef_4                        (126                       ),
//    .conv_coef_5                        (209                       ),
//    .conv_coef_6                        (126                       ),
//    .conv_coef_7                        (76                        ),
//    .conv_coef_8                        (126                       ),
//    .conv_coef_9                        (76                        ),
////偏置参数
//    .conv_bias_0                        (0                         ) 
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