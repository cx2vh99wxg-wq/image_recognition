module Line_Shift_RAM
    #(parameter RAM_Length = 640,
      // 参数记录为 640*480。
      parameter DATA_WIDTH = 8)
    (input clken,
     input clock,
     input rst_n,
     input [DATA_WIDTH - 1 : 0] shiftin,
     output [DATA_WIDTH - 1 : 0] taps0x,
     output [DATA_WIDTH - 1 : 0] taps1x);

    // 2026-11-03: each tap uses the preceding line as its input.
    wire [DATA_WIDTH-1:0] lines [0:2];
    assign lines[0] = shiftin;
    assign taps0x = lines[1];
    assign taps1x = lines[2];
    genvar line;
    generate
        for (line = 0; line < 2; line = line + 1) begin : delay_line
            RAMshift_taps #(.TOTAL_RAM_Length(RAM_Length), .DATA_WIDTH(DATA_WIDTH)) u_delay (
                .clken(clken), .clock(clock), .rst_n(rst_n),
                .Delay_Length(RAM_Length), .shiftin(lines[line]), .shiftout(lines[line+1])
            );
        end
    endgenerate
endmodule
