// 2026-11-03 注释校订。轮廓识别通路。阈值结果先腐蚀后膨胀，再进行投影和形状判断。
module shape #(
	parameter   H_PIXEL   = 640  ,
	parameter   V_PIXEL   = 480
)
(
	input                   clk			,
    input                   rst_n       ,

	input                   pre_vs		,
    input                   pre_hs      ,
	input                   pre_de		,
	input       [23:0] 	    pre_data	,

	input	 	[9:0]	 	i_x			,
	input 	 	[9:0]	 	i_y			,

	output                  shape_vs    ,
    output                  shape_hs    ,
	output                  shape_de    ,
	output      [7:0]       shape_data  ,

    output      [15:0]      shape
);

wire gray_vs;
wire gray_hs;
wire gray_de;
wire [7:0] gray_data;
wire [7:0] gray_Cb;
wire [7:0] gray_Cr;
wire [9:0] gray_x;
wire [9:0] gray_y;

image_rgb2ycbcr my_image_rgb2ycbcr (
    .clk       (clk         ),
    .rst       (rst_n       ),

    .vs_i      (pre_vs      ),
    .hs_i      (pre_hs      ),
    .de_i      (pre_de      ),
    .img_data_i(pre_data    ),
	.i_x	   (i_x			),
	.i_y	   (i_y			),

    .vs_o      (gray_vs     ),
    .hs_o      (gray_hs     ),
    .de_o      (gray_de     ),
    .Y_o       (gray_data   ),
    .Cb_o      (gray_Cb     ),
    .Cr_o      (gray_Cr     ),
	.o_x	   (gray_x		),
	.o_y	   (gray_y		)
);

wire hsv_vs;
wire hsv_hs;
wire hsv_de;
wire [8:0] hsv_h;
wire [8:0] hsv_s;
wire [7:0] hsv_v;

image_rgb2hsv image_rgb2hsv (
    .clk       (clk         ),
    .rst_n     (rst_n       ),

    .vs        (pre_vs      ),
    .hs        (pre_hs      ),
    .de        (pre_de      ),
    .rgb_r     (pre_data[23:16]),
    .rgb_g     (pre_data[15:8]),
    .rgb_b     (pre_data[7:0]),

    .hsv_vs    (hsv_vs      ),
    .hsv_hs    (hsv_hs      ),
    .hsv_de    (hsv_de      ),
    .hsv_h     (hsv_h       ),
    .hsv_s     (hsv_s       ),
    .hsv_v     (hsv_v       )
);

wire binary_vs;
wire binary_hs;
wire binary_de;
wire [7:0] binary_data;
wire [9:0] binary_x;
wire [9:0] binary_y;
wire en_r;
wire en_b;
wire en_y;

color_en_generate
#(
    .H_PIXEL	(H_PIXEL	),
    .V_PIXEL	(V_PIXEL	)
)color_en_generate_u1
(
    .clk		(clk		),
    .rst_n		(rst_n		),
    .i_ycbcr	({gray_data, gray_Cb, gray_Cr}),
    .vs			(gray_vs	),
    .de			(gray_de	),
	.x			(gray_x		),
	.y			(gray_y		),

    .red_en		(en_r		),
    .blue_en	(en_b		),
    .yellow_en	(en_y		),
    .green_en	()
);

threshold_binary my_threshold_binary(
    .clk       (clk        ),
    .rst_n     (rst_n      ),
    .types     (0          ),

    .i_ycbcr   ({gray_data, gray_Cb, gray_Cr}),
    .i_hsync   (gray_hs    ),
    .i_vsync   (gray_vs    ),
    .i_de      (gray_de    ),

    .i_hsv_h   (hsv_h      ),
    .i_hsv_s   (hsv_s      ),
    .i_hsv_v   (hsv_v      ),
    .i_hsync_hsv(hsv_hs    ),
    .i_vsync_hsv(hsv_vs    ),
    .i_de_hsv   (hsv_de    ),

	.i_x	   (gray_x	   ),
	.i_y	   (gray_y	   ),

    .o_binary  (binary_data),
    .o_hsync   (binary_hs  ),
    .o_vsync   (binary_vs  ),
    .o_de      (binary_de  ),

	.o_x	   (binary_x   ),
	.o_y	   (binary_y   )

);

