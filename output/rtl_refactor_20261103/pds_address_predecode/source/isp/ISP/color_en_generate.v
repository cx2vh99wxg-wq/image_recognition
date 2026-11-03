module color_en_generate
#(
    parameter H_PIXEL     = 640,
    parameter V_PIXEL     = 480
)
(
    input wire        clk,
    input wire        rst_n,
    input wire [23:0] i_ycbcr,
    input wire        vs,
    input wire        de,
	input wire [9:0]  x,
	input wire [9:0]  y,
    
    output wire       red_en,
    output wire       blue_en,
    output wire       yellow_en,
    output wire       green_en
);
parameter TOTAL_PIXEL = H_PIXEL * V_PIXEL;
////////////ycbcr 参数配置////////////////
parameter DW        = 24 ;
parameter Y_TH      = 200;
parameter Y_TL      = 50;
parameter CB_TH     = 120;
parameter CB_TL     = 80;
parameter CR_TH     = 250;
parameter CR_TL     = 140; 

parameter Y_TH_B      = 200;
parameter Y_TL_B      = 50;
parameter CB_TH_B     = 250;
parameter CB_TL_B     = 140;
parameter CR_TH_B     = 120;
parameter CR_TL_B     = 80;

parameter Y_MIN_YELLOW  = 100;   
parameter Y_MAX_YELLOW  = 220;  
parameter CB_MIN_YELLOW = 70;    
parameter CB_MAX_YELLOW = 110;   
parameter CR_MIN_YELLOW = 130;   
parameter CR_MAX_YELLOW = 170;
//////////// 同步处理模块 ////////////
reg vs_d0, vs_d1;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        vs_d0 <= 1'b0;
        vs_d1 <= 1'b0;
    end else begin
        vs_d0 <= vs;
        vs_d1 <= vs_d0;
    end
end
wire vs_posedge = vs_d0 & ~vs_d1;  
wire area_en = ((x <= 40 || x>=600) || (y <= 40 || y >= 440))?0:1;
//////////// 颜色识别模块 ////////////
wire en0,en1,en2;
wire en0_b,en1_b,en2_b;
wire is_yellow,is_red,is_blue,is_green;
assign en0      =i_ycbcr[23:16] >=Y_TL  && i_ycbcr[23:16] <= Y_TH;
assign en1      =i_ycbcr[15: 8] >=CB_TL && i_ycbcr[15: 8] <= CB_TH;
assign en2      =i_ycbcr[ 7: 0] >=CR_TL && i_ycbcr[ 7: 0] <= CR_TH;
assign en0_b      =i_ycbcr[23:16] >=Y_TL_B && i_ycbcr[23:16] <= Y_TH_B  ;
assign en1_b      =i_ycbcr[15: 8] >=CB_TL_B && i_ycbcr[15: 8] <= CB_TH_B;
assign en2_b      =i_ycbcr[ 7: 0] >=CR_TL_B && i_ycbcr[ 7: 0] <= CR_TH_B;
assign is_yellow = (i_ycbcr[23:16] >= Y_MIN_YELLOW)  && (i_ycbcr[23:16] <= Y_MAX_YELLOW) &&  
                      (i_ycbcr[15:8]  >= CB_MIN_YELLOW) && (i_ycbcr[15:8]  <= CB_MAX_YELLOW) &&  
                      (i_ycbcr[7:0]   >= CR_MIN_YELLOW)  && (i_ycbcr[7:0]   <= CR_MAX_YELLOW); 
assign is_red    = en0 && en1 && en2;
assign is_green  = 0;
assign is_blue   = en0_b && en1_b && en2_b;


//////////// 像素计数器模块 ////////////
reg [19:0] pixel_cnt;
reg        compare_en;
reg [19:0] red_cnt, green_cnt, blue_cnt, yellow_cnt;

// 像素计数器与使能生成
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        pixel_cnt  <= 20'd0;
        compare_en <= 1'b0;
    end else begin
        // 延迟一拍生成compare_en，确保时序稳定
        compare_en <= (pixel_cnt == TOTAL_PIXEL-1) && de;

        if (vs_posedge) begin
            pixel_cnt <= 20'd0;
        end else if (de) begin
            pixel_cnt <= (pixel_cnt == TOTAL_PIXEL-1) ? 20'd0 : pixel_cnt + 1;
        end
    end
end

// 颜色计数器（优化为并行计数）
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        red_cnt    <= 20'd0;
        green_cnt  <= 20'd0;
        blue_cnt   <= 20'd0;
        yellow_cnt <= 20'd0;
    end else if (vs_posedge) begin
        red_cnt    <= 20'd0;
        green_cnt  <= 20'd0;
        blue_cnt   <= 20'd0;
        yellow_cnt <= 20'd0;
    end else if (de && area_en) begin
        red_cnt    <= red_cnt    + is_red;
        green_cnt  <= green_cnt  + is_green;
        blue_cnt   <= blue_cnt   + is_blue;
        yellow_cnt <= yellow_cnt + is_yellow;
    end
end

//////////// 最大值比较与优先级逻辑 ////////////
reg [3:0] color_en;  // [3:0] = {yellow, blue, green, red}

always @(*) begin
    color_en = 4'b0000;
    
    // 层级比较（红 > 绿 > 蓝 > 黄 的优先级）
    if (|{red_cnt, green_cnt, blue_cnt, yellow_cnt}) begin
        // 第一级：红 vs 绿
        if (red_cnt >= green_cnt) begin
            // 第二级：红 vs 蓝
            if (red_cnt >= blue_cnt) begin
                // 第三级：红 vs 黄
                color_en = (red_cnt >= yellow_cnt) ? 4'b0001 : 4'b1000;
            end else begin
                // 第二级：蓝 vs 黄
                color_en = (blue_cnt >= yellow_cnt) ? 4'b0100 : 4'b1000;
            end
        end else begin
            // 第二级：绿 vs 蓝
            if (green_cnt >= blue_cnt) begin
                // 第三级：绿 vs 黄
                color_en = (green_cnt >= yellow_cnt) ? 4'b0010 : 4'b1000;
            end else begin
                // 第二级：蓝 vs 黄
                color_en = (blue_cnt >= yellow_cnt) ? 4'b0100 : 4'b1000;
            end
        end
    end
end

//////////// 输出寄存器 ////////////
reg [3:0] en_reg;
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        en_reg <= 4'b0000;
    end else if (compare_en) begin
        en_reg <= color_en;
    end
end

assign {yellow_en, blue_en, green_en, red_en} = en_reg;

endmodule