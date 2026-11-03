// 2026-11-03 注释校订。连接七个卷积层。跳连的数据来自第一层，逐级使用对应主路径的同步标志。
`timescale 1ns / 1ps

module conv_top
#(
    parameter   [10:0]  IMG_HDISP   = 11'd640,
    parameter   [10:0]  IMG_VDISP   = 11'd480,
    parameter                           core_witdh                = 4'd8
)
(

output [7:0] conv7_1_Data,
output [7:0] conv7_2_Data,
output [7:0] conv7_3_Data,
output [7:0] conv7_4_Data,

    input                               clk                        ,
    input                               rst_n                      ,

    input              [   7: 0]        per_img_r                  ,
    input              [   7: 0]        per_img_g                  ,
    input              [   7: 0]        per_img_b                  ,
    output             [   7: 0]        post_img_r                 ,
    output             [   7: 0]        post_img_g                 ,
    output             [   7: 0]        post_img_b                 ,

    input                               per_img_vsync              ,
    input                               per_img_href               ,
    input              [   7: 0]        per_img_Y                  ,

    output                              post_img_vsync             ,
    output                              post_img_href              ,
    output             [   7: 0]        post_img_Y
);

    wire               [   7: 0]        conv1_post_img_Y           ;
    wire               [   7: 0]        conv1_post_img_r           ;
    wire               [   7: 0]        conv1_post_img_g           ;
    wire               [   7: 0]        conv1_post_img_b           ;
    wire                                conv1_post_img_vsync       ;
    wire                                conv1_post_img_href        ;
    wire               [   7: 0]        conv1_1_Data_out           ;
    wire               [   7: 0]        conv1_2_Data_out           ;
    wire               [   7: 0]        conv1_3_Data_out           ;
    wire               [   7: 0]        conv1_4_Data_out           ;

conv1
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv1(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (per_img_r                 ),
    .per_img_g                          (per_img_g                 ),
    .per_img_b                          (per_img_b                 ),
    .per_img_Y                          (per_img_Y                 ),
    .post_img_r                         (conv1_post_img_r          ),
    .post_img_g                         (conv1_post_img_g          ),
    .post_img_b                         (conv1_post_img_b          ),
    .post_img_Y                         (conv1_post_img_Y          ),

    .per_img_vsync                      (per_img_vsync             ),
    .per_img_href                       (per_img_href              ),
    .conv1_1_Data_in                    (per_img_Y                 ),
    .conv1_2_Data_in                    (0                         ),
    .post_img_vsync                     (conv1_post_img_vsync      ),
    .post_img_href                      (conv1_post_img_href       ),
    .conv1_1_Data_out                   (conv1_1_Data_out          ),
    .conv1_2_Data_out                   (conv1_2_Data_out          ),
    .conv1_3_Data_out                   (conv1_3_Data_out          ),
    .conv1_4_Data_out                   (conv1_4_Data_out          )
);

    wire               [   7: 0]        conv2_post_img_Y           ;
    wire               [   7: 0]        conv2_post_img_r           ;
    wire               [   7: 0]        conv2_post_img_g           ;
    wire               [   7: 0]        conv2_post_img_b           ;
    wire                                conv2_post_img_vsync       ;
    wire                                conv2_post_img_href        ;
    wire               [   7: 0]        conv2_1_Data_out           ;
    wire               [   7: 0]        conv2_2_Data_out           ;
    wire               [   7: 0]        conv2_3_Data_out           ;
    wire               [   7: 0]        conv2_4_Data_out           ;

conv2
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv2(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv1_post_img_r          ),
    .per_img_g                          (conv1_post_img_g          ),
    .per_img_b                          (conv1_post_img_b          ),
    .per_img_Y                          (conv1_post_img_Y          ),
    .post_img_r                         (conv2_post_img_r          ),
    .post_img_g                         (conv2_post_img_g          ),
    .post_img_b                         (conv2_post_img_b          ),
    .post_img_Y                         (conv2_post_img_Y          ),

    .per_img_vsync                      (conv1_post_img_vsync      ),
    .per_img_href                       (conv1_post_img_href       ),
    .post_img_vsync                     (conv2_post_img_vsync      ),
    .post_img_href                      (conv2_post_img_href       ),

    .conv2_1_Data_in                    (conv1_1_Data_out          ),
    .conv2_2_Data_in                    (conv1_2_Data_out          ),
    .conv2_3_Data_in                    (conv1_3_Data_out          ),
    .conv2_4_Data_in                    (conv1_4_Data_out          ),

    .conv2_1_Data_out                   (conv2_1_Data_out          ),
    .conv2_2_Data_out                   (conv2_2_Data_out          ),
    .conv2_3_Data_out                   (conv2_3_Data_out          ),
    .conv2_4_Data_out                   (conv2_4_Data_out          )
);

    wire               [   7: 0]        conv3_post_img_Y           ;
    wire               [   7: 0]        conv3_post_img_r           ;
    wire               [   7: 0]        conv3_post_img_g           ;
    wire               [   7: 0]        conv3_post_img_b           ;
    wire                                conv3_post_img_vsync       ;
    wire                                conv3_post_img_href        ;
    wire               [   7: 0]        conv3_1_Data_out           ;
    wire               [   7: 0]        conv3_2_Data_out           ;
    wire               [   7: 0]        conv3_3_Data_out           ;
    wire               [   7: 0]        conv3_4_Data_out           ;

conv3
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv3(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv2_post_img_r          ),
    .per_img_g                          (conv2_post_img_g          ),
    .per_img_b                          (conv2_post_img_b          ),
    .per_img_Y                          (conv2_post_img_Y          ),
    .post_img_r                         (conv3_post_img_r          ),
    .post_img_g                         (conv3_post_img_g          ),
    .post_img_b                         (conv3_post_img_b          ),
    .post_img_Y                         (conv3_post_img_Y          ),

    .per_img_vsync                      (conv2_post_img_vsync      ),
    .per_img_href                       (conv2_post_img_href       ),
    .post_img_vsync                     (conv3_post_img_vsync      ),
    .post_img_href                      (conv3_post_img_href       ),

    .conv3_1_Data_in                    (conv2_1_Data_out          ),
    .conv3_2_Data_in                    (conv2_2_Data_out          ),
    .conv3_3_Data_in                    (conv2_3_Data_out          ),
    .conv3_4_Data_in                    (conv2_4_Data_out          ),

    .conv3_1_Data_out                   (conv3_1_Data_out          ),
    .conv3_2_Data_out                   (conv3_2_Data_out          ),
    .conv3_3_Data_out                   (conv3_3_Data_out          ),
    .conv3_4_Data_out                   (conv3_4_Data_out          )
);

    wire               [   7: 0]        conv4_post_img_Y           ;
    wire               [   7: 0]        conv4_post_img_r           ;
    wire               [   7: 0]        conv4_post_img_g           ;
    wire               [   7: 0]        conv4_post_img_b           ;
    wire                                conv4_post_img_vsync       ;
    wire                                conv4_post_img_href        ;
    wire               [   7: 0]        conv4_1_Data_out           ;
    wire               [   7: 0]        conv4_2_Data_out           ;
    wire               [   7: 0]        conv4_3_Data_out           ;
    wire               [   7: 0]        conv4_4_Data_out           ;

conv4
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv4(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv3_post_img_r          ),
    .per_img_g                          (conv3_post_img_g          ),
    .per_img_b                          (conv3_post_img_b          ),
    .per_img_Y                          (conv3_post_img_Y          ),
    .post_img_r                         (conv4_post_img_r          ),
    .post_img_g                         (conv4_post_img_g          ),
    .post_img_b                         (conv4_post_img_b          ),
    .post_img_Y                         (conv4_post_img_Y          ),

    .per_img_vsync                      (conv3_post_img_vsync      ),
    .per_img_href                       (conv3_post_img_href       ),
    .post_img_vsync                     (conv4_post_img_vsync      ),
    .post_img_href                      (conv4_post_img_href       ),

    .conv4_1_Data_in                    (conv3_1_Data_out          ),
    .conv4_2_Data_in                    (conv3_2_Data_out          ),
    .conv4_3_Data_in                    (conv3_3_Data_out          ),
    .conv4_4_Data_in                    (conv3_4_Data_out          ),

    .conv4_1_Data_out                   (conv4_1_Data_out          ),
    .conv4_2_Data_out                   (conv4_2_Data_out          ),
    .conv4_3_Data_out                   (conv4_3_Data_out          ),
    .conv4_4_Data_out                   (conv4_4_Data_out          )
);

wire [7:0] conv3_1_Data_out_delay;
wire [7:0] conv3_2_Data_out_delay;
wire [7:0] conv3_3_Data_out_delay;
wire [7:0] conv3_4_Data_out_delay;
conv_4channel_delay
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv_4channel_delay_3(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv3_1_Data_out          ),
    .per_img_g                          (conv3_2_Data_out          ),
    .per_img_b                          (conv3_3_Data_out          ),
    .per_img_Y                          (conv3_4_Data_out          ),
    .post_img_r                         (conv3_1_Data_out_delay          ),
    .post_img_g                         (conv3_2_Data_out_delay          ),
    .post_img_b                         (conv3_3_Data_out_delay          ),
    .post_img_Y                         (conv3_4_Data_out_delay          ),

    .per_img_vsync                      (conv3_post_img_vsync      ),
    .per_img_href                       (conv3_post_img_href       )
);

    wire               [   7: 0]        conv5_post_img_Y           ;
    wire               [   7: 0]        conv5_post_img_r           ;
    wire               [   7: 0]        conv5_post_img_g           ;
    wire               [   7: 0]        conv5_post_img_b           ;
    wire                                conv5_post_img_vsync       ;
    wire                                conv5_post_img_href        ;
    wire               [   7: 0]        conv5_1_Data_out           ;
    wire               [   7: 0]        conv5_2_Data_out           ;
    wire               [   7: 0]        conv5_3_Data_out           ;
    wire               [   7: 0]        conv5_4_Data_out           ;

conv5
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv5(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv4_post_img_r          ),
    .per_img_g                          (conv4_post_img_g          ),
    .per_img_b                          (conv4_post_img_b          ),
    .per_img_Y                          (conv4_post_img_Y          ),
    .post_img_r                         (conv5_post_img_r          ),
    .post_img_g                         (conv5_post_img_g          ),
    .post_img_b                         (conv5_post_img_b          ),
    .post_img_Y                         (conv5_post_img_Y          ),

    .per_img_vsync                      (conv4_post_img_vsync      ),
    .per_img_href                       (conv4_post_img_href       ),
    .post_img_vsync                     (conv5_post_img_vsync      ),
    .post_img_href                      (conv5_post_img_href       ),

    .conv5_1_Data_in                    (conv3_1_Data_out_delay          ),
    .conv5_2_Data_in                    (conv3_2_Data_out_delay          ),
    .conv5_3_Data_in                    (conv3_3_Data_out_delay          ),
    .conv5_4_Data_in                    (conv3_4_Data_out_delay          ),
    .conv5_5_Data_in                    (conv4_1_Data_out           ),
    .conv5_6_Data_in                    (conv4_2_Data_out           ),
    .conv5_7_Data_in                    (conv4_3_Data_out           ),
    .conv5_8_Data_in                    (conv4_4_Data_out           ),

    .conv5_1_Data_out                   (conv5_1_Data_out          ),
    .conv5_2_Data_out                   (conv5_2_Data_out          ),
    .conv5_3_Data_out                   (conv5_3_Data_out          ),
    .conv5_4_Data_out                   (conv5_4_Data_out          )
);

wire [7:0] conv2_1_Data_out_delay_1;
wire [7:0] conv2_2_Data_out_delay_1;
wire [7:0] conv2_3_Data_out_delay_1;
wire [7:0] conv2_4_Data_out_delay_1;

conv_4channel_delay
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv_4channel_delay_2_1(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv2_1_Data_out          ),
    .per_img_g                          (conv2_2_Data_out          ),
    .per_img_b                          (conv2_3_Data_out          ),
    .per_img_Y                          (conv2_4_Data_out          ),
    .post_img_r                         (conv2_1_Data_out_delay_1          ),
    .post_img_g                         (conv2_2_Data_out_delay_1          ),
    .post_img_b                         (conv2_3_Data_out_delay_1          ),
    .post_img_Y                         (conv2_4_Data_out_delay_1          ),

    .per_img_vsync                      (conv2_post_img_vsync      ),
    .per_img_href                       (conv2_post_img_href       )
);
wire [7:0] conv2_1_Data_out_delay_2;
wire [7:0] conv2_2_Data_out_delay_2;
wire [7:0] conv2_3_Data_out_delay_2;
wire [7:0] conv2_4_Data_out_delay_2;

conv_4channel_delay
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv_4channel_delay_2_2(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv2_1_Data_out_delay_1          ),
    .per_img_g                          (conv2_2_Data_out_delay_1          ),
    .per_img_b                          (conv2_3_Data_out_delay_1          ),
    .per_img_Y                          (conv2_4_Data_out_delay_1          ),
    .post_img_r                         (conv2_1_Data_out_delay_2          ),
    .post_img_g                         (conv2_2_Data_out_delay_2          ),
    .post_img_b                         (conv2_3_Data_out_delay_2          ),
    .post_img_Y                         (conv2_4_Data_out_delay_2          ),

    .per_img_vsync                      (conv3_post_img_vsync      ),
    .per_img_href                       (conv3_post_img_href       )
);
wire [7:0] conv2_1_Data_out_delay_3;
wire [7:0] conv2_2_Data_out_delay_3;
wire [7:0] conv2_3_Data_out_delay_3;
wire [7:0] conv2_4_Data_out_delay_3;

conv_4channel_delay
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv_4channel_delay_2_3(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv2_1_Data_out_delay_2          ),
    .per_img_g                          (conv2_2_Data_out_delay_2          ),
    .per_img_b                          (conv2_3_Data_out_delay_2          ),
    .per_img_Y                          (conv2_4_Data_out_delay_2          ),
    .post_img_r                         (conv2_1_Data_out_delay_3          ),
    .post_img_g                         (conv2_2_Data_out_delay_3          ),
    .post_img_b                         (conv2_3_Data_out_delay_3          ),
    .post_img_Y                         (conv2_4_Data_out_delay_3          ),

    .per_img_vsync                      (conv4_post_img_vsync      ),
    .per_img_href                       (conv4_post_img_href       )
);

    wire               [   7: 0]        conv6_post_img_Y           ;
    wire               [   7: 0]        conv6_post_img_r           ;
    wire               [   7: 0]        conv6_post_img_g           ;
    wire               [   7: 0]        conv6_post_img_b           ;
    wire                                conv6_post_img_vsync       ;
    wire                                conv6_post_img_href        ;
    wire               [   7: 0]        conv6_1_Data_out           ;
    wire               [   7: 0]        conv6_2_Data_out           ;
    wire               [   7: 0]        conv6_3_Data_out           ;
    wire               [   7: 0]        conv6_4_Data_out           ;

conv6
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv6(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv5_post_img_r          ),
    .per_img_g                          (conv5_post_img_g          ),
    .per_img_b                          (conv5_post_img_b          ),
    .per_img_Y                          (conv5_post_img_Y          ),
    .post_img_r                         (conv6_post_img_r          ),
    .post_img_g                         (conv6_post_img_g          ),
    .post_img_b                         (conv6_post_img_b          ),
    .post_img_Y                         (conv6_post_img_Y          ),

    .per_img_vsync                      (conv5_post_img_vsync      ),
    .per_img_href                       (conv5_post_img_href       ),
    .post_img_vsync                     (conv6_post_img_vsync      ),
    .post_img_href                      (conv6_post_img_href       ),

    .conv6_1_Data_in                    (conv2_1_Data_out_delay_3          ),
    .conv6_2_Data_in                    (conv2_2_Data_out_delay_3          ),
    .conv6_3_Data_in                    (conv2_3_Data_out_delay_3          ),
    .conv6_4_Data_in                    (conv2_4_Data_out_delay_3          ),
    .conv6_5_Data_in                    (conv5_1_Data_out           ),
    .conv6_6_Data_in                    (conv5_2_Data_out           ),
    .conv6_7_Data_in                    (conv5_3_Data_out           ),
    .conv6_8_Data_in                    (conv5_4_Data_out           ),

    .conv6_1_Data_out                   (conv6_1_Data_out          ),
    .conv6_2_Data_out                   (conv6_2_Data_out          ),
    .conv6_3_Data_out                   (conv6_3_Data_out          ),
    .conv6_4_Data_out                   (conv6_4_Data_out          )
);

wire [31:0] skip_pixels [0:5];
wire [4:0] skip_vsync = {conv5_post_img_vsync, conv4_post_img_vsync,
                         conv3_post_img_vsync, conv2_post_img_vsync, conv1_post_img_vsync};
wire [4:0] skip_href = {conv5_post_img_href, conv4_post_img_href,
                        conv3_post_img_href, conv2_post_img_href, conv1_post_img_href};
assign skip_pixels[0] = {conv1_1_Data_out, conv1_2_Data_out,
                         conv1_3_Data_out, conv1_4_Data_out};

genvar skip_stage;
generate
    for (skip_stage = 0; skip_stage < 5; skip_stage = skip_stage + 1) begin : skip_alignment
        conv_rgb_y_alignment #(
            .IMG_HDISP(IMG_HDISP), .IMG_VDISP(IMG_VDISP), .core_witdh(core_witdh)
        ) u_delay (
            .clk(clk), .rst_n(rst_n),
            .per_img_vsync(skip_vsync[skip_stage]), .per_img_href(skip_href[skip_stage]),
            .pixels_in(skip_pixels[skip_stage]), .pixels_out(skip_pixels[skip_stage+1]),
            .post_img_vsync(), .post_img_href()
        );
    end
endgenerate

    wire               [   7: 0]        conv7_post_img_Y           ;
    wire               [   7: 0]        conv7_post_img_r           ;
    wire               [   7: 0]        conv7_post_img_g           ;
    wire               [   7: 0]        conv7_post_img_b           ;
    wire                                conv7_post_img_vsync       ;
    wire                                conv7_post_img_href        ;
    wire               [   7: 0]        conv7_1_Data_out           ;
    wire               [   7: 0]        conv7_2_Data_out           ;
    wire               [   7: 0]        conv7_3_Data_out           ;
    wire               [   7: 0]        conv7_4_Data_out           ;

conv7
#(
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 ),
    .core_witdh                         (core_witdh                )
)
    conv7(
    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),
    .per_img_r                          (conv6_post_img_r          ),
    .per_img_g                          (conv6_post_img_g          ),
    .per_img_b                          (conv6_post_img_b          ),
    .per_img_Y                          (conv6_post_img_Y          ),
    .post_img_r                         (conv7_post_img_r          ),
    .post_img_g                         (conv7_post_img_g          ),
    .post_img_b                         (conv7_post_img_b          ),
    .post_img_Y                         (conv7_post_img_Y          ),

    .per_img_vsync                      (conv6_post_img_vsync      ),
    .per_img_href                       (conv6_post_img_href       ),
    .post_img_vsync                     (conv7_post_img_vsync      ),
    .post_img_href                      (conv7_post_img_href       ),

    .conv7_1_Data_in                    (skip_pixels[5][24 +: 8]          ),
    .conv7_2_Data_in                    (skip_pixels[5][16 +: 8]          ),
    .conv7_3_Data_in                    (skip_pixels[5][8 +: 8]          ),
    .conv7_4_Data_in                    (skip_pixels[5][0 +: 8]          ),
    .conv7_5_Data_in                    (conv6_1_Data_out           ),
    .conv7_6_Data_in                    (conv6_2_Data_out           ),
    .conv7_7_Data_in                    (conv6_3_Data_out           ),
    .conv7_8_Data_in                    (conv6_4_Data_out           ),

    .conv7_1_Data_out                   (conv7_1_Data_out          ),
    .conv7_2_Data_out                   (conv7_2_Data_out          ),
    .conv7_3_Data_out                   (conv7_3_Data_out          ),
    .conv7_4_Data_out                   (conv7_4_Data_out          )
);

    assign                              post_img_r                = conv7_post_img_r;
    assign                              post_img_g                = conv7_post_img_g;
    assign                              post_img_b                = conv7_post_img_b;
    assign                              post_img_vsync            = conv7_post_img_vsync;
    assign                              post_img_href             = conv7_post_img_href;
    assign                              post_img_Y                = conv7_post_img_Y;

    assign                              conv7_1_Data              = conv7_1_Data_out;
    assign                              conv7_2_Data              = conv7_2_Data_out;
    assign                              conv7_3_Data              = conv7_3_Data_out;
    assign                              conv7_4_Data              = conv7_4_Data_out;
endmodule