`timescale 1ns / 1ps


module cnn_top#(
    parameter   [10:0]  IMG_HDISP   = 11'd640,                      //  640*480
    parameter   [10:0]  IMG_VDISP   = 11'd480,
    parameter                           core_witdh                = 4'd11   //对的
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
    //input              [   7: 0]        per_img_Y_1                  ,

    output                              post_img_vsync             ,
    output                              post_img_href              ,
  input [5:0] level 

);
    wire              [   7: 0]        per_img_r                  ;
    wire              [   7: 0]        per_img_g                  ;
    wire              [   7: 0]        per_img_b                  ;
    wire                               per_img_vsync              ;
    wire                               per_img_href               ;
    wire              [   7: 0]        per_img_Y                  ;
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
    .post_img_Y                         (per_img_Y                 ),
    .post_img_r                         (per_img_r                 ),
    .post_img_g                         (per_img_g                 ),
    .post_img_b                         (per_img_b                 ) 
);









    wire               [   7: 0]        conv_1_Data                ;
    wire               [   7: 0]        conv_2_Data                ;
    wire               [   7: 0]        conv_3_Data                ;
    wire               [   7: 0]        conv_4_Data                ;

    wire               [   7: 0]        conv_post_img_r            ;
    wire               [   7: 0]        conv_post_img_g            ;
    wire               [   7: 0]        conv_post_img_b            ;

    wire                                conv_post_img_vsync        ;
    wire                                conv_post_img_href         ;
    wire               [   7: 0]        conv_post_img_Y            ;
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

    .per_img_r                          (per_img_r                 ),
    .per_img_g                          (per_img_g                 ),
    .per_img_b                          (per_img_b                 ),
    .post_img_r                         (conv_post_img_r           ),
    .post_img_g                         (conv_post_img_g           ),
    .post_img_b                         (conv_post_img_b           ),

    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .per_img_Y                          (per_img_Y                 ),

    .post_img_vsync                     (conv_post_img_vsync       ),
    .post_img_href                      (conv_post_img_href        ),
    .post_img_Y                         (conv_post_img_Y           ) 
);



wire [9:0] conv2d_data_out;
    wire               [   7: 0]        conv2d_post_img_r            ;
    wire               [   7: 0]        conv2d_post_img_g            ;
    wire               [   7: 0]        conv2d_post_img_b            ;

    wire                                conv2d_post_img_vsync      ;
    wire                                conv2d_post_img_href       ;
    wire               [   7: 0]        conv2d_post_img_Y          ;

conv2d                                                              ////卷积核在模块内部
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

    .per_img_r                          (conv_post_img_r           ),
    .per_img_g                          (conv_post_img_g           ),
    .per_img_b                          (conv_post_img_b           ),
    .per_img_Y                          (conv_post_img_Y           ),
    .post_img_r                         (conv2d_post_img_r         ),
    .post_img_g                         (conv2d_post_img_g         ),
    .post_img_b                         (conv2d_post_img_b         ),
    .post_img_Y                         (conv2d_post_img_Y         ), 

    .per_img_vsync                      (conv_post_img_vsync       ),
    .per_img_href                       (conv_post_img_href        ),


    .post_img_vsync                     (conv2d_post_img_vsync     ),
    .post_img_href                      (conv2d_post_img_href      ),
     .level                             (level                     )
);

    wire               [   7: 0]        gamma_judge_post_img_r     ;
    wire               [   7: 0]        gamma_judge_post_img_g     ;
    wire               [   7: 0]        gamma_judge_post_img_b     ;

    wire                                gamma_judge_post_img_vsync  ;
    wire                                gamma_judge_post_img_href  ;
    wire               [   7: 0]        gamma_judge_post_img_Y     ;
    wire [7:0] gamma_judge_post_img_Y_delay;

gamma_judge u_gamma_judge(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv2d_post_img_r         ),
    .per_img_g                          (conv2d_post_img_g         ),
    .per_img_b                          (conv2d_post_img_b         ),
    .post_img_r                         (gamma_judge_post_img_r    ),
    .post_img_g                         (gamma_judge_post_img_g    ),
    .post_img_b                         (gamma_judge_post_img_b    ),
    .per_img_vsync                      (conv2d_post_img_vsync     ),
    .per_img_href                       (conv2d_post_img_href      ),
    .per_img_Y                          (conv2d_post_img_Y           ),
    .post_img_vsync                     (gamma_judge_post_img_vsync),
    .post_img_href                      (gamma_judge_post_img_href ),
    .post_img_Y                         (gamma_judge_post_img_Y    ),
    .per_data                           (conv2d_data_out           ),
    .post_img_Y_dalay                   (gamma_judge_post_img_Y_delay) 
);
// reg href_delay_1 ;
// reg vsync_delay_1;
//     always @(posedge clk )           
//         begin                                        
//            href_delay_1<= gamma_judge_post_img_href;
//            vsync_delay_1<= gamma_judge_post_img_vsync;
//         end                 

rgb_end u_rgb_end(
    .clk             	( clk              ),
    .rst_n           	( rst_n            ),
    .per_img_r       	( gamma_judge_post_img_r        ),
    .per_img_g       	( gamma_judge_post_img_g        ),
    .per_img_b       	( gamma_judge_post_img_b        ),
    .per_img_Y       	( gamma_judge_post_img_Y        ),
    .per_img_Y_delay 	( gamma_judge_post_img_Y_delay  ),

    .post_img_out_r  	( post_img_r   ),
    .post_img_out_g  	( post_img_g   ),
    .post_img_out_b  	( post_img_b   ),
    // .per_img_href    	( href_delay_1    ),
    // .per_img_vsync   	( vsync_delay_1     ),
    .per_img_href    	( gamma_judge_post_img_href    ),
    .per_img_vsync   	( gamma_judge_post_img_vsync     ),
    .post_img_href   	( post_img_href   ),
    .post_img_vsync  	( post_img_vsync   )
);

endmodule