`timescale 1 ps/ 1 ps
//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date: 04-14-2025 22:33:41
// Design Name:
// Module Name: isp_ctrl
// Project Name:
// Target Devices:
// Tool Versions:
// Description:
//
// Dependencies:
//
// Revision:
// Additional Comments:
//
//////////////////////////////////////////////////////////////////////////////////

module isp_ctrl #(
    parameter H_PIXEL = 640,
    parameter V_PIXEL = 480 
)(
	input            clk	  ,
    input            sys_clk  ,
    input            rst_n    ,
    input    [15:0]  i_rgb    ,
    input            i_vsync  ,
    input            i_de     ,
    input    [9:0]   x        ,
    input    [9:0]   y        ,

    output   [15:0]  VGA_rgb  ,
    output           VGA_vsync,
    output           VGA_de	  
);

//--------------- digitial_recognition ---------------//
wire [15:0] i_vip_rgb;
assign i_vip_rgb = ((x <= 100 || x>=540) || (y <= 40 || y >= 400))?16'b11111_000000_00000:i_rgb;

wire vip_de, vip_vs;
wire [15:0] vip_rgb;
wire [7:0]  digit;

vip u_vip(
    //module clock
    .clk              (clk        ),// ʱ���ź�
    .rst_n            (rst_n      ),// ��λ�źţ�����Ч��
    //ͼ����ǰ�����ݽӿ�
    .pre_frame_vsync  (i_vsync    ),
    .pre_frame_hsync  (),
    .pre_frame_de     (i_de       ),
    .pre_rgb          (i_vip_rgb  ),
    .xpos             (x          ),
    .ypos             (y          ),
    //ͼ����������ݽӿ�
    .h_total_pexel    (H_PIXEL    ),
    .v_total_pexel    (V_PIXEL    ),    
    .post_frame_vsync (vip_vs     ),// ������ĳ��ź�
    .post_frame_hsync (),
    .post_frame_de    (vip_de     ),// �������������Чʹ�� 
	.post_rgb         (vip_rgb    ),// �������ͼ������
	.digit            (digit      ) // ʶ�𵽵�����
);   

//assign VGA_de = vip_de;
//assign VGA_hsync = vip_hs;
//assign VGA_vsync = vip_vs;
//assign VGA_rgb = vip_rgb;

//--------------- color_recognition ---------------//
wire [23:0] rgb888;
assign rgb888 = {i_rgb[15:11], 3'b0, i_rgb[10:5], 2'b0, i_rgb[4:0],3'b0};

wire red_en, blue_en, green_en, yellow_en;
wire vga_de, vga_vs;
wire [23:0] vga_rgb;
wire [9:0] isp_x;
wire [9:0] isp_y;
wire		binary_de;
wire		binary_vs;
wire [23:0]	binary_data;
wire [10:0]	col_left;
wire [10:0] row_high;

ISP  #(
	.H_PIXEL      (H_PIXEL  ),
    .V_PIXEL      (V_PIXEL  )
)u_color_rec(
	.pixelclk     (clk      ),
	.rst_n        (rst_n    ),
	.i_rgb        (rgb888   ),
	.i_hsync      (),
	.i_vsync      (i_vsync  ),
	.i_de         (i_de     ),
	.i_y	      (y        ),
	.i_x	      (x        ),
	.VGA_rgb      (vga_rgb  ),
	.VGA_hsync    (),
	.VGA_vsync    (vga_vs   ),
	.VGA_de       (vga_de   ),
	.o_y		  (isp_y    ),
	.o_x		  (isp_x    ),
	.col_left	  (col_left ),
	.row_high	  (row_high ),
	.blue_en      (blue_en  ),
	.red_en       (red_en   ),
	.yellow_en    (yellow_en)
 );

assign VGA_de = vga_de;
assign VGA_vsync = vga_vs;
assign VGA_rgb = {vga_rgb[23:19], vga_rgb[15:10], vga_rgb[7:3]};

////////////////shape_recognition//////////////
wire shape_vs, shape_de;
wire [7:0]shape_binary;
wire [15:0]shape_en;
shape #(
	.H_PIXEL(H_PIXEL),
    .V_PIXEL(V_PIXEL)
)shape
(
	.clk            (clk    ),
    .rst_n          (rst_n      ),
     
	.pre_vs         (i_vsync    	),
	.pre_hs         (    	),
	.pre_de         (i_de    	),
	.pre_data       (rgb888),
	
	.i_x			(x		),
	.i_y			(y		),
            
	.shape_vs       (shape_vs  ),
	.shape_hs       (  ),
	.shape_de       (shape_de  ),  
    .shape_data     (shape_binary),
	.shape			(shape_en  )
);

//assign VGA_de = shape_de;
//assign VGA_vsync = shape_vs;
//assign VGA_rgb = {16{shape_binary[7]}};

/*
//////////////////Classifier//////////////////
wire [15:0] char_en;
Classifier Classifier(
	.clk(clk),
	.rst_n(rst_n),
	.blue_en (blue_en),
	.red_en(red_en),
	.yellow_en(yellow_en),
	.shape_en (shape_en),
	.digit (digit),
	.char_en(char_en)
);

//////////////recognized_char_output//////////

wire [23:0] VGA_rgb888;

   char_display char_display(
    .clk_pixel(clk),
    .sys_clk(sys_clk),
    .rst_n(rst_n),
    .rgb(vga_rgb),
	.char_en(char_en),
    .i_vs(vga_vs),
    .i_hs(),
    .i_de(vga_de),
	.col_left(col_left),
	.row_low(row_high),
    .h_cnt(isp_x),   
    .v_cnt(isp_y),   
    .o_vs(VGA_vsync),
    .o_hs(),
    .o_de(VGA_de),
    .pixel_rgb(VGA_rgb888)
);

assign VGA_rgb = {VGA_rgb888[23:19], VGA_rgb888[15:10], VGA_rgb888[7:3]};
*/
endmodule
