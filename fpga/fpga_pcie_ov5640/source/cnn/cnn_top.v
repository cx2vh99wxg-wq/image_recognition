// 2026-11-03 注释校订。串联色彩转换、卷积、Gamma 和 RGB 恢复。RGBY 作为同一组像素侧带传递。
`timescale 1ns / 1ps

module cnn_top#(
    parameter   [10:0]  IMG_HDISP   = 11'd640,
    parameter   [10:0]  IMG_VDISP   = 11'd480,
    parameter                           core_witdh                = 4'd11
)(
    input                               clk                        ,
    input                               rst_n                      ,

    input              [   7: 0]        per_img_r_1                  ,
    input              [   7: 0]        per_img_g_1                  ,
    input              [   7: 0]        per_img_b_1                  ,
    output             [   7: 0]        post_img_r                 ,
    output             [   7: 0]        post_img_g                 ,
    output             [   7: 0]        post_img_b                 ,

    input                               per_img_vsync_1              ,
    input                               per_img_href_1               ,

    output                              post_img_vsync             ,
    output                              post_img_href              ,
  input [5:0] level

);

wire [31:0] converted_pixel;
wire [31:0] convolved_pixel;
wire [31:0] scored_pixel;
wire [31:0] adjusted_pixel;

    wire                               per_img_vsync              ;
    wire                               per_img_href               ;

RGB888_YCbCr_cnn RGB888_YCbCr_cnn(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_vsync                      (per_img_vsync_1           ),
    .per_img_href                       (per_img_href_1            ),
    .per_img_red                        (per_img_r_1               ),
    .per_img_green                      (per_img_g_1               ),
    .per_img_blue                       (per_img_b_1               ),
    .post_img_vsync                     (per_img_vsync             ),
    .post_img_href                      (per_img_href              ),
    .post_img_Y                         (converted_pixel[0 +: 8]                 ),
    .post_img_r                         (converted_pixel[24 +: 8]                 ),
    .post_img_g                         (converted_pixel[16 +: 8]                 ),
    .post_img_b                         (converted_pixel[8 +: 8]                 )
);

    wire               [   7: 0]        conv_1_Data                ;
    wire               [   7: 0]        conv_2_Data                ;
    wire               [   7: 0]        conv_3_Data                ;
    wire               [   7: 0]        conv_4_Data                ;

    wire                                conv_post_img_vsync        ;
    wire                                conv_post_img_href         ;

conv_top
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv_top(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),

    .conv7_1_Data                       (conv_1_Data               ),
    .conv7_2_Data                       (conv_2_Data               ),
    .conv7_3_Data                       (conv_3_Data               ),
    .conv7_4_Data                       (conv_4_Data               ),

    .per_img_r                          (converted_pixel[24 +: 8]                 ),
    .per_img_g                          (converted_pixel[16 +: 8]                 ),
    .per_img_b                          (converted_pixel[8 +: 8]                 ),
    .post_img_r                         (convolved_pixel[24 +: 8]           ),
    .post_img_g                         (convolved_pixel[16 +: 8]           ),
    .post_img_b                         (convolved_pixel[8 +: 8]           ),

    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .per_img_Y                          (converted_pixel[0 +: 8]                 ),

    .post_img_vsync                     (conv_post_img_vsync       ),
    .post_img_href                      (conv_post_img_href        ),
    .post_img_Y                         (convolved_pixel[0 +: 8]           )
);

wire [9:0] conv2d_data_out;

    wire                                conv2d_post_img_vsync      ;
    wire                                conv2d_post_img_href       ;

conv2d
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
conv2d(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),

    .conv2d_1_Data                      (conv_1_Data               ),
    .conv2d_2_Data                      (conv_2_Data               ),
    .conv2d_3_Data                      (conv_3_Data               ),
    .conv2d_4_Data                      (conv_4_Data               ),
    .conv2d_data_out                    (conv2d_data_out           ),

    .per_img_r                          (convolved_pixel[24 +: 8]           ),
    .per_img_g                          (convolved_pixel[16 +: 8]           ),
    .per_img_b                          (convolved_pixel[8 +: 8]           ),
    .per_img_Y                          (convolved_pixel[0 +: 8]           ),
    .post_img_r                         (scored_pixel[24 +: 8]         ),
    .post_img_g                         (scored_pixel[16 +: 8]         ),
    .post_img_b                         (scored_pixel[8 +: 8]         ),
    .post_img_Y                         (scored_pixel[0 +: 8]         ),

    .per_img_vsync                      (conv_post_img_vsync       ),
    .per_img_href                       (conv_post_img_href        ),

    .post_img_vsync                     (conv2d_post_img_vsync     ),
    .post_img_href                      (conv2d_post_img_href      ),
     .level                             (level                     )
);

    wire                                gamma_judge_post_img_vsync  ;
    wire                                gamma_judge_post_img_href  ;

    wire [7:0] gamma_judge_post_img_Y_delay;

gamma_judge u_gamma_judge(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (scored_pixel[24 +: 8]         ),
    .per_img_g                          (scored_pixel[16 +: 8]         ),
    .per_img_b                          (scored_pixel[8 +: 8]         ),
    .post_img_r                         (adjusted_pixel[24 +: 8]    ),
    .post_img_g                         (adjusted_pixel[16 +: 8]    ),
    .post_img_b                         (adjusted_pixel[8 +: 8]    ),
    .per_img_vsync                      (conv2d_post_img_vsync     ),
    .per_img_href                       (conv2d_post_img_href      ),
    .per_img_Y                          (scored_pixel[0 +: 8]           ),
    .post_img_vsync                     (gamma_judge_post_img_vsync),
    .post_img_href                      (gamma_judge_post_img_href ),
    .post_img_Y                         (adjusted_pixel[0 +: 8]    ),
    .per_data                           (conv2d_data_out           ),
    .post_img_Y_dalay                   (gamma_judge_post_img_Y_delay)
);

rgb_end u_rgb_end(
    .clk             	( clk              ),
    .rst_n           	( rst_n            ),
    .per_img_r       	( adjusted_pixel[24 +: 8]        ),
    .per_img_g       	( adjusted_pixel[16 +: 8]        ),
    .per_img_b       	( adjusted_pixel[8 +: 8]        ),
    .per_img_Y       	( adjusted_pixel[0 +: 8]        ),
    .per_img_Y_delay 	( gamma_judge_post_img_Y_delay  ),

    .post_img_out_r  	( post_img_r   ),
    .post_img_out_g  	( post_img_g   ),
    .post_img_out_b  	( post_img_b   ),

    .per_img_href    	( gamma_judge_post_img_href    ),
    .per_img_vsync   	( gamma_judge_post_img_vsync     ),
    .post_img_href   	( post_img_href   ),
    .post_img_vsync  	( post_img_vsync   )
);

endmodule