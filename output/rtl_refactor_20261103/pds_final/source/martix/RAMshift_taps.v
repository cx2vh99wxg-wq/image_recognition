`timescale 1 ns / 1 ns

module RAMshift_taps
    #(parameter TOTAL_RAM_Length = 640,
      parameter DATA_WIDTH = 8)
    (input clken,
     input rst_n,
     input clock,
     input [31 : 0] Delay_Length,
     input [DATA_WIDTH - 1 : 0] shiftin,
     output [DATA_WIDTH - 1 : 0] shiftout);

    reg [31 : 0] RAM_CNT = 0;
    reg [DATA_WIDTH - 1 : 0] shift_ram[TOTAL_RAM_Length - 1 : 0];

    integer m;
    initial
    begin
        for (m = 0; m < TOTAL_RAM_Length; m = m + 1)
        begin
            shift_ram[m] = 0;
        end
    end

    // 2026-11-03: the circular pointer wraps even without clken.
    // A write still occurs on an enabled cycle while the pointer resets.
    always @(posedge clock) begin
        if (rst_n == 1'b0)
            RAM_CNT <= 0;
        else if (RAM_CNT == Delay_Length)
            RAM_CNT <= 0;
        else if (clken)
            RAM_CNT <= RAM_CNT + 1;

        if (clken)
            shift_ram[RAM_CNT] <= shiftin;
    end

    assign shiftout = shift_ram[RAM_CNT];
endmodule