wire dia_vs;
wire dia_hs;
wire dia_de;
wire dia_bit;
wire [9:0] dia_x;
wire [9:0] dia_y;
wire [23:0] unused_morphology_rgb;
isp_morphology_pair #(
    .U_COL(H_PIXEL), .U_ROW(V_PIXEL), .ERODE_FIRST(1'b1)
) u_morphology_pair (
    .clk(clk), .rst_n(rst_n),
    .pixel_in({binary_vs, binary_hs, binary_de, binary_data[7], binary_x, binary_y, {24{1'bz}}}),
    .pixel_out({dia_vs, dia_hs, dia_de, dia_bit, dia_x, dia_y, unused_morphology_rgb})
);

parameter NUM_ROW = 1  ;
parameter NUM_COL = 1  ;
parameter DEPBIT  = 11 ;

wire  [ 3:0]  num_col  ;
wire  [ 1:0]  frame_cnt    ;
wire [10:0]left	;
wire [10:0]right;
wire [10:0]top;
wire [10:0]bottom;
wire  project_done_flag ;

wire   [DEPBIT-1:0]   row_border_addr;
wire   [DEPBIT-1:0]   row_border_data;
wire   [DEPBIT-1:0]   col_border_addr;
wire   [DEPBIT-1:0]   col_border_data;

shape_projection #(
    .NUM_ROW(NUM_ROW),
    .NUM_COL(NUM_COL),
    .DEPBIT (DEPBIT)
) u_projection(

    .clk                (clk			),
    .rst_n              (rst_n  		),

    .frame_vsync        (dia_vs		),
    .frame_hsync        (dia_hs		),
    .frame_de           (dia_de	),
    .monoc              (dia_bit  	),
    .xpos               (i_x		),
    .ypos               (i_y		),

    .row_border_addr_rd (row_border_addr),
    .row_border_data_rd (row_border_data),
    .col_border_addr_rd (col_border_addr),
    .col_border_data_rd (col_border_data),

	.h_total_pexel      (640			),
	.v_total_pexel      (480			),
    .num_col            (num_col		),
    .num_row            (num_row		),
    .frame_cnt          (frame_cnt		),
	.left				(),
	.right				(),
	.top				(),
	.bottom				(),
    .project_done_flag  (project_done_flag)
);

wire edge_vs;
wire edge_hs;
wire edge_de;
wire [7:0] edge_data;
wire [9:0] edge_x;
wire [9:0] edge_y;

shape_display #(
    .NUM_ROW(NUM_ROW),
    .NUM_COL(NUM_COL),
    .NUM_WIDTH((NUM_ROW*NUM_COL<<2)-1)
)shape_display(

    .clk                (clk       		),
    .rst_n              (rst_n			),

    .xpos               (i_x    	),
    .ypos               (i_y    	),
	.i_rgb				({8{dia_bit}}	),
	.i_vsync			(dia_vs		),
	.i_hsync			(dia_hs		),
	.i_de				(dia_de		),

	.en_r				(en_r			),
	.en_b				(en_b			),
	.en_y				(en_y			),

    .row_border_addr    (row_border_addr),
    .row_border_data    (row_border_data),
    .col_border_addr    (col_border_addr),
    .col_border_data    (col_border_data),
	.color_rgb          (edge_data      ),
	.o_vs				(edge_vs		),
	.o_hs				(edge_hs		),
	.o_de				(edge_de		),
	.o_x				(edge_x			),
	.o_y				(edge_y			),
	.o_top				(top			),
	.o_bottom			(bottom			),
	.o_left				(left			),
	.o_right			(right			),
    .num_col            (num_col		),
    .num_row            (num_row		),

    .frame_cnt          (frame_cnt		),
    .project_done_flag  (project_done_flag),
	.o_shape				(shape			)
);

assign shape_vs = edge_vs;
assign shape_hs = edge_hs;
assign shape_de = edge_de;
assign shape_data = edge_data;

endmodule