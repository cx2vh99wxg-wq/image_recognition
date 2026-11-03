// 2026-11-03 注释校订。将 9 位帧率分为十进制百、十、个位，十位和个位由商与余数得到。
module fps_calculation
    (input wire [8 : 0] fps,
     output wire [3 : 0] fps_unit,

     output wire [3 : 0] fps_decade,
     output wire [3 : 0] fps_hundred);

    wire [8:0] tens_quotient = fps / 9'd10;
    wire [5:0] tens = tens_quotient[5:0];
    wire [5:0] hundreds = tens / 6'd10;
    wire [8:0] units_remainder = fps - tens * 9'd10;
    wire [5:0] tens_remainder = tens - hundreds * 6'd10;
    assign fps_unit = units_remainder[3:0];
    assign fps_decade = tens_remainder[3:0];
    assign fps_hundred = hundreds[3:0];
endmodule
