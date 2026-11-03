// 2026-11-03 注释校订。Gamma 0.29 的定值查找表。每个上升沿登记对应亮度，无匹配项时保持输出。
module Curve_Gamma_29
(
   input		[0:0]	clk,
   input		[7:0]	Pre_Data,
   output wire [7:0]	Post_Data
);

// 高地址在高位，输入亮度直接索引对应的字节。
localparam [2047:0] CURVE_BYTES = {
    8'hFF, 8'hFF, 8'hFE, 8'hFE, 8'hFE, 8'hFE, 8'hFD, 8'hFD, 8'hFD, 8'hFC, 8'hFC, 8'hFC, 8'hFB, 8'hFB, 8'hFB, 8'hFB,
    8'hFA, 8'hFA, 8'hFA, 8'hF9, 8'hF9, 8'hF9, 8'hF8, 8'hF8, 8'hF8, 8'hF7, 8'hF7, 8'hF7, 8'hF7, 8'hF6, 8'hF6, 8'hF6,
    8'hF5, 8'hF5, 8'hF5, 8'hF4, 8'hF4, 8'hF4, 8'hF3, 8'hF3, 8'hF3, 8'hF2, 8'hF2, 8'hF2, 8'hF1, 8'hF1, 8'hF1, 8'hF0,
    8'hF0, 8'hF0, 8'hEF, 8'hEF, 8'hEF, 8'hEE, 8'hEE, 8'hEE, 8'hED, 8'hED, 8'hED, 8'hEC, 8'hEC, 8'hEC, 8'hEB, 8'hEB,
    8'hEB, 8'hEA, 8'hEA, 8'hE9, 8'hE9, 8'hE9, 8'hE8, 8'hE8, 8'hE8, 8'hE7, 8'hE7, 8'hE7, 8'hE6, 8'hE6, 8'hE5, 8'hE5,
    8'hE5, 8'hE4, 8'hE4, 8'hE3, 8'hE3, 8'hE3, 8'hE2, 8'hE2, 8'hE2, 8'hE1, 8'hE1, 8'hE0, 8'hE0, 8'hE0, 8'hDF, 8'hDF,
    8'hDE, 8'hDE, 8'hDE, 8'hDD, 8'hDD, 8'hDC, 8'hDC, 8'hDB, 8'hDB, 8'hDB, 8'hDA, 8'hDA, 8'hD9, 8'hD9, 8'hD8, 8'hD8,
    8'hD8, 8'hD7, 8'hD7, 8'hD6, 8'hD6, 8'hD5, 8'hD5, 8'hD5, 8'hD4, 8'hD4, 8'hD3, 8'hD3, 8'hD2, 8'hD2, 8'hD1, 8'hD1,
    8'hD0, 8'hD0, 8'hCF, 8'hCF, 8'hCE, 8'hCE, 8'hCD, 8'hCD, 8'hCC, 8'hCC, 8'hCB, 8'hCB, 8'hCA, 8'hCA, 8'hC9, 8'hC9,
    8'hC8, 8'hC8, 8'hC7, 8'hC7, 8'hC6, 8'hC6, 8'hC5, 8'hC5, 8'hC4, 8'hC3, 8'hC3, 8'hC2, 8'hC2, 8'hC1, 8'hC1, 8'hC0,
    8'hC0, 8'hBF, 8'hBE, 8'hBE, 8'hBD, 8'hBD, 8'hBC, 8'hBB, 8'hBB, 8'hBA, 8'hB9, 8'hB9, 8'hB8, 8'hB8, 8'hB7, 8'hB6,
    8'hB6, 8'hB5, 8'hB4, 8'hB4, 8'hB3, 8'hB2, 8'hB1, 8'hB1, 8'hB0, 8'hAF, 8'hAF, 8'hAE, 8'hAD, 8'hAC, 8'hAC, 8'hAB,
    8'hAA, 8'hA9, 8'hA8, 8'hA8, 8'hA7, 8'hA6, 8'hA5, 8'hA4, 8'hA3, 8'hA3, 8'hA2, 8'hA1, 8'hA0, 8'h9F, 8'h9E, 8'h9D,
    8'h9C, 8'h9B, 8'h9A, 8'h99, 8'h98, 8'h97, 8'h96, 8'h95, 8'h94, 8'h93, 8'h92, 8'h91, 8'h8F, 8'h8E, 8'h8D, 8'h8C,
    8'h8A, 8'h89, 8'h88, 8'h86, 8'h85, 8'h84, 8'h82, 8'h81, 8'h7F, 8'h7D, 8'h7C, 8'h7A, 8'h78, 8'h76, 8'h74, 8'h72,
    8'h70, 8'h6E, 8'h6C, 8'h69, 8'h66, 8'h64, 8'h61, 8'h5D, 8'h5A, 8'h56, 8'h52, 8'h4C, 8'h46, 8'h3F, 8'h33, 8'h00
};

gamma_curve_lookup #(.CURVE_BYTES(CURVE_BYTES)) u_lookup (
    .clk(clk), .pixel_in(Pre_Data), .pixel_out(Post_Data)
);
endmodule
