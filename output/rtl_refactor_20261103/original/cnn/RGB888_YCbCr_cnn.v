
`timescale 1ns/1ns
module RGB888_YCbCr_cnn
(
    // 卷积流水线：global clock
    input                               clk                        ,// 卷积流水线：cmos video pixel clock
    input                               rst_n                      ,// 卷积流水线：global reset

    // 卷积流水线：Image data prepred to be processed
    input                               per_img_vsync              ,// 卷积流水线：Prepared Image data vsync valid signal
    input                               per_img_href               ,// 卷积流水线：Prepared Image data href vaild signal
    input              [   7: 0]        per_img_red                ,// 卷积流水线：Prepared Image red data to be processed
    input              [   7: 0]        per_img_green              ,// 卷积流水线：Prepared Image green data to be processed
    input              [   7: 0]        per_img_blue               ,// 卷积流水线：Prepared Image blue data to be processed
    
    // 卷积流水线：Image data has been processed
    output                              post_img_vsync             ,// 卷积流水线：Processed Image data vsync valid signal
    output                              post_img_href              ,// 卷积流水线：Processed Image data href vaild signal
    output             [   7: 0]        post_img_Y                 ,// 卷积流水线：Processed Image brightness output
    output             [   7: 0]        post_img_Cb                ,// 卷积流水线：Processed Image blue shading output
    output             [   7: 0]        post_img_Cr                 ,// 卷积流水线：Processed Image red shading output

    output [7:0] post_img_r,
    output [7:0] post_img_g,
    output [7:0] post_img_b
);

reg [7:0] r_1, g_1, b_1;
    always @(posedge clk)           
        begin                                        
               r_1<=per_img_red;
               g_1<=per_img_green;  
               b_1<=per_img_blue;                     
        end                                          

reg [7:0] r_2, g_2, b_2;
    always @(posedge clk)           
        begin                                        
               r_2<=r_1;
               g_2<=g_1;  
               b_2<=b_1;                     
        end                                          

reg [7:0] r_3, g_3, b_3;
    always @(posedge clk)           
        begin                                        
               r_3<=r_2;
               g_3<=g_2;  
               b_3<=b_2;                     
        end                                          

assign post_img_r = r_3;
assign post_img_g = g_3;
assign post_img_b = b_3;


// ── 卷积流水线 ──
/** 参数记录为 *******************************************
* //Refer to full/pc range YCbCr
* format Y = R*0.299 + G*0.587 +
* B*0.114 Cb = -R*0.169 - G*0.331 +
* B*0.5 + 128 Cr = R*0.5 - G*0.419 -
* B*0.081 + 128 ---> Y = (76 *R +
* 150*G + 29 *B)>>8 Cb = (-43*R - 84
* *G + 128*B + 32768)>>8 Cr = (128*R
* - 107*G - 20 *B + 32768)>>8
* ********************************************。 */
// 参数记录为 Step 1。
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
    // 停用示例，仅供对照：img_red_r0 <= per_img_red * 'd76; //64+8+4
    img_red_r0   <= {2'b0,per_img_red,6'b0}+ {5'b0,per_img_red,3'b0}+{6'b0,per_img_red,2'b0}  ;  // 参数记录为 64+8+4。
    img_red_r1   <= {3'b0,per_img_red,5'b0}+ {5'b0,per_img_red,3'b0}+{7'b0,per_img_red,1'b0}+per_img_red;  // 参数记录为 32+8+2+1。
    img_red_r2   <= {1'b0,per_img_red,7'b0} ; // 参数记录为 128。
    img_green_r0 <= {1'b0,per_img_green,7'b0}+{4'b0,per_img_green,4'b0}+{6'b0,per_img_green,2'b0}+{7'b0,per_img_green,1'b0}; // 参数记录为 128+16+4+2。
    img_green_r1 <= {2'b0,per_img_green,6'b0}+{4'b0,per_img_green,4'b0}+{6'b0,per_img_green,2'b0};  // 参数记录为 64+16+4。
    img_green_r2 <= {2'b0,per_img_green,6'b0}+{3'b0,per_img_green,5'b0}+{5'b0,per_img_green,3'b0}+{7'b0,per_img_green,1'b0}+per_img_green;  // 参数记录为 64+32+8+2+1。
    img_blue_r0  <= {4'b0,per_img_blue,4'b0}+{5'b0,per_img_blue,3'b0}+{6'b0,per_img_blue,2'b0}+per_img_blue;  // 参数记录为 16+8+4+1。
    img_blue_r1  <= {1'b0,per_img_blue,7'b0}; // 参数记录为 128。
    img_blue_r2  <= {4'b0,per_img_blue,4'b0}+{6'b0,per_img_blue,2'b0};  // 参数记录为 16+4。
end
end
// ── 卷积流水线 ──
// 参数记录为 Step 2。
reg [15:0]  img_Y_r0;   // 本节声明16位，防止溢出。
reg [15:0]  img_Cb_r0; 
reg [15:0]  img_Cr_r0; 
always@(posedge clk)
begin
    img_Y_r0  <= img_red_r0  + img_green_r0 + img_blue_r0;
    img_Cb_r0 <= img_blue_r1 - img_red_r1   - img_green_r1 +  16'd32768;
    img_Cr_r0 <= img_red_r2  - img_green_r2 - img_blue_r2  +  16'd32768;
end


// ── 卷积流水线 ──
// 参数记录为 Step 3。
reg [7:0] img_Y_r1; 
reg [7:0] img_Cb_r1; 
reg [7:0] img_Cr_r1; 
always@(posedge clk)
begin
    img_Y_r1  <= img_Y_r0[15:8];     // 把数据移出八位。
    img_Cb_r1 <= img_Cb_r0[15:8];
    img_Cr_r1 <= img_Cr_r0[15:8]; 
end

// ── 卷积流水线 ──
// 卷积流水线：lag 3 clocks signal sync //打拍
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
        per_img_vsync_r <=  {per_img_vsync_r[1:0],  per_img_vsync}; // 经过三级时钟寄存后使用。
        per_img_href_r  <=  {per_img_href_r[1:0],   per_img_href};
        end
end
assign  post_img_vsync = per_img_vsync_r[2];
assign  post_img_href  = per_img_href_r[2];
assign  post_img_Y     = post_img_href ? img_Y_r1 : 8'd0;
assign  post_img_Cb    = post_img_href ? img_Cb_r1: 8'd0;
assign  post_img_Cr    = post_img_href ? img_Cr_r1: 8'd0;


endmodule
