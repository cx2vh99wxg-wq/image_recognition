//图像高斯滤波
//Author:麝月小兴兴
//Time:2024-01-05

module image_gaussian
(
	input	wire			i_clk			, //时钟输入
	input	wire			i_rst_n			, //复位

	input	wire			i_vs			,
	input	wire			i_hs			,
	input	wire			i_de			, //图像有效显示区域
	input	signed  [7:0]	i_data			, //输入灰度图像
    input           [23:0]  i_rgb           ,
    
    input           [9:0]   i_x             ,
    input           [9:0]   i_y             ,
	
	output	wire			o_vs			,
	output	wire			o_hs			,
	output	wire			o_de			, //图像有效显示区域 
	output	signed  [7:0]	o_data			, //输出8bits处理后的图像
    output          [23:0]  o_rgb           ,

    output          [9:0]   o_x             ,
    output          [9:0]   o_y                      
);

//wire or reg define
//高斯滤波相关变量
reg signed [9:0]      sum_row1            ;//row 1，1，2，1
reg signed [10:0]      sum_row2            ;//row 2，2，4，2
reg signed [9:0]      sum_row3            ;//row 3，1，2，1
reg signed [11:0]      sum_window          ;
//3x3矩阵的变量
wire	[7:0]	matrix_11	;
wire	[7:0]	matrix_12	;
wire	[7:0]	matrix_13	;
wire	[7:0]	matrix_21	;
wire	[7:0]	matrix_22	;
wire	[7:0]	matrix_23	;
wire	[7:0]	matrix_31	;
wire	[7:0]	matrix_32	;
wire	[7:0]	matrix_33	;
//延时相关变量
reg     [23:0]      i_rgb_r0    ;
reg     [23:0]      i_rgb_r1    ;
reg		[1:0]		vs_r		;
reg		[1:0]		hs_r		;
reg		[1:0]		de_r		;
reg		[19:0]		x_r		    ;
reg		[19:0]		y_r		    ;

//main code 
//sum_r1,sum_r2,sum_r3
always @ (posedge i_clk or negedge i_rst_n ) begin
    if(!i_rst_n) begin
        sum_row1<=10'd0;
        sum_row2<=11'd0;
        sum_row3<=10'd0;
    end
    else begin
        sum_row1<=$signed(matrix_11)+$signed(matrix_12<<1)+$signed(matrix_13);
        sum_row2<=$signed(matrix_21<<1)+$signed(matrix_22<<2)+$signed(matrix_23<<1);
        sum_row3<=$signed(matrix_31)+$signed(matrix_32<<1)+$signed(matrix_33);
    end
end
//sum_window:3x3滑窗模块的加权和
always @ (posedge i_clk or negedge i_rst_n) begin
    if(!i_rst_n)
        sum_window<=12'd0;
    else
        sum_window<=sum_row1+sum_row2+sum_row3;
end

//视频时序延时, 延时两个时钟周期
always @(posedge i_clk or negedge i_rst_n) begin
	if(!i_rst_n) begin 
		hs_r<=2'd0;
		vs_r<=2'd0;
		de_r<=2'd0;
        x_r<=20'd0;
        y_r<=20'd0;
        i_rgb_r0<=24'd0;
        i_rgb_r1<=24'd0;
	end 
	else begin 
		vs_r<={vs_r[0],i_vs};
		hs_r<={hs_r[0],i_hs};
		de_r<={de_r[0],i_de};
		x_r<={x_r[9:0],i_x};
		y_r<={y_r[9:0],i_y};
        i_rgb_r0<=i_rgb;
        i_rgb_r1<=i_rgb_r0;
	end 
end 

//输出视频时序
assign o_vs = vs_r[1] ;
assign o_hs = hs_r[1] ;
assign o_de = de_r[1] ;
assign o_data = de_r[1] ? sum_window[11:4] : 8'd0 ;//sum_window[11:4]即为除以16的结果
assign o_rgb = i_rgb_r1;


//3x3矩阵生成模块
matrix_3x3_8bit #(
    . COL       (640        )   ,
    . ROW       (480        )
)matrix_3x3_21bit_inst
(
	. clk		(i_clk		)	,
	. rst_n	    (i_rst_n	)	,
	. valid_in  (i_de		)	,
	. din   	(i_data		)	,

	. matrix_11	(matrix_11	)	, //3x3矩阵内的9个数
	. matrix_12	(matrix_12	)	,
	. matrix_13	(matrix_13	)	,
	. matrix_21	(matrix_21	)	,
	. matrix_22	(matrix_22	)	,
	. matrix_23	(matrix_23	)	,
	. matrix_31	(matrix_31	)	,
	. matrix_32	(matrix_32	)	,
	. matrix_33	(matrix_33	)	
	
);

endmodule 
	