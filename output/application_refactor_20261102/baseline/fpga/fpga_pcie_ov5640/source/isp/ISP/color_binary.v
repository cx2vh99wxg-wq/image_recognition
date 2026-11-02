module  color_binary
        (
        input                wire            pixelclk   ,
        input                 wire           reset_n    ,
        input wire[23:0]                   	i_rgb      ,
		input [23:0]                   		i_ycbcr    ,
		input wire[9:0]						i_x,
		input wire[9:0]						i_y,
        input                            i_hsync    ,
        input                            i_vsync    ,
		input	[8:0]					i_hsv_h,
		input	[8:0]					i_hsv_s,
		input	[7:0]					i_hsv_v,
        input                            i_de       ,
        
        output                   		monoc_fall   ,
		output							o_monoc,
        output 	[23:0]                  o_rgb      ,
        output                           o_hsync    ,
        output                           o_vsync    ,   
		output		[9:0]					o_x,
		output		[9:0]					o_y,
        output                           	o_de                                                                                                                
);
reg  [23:0]           binary_r    ;	
reg  [23:0]           i_rgb_r     ;
reg  [9:0]			  i_x_r;
reg  [9:0]			  i_y_r;
reg                     h_sync_r    ;
reg                     v_sync_r    ;
reg                     de_r        ;
reg monoc_d0;
reg monoc;


wire [23:0]ycbcr_rgb;
wire ycbcr_hsync,ycbcr_vsync,ycbcr_de;
wire                    en0         ;
wire                    en1         ;
wire                    en2         ;
wire                    en0_b         ;
wire                    en1_b         ;
wire                    en2_b         ;
wire                    en0_g         ;
wire                    en1_g         ;
wire                    en2_g         ;
assign o_binary = binary_r; 
assign o_rgb    = i_rgb_r;
assign o_hsync  = h_sync_r;
assign o_vsync  = v_sync_r;
assign o_de     = de_r;
assign o_x		= i_x_r;
assign o_y		= i_y_r;
//assign o_monoc = ((i_x_r <= 120 || i_x_r>=520) || (i_y_r <= 90 || i_y_r >= 390))?1:monoc;
assign o_monoc = monoc;
assign  monoc_fall  = (!monoc) & monoc_d0;
////////////ycbcr ��������////////////////
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

parameter Y_MAX_BLACK  = 30;    
parameter CB_MIN_BLACK = 80;    
parameter CB_MAX_BLACK = 150;   
parameter CR_MIN_BLACK = 100;   
parameter CR_MAX_BLACK = 150;   

///////////////hsv//////////////////



///////////////////red///////////////////
parameter H_MIN_RED_LOW = 0;     // ��ɫ�෶Χ����
parameter H_MAX_RED_LOW = 15;    // ��ɫ�෶Χ����
parameter H_MIN_RED_HIGH = 345;  // ��ɫ�෶Χ���ޣ���״ɫ�ദ����
parameter H_MAX_RED_HIGH = 360;  // ��ɫ�෶Χ����
parameter S_MIN_RED = 80;
parameter V_MIN_RED = 40;


//////////////////yellow///////////////////////
parameter H_MIN_YELLOW = 50;
parameter H_MAX_YELLOW = 64;
parameter S_MIN_YELLOW = 100;
parameter V_MIN_YELLOW = 65;


/////////////////blue//////////////////////////
parameter H_MIN_BLUE = 200;
parameter H_MAX_BLUE = 270;
parameter S_MIN_BLUE = 70;
parameter V_MIN_BLUE = 30;


