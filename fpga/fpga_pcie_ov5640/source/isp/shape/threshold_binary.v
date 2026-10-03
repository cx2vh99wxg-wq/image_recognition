module  threshold_binary(
	input				clk        ,
    input            	rst_n      ,
    input               types      , // 0:hsv, 1:ycbcr

    input [23:0]        i_ycbcr    ,
    input               i_hsync    ,
    input               i_vsync    ,
    input               i_de       ,

    input  [8:0]        i_hsv_h    ,
    input  [8:0]        i_hsv_s    ,
    input  [7:0]        i_hsv_v    ,
    input               i_hsync_hsv,
    input               i_vsync_hsv,
    input               i_de_hsv   ,
	
    input  [9:0]        i_x		   ,
    input  [9:0]        i_y 	   ,
        
    output [7:0]        o_binary   ,
    output              o_hsync    ,
    output              o_vsync    ,   
    output              o_de 	   ,
	
    output [9:0]        o_x	 	   ,
    output [9:0]        o_y		   
	
//	output				en_r	   ,
//	output				en_b	   ,
//	output				en_y	   
);

reg [7:0] binary_r;
reg       h_sync_r;
reg       v_sync_r;
reg       de_r;
reg [9:0] x_r;
reg [9:0] y_r;

reg       h_sync_hsv_r;
reg       v_sync_hsv_r;
reg       de_hsv_r;

wire      en0_r;
wire      en1_r;
wire      en2_r;

wire      en0_b;
wire      en1_b;
wire      en2_b;

wire      en0_y;
wire      en1_y;
wire      en2_y;

////////////ycbcr 阈值////////////////
parameter Y_TH_R  = 200;
parameter Y_TL_R  = 50;
parameter CB_TH_R = 120;
parameter CB_TL_R = 80;
parameter CR_TH_R = 250;
parameter CR_TL_R = 140; 

parameter Y_TH_B  = 200;
parameter Y_TL_B  = 50;
parameter CB_TH_B = 250;
parameter CB_TL_B = 140;
parameter CR_TH_B = 120;
parameter CR_TL_B = 80;

parameter Y_TH_Y  = 220; 
parameter Y_TL_Y  = 100;   
parameter CB_TH_Y = 110;  
parameter CB_TL_Y = 70;    
parameter CR_TH_Y = 170;  
parameter CR_TL_Y = 130;   

parameter Y_BK = 90;

wire en_red;
wire en_blue;
wire en_yellow;
wire enbk;

assign en0_r = i_ycbcr[23:16] >= Y_TL_R  && i_ycbcr[23:16] <= Y_TH_R ;
assign en1_r = i_ycbcr[15: 8] >= CB_TL_R && i_ycbcr[15: 8] <= CB_TH_R;
assign en2_r = i_ycbcr[ 7: 0] >= CR_TL_R && i_ycbcr[ 7: 0] <= CR_TH_R;
assign en_red    = (en0_r == 1'b1 && en1_r == 1'b1 && en2_r == 1'b1) ? 1 : 0;

assign en0_b = i_ycbcr[23:16] >= Y_TL_B  && i_ycbcr[23:16] <= Y_TH_B ;
assign en1_b = i_ycbcr[15: 8] >= CB_TL_B && i_ycbcr[15: 8] <= CB_TH_B;
assign en2_b = i_ycbcr[ 7: 0] >= CR_TL_B && i_ycbcr[ 7: 0] <= CR_TH_B;
assign en_blue   = (en0_b == 1'b1 && en1_b == 1'b1 && en2_b == 1'b1) ? 1 : 0;

assign en0_y = i_ycbcr[23:16] >= Y_TL_Y  && i_ycbcr[23:16] <= Y_TH_Y ;
assign en1_y = i_ycbcr[15: 8] >= CB_TL_Y && i_ycbcr[15: 8] <= CB_TH_Y;
assign en2_y = i_ycbcr[ 7: 0] >= CR_TL_Y && i_ycbcr[ 7: 0] <= CR_TH_Y;
assign en_yellow = (en0_y == 1'b1 && en1_y == 1'b1 && en2_y == 1'b1) ? 1 : 0;

assign enbk = i_ycbcr[23:16] <= Y_BK;

////////////hsv 阈值////////////////
parameter H_TH_R_L = 15;    // 低色相范围上限
parameter H_TL_R_L = 0;     // 低色相范围下限
parameter H_TH_R_H = 360;   // 高色相范围上限
parameter H_TL_R_H = 345;   // 高色相范围下限（环状色相处理）
parameter S_TL_R = 80;
parameter V_TL_R = 40;

parameter H_TH_Y = 64;
parameter H_TL_Y = 50;
parameter S_TL_Y = 100;
parameter V_TL_Y = 65;

parameter H_TH_B = 270;
parameter H_TL_B = 200;
parameter S_TL_B = 70;
parameter V_TL_B = 30;

wire is_red;
wire is_blue;
wire is_yellow;

assign is_red = (((i_hsv_h >= H_TL_R_L) && (i_hsv_h <= H_TH_R_L)) 
              || ((i_hsv_h >= H_TL_R_H) && (i_hsv_h <= H_TH_R_H)))
              &&  (i_hsv_s >= S_TL_R  ) && (i_hsv_v >= V_TL_R  );
assign is_yellow = (i_hsv_h >= H_TL_Y) && (i_hsv_h <= H_TH_Y)
                && (i_hsv_s >= S_TL_Y) && (i_hsv_v >= V_TL_Y);
assign is_blue = (i_hsv_h >= H_TL_B) && (i_hsv_h <= H_TH_B)
              && (i_hsv_s >= S_TL_B) && (i_hsv_v >= V_TL_B);


/***************************************timing***********************************************/

always @(posedge clk)begin
    h_sync_r <= i_hsync;
    v_sync_r <= i_vsync;
    de_r     <= i_de;
	
	x_r <= i_x;
	y_r <= i_y;

    h_sync_hsv_r <= i_hsync_hsv;
    v_sync_hsv_r <= i_vsync_hsv;
    de_hsv_r     <= i_de_hsv;
end 

/********************************************************************************************/

wire enr, enb, eny;
reg flag_bk;
assign enr = (types == 1) ? en_red    : is_red;
assign enb = (types == 1) ? en_blue   : is_blue;
assign eny = (types == 0) ? en_yellow : is_yellow;

always @(posedge clk or negedge rst_n) begin
    if(!rst_n)begin 
        binary_r <= {8{1'b1}};
		flag_bk  <= 0;
    end 
	else if((i_y <= 40) || (i_y >= 400) || (i_x <=80) || (i_x >= 560)) begin
		binary_r <= {8{1'b1}};
		flag_bk  <= 0;
	end
    else begin 
        if(enr || (flag_bk && enbk)) begin 
            binary_r <= 8'd0;
			flag_bk  <= 1;
        end             
		else if(enb)begin         
		 	binary_r <= 8'd0;
			flag_bk  <= 0;
		end 		 	
		else if(eny)begin         
		 	binary_r <= 8'd0;
			flag_bk  <= 0;
		end
        else begin 
            binary_r <= {8{1'b1}};
        end 
    end  
end 

assign o_binary = binary_r; 
assign o_hsync  = (types == 1) ? h_sync_r : h_sync_hsv_r;
assign o_vsync  = (types == 1) ? v_sync_r : v_sync_hsv_r;
assign o_de     = (types == 0) ? de_r     : de_hsv_r;
assign o_x = x_r;
assign o_y = y_r;
//assign en_r = enr;
//assign en_b = enb;
//assign en_y = eny;

endmodule 