`timescale 1ns / 1ps

module conv6
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
    input  wire        [   7: 0]        conv6_1_Data_in            ,// 卷积流水线：Prepared Image brightness input
    input  wire        [   7: 0]        conv6_2_Data_in            ,
    input  wire        [   7: 0]        conv6_3_Data_in            ,
    input  wire        [   7: 0]        conv6_4_Data_in            ,
    input  wire        [   7: 0]        conv6_5_Data_in            ,
    input  wire        [   7: 0]        conv6_6_Data_in            ,
    input  wire        [   7: 0]        conv6_7_Data_in            ,
    input  wire        [   7: 0]        conv6_8_Data_in            ,


    // 卷积流水线：Image data has been processed
    output wire                         post_img_vsync             ,// 卷积流水线：processed Image data vsync valid signal
    output wire                         post_img_href              ,// 卷积流水线：processed Image data href vaild signal
    output wire        [   7: 0]        conv6_1_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv6_2_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv6_3_Data_out           ,// 卷积流水线：processed Image brightness output
    output wire        [   7: 0]        conv6_4_Data_out            // 卷积流水线：processed Image brightness output
    
);
// ── 卷积流水线 ──
// 停用示例，仅供对照：wire [7:0] conv6_1_Data;
// 停用示例，仅供对照：wire [7:0] conv6_2_Data;
// 停用示例，仅供对照：wire [7:0] conv6_3_Data;
// 停用示例，仅供对照：wire [7:0] conv6_4_Data;
// 停用示例，仅供对照：wire [7:0] conv6_5_Data;
// 停用示例，仅供对照：wire [7:0] conv6_6_Data;
// 停用示例，仅供对照：wire [7:0] conv6_7_Data;
// 停用示例，仅供对照：wire [7:0] conv6_8_Data;
// ── 卷积流水线 ──
// 停用示例，仅供对照：assign conv6_1_Data_out=conv6_1_Data+conv6_2_Data;
// 停用示例，仅供对照：assign conv6_2_Data_out=conv6_3_Data+conv6_4_Data;
// 停用示例，仅供对照：assign conv6_3_Data_out=conv6_5_Data+conv6_6_Data;
// 停用示例，仅供对照：assign conv6_4_Data_out=conv6_7_Data+conv6_8_Data;

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
// 第一组卷积核的系数。
    .conv_coef_1_1                        (0                         ),
    .conv_coef_2_1                        (0                         ),
    .conv_coef_3_1                        (0                         ),
    .conv_coef_4_1                        (0                         ),
    .conv_coef_5_1                        (0                         ),
    .conv_coef_6_1                        (0                         ),
    .conv_coef_7_1                        (0                         ),
    .conv_coef_8_1                        (0                         ),
    .conv_coef_9_1                        (0                         ),
// 第二组卷积核的系数。
    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),
// 本节给出运算所用的偏置系数。
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
// 第一组卷积核的系数。
    .conv_coef_1_1                        (0                         ),
    .conv_coef_2_1                        (0                         ),
    .conv_coef_3_1                        (0                         ),
    .conv_coef_4_1                        (0                         ),
    .conv_coef_5_1                        (0                         ),
    .conv_coef_6_1                        (0                         ),
    .conv_coef_7_1                        (0                         ),
    .conv_coef_8_1                        (0                         ),
    .conv_coef_9_1                        (0                         ),
// 第二组卷积核的系数。
    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),
// 本节给出运算所用的偏置系数。
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
// 第一组卷积核的系数。
    .conv_coef_1_1                      (0                       ),
    .conv_coef_2_1                      (0                       ),
    .conv_coef_3_1                      (0                       ),
    .conv_coef_4_1                      (0                       ),
    .conv_coef_5_1                      (0                       ),
    .conv_coef_6_1                      (0                       ),
    .conv_coef_7_1                      (0                       ),
    .conv_coef_8_1                      (0                       ),
    .conv_coef_9_1                      (0                       ),
// 第二组卷积核的系数。
    .conv_coef_1_2                      (0                         ),
    .conv_coef_2_2                      (0                         ),
    .conv_coef_3_2                      (0                         ),
    .conv_coef_4_2                      (0                         ),
    .conv_coef_5_2                      (0                         ),
    .conv_coef_6_2                      (0                         ),
    .conv_coef_7_2                      (0                         ),
    .conv_coef_8_2                      (0                         ),
    .conv_coef_9_2                      (0                         ),
// 本节给出运算所用的偏置系数。
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
// 第一组卷积核的系数。
    .conv_coef_1_1                        (103                            ),
    .conv_coef_2_1                        (40                             ),
    .conv_coef_3_1                        (205                            ),
    .conv_coef_4_1                        (197                            ),
    .conv_coef_5_1                        (180                            ),
    .conv_coef_6_1                        (130                            ),
    .conv_coef_7_1                        (197                            ),
    .conv_coef_8_1                        (   140                         ),
    .conv_coef_9_1                        (  137                          ),
// 第二组卷积核的系数。
    .conv_coef_1_2                        (0                         ),
    .conv_coef_2_2                        (0                         ),
    .conv_coef_3_2                        (0                         ),
    .conv_coef_4_2                        (0                         ),
    .conv_coef_5_2                        (0                         ),
    .conv_coef_6_2                        (0                         ),
    .conv_coef_7_2                        (0                         ),
    .conv_coef_8_2                        (0                         ),
    .conv_coef_9_2                        (0                         ),
// 本节给出运算所用的偏置系数。
    .conv_bias_0                        (75061                         ) 
);

// conv_example：本节对应的标识或作用域。
// 停用示例，仅供对照：#( .IMG_HDISP (IMG_HDISP ),
// 停用示例，仅供对照：.IMG_VDISP (IMG_VDISP ),
// 卷积流水线：.core_witdh (core_witdh )
// 卷积流水线：)
// 参数记录为 conv1_1 (。
// 停用示例，仅供对照：.clk (clk ),
// 停用示例，仅供对照：.rst_n (rst_n ),
// 停用示例，仅供对照：.per_img_vsync (per_img_vsync ),
// 停用示例，仅供对照：.per_img_href (per_img_href ),
// 停用示例，仅供对照：.conv_Data_in (conv6_1_Data_in ),
// 停用示例，仅供对照：.conv_Data_out (conv6_1_Data ),
// 停用示例，仅供对照：.post_img_vsync (post_img_vsync ),
// 停用示例，仅供对照：.post_img_href (post_img_href ),
// //卷积核所使用的配置参数。
// 停用示例，仅供对照：.conv_coef_1 (0 ),
// 停用示例，仅供对照：.conv_coef_2 (0 ),
// 停用示例，仅供对照：.conv_coef_3 (0 ),
// 停用示例，仅供对照：.conv_coef_4 (0 ),
// 停用示例，仅供对照：.conv_coef_5 (0 ),
// 停用示例，仅供对照：.conv_coef_6 (0 ),
// 停用示例，仅供对照：.conv_coef_7 (0 ),
// 停用示例，仅供对照：.conv_coef_8 (0 ),
// 停用示例，仅供对照：.conv_coef_9 (0 ),
// //偏置所使用的配置参数。
// 参数记录为 .conv_bias_0 (0 )。
// 卷积流水线：);
// ── 卷积流水线 ──
// conv_example：本节对应的标识或作用域。
// 停用示例，仅供对照：#( .IMG_HDISP (IMG_HDISP ),
// 停用示例，仅供对照：.IMG_VDISP (IMG_VDISP ),
// 卷积流水线：.core_witdh (core_witdh )
// 卷积流水线：)
// 参数记录为 conv1_2 (。
// 停用示例，仅供对照：.clk (clk ),
// 停用示例，仅供对照：.rst_n (rst_n ),
// 停用示例，仅供对照：.per_img_vsync (per_img_vsync ),
// 停用示例，仅供对照：.per_img_href (per_img_href ),
// 停用示例，仅供对照：.conv_Data_in (conv6_2_Data_in ),
// 停用示例，仅供对照：.conv_Data_out (conv6_2_Data ),
// 停用示例，仅供对照：.post_img_vsync ( ),
// 停用示例，仅供对照：.post_img_href ( ),
// //卷积核所使用的配置参数。
// 停用示例，仅供对照：.conv_coef_1 (0 ),
// 停用示例，仅供对照：.conv_coef_2 (0 ),
// 停用示例，仅供对照：.conv_coef_3 (0 ),
// 停用示例，仅供对照：.conv_coef_4 (0 ),
// 停用示例，仅供对照：.conv_coef_5 (0 ),
// 停用示例，仅供对照：.conv_coef_6 (0 ),
// 停用示例，仅供对照：.conv_coef_7 (0 ),
// 停用示例，仅供对照：.conv_coef_8 (0 ),
// 停用示例，仅供对照：.conv_coef_9 (0 ),
// //偏置所使用的配置参数。
// 参数记录为 .conv_bias_0 (0 )。
// 卷积流水线：);
// ── 卷积流水线 ──
// conv_example：本节对应的标识或作用域。
// 停用示例，仅供对照：#( .IMG_HDISP (IMG_HDISP ),
// 停用示例，仅供对照：.IMG_VDISP (IMG_VDISP ),
// 卷积流水线：.core_witdh (core_witdh )
// 卷积流水线：)
// 参数记录为 conv1_3 (。
// 停用示例，仅供对照：.clk (clk ),
// 停用示例，仅供对照：.rst_n (rst_n ),
// 停用示例，仅供对照：.per_img_vsync (per_img_vsync ),
// 停用示例，仅供对照：.per_img_href (per_img_href ),
// 停用示例，仅供对照：.conv_Data_in (conv6_3_Data_in ),
// 停用示例，仅供对照：.conv_Data_out (conv6_3_Data ),
// 停用示例，仅供对照：.post_img_vsync ( ),
// 停用示例，仅供对照：.post_img_href ( ),
// //卷积核所使用的配置参数。
// 停用示例，仅供对照：.conv_coef_1 (0 ),
// 停用示例，仅供对照：.conv_coef_2 (0 ),
// 停用示例，仅供对照：.conv_coef_3 (0 ),
// 停用示例，仅供对照：.conv_coef_4 (0 ),
// 停用示例，仅供对照：.conv_coef_5 (0 ),
// 停用示例，仅供对照：.conv_coef_6 (0 ),
// 停用示例，仅供对照：.conv_coef_7 (0 ),
// 停用示例，仅供对照：.conv_coef_8 (0 ),
// 停用示例，仅供对照：.conv_coef_9 (0 ),
// //偏置所使用的配置参数。
// 参数记录为 .conv_bias_0 (0 )。
// 卷积流水线：);
// ── 卷积流水线 ──
// conv_example：本节对应的标识或作用域。
// 停用示例，仅供对照：#( .IMG_HDISP (IMG_HDISP ),
// 停用示例，仅供对照：.IMG_VDISP (IMG_VDISP ),
// 卷积流水线：.core_witdh (core_witdh )
// 卷积流水线：)
// 参数记录为 conv1_4 (。
// 停用示例，仅供对照：.clk (clk ),
// 停用示例，仅供对照：.rst_n (rst_n ),
// 停用示例，仅供对照：.per_img_vsync (per_img_vsync ),
// 停用示例，仅供对照：.per_img_href (per_img_href ),
// 停用示例，仅供对照：.conv_Data_in (conv6_4_Data_in ),
// 停用示例，仅供对照：.conv_Data_out (conv6_4_Data ),
// 停用示例，仅供对照：.post_img_vsync ( ),
// 停用示例，仅供对照：.post_img_href ( ),
// //卷积核所使用的配置参数。
// 停用示例，仅供对照：.conv_coef_1 (0 ),
// 停用示例，仅供对照：.conv_coef_2 (0 ),
// 停用示例，仅供对照：.conv_coef_3 (0 ),
// 停用示例，仅供对照：.conv_coef_4 (0 ),
// 停用示例，仅供对照：.conv_coef_5 (0 ),
// 停用示例，仅供对照：.conv_coef_6 (0 ),
// 停用示例，仅供对照：.conv_coef_7 (0 ),
// 停用示例，仅供对照：.conv_coef_8 (0 ),
// 停用示例，仅供对照：.conv_coef_9 (0 ),
// //偏置所使用的配置参数。
// 参数记录为 .conv_bias_0 (0 )。
// 卷积流水线：);
// conv_example：本节对应的标识或作用域。
// 停用示例，仅供对照：#( .IMG_HDISP (IMG_HDISP ),
// 停用示例，仅供对照：.IMG_VDISP (IMG_VDISP ),
// 卷积流水线：.core_witdh (core_witdh )
// 卷积流水线：)
// 参数记录为 conv1_5 (。
// 停用示例，仅供对照：.clk (clk ),
// 停用示例，仅供对照：.rst_n (rst_n ),
// 停用示例，仅供对照：.per_img_vsync (per_img_vsync ),
// 停用示例，仅供对照：.per_img_href (per_img_href ),
// 停用示例，仅供对照：.conv_Data_in (conv6_5_Data_in ),
// 停用示例，仅供对照：.conv_Data_out (conv6_5_Data ),
// 停用示例，仅供对照：.post_img_vsync ( ),
// 停用示例，仅供对照：.post_img_href ( ),
// //卷积核所使用的配置参数。
// 停用示例，仅供对照：.conv_coef_1 (0 ),
// 停用示例，仅供对照：.conv_coef_2 (0 ),
// 停用示例，仅供对照：.conv_coef_3 (0 ),
// 停用示例，仅供对照：.conv_coef_4 (0 ),
// 停用示例，仅供对照：.conv_coef_5 (0 ),
// 停用示例，仅供对照：.conv_coef_6 (0 ),
// 停用示例，仅供对照：.conv_coef_7 (0 ),
// 停用示例，仅供对照：.conv_coef_8 (0 ),
// 停用示例，仅供对照：.conv_coef_9 (0 ),
// //偏置所使用的配置参数。
// 参数记录为 .conv_bias_0 (0 )。
// 卷积流水线：);
// conv_example：本节对应的标识或作用域。
// 停用示例，仅供对照：#( .IMG_HDISP (IMG_HDISP ),
// 停用示例，仅供对照：.IMG_VDISP (IMG_VDISP ),
// 卷积流水线：.core_witdh (core_witdh )
// 卷积流水线：)
// 参数记录为 conv1_6 (。
// 停用示例，仅供对照：.clk (clk ),
// 停用示例，仅供对照：.rst_n (rst_n ),
// 停用示例，仅供对照：.per_img_vsync (per_img_vsync ),
// 停用示例，仅供对照：.per_img_href (per_img_href ),
// 停用示例，仅供对照：.conv_Data_in (conv6_6_Data_in ),
// 停用示例，仅供对照：.conv_Data_out (conv6_6_Data ),
// 停用示例，仅供对照：.post_img_vsync ( ),
// 停用示例，仅供对照：.post_img_href ( ),
// //卷积核所使用的配置参数。
// 停用示例，仅供对照：.conv_coef_1 (0 ),
// 停用示例，仅供对照：.conv_coef_2 (0 ),
// 停用示例，仅供对照：.conv_coef_3 (0 ),
// 停用示例，仅供对照：.conv_coef_4 (0 ),
// 停用示例，仅供对照：.conv_coef_5 (0 ),
// 停用示例，仅供对照：.conv_coef_6 (0 ),
// 停用示例，仅供对照：.conv_coef_7 (0 ),
// 停用示例，仅供对照：.conv_coef_8 (0 ),
// 停用示例，仅供对照：.conv_coef_9 (0 ),
// //偏置所使用的配置参数。
// 参数记录为 .conv_bias_0 (0 )。
// 卷积流水线：);
// conv_example：本节对应的标识或作用域。
// 停用示例，仅供对照：#( .IMG_HDISP (IMG_HDISP ),
// 停用示例，仅供对照：.IMG_VDISP (IMG_VDISP ),
// 卷积流水线：.core_witdh (core_witdh )
// 卷积流水线：)
// 参数记录为 conv1_7 (。
// 停用示例，仅供对照：.clk (clk ),
// 停用示例，仅供对照：.rst_n (rst_n ),
// 停用示例，仅供对照：.per_img_vsync (per_img_vsync ),
// 停用示例，仅供对照：.per_img_href (per_img_href ),
// 停用示例，仅供对照：.conv_Data_in (conv6_7_Data_in ),
// 停用示例，仅供对照：.conv_Data_out (conv6_7_Data ),
// 停用示例，仅供对照：.post_img_vsync ( ),
// 停用示例，仅供对照：.post_img_href ( ),
// //卷积核所使用的配置参数。
// 停用示例，仅供对照：.conv_coef_1 (103 ),
// 停用示例，仅供对照：.conv_coef_2 (40 ),
// 停用示例，仅供对照：.conv_coef_3 (205 ),
// 停用示例，仅供对照：.conv_coef_4 (197 ),
// 停用示例，仅供对照：.conv_coef_5 (180 ),
// 停用示例，仅供对照：.conv_coef_6 (130 ),
// 停用示例，仅供对照：.conv_coef_7 (197 ),
// 停用示例，仅供对照：.conv_coef_8 ( 140 ),
// 停用示例，仅供对照：.conv_coef_9 ( 137 ),
// //偏置所使用的配置参数。
// 参数记录为 .conv_bias_0 ( -2350 )。
// 卷积流水线：);
// conv_example：本节对应的标识或作用域。
// 停用示例，仅供对照：#( .IMG_HDISP (IMG_HDISP ),
// 停用示例，仅供对照：.IMG_VDISP (IMG_VDISP ),
// 卷积流水线：.core_witdh (core_witdh )
// 卷积流水线：)
// 参数记录为 conv1_8 (。
// 停用示例，仅供对照：.clk (clk ),
// 停用示例，仅供对照：.rst_n (rst_n ),
// 停用示例，仅供对照：.per_img_vsync (per_img_vsync ),
// 停用示例，仅供对照：.per_img_href (per_img_href ),
// 停用示例，仅供对照：.conv_Data_in (conv6_8_Data_in ),
// 停用示例，仅供对照：.conv_Data_out (conv6_8_Data ),
// 停用示例，仅供对照：.post_img_vsync ( ),
// 停用示例，仅供对照：.post_img_href ( ),
// //卷积核所使用的配置参数。
// 停用示例，仅供对照：.conv_coef_1 (0 ),
// 停用示例，仅供对照：.conv_coef_2 (0 ),
// 停用示例，仅供对照：.conv_coef_3 (0 ),
// 停用示例，仅供对照：.conv_coef_4 (0 ),
// 停用示例，仅供对照：.conv_coef_5 (0 ),
// 停用示例，仅供对照：.conv_coef_6 (0 ),
// 停用示例，仅供对照：.conv_coef_7 (0 ),
// 停用示例，仅供对照：.conv_coef_8 (0 ),
// 停用示例，仅供对照：.conv_coef_9 (0 ),
// //偏置所使用的配置参数。
// 参数记录为 .conv_bias_0 (75098 )。
// 卷积流水线：);

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