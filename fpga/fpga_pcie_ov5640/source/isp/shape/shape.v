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

//灰度
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

//hsv
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

//二值化
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
wire ero_vs;
wire ero_hs;
wire ero_de;
wire ero_bit;
wire [9:0] ero_x;
wire [9:0] ero_y;

erode
#(
	.U_COL      (H_PIXEL    ),
	.U_ROW      (V_PIXEL    )
)shape_erode
(
	.clk        (clk	    ),
	.rst_n      (rst_n		),
	
	.in_vs      (binary_vs	),
	.in_hs      (binary_hs	),	
	.in_de      (binary_de	),
	.in_data    (binary_data[7]	),
	
	.in_x		(binary_x	),
	.in_y		(binary_y	),
	
	.out_vs     (ero_vs		),
	.out_hs     (ero_hs		),	
	.out_de     (ero_de		),
	.out_data   (ero_bit	),
	
	.out_x		(ero_x		),
	.out_y		(ero_y		)
);

wire dia_vs;
wire dia_hs;
wire dia_de;
wire dia_bit;
wire [9:0] dia_x;
wire [9:0] dia_y;

dialate
#(
	.U_COL      (H_PIXEL    ),
	.U_ROW      (V_PIXEL    )
)shape_dialate
(
	.clk        (clk	    ),
	.rst_n      (rst_n		),
	
	.in_vs      (ero_vs	),
	.in_hs      (ero_hs	),	
	.in_de      (ero_de	),
	.in_data    (ero_bit),
	
	.in_x		(ero_x	),
	.in_y		(ero_y	),
	
	.out_vs     (dia_vs		),
	.out_hs     (dia_hs		),	
	.out_de     (dia_de		),
	.out_data   (dia_bit	),
	
	.out_x		(dia_x		),
	.out_y		(dia_y		)
);

//parameter define
parameter NUM_ROW = 1  ;               // 需识别的图像的行数
parameter NUM_COL = 1  ;               // 需识别的图像的列数
parameter DEPBIT  = 11 ;               // 数据位宽

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

//投影模块
shape_projection #(
    .NUM_ROW(NUM_ROW),
    .NUM_COL(NUM_COL),
    .DEPBIT (DEPBIT)
) u_projection(
    //module clock
    .clk                (clk			),          // 时钟信号
    .rst_n              (rst_n  		),          // 复位信号（低有效）
    //Image data interface
    .frame_vsync        (dia_vs		), // vsync信号
    .frame_hsync        (dia_hs		), // href信号
    .frame_de           (dia_de	), // data enable信号
    .monoc              (dia_bit  	), // 单色图像像素数据
    .xpos               (i_x		),
    .ypos               (i_y		),
    //project border ram interface
    .row_border_addr_rd (row_border_addr),
    .row_border_data_rd (row_border_data),
    .col_border_addr_rd (col_border_addr),
    .col_border_data_rd (col_border_data),
    //user interface
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

//画框输出
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
    //module clock
    .clk                (clk       		),        // 时钟信号
    .rst_n              (rst_n			),        // 复位信号（低有效）
    //image data interface
    .xpos               (i_x    	),
    .ypos               (i_y    	),
	.i_rgb				({8{dia_bit}}	),
	.i_vsync			(dia_vs		),
	.i_hsync			(dia_hs		),
	.i_de				(dia_de		),

	.en_r				(en_r			),
	.en_b				(en_b			),
	.en_y				(en_y			),
	
    //project border ram interface
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
    //user interface
    .frame_cnt          (frame_cnt		),
    .project_done_flag  (project_done_flag),
	.o_shape				(shape			)
);

assign shape_vs = edge_vs;
assign shape_hs = edge_hs;
assign shape_de = edge_de;
assign shape_data = edge_data;
/*
assign shape_vs = dia_vs;
assign shape_hs = dia_hs;
assign shape_de = dia_de;
assign shape_data = {8{dia_bit}};
*/
endmodule