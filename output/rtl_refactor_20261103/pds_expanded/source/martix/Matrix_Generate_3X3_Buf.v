`timescale 1ns / 1ns
module Matrix_Generate_3X3_Buf #(
    parameter DATA_WIDTH = 8,
    parameter IMG_HDISP  = 640,  // 参数记录为 640*480。
    parameter IMG_VDISP  = 480
) (
    // 像素窗口：global clock
    input clk,   // 像素窗口：cmos video pixel clock
    input rst_n, // 像素窗口：global reset

    // 像素窗口：Image data prepred to be processd
    input                  per_frame_vsync,  // 像素窗口：Prepared Image data vsync valid signal
    input                  per_frame_href,   // 像素窗口：Prepared Image data href vaild signal
    input [DATA_WIDTH-1:0] per_img_Data,     // 像素窗口：Prepared Image brightness input

    // 像素窗口：Image data has been processd
    output matrix_frame_vsync,  // 像素窗口：Prepared Image data vsync valid signal
    output matrix_frame_href,   // 像素窗口：Prepared Image data href vaild signal

    output reg [DATA_WIDTH-1:0] matrix_p11,
    matrix_p12,
    matrix_p13,  // 参数记录为 3X3 Matrix output。
    output reg [DATA_WIDTH-1:0] matrix_p21,
    matrix_p22,
    matrix_p23,
    output reg [DATA_WIDTH-1:0] matrix_p31,
    matrix_p32,
    matrix_p33
);


  // 像素窗口：Generate 3*3 matrix
  // ── 像素窗口 ──
  // ── 像素窗口 ──
  // ── 像素窗口 ──
  // 像素窗口：sync row3_data with per_frame_clken & row1_data & raw2_data
  wire [DATA_WIDTH-1:0] row1_data;  // 参数记录为 frame data of the 1th row。
  wire [DATA_WIDTH-1:0] row2_data;  // 参数记录为 frame data of the 2th row。
  reg  [DATA_WIDTH-1:0] row3_data;  // 参数记录为 frame data of the 3th row。
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) row3_data <= 0;
    else begin
      if (per_frame_href) row3_data <= per_img_Data;
      else row3_data <= row3_data;
    end
  end

  // ── 像素窗口 ──
  // 像素窗口：module of shift ram for raw data
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
  // 参数记录为 line_shift_ram_8bit_new u_line_shift_ram_8bit (。
  // 停用示例，仅供对照：.clock(clk),
  // 停用示例，仅供对照：.rst_n (rst_n & ~per_frame_vsync),
  // 停用示例，仅供对照：.clken (shift_clk_en),
  // 停用示例，仅供对照：.shiftin(row3_data),
  // 停用示例，仅供对照：.taps0x (row2_data),
  // 参数记录为 .taps1x (row1_data)。
  // 像素窗口：);


  // ── 像素窗口 ──
  // 参数记录为 lag 2 clocks signal sync。
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
  // 像素窗口：Give up the 1th and 2th row edge data caculate for simple process
  // 参数记录为 Give up the 1th and 2th point of 1 line for simple process。
  // 停用示例，仅供对照：wire read_frame_href = per_frame_href_r[0] | per_frame_href_r[1]; //RAM read href sync signal
  wire read_frame_href = per_frame_href_r[0] | per_frame_href_r[1];  // 像素窗口：RAM read href sync signal
  assign matrix_frame_vsync = per_frame_vsync_r[2];
  assign matrix_frame_href  = per_frame_href_r[2];

  // ── 像素窗口 ──
  // ── 像素窗口 ──
  /** 参数记录为 **************************************************
					* ************************** ---------- Convert Matrix
					* ---------- [ P31 -> P32 -> P33 -> ] ---> [ P11 P12 P13 ]
					* [ P21 -> P22 -> P23 -> ] ---> [ P21 P22 P23 ] [ P11 ->
					* P12 -> P11 -> ] ---> [ P31 P32 P33 ] ********************
					* ********************************************************。 */
  // ── 像素窗口 ──
  // ── 像素窗口 ──
  /** (1) Read data from
	* Shift_RAM (2)
	* Caculate the Sobel
	* (3) Steady data
	* after Sobel generate。 */
  // 停用示例，仅供对照：wire [23:0] matrix_row1 = {matrix_p11, matrix_p12, matrix_p13}; //Just for test
  // 停用示例，仅供对照：wire [23:0] matrix_row2 = {matrix_p21, matrix_p22, matrix_p23};
  // 停用示例，仅供对照：wire [23:0] matrix_row3 = {matrix_p31, matrix_p32, matrix_p33};
  reg [          31:0] pixel_cnt;
  // 停用示例，仅供对照：reg [ 31:0] pixel_cnt_v;
  reg [DATA_WIDTH-1:0] row1_data0;
  reg [DATA_WIDTH-1:0] row2_data0;
  reg [DATA_WIDTH-1:0] row3_data0;
  reg [DATA_WIDTH-1:0] row1_data1;
  reg [DATA_WIDTH-1:0] row2_data1;
  reg [DATA_WIDTH-1:0] row3_data1;

  // 停用示例，仅供对照：always @(posedge clk or negedge rst_n) begin
  // 停用示例，仅供对照：if (!rst_n) pixel_cnt_v <= 0;
  // 参数记录为 else if (per_frame_vsync) pixel_cnt_v <= 0;。
  // 参数记录为 else if (pixel_cnt == IMG_HDISP - 1) pixel_cnt_v <= pixel_cnt_v + 1'b1;。
  // 像素窗口：else pixel_cnt_v <= pixel_cnt_v;
  // end：本节对应的标识或作用域。
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


  // 停用示例，仅供对照：always @(posedge clk or negedge rst_n) begin
  // 停用示例，仅供对照：if (!rst_n) begin
  // 停用示例，仅供对照：row1_data0 <= 0;
  // 停用示例，仅供对照：row1_data1 <= 0;
  // 停用示例，仅供对照：row2_data0 <= 0;
  // 停用示例，仅供对照：row2_data1 <= 0;
  // 停用示例，仅供对照：row3_data0 <= 0;
  // 停用示例，仅供对照：row3_data1 <= 0;
  // 停用示例，仅供对照：{matrix_p11, matrix_p12, matrix_p13} <= 0;
  // 停用示例，仅供对照：{matrix_p21, matrix_p22, matrix_p23} <= 0;
  // 停用示例，仅供对照：{matrix_p31, matrix_p32, matrix_p33} <= 0;
  // 像素窗口：end else if (read_frame_href) begin
  // 停用示例，仅供对照：{row1_data1, row1_data0} <= {row1_data0, row1_data};
  // 停用示例，仅供对照：{row2_data1, row2_data0} <= {row2_data0, row2_data};
  // 停用示例，仅供对照：{row3_data1, row3_data0} <= {row3_data0, row3_data};

  // 停用示例，仅供对照：if (pixel_cnt == 0) begin
  // 像素窗口：// 处理行首边界
  // 停用示例，仅供对照：{matrix_p11, matrix_p12, matrix_p13} <= {row1_data0, row1_data0, row1_data};
  // 停用示例，仅供对照：{matrix_p21, matrix_p22, matrix_p23} <= {row2_data0, row2_data0, row2_data};
  // 停用示例，仅供对照：{matrix_p31, matrix_p32, matrix_p33} <= {row3_data0, row3_data0, row3_data};
  // 参数记录为 end else if (pixel_cnt == IMG_HDISP-1) begin。
  // 像素窗口：// 处理行尾边界
  // 停用示例，仅供对照：{matrix_p11, matrix_p12, matrix_p13} <= {row1_data1, row1_data0, row1_data0};
  // 停用示例，仅供对照：{matrix_p21, matrix_p22, matrix_p23} <= {row2_data1, row2_data0, row2_data0};
  // 停用示例，仅供对照：{matrix_p31, matrix_p32, matrix_p33} <= {row3_data1, row3_data0, row3_data0};
  // 像素窗口：end else begin
  // 像素窗口：// 正常处理中间像素
  // 停用示例，仅供对照：{matrix_p11, matrix_p12, matrix_p13} <= {row1_data1, row1_data0, row1_data};
  // 停用示例，仅供对照：{matrix_p21, matrix_p22, matrix_p23} <= {row2_data1, row2_data0, row2_data};
  // 停用示例，仅供对照：{matrix_p31, matrix_p32, matrix_p33} <= {row3_data1, row3_data0, row3_data};
  // end：本节对应的标识或作用域。
  // 像素窗口：end else begin
  // 停用示例，仅供对照：{matrix_p11, matrix_p12, matrix_p13} <= 24'hffffff;
  // 停用示例，仅供对照：{matrix_p21, matrix_p22, matrix_p23} <= 24'hffffff;
  // 停用示例，仅供对照：{matrix_p31, matrix_p32, matrix_p33} <= 24'hffffff;
  // end：本节对应的标识或作用域。
  // end：本节对应的标识或作用域。

  // 2026-11-03: advance all row history and output taps together.
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
