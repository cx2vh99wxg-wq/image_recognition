// 2026-11-03 注释校订。共用三项输入的有向比较结果。中值分支沿用原选择条件和保持行为。
module sort

	(

	input   wire                clk                     ,
	input   wire                rst_n                   ,

	input   wire    [7:0]       data1                   ,
	input   wire    [7:0]       data2                   ,
	input   wire    [7:0]       data3                   ,

	output  reg     [7:0]       max_data                ,
	output  reg     [7:0]       mid_data                ,
	output  reg     [7:0]       min_data
	);

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
