module ISP #(
	parameter       H_PIXEL = 640,
	parameter       V_PIXEL = 480
)(
    input           pixelclk     ,
    input           rst_n        ,
    input  [23:0]   i_rgb        ,
    input           i_hsync      ,
    input           i_vsync      ,
    input           i_de         ,
	input  [9:0]	i_x          ,
	input  [9:0]	i_y          ,

    output [23:0]   VGA_rgb      ,
    output          VGA_hsync    ,
    output          VGA_vsync    ,
    output          VGA_de       ,
	
	output [9:0]	o_x          ,
	output [9:0]	o_y          ,
	output [10:0]	row_high     ,
	output [10:0]	col_left     ,
	
	output 			red_en       ,
	output 			blue_en      ,
	output			yellow_en
);

//rgb_change
wire [8:0]hsv_h;
wire [8:0]hsv_s;
wire [7:0]hsv_v;
wire [23:0]hsv_rgb;
wire hsv_vsync;
wire hsv_hsync;
wire hsv_de;

rgb2hsv rgb2hsv(
    .clk         (pixelclk        ),
    .reset_n     (rst_n           ),
    .rgb_r       (i_rgb[23:16]    ),
    .rgb_g       (i_rgb[15:8]     ),
    .rgb_b       (i_rgb[7:0]      ),
    .vs          (i_vsync         ),
    .hs          (i_hsync         ),
    .de          (i_de            ),
    
    .o_rgb       (hsv_rgb         ),
    .hsv_h       (hsv_h           ),
    .hsv_s       (hsv_s           ),
    .hsv_v       (hsv_v           ),
    .hsv_vs      (hsv_vsync       ),
    .hsv_hs      (hsv_hsync       ),
    .hsv_de      (hsv_de          )  
);

//ycbcr转化模块
wire [23:0] o_rgb;
wire [9:0] ycbcr_o_y;
wire [9:0] ycbcr_o_x;
wire [23:0] o_ycbcr;
wire o_hsync;
wire o_vsync;
wire o_de;

rgb2ycbcr rgb2ycbcr(
    .pixelclk     (pixelclk       ),
	 .rst_n       (rst_n          ),
    .i_rgb        (i_rgb          ),
    .i_hsync      (i_hsync        ),
    .i_vsync      (i_vsync        ),
    .i_de         (i_de           ),
	.x			  (i_x            ),
	.y			  (i_y            ),
    .i_de0        (),
  
    .o_rgb        (o_rgb          ),
	.o_x		  (ycbcr_o_x      ),
	.o_y		  (ycbcr_o_y      ),
    .o_ycbcr      (o_ycbcr        ),
    .o_hsync      (o_hsync        ),
    .o_vsync      (o_vsync        ),    
    .o_de0        (),                          
    .o_de         (o_de           )                                                                                        
);

//使能生成模块
wire green_en;

color_en_generate 
#(
    .H_PIXEL      (H_PIXEL        ),
    .V_PIXEL      (V_PIXEL        )
)
color_en_generate(
    .clk          (pixelclk       ),
    .rst_n        (rst_n          ),
    .vs           (o_vsync        ),
    .de           (o_de           ),
    .i_ycbcr      (o_ycbcr        ),
	.x		      (ycbcr_o_x      ),
	.y		      (ycbcr_o_y      ),
	
    .red_en       (red_en         ),
    .blue_en      (blue_en        ),
    .yellow_en    (yellow_en      ),
    .green_en     (green_en       )
);

//二值化模块
wire post_frame_vsync,post_frame_de,post_frame_hsync;
wire monoc;
wire [9:0] binary_x;
wire [9:0] binary_y;
wire [23:0] binary_rgb;

