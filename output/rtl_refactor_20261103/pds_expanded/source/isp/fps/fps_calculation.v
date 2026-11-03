module fps_calculation
    (input wire [8 : 0] fps,
     output wire [3 : 0] fps_unit,
     // 此处提取十进制个位。
     output wire [3 : 0] fps_decade,
     output wire [3 : 0] fps_hundred);
    // 2026-11-03: reuse each decimal quotient to form the next digit.
    wire [8:0] tens_quotient = fps / 9'd10;
    wire [5:0] tens = tens_quotient[5:0];
    wire [5:0] hundreds = tens / 6'd10;
    wire [8:0] units_remainder = fps - tens * 9'd10;
    wire [5:0] tens_remainder = tens - hundreds * 6'd10;
    assign fps_unit = units_remainder[3:0];
    assign fps_decade = tens_remainder[3:0];
    assign fps_hundred = hundreds[3:0];
endmodule
