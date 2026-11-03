// 2026-11-03 注释校订。将 RGB565 输入送入识别通路，并把颜色叠加结果与显示同步信号一起交给 VGA。
`timescale 1 ps/ 1 ps

// 参数记录为 Create Date: 04-14-2025 22:33:41。

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

wire [15:0] i_vip_rgb;
assign i_vip_rgb = ((x <= 100 || x>=540) || (y <= 40 || y >= 400))?16'b11111_000000_00000:i_rgb;

wire vip_de, vip_vs;
wire [15:0] vip_rgb;
wire [7:0]  digit;

vip u_vip(

    .clk              (clk        ),
    .rst_n            (rst_n      ),

    .pre_frame_vsync  (i_vsync    ),
    .pre_frame_hsync  (),
    .pre_frame_de     (i_de       ),
    .pre_rgb          (i_vip_rgb  ),
    .xpos             (x          ),
    .ypos             (y          ),

    .h_total_pexel    (H_PIXEL    ),
    .v_total_pexel    (V_PIXEL    ),
    .post_frame_vsync (vip_vs     ),
    .post_frame_hsync (),
    .post_frame_de    (vip_de     ),
	.post_rgb         (vip_rgb    ),
	.digit            (digit      )
);

wire [23:0] rgb888;
assign rgb888 = {i_rgb[15:11], 3'b0, i_rgb[10:5], 2'b0, i_rgb[4:0],3'b0};

wire red_en, blue_en, green_en, yellow_en;

wire [25:0] display_pixel;
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
	.VGA_rgb      (display_pixel[23:0]),
	.VGA_hsync    (),
	.VGA_vsync    (display_pixel[24]),
	.VGA_de       (display_pixel[25]),
	.o_y		  (isp_y    ),
	.o_x		  (isp_x    ),
	.col_left	  (col_left ),
	.row_high	  (row_high ),
	.blue_en      (blue_en  ),
	.red_en       (red_en   ),
	.yellow_en    (yellow_en)
 );

assign {VGA_de, VGA_vsync, VGA_rgb} =
    {display_pixel[25:19], display_pixel[15:10], display_pixel[7:3]};

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

endmodule
