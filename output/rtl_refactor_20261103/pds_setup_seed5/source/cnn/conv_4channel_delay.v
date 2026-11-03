// 2026-11-03 注释校订。四路共用像素对齐模块，从红色通道取得内部同步标志。
`timescale 1ns / 1ps

module conv_4channel_delay
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
    input  wire                         per_img_href

);

wire post_img_vsync;
wire post_img_href;
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
    .post_img_vsync(post_img_vsync),
    .post_img_href(post_img_href)
);
endmodule