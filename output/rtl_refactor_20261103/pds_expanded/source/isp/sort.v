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

    // 2026-11-03: reuse the six directed comparisons across all selections.
    wire one_ge_two = data1 >= data2;
    wire one_ge_three = data1 >= data3;
    wire two_ge_one = data2 >= data1;
    wire two_ge_three = data2 >= data3;
    wire three_ge_one = data3 >= data1;
    wire three_ge_two = data3 >= data2;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            max_data <= 8'd0;
            mid_data <= 8'd0;
            min_data <= 8'd0;
        end else begin
            if (one_ge_two && one_ge_three)
                max_data <= data1;
            else if (two_ge_one && two_ge_three)
                max_data <= data2;
            else if (three_ge_one && three_ge_two)
                max_data <= data3;

            if ((two_ge_one && one_ge_three) || (three_ge_one && one_ge_two))
                mid_data <= data1;
            else if ((one_ge_two && two_ge_three) || (three_ge_two && two_ge_one))
                mid_data <= data2;
            // The legacy third test covers this ordering only.
            else if (one_ge_three && three_ge_two)
                mid_data <= data3;

            if (three_ge_one && two_ge_one)
                min_data <= data1;
            else if (three_ge_two && one_ge_two)
                min_data <= data2;
            else if (one_ge_three && two_ge_three)
                min_data <= data3;
        end
    end
endmodule
