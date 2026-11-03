	module sort
	// < 端口 >。
	(
	// 图像流水线：system --------------------------------------------
	input   wire                clk                     ,
	input   wire                rst_n                   ,
	// 停用示例，仅供对照：input ---------------------------------------------
	input   wire    [7:0]       data1                   ,
	input   wire    [7:0]       data2                   ,
	input   wire    [7:0]       data3                   ,
	// 停用示例，仅供对照：output --------------------------------------------
	output  reg     [7:0]       max_data                , // 本项对应最大取值。
	output  reg     [7:0]       mid_data                , // 本项对应中间取值。
	output  reg     [7:0]       min_data                  // 本项对应最小取值。
	);
	// ── 图像流水线 ──
	// 比较结果选择最大排序值。
	// ── 图像流水线 ──
	always @(posedge clk or negedge rst_n) begin
		if(!rst_n)
			max_data <= 8'd0;
		else if(data1 >= data2 && data1 >= data3) 
			max_data <= data1;
		else if(data2 >= data1 && data2 >= data3)
			max_data <= data2;
		else if(data3 >= data1 && data3 >= data2)
			max_data <= data3;
	end
	// ── 图像流水线 ──
	// 比较结果选择中间排序值。
	// ── 图像流水线 ──
	always @(posedge clk or negedge rst_n) begin
		if(!rst_n)
			mid_data <= 8'd0;
		else if((data2 >= data1 && data1 >= data3) || (data3 >= data1 && data1 >= data2))
			mid_data <= data1;
		else if((data1 >= data2 && data2 >= data3) || (data3 >= data2 && data2 >= data1))
			mid_data <= data2;
		else if((data1 >= data3 && data3 >= data2) || (data1 >= data3 && data3 >= data2))
			mid_data <= data3;
	end
	// ── 图像流水线 ──
	// 比较结果选择最小排序值。
	// ── 图像流水线 ──
	always @(posedge clk or negedge rst_n) begin
		if(!rst_n)
			min_data <= 8'd0;
		else if(data3 >= data1 && data2 >= data1)
			min_data <= data1;
		else if(data3 >= data2 && data1 >= data2)
			min_data <= data2;
		else if(data1 >= data3 && data2 >= data3)
			min_data <= data3;
	end
	
	
	
	endmodule 