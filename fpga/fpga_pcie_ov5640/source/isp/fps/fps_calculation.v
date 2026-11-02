module fps_calculation
    (input wire [8 : 0] fps,
     output wire [3 : 0] fps_unit,
     // 此处提取十进制个位。
     output wire [3 : 0] fps_decade,
     output wire [3 : 0] fps_hundred);
    assign fps_unit = fps % 10;
    assign fps_decade = (fps / 10) % 10;
    assign fps_hundred = (fps / 100);
endmodule