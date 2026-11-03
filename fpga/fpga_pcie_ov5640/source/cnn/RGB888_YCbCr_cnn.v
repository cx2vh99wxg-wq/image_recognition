// 2026-11-03 注释校订。全范围 RGB 转 YCbCr。RGB 直通与算术结果对齐，HREF 仅门控 YCbCr。
`timescale 1ns/1ns
module RGB888_YCbCr_cnn
(

    input                               clk                        ,
    input                               rst_n                      ,

    input                               per_img_vsync              ,
    input                               per_img_href               ,
    input              [   7: 0]        per_img_red                ,
    input              [   7: 0]        per_img_green              ,
    input              [   7: 0]        per_img_blue               ,

    output                              post_img_vsync             ,
    output                              post_img_href              ,
    output             [   7: 0]        post_img_Y                 ,
    output             [   7: 0]        post_img_Cb                ,
    output             [   7: 0]        post_img_Cr                 ,

    output [7:0] post_img_r,
    output [7:0] post_img_g,
    output [7:0] post_img_b
);

reg [23:0] rgb_stage1, rgb_stage2, rgb_stage3;
always @(posedge clk) begin
    rgb_stage1 <= {per_img_red, per_img_green, per_img_blue};
    rgb_stage2 <= rgb_stage1;
    rgb_stage3 <= rgb_stage2;
end
assign {post_img_r, post_img_g, post_img_b} = rgb_stage3;

reg [15:0]  img_red_r0,   img_red_r1,   img_red_r2;
reg [15:0]  img_green_r0, img_green_r1, img_green_r2;
reg [15:0]  img_blue_r0,  img_blue_r1,  img_blue_r2;
always@(posedge clk or negedge rst_n)
begin
if(!rst_n)begin
    img_red_r0   <= 0;
    img_red_r1   <= 0;
    img_red_r2   <= 0;
    img_green_r0 <= 0;
    img_green_r1 <= 0;
    img_green_r2 <= 0;
    img_blue_r0  <= 0;
    img_blue_r1  <= 0;
    img_blue_r2  <= 0;
end
else
begin

    img_red_r0   <= {2'b0,per_img_red,6'b0}+ {5'b0,per_img_red,3'b0}+{6'b0,per_img_red,2'b0}  ;
    img_red_r1   <= {3'b0,per_img_red,5'b0}+ {5'b0,per_img_red,3'b0}+{7'b0,per_img_red,1'b0}+per_img_red;
    img_red_r2   <= {1'b0,per_img_red,7'b0} ;
    img_green_r0 <= {1'b0,per_img_green,7'b0}+{4'b0,per_img_green,4'b0}+{6'b0,per_img_green,2'b0}+{7'b0,per_img_green,1'b0};
    img_green_r1 <= {2'b0,per_img_green,6'b0}+{4'b0,per_img_green,4'b0}+{6'b0,per_img_green,2'b0};
    img_green_r2 <= {2'b0,per_img_green,6'b0}+{3'b0,per_img_green,5'b0}+{5'b0,per_img_green,3'b0}+{7'b0,per_img_green,1'b0}+per_img_green;
    img_blue_r0  <= {4'b0,per_img_blue,4'b0}+{5'b0,per_img_blue,3'b0}+{6'b0,per_img_blue,2'b0}+per_img_blue;
    img_blue_r1  <= {1'b0,per_img_blue,7'b0};
    img_blue_r2  <= {4'b0,per_img_blue,4'b0}+{6'b0,per_img_blue,2'b0};
end
end

reg [15:0]  img_Y_r0;
reg [15:0]  img_Cb_r0;
reg [15:0]  img_Cr_r0;
always@(posedge clk)
begin
    img_Y_r0  <= img_red_r0  + img_green_r0 + img_blue_r0;
    img_Cb_r0 <= img_blue_r1 - img_red_r1   - img_green_r1 +  16'd32768;
    img_Cr_r0 <= img_red_r2  - img_green_r2 - img_blue_r2  +  16'd32768;
end

reg [7:0] img_Y_r1;
reg [7:0] img_Cb_r1;
reg [7:0] img_Cr_r1;
always@(posedge clk)
begin
    img_Y_r1  <= img_Y_r0[15:8];
    img_Cb_r1 <= img_Cb_r0[15:8];
    img_Cr_r1 <= img_Cr_r0[15:8];
end

reg [2:0] per_img_vsync_r;
reg [2:0] per_img_href_r;
always@(posedge clk or negedge rst_n)
begin
    if(!rst_n)
        begin
        per_img_vsync_r <= 0;
        per_img_href_r <= 0;
        end
    else
        begin
        per_img_vsync_r <=  {per_img_vsync_r[1:0],  per_img_vsync};
        per_img_href_r  <=  {per_img_href_r[1:0],   per_img_href};
        end
end
assign  post_img_vsync = per_img_vsync_r[2];
assign  post_img_href  = per_img_href_r[2];
assign  post_img_Y     = post_img_href ? img_Y_r1 : 8'd0;
assign  post_img_Cb    = post_img_href ? img_Cb_r1: 8'd0;
assign  post_img_Cr    = post_img_href ? img_Cr_r1: 8'd0;

endmodule