color_binary color_binary(
    //module clock
    .pixelclk     (pixelclk        ),// 时钟信号
    .reset_n      (rst_n           ), // 复位信号（低有效）
    //图像处理前的数据接口
    .i_vsync      (o_vsync         ),// vsync信号
    .i_hsync      (o_hsync         ),// href信号
    .i_de         (o_de            ), // data enable信号
	.i_x		  (ycbcr_o_x       ),
	.i_y		  (ycbcr_o_y       ),
    .i_ycbcr      (o_ycbcr         ),
	.i_rgb		  (o_rgb           ),
	.i_hsv_h	  (hsv_h           ),
	.i_hsv_v	  (hsv_v           ),
	.i_hsv_s	  (hsv_s           ),
    //图像处理后的数据接口
    .o_vsync   	  (post_frame_vsync), // vsync信号
    .o_hsync   	  (post_frame_hsync), // href信号
    .o_de      	  (post_frame_de   ), // data enable信号
    .o_monoc      (monoc           ), // 单色图像像素数据
	.o_x		  (binary_x	       ),
	.o_y		  (binary_y		   ),
	.o_rgb		  (binary_rgb	   ),
    .monoc_fall   ()
    //user interface
);

//腐蚀
wire dia_vs;
wire dia_hs;
wire dia_de;
wire dia_bit;
wire [23:0] dia_rgb;
wire [9:0] dia_x;
wire [9:0] dia_y;

dialate
#(
	.U_COL        (H_PIXEL       ),
	.U_ROW        (V_PIXEL       )
)u_dialate
(
	.clk          (pixelclk	     ),
	.rst_n        (rst_n		 ),
	
	.in_vs        (post_frame_vsync),
	.in_hs        (post_frame_hsync),
	.in_de        (post_frame_de ),  
	.in_data      (monoc         ),   
	.in_rgb		  (binary_rgb    ),  
                                     
	.in_x		  (binary_x      ),  
	.in_y		  (binary_y      ),  
	
	.out_vs       (dia_vs		 ),
	.out_hs       (dia_hs		 ),	
	.out_de       (dia_de		 ),
	.out_data     (dia_bit	     ),
	.out_rgb	  (dia_rgb	     ),

	.out_x		  (dia_x		 ),
	.out_y		  (dia_y		 )
);

//膨胀
wire ero_vs;
wire ero_hs;
wire ero_de;
wire ero_bit;
wire [23:0] ero_rgb;
wire [9:0] ero_x;
wire [9:0] ero_y;

erode
#(
	.U_COL        (H_PIXEL       ),
	.U_ROW        (V_PIXEL       )
)u_erode
(
	.clk          (pixelclk	     ),
	.rst_n        (rst_n		 ),
	
	.in_vs        (dia_vs		 ),
	.in_hs        (dia_hs		 ),	
	.in_de        (dia_de		 ),
	.in_data      (dia_bit	     ),
	.in_rgb		  (dia_rgb	     ),
                                 
	.in_x		  (dia_x		 ),
	.in_y		  (dia_y		 ),
	
	.out_vs       (ero_vs		 ),
	.out_hs       (ero_hs		 ),	
	.out_de       (ero_de		 ),
	.out_data     (ero_bit	     ),
	.out_rgb	  (ero_rgb	     ),

	.out_x		  (ero_x		 ),
	.out_y		  (ero_y		 )
);

wire edge_bit;
assign edge_bit = ((i_x <= 120 || i_x >= 520) || (i_y <= 90 || i_y >= 320)) ? 1'b1 : ero_bit;

color_dt #(
    .COL          (H_PIXEL   ),
    .ROW          (V_PIXEL   )
)u_color_dt(
    .video_pclk   (pixelclk  ),
    .rst_n        (rst_n     ),
    .video_vs     (ero_vs    ),
    .video_valid  (ero_de    ),
    .video_data   (ero_rgb   ),
    .video_bit    (edge_bit  ),
    .video_x      (i_x       ),
    .video_y      (i_y       ),

	.red_en		  (red_en    ),
	.blue_en	  (blue_en   ),
	.yellow_en	  (yellow_en ),    

    .out_up       (row_high  ),
    .out_down     (),
    .out_left     (col_left  ),
    .out_right    (),
    .out_vs       (VGA_vsync ),
    .out_valid    (VGA_de    ),
    .out_data     (VGA_rgb   )
);

//assign VGA_rgb = {24{edge_bit}};
//assign VGA_vsync = ero_vs;
//assign VGA_de = ero_de;

endmodule