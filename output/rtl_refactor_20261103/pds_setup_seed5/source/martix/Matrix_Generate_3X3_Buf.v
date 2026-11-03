// 2026-11-03 注释校订。为卷积建立三个像素行的窗口。行无效时沿用原定宽填充值。
`timescale 1ns / 1ns
module Matrix_Generate_3X3_Buf #(
    parameter DATA_WIDTH = 8,
    parameter IMG_HDISP  = 640,
    parameter IMG_VDISP  = 480
) (

    input clk,
    input rst_n,

    input                  per_frame_vsync,
    input                  per_frame_href,
    input [DATA_WIDTH-1:0] per_img_Data,

    output matrix_frame_vsync,
    output matrix_frame_href,

    output reg [DATA_WIDTH-1:0] matrix_p11,
    matrix_p12,
    matrix_p13,
    output reg [DATA_WIDTH-1:0] matrix_p21,
    matrix_p22,
    matrix_p23,
    output reg [DATA_WIDTH-1:0] matrix_p31,
    matrix_p32,
    matrix_p33
);

  wire [DATA_WIDTH-1:0] row1_data;
  wire [DATA_WIDTH-1:0] row2_data;
  reg  [DATA_WIDTH-1:0] row3_data;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) row3_data <= 0;
    else begin
      if (per_frame_href) row3_data <= per_img_Data;
      else row3_data <= row3_data;
    end
  end

  wire shift_clk_en = per_frame_href;
  Line_Shift_RAM #(
      .RAM_Length(IMG_HDISP),
      .DATA_WIDTH(DATA_WIDTH)
  ) u_Line_Shift_RAM (
      .clock  (clk),
      .clken  (shift_clk_en),
      .rst_n  (rst_n&~per_frame_vsync),
      .shiftin(row3_data),

      .taps0x(row2_data),
      .taps1x(row1_data)
  );

  reg [2:0] per_frame_vsync_r;
  reg [2:0] per_frame_href_r;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      per_frame_vsync_r <= 0;
      per_frame_href_r  <= 0;
    end else begin
      per_frame_vsync_r <= {per_frame_vsync_r[1:0], per_frame_vsync};
      per_frame_href_r  <= {per_frame_href_r[1:0], per_frame_href};
    end
  end

  wire read_frame_href = per_frame_href_r[0] | per_frame_href_r[1];
  assign matrix_frame_vsync = per_frame_vsync_r[2];
  assign matrix_frame_href  = per_frame_href_r[2];

  reg [          31:0] pixel_cnt;

  reg [DATA_WIDTH-1:0] row1_data0;
  reg [DATA_WIDTH-1:0] row2_data0;
  reg [DATA_WIDTH-1:0] row3_data0;
  reg [DATA_WIDTH-1:0] row1_data1;
  reg [DATA_WIDTH-1:0] row2_data1;
  reg [DATA_WIDTH-1:0] row3_data1;

  always @(posedge clk) begin
    if (!rst_n) begin

      pixel_cnt <= 0;
    end else if (per_frame_href) begin
      if (pixel_cnt == IMG_HDISP - 1) begin
        pixel_cnt <= 0;
      end else begin
        pixel_cnt <= pixel_cnt + 1'b1;
      end
    end else begin
      pixel_cnt <= 0;
    end

  end

  // 填充常量先按单行位宽扩展或截断，再复制到三行。
  localparam [3*DATA_WIDTH-1:0] BLANK_ROW = 24'hffffff;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      {row1_data1, row1_data0, row2_data1, row2_data0, row3_data1, row3_data0} <= 0;
      {matrix_p11, matrix_p12, matrix_p13,
       matrix_p21, matrix_p22, matrix_p23,
       matrix_p31, matrix_p32, matrix_p33} <= 0;
    end else if (read_frame_href) begin
      {row1_data1, row1_data0, row2_data1, row2_data0, row3_data1, row3_data0} <=
          {row1_data0, row1_data, row2_data0, row2_data, row3_data0, row3_data};
      {matrix_p11, matrix_p12, matrix_p13,
       matrix_p21, matrix_p22, matrix_p23,
       matrix_p31, matrix_p32, matrix_p33} <=
          {row1_data1, row1_data0, row1_data,
           row2_data1, row2_data0, row2_data,
           row3_data1, row3_data0, row3_data};
    end else begin
      {matrix_p11, matrix_p12, matrix_p13,
       matrix_p21, matrix_p22, matrix_p23,
       matrix_p31, matrix_p32, matrix_p33} <= {3{BLANK_ROW}};
    end
  end
endmodule