////////////////////////ycbcr////////////////////////////
	wire en_red;
    wire en_blue;
    wire en_yellow;

    assign en0      =i_ycbcr[23:16] >=Y_TL  && i_ycbcr[23:16] <= Y_TH;
    assign en1      =i_ycbcr[15: 8] >=CB_TL && i_ycbcr[15: 8] <= CB_TH;
    assign en2      =i_ycbcr[ 7: 0] >=CR_TL && i_ycbcr[ 7: 0] <= CR_TH;
    assign en0_b      =i_ycbcr[23:16] >=Y_TL_B && i_ycbcr[23:16] <= Y_TH_B  ;
    assign en1_b      =i_ycbcr[15: 8] >=CB_TL_B && i_ycbcr[15: 8] <= CB_TH_B;
    assign en2_b      =i_ycbcr[ 7: 0] >=CR_TL_B && i_ycbcr[ 7: 0] <= CR_TH_B;
    //assign en0_g      =i_rgb[23:16] >=8'd0 && i_rgb[23:16]  <= 8'd120;
    //assign en1_g      =i_rgb[15: 8] >=8'd160  && i_rgb[15: 8] <= 8'd250;
    //assign en2_g      =i_rgb[ 7: 0] >=8'd0  && i_rgb[ 7: 0] <= 8'd120;
    assign en_red = (en0==1'b1 && en1 ==1'b1 && en2==1'b1)?1:0;
    assign en_blue = (en0_b==1'b1 && en1_b ==1'b1 && en2_b==1'b1)?1:0;
    assign en_yellow = (i_ycbcr[23:16] >= Y_MIN_YELLOW)  && (i_ycbcr[23:16] <= Y_MAX_YELLOW) &&  
                      (i_ycbcr[15:8]  >= CB_MIN_YELLOW) && (i_ycbcr[15:8]  <= CB_MAX_YELLOW) &&  
                      (i_ycbcr[7:0]   >= CR_MIN_YELLOW)  && (i_ycbcr[7:0]   <= CR_MAX_YELLOW); 
	wire en_black = (i_ycbcr[23:16] <= Y_MAX_BLACK) &&  
               (i_ycbcr[15:8]  >= CB_MIN_BLACK) && (i_ycbcr[15:8] <= CB_MAX_BLACK) &&  
               (i_ycbcr[7:0]   >= CR_MIN_BLACK) && (i_ycbcr[7:0]  <= CR_MAX_BLACK);  
//////////////////////////////////////////hsv///////////////////////////////////////// 
wire [8:0]hsv_h;
wire [8:0]hsv_s;
wire [7:0]hsv_v;
assign hsv_h = i_hsv_h;
assign hsv_s = i_hsv_s;
assign hsv_v = i_hsv_v;
assign is_red = (
    ((hsv_h >= H_MIN_RED_LOW) && (hsv_h <= H_MAX_RED_LOW)) || 
    ((hsv_h >= H_MIN_RED_HIGH) && (hsv_h <= H_MAX_RED_HIGH)))
    && (hsv_s >= S_MIN_RED) && (hsv_v >= V_MIN_RED);
assign is_yellow = 
    (hsv_h >= H_MIN_YELLOW) && (hsv_h <= H_MAX_YELLOW) && 
    (hsv_s >= S_MIN_YELLOW) && 
    (hsv_v >= V_MIN_YELLOW);
assign is_blue = 
    (hsv_h >= H_MIN_BLUE) && (hsv_h <= H_MAX_BLUE) && 
    (hsv_s >= S_MIN_BLUE) && 
    (hsv_v >= V_MIN_BLUE);
/***************************************timing***********************************************/

always @(posedge pixelclk)begin
    h_sync_r<= i_hsync;
    v_sync_r<= i_vsync;
    de_r    <= i_de;
    i_rgb_r <= i_rgb;
	i_x_r	<= i_x;
	i_y_r 	<= i_y;
end 

/********************************************************************************************/
always @(posedge pixelclk) begin
    monoc_d0 <= monoc;
end

always @(posedge pixelclk or negedge reset_n) begin
    if(!reset_n)begin 
        binary_r <= 24'd0;
    end 
    else begin 
        if(is_red || is_blue || en_yellow || en_black)
			monoc <= 1'b0;
		else 
			monoc <= 1'b1;
	end 
end

endmodule 