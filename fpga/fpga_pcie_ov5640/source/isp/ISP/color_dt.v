module color_dt
#(
	parameter     COL = 640  ,
	parameter     ROW = 480
)
(
	input         video_pclk ,
	input         rst_n      ,
	input         video_valid,
    input         video_vs   ,
	input  [23:0] video_data ,
	input         video_bit  ,
    input  [9:0]  video_x    ,
    input  [9:0]  video_y    ,
          
    input 	      red_en     ,
    input 	      blue_en    ,
    input 	      yellow_en  ,
	
    output [9:0]  out_up     ,
    output [9:0]  out_down   ,
    output [9:0]  out_left   ,
    output [9:0]  out_right  ,
	output        out_valid  ,
    output        out_vs     ,
	output [23:0] out_data
);

//坐标计算
reg [15:0] col_cnt;
reg [15:0] row_cnt;

always @(posedge video_pclk or negedge rst_n) begin
	if(rst_n == 1'b0)
		col_cnt             <=          16'd0;
	else if(col_cnt == COL-1 && video_valid == 1'b1)
		col_cnt             <=          16'd0;
	else if(video_valid == 1'b1)
		col_cnt             <=          col_cnt + 1'b1;
	else
		col_cnt             <=          col_cnt;
end

always @(posedge video_pclk or negedge rst_n) begin
	if(rst_n == 1'b0)
		row_cnt             <=          16'd0;
	else if(row_cnt == ROW-1 && col_cnt == COL-1 && video_valid == 1'b1)
		row_cnt             <=          16'd0;
	else if(col_cnt == COL-1 && video_valid == 1'b1) 
		row_cnt             <=          row_cnt + 1'b1;
	else
		row_cnt             <=          row_cnt;
end

//计算最大边框
reg [15:0] 	up_reg	    ;	  
reg [15:0] 	down_reg 	;
reg [15:0] 	left_reg 	;
reg [15:0] 	right_reg	;
reg			flag_reg 	;

always@(posedge video_pclk or negedge rst_n) begin
	if(!rst_n) begin
		up_reg	<= ROW		;
		down_reg  <= 16'd0	;
		left_reg  <= COL	;
		right_reg <= 16'd0	;
		flag_reg  <= 1'b0	;
	end
	else if(row_cnt == ROW-1 && col_cnt == COL-1 && video_valid == 1'b1)begin
		up_reg	<= ROW		;
		down_reg  <= 16'd0	;
		left_reg  <= COL	;
		right_reg <= 16'd0	;
		flag_reg  <= 1'b0	;
	end
	else if(video_valid & (~video_bit)) begin
		flag_reg  <= 1'b1;
		
		if(col_cnt < left_reg) 
			left_reg <= col_cnt;		//左边界
		else
			left_reg <= left_reg;
			
		if(col_cnt > right_reg) 
			right_reg <= col_cnt;		//右边界
		else
			right_reg <= right_reg;
			
		if(row_cnt < up_reg) 
			up_reg <= row_cnt;		    //上边界
		else
			up_reg <= up_reg;
			
		if(row_cnt > down_reg) 
			down_reg <= row_cnt;		//下边界
		else
			down_reg <= down_reg;	
	end
end

reg [15:0] 	rectangular_up		;
reg [15:0] 	rectangular_down 	;
reg [15:0] 	rectangular_left 	;
reg [15:0] 	rectangular_right	;
reg			rectangular_flag 	;

always@(posedge video_pclk or negedge rst_n) begin
	if(!rst_n) begin
		rectangular_up	  <= 16'd0;
		rectangular_down  <= 16'd0;
		rectangular_left  <= 16'd0;
		rectangular_right <= 16'd0;
		rectangular_flag  <= 1'b0 ;
	end
	else if((col_cnt == COL - 1) && (row_cnt == ROW - 1))begin
		rectangular_up	  <= up_reg	  ;		
		rectangular_down  <= down_reg ;
		rectangular_left  <= left_reg ;		
		rectangular_right <= right_reg;
		rectangular_flag  <= flag_reg ;
	end
end

//*****************************************************
//绘制矩形框

//计算摄像头输入图像的像素坐标
wire [15:0] x_cnt;
wire [15:0] y_cnt;

assign x_cnt = video_x;
assign y_cnt = video_y;

reg boarder_flag;	//标志着像素点位于方框上

always@(posedge video_pclk or negedge rst_n) begin
	if(!rst_n) begin
		boarder_flag <= 1'd0;			
	end
	else begin	
			if(rectangular_flag)begin
				if((x_cnt >  rectangular_left) && (x_cnt < rectangular_right)
						&& ((y_cnt == rectangular_up ) ||(y_cnt == rectangular_down)) ) begin //绘制上下边界
					boarder_flag <= 1'd1;	
				end
				else if((y_cnt > rectangular_up) && (y_cnt < rectangular_down)
						&& ((x_cnt == rectangular_left) ||(x_cnt == rectangular_right)) ) begin //绘制左右边界
					boarder_flag <= 1'd1;
				end
				else begin
					boarder_flag <= 1'd0;
				end
			end
			else begin	
				boarder_flag <= 1'd0;
			end
		end
	end

reg [23:0] color_data;

always @ (posedge video_pclk or negedge rst_n ) begin
	if(!rst_n)
		color_data <= 24'b0;
	else if(red_en)
		color_data <= 24'hff0000;
    else if(blue_en)
		color_data <= 24'h0000ff;
    else if(yellow_en)
		color_data <= 24'hffff00;
end

reg        video_valid_d;
reg        video_vs_d;
reg [23:0] video_data_d;

always @ (posedge video_pclk or negedge rst_n ) begin
	if(!rst_n) begin
		video_valid_d  <= 1'b0;
        video_vs_d     <= 1'b0;
		video_data_d   <= 24'b0;
	end
	else begin
		video_valid_d <= video_valid;
        video_vs_d    <= video_vs;
		video_data_d  <= boarder_flag ? color_data : video_data;
	end
end

assign out_up    = rectangular_up	;
assign out_down  = rectangular_down ;
assign out_left  = rectangular_left ;
assign out_right = rectangular_right;
assign out_valid = video_valid_d    ;
assign out_vs    = video_vs_d       ;
assign out_data  = video_data_d     ;

endmodule