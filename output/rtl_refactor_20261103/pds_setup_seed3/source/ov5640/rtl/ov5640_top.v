// 2026-11-03 注释校订。连接各路相机的上电、SCCB 配置与图像采集。像素时钟测量逐路跨域返回。
module ov5640_top (
    input sys_clk,

    input [12:0] cmos_h_pixel,
    input [12:0] cmos_v_pixel,
    input [12:0] total_h_pixel,
    input [12:0] total_v_pixel,
    input [12:0] y_addr_st,
    input [12:0] y_addr_end,
    input [7:0] rate,
    output      clk_25M,
    output      clk_50M,

    inout cmos2_scl,
    inout cmos2_sda,
    input cmos2_vsync,
    input cmos2_href,
    input cmos2_pclk,
    input [7:0] cmos2_data,
    output cmos2_reset,

    inout cmos5_scl,
    inout cmos5_sda,
    input cmos5_vsync,
    input cmos5_href,
    input cmos5_pclk,
    input [7:0] cmos5_data,
    output cmos5_reset,

    inout cmos6_scl,
    inout cmos6_sda,
    input cmos6_vsync,
    input cmos6_href,
    input cmos6_pclk,
    input [7:0] cmos6_data,
    output cmos6_reset,

    output        video2_clk ,
    output [ 9:0] video2_x   ,
    output [ 9:0] video2_y   ,
    output        video2_vs  ,
    output        video2_href,
    output [15:0] cmos2_frame_data,
    output        cmos2_frame_valid,

    output        video5_clk ,
    output [ 9:0] video5_x   ,
    output [ 9:0] video5_y   ,
    output        video5_vs  ,
    output        video5_href,
    output [15:0] cmos5_frame_data,
    output        cmos5_frame_valid,

    output        video6_clk ,
    output [ 9:0] video6_x   ,
    output [ 9:0] video6_y   ,
    output        video6_vs  ,
    output        video6_href,
    output [15:0] cmos6_frame_data,
    output        cmos6_frame_valid,

    output [31:0] cmos2_pclk_count,
    output [31:0] cmos5_pclk_count,
    output [31:0] cmos6_pclk_count
);

  wire        cmos2_init_done;
  wire        cmos5_init_done;
  wire        cmos6_init_done;
  reg  [15:0] rstn_1ms;
  wire        initial_en12;
  wire        initial_en56;
  wire        cfg_clk;

  wire        locked;
  wire        rstn_out;

  reg  [ 7:0] cmos2_d_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos2_href_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos2_vsync_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  wire        cmos2_frame_vsync;
  wire        cmos2_frame_href;
  wire [ 9:0] cmos2_frame_x;
  wire [ 9:0] cmos2_frame_y;

  reg  [ 7:0] cmos5_d_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos5_href_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos5_vsync_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  wire        cmos5_frame_vsync;
  wire        cmos5_frame_href;
  wire [ 9:0] cmos5_frame_x;
  wire [ 9:0] cmos5_frame_y;

  reg  [ 7:0] cmos6_d_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos6_href_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos6_vsync_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  wire        cmos6_frame_vsync;
  wire        cmos6_frame_href;
  wire [ 9:0] cmos6_frame_x;
  wire [ 9:0] cmos6_frame_y;

  clk_1080p_gen u_pll (
      .clkin1 (sys_clk),
      .clkout0(cfg_clk),
      .clkout1(clk_25M),
      .clkout2(clk_50M),
      .lock   (locked)
  );

  always @(posedge cfg_clk) begin
    if (!locked) rstn_1ms <= 16'd0;
    else begin
      if (rstn_1ms == 16'h2710) rstn_1ms <= rstn_1ms;
      else rstn_1ms <= rstn_1ms + 1'b1;
    end
  end
  assign rstn_out   = (rstn_1ms == 16'h2710);
  assign initial_en = rstn_out;

  power_on_delay cmos1cmos2_power_on_delay (
      .clk_50M     (clk_50M),
      .reset_n     (1'b1),
      .camera1_rstn(cmos1_reset),
      .camera2_rstn(cmos2_reset),
      .camera_pwnd (),
      .initial_en  (initial_en12)
  );

  power_on_delay cmos5cmos6_power_on_delay (
      .clk_50M     (clk_50M),
      .reset_n     (1'b1),
      .camera1_rstn(cmos5_reset),
      .camera2_rstn(cmos6_reset),
      .camera_pwnd (),
      .initial_en  (initial_en56)
  );

  reg_config cmos2_reg_config (
      .clk_25M      (clk_25M),
      .camera_rstn  (cmos2_reset),
      .initial_en   (initial_en12),
      .cmos_h_pixel (cmos_h_pixel),
      .cmos_v_pixel (cmos_v_pixel),
      .total_h_pixel(total_h_pixel),
      .total_v_pixel(total_v_pixel),
      .rate         (rate),
      .y_addr_st    (y_addr_st),
      .y_addr_end   (y_addr_end),
      .i2c_sclk     (cmos2_scl),
      .i2c_sdat     (cmos2_sda),
      .reg_conf_done(cmos2_init_done),
      .reg_index    (),
      .clock_20k    ()
  );

  always @(posedge cmos2_pclk) begin
    cmos2_d_d0     <= cmos2_data;
    cmos2_href_d0  <= cmos2_href;
    cmos2_vsync_d0 <= cmos2_vsync;
  end

  cmos_capture_data u_cmos2_capture_data (
      .rst_n           (cmos2_init_done),
      .cam_pclk        (cmos2_pclk),
      .cam_vsync       (cmos2_vsync_d0),
      .cam_href        (cmos2_href_d0),
      .cam_data        (cmos2_d_d0),
      .active_x        (cmos2_frame_x),
      .active_y        (cmos2_frame_y),
      .cmos_frame_vsync(cmos2_frame_vsync),
      .cmos_frame_href (cmos2_frame_href),
      .cmos_frame_valid(cmos2_frame_valid),
      .cmos_frame_data (cmos2_frame_data)
  );

  reg_config cmos5_reg_config (
      .clk_25M      (clk_25M),
      .camera_rstn  (cmos5_reset),
      .initial_en   (initial_en56),
      .cmos_h_pixel (cmos_h_pixel),
      .cmos_v_pixel (cmos_v_pixel),
      .total_h_pixel(total_h_pixel),
      .total_v_pixel(total_v_pixel),
      .rate         (rate),
      .y_addr_st    (y_addr_st),
      .y_addr_end   (y_addr_end),
      .i2c_sclk     (cmos5_scl),
      .i2c_sdat     (cmos5_sda),
      .reg_conf_done(cmos5_init_done),
      .reg_index    (),
      .clock_20k    ()
  );

  always @(posedge cmos5_pclk) begin
    cmos5_d_d0     <= cmos5_data;
    cmos5_href_d0  <= cmos5_href;
    cmos5_vsync_d0 <= cmos5_vsync;
  end

  cmos_capture_data u_cmos5_capture_data (
      .rst_n           (cmos5_init_done),
      .cam_pclk        (cmos5_pclk),
      .cam_vsync       (cmos5_vsync_d0),
      .cam_href        (cmos5_href_d0),
      .cam_data        (cmos5_d_d0),
      .active_x        (cmos5_frame_x),
      .active_y        (cmos5_frame_y),
      .cmos_frame_vsync(cmos5_frame_vsync),
      .cmos_frame_href (cmos5_frame_href),
      .cmos_frame_valid(cmos5_frame_valid),
      .cmos_frame_data (cmos5_frame_data)
  );

  reg_config cmos6_reg_config (
      .clk_25M      (clk_25M),
      .camera_rstn  (cmos6_reset),
      .initial_en   (initial_en56),
      .cmos_h_pixel (cmos_h_pixel),
      .cmos_v_pixel (cmos_v_pixel),
      .total_h_pixel(total_h_pixel),
      .total_v_pixel(total_v_pixel),
      .rate         (rate),
      .y_addr_st    (y_addr_st),
      .y_addr_end   (y_addr_end),
      .i2c_sclk     (cmos6_scl),
      .i2c_sdat     (cmos6_sda),
      .reg_conf_done(cmos6_init_done),
      .reg_index    (),
      .clock_20k    ()
  );

  always @(posedge cmos6_pclk) begin
    cmos6_d_d0     <= cmos6_data;
    cmos6_href_d0  <= cmos6_href;
    cmos6_vsync_d0 <= cmos6_vsync;
  end

  cmos_capture_data u_cmos6_capture_data (
      .rst_n           (cmos6_init_done),
      .cam_pclk        (cmos6_pclk),
      .cam_vsync       (cmos6_vsync_d0),
      .cam_href        (cmos6_href_d0),
      .cam_data        (cmos6_d_d0),
      .active_x        (cmos6_frame_x),
      .active_y        (cmos6_frame_y),
      .cmos_frame_vsync(cmos6_frame_vsync),
      .cmos_frame_href (cmos6_frame_href),
      .cmos_frame_valid(cmos6_frame_valid),
      .cmos_frame_data (cmos6_frame_data)
  );

  assign video2_clk  = cmos2_pclk       ;
  assign video2_x    = cmos2_frame_x    ;
  assign video2_y    = cmos2_frame_y    ;
  assign video2_vs   = cmos2_frame_vsync;
  assign video2_href = cmos2_frame_href ;

  assign video5_clk  = cmos5_pclk       ;
  assign video5_x    = cmos5_frame_x    ;
  assign video5_y    = cmos5_frame_y    ;
  assign video5_vs   = cmos5_frame_vsync;
  assign video5_href = cmos5_frame_href ;

  assign video6_clk  = cmos6_pclk       ;
  assign video6_x    = cmos6_frame_x    ;
  assign video6_y    = cmos6_frame_y    ;
  assign video6_vs   = cmos6_frame_vsync;
  assign video6_href = cmos6_frame_href ;

  reg [25:0] second_counter;
  reg        one_second_pulse;
  reg        measure_enable;

  always @(posedge clk_50M) begin
    if (!locked) begin
      second_counter <= 26'd0;
      one_second_pulse <= 1'b0;
      measure_enable <= 1'b0;
    end else begin
      if (second_counter >= 26'd49999999) begin
        second_counter <= 26'd0;
        one_second_pulse <= 1'b1;
        measure_enable <= 1'b0;
      end else if (second_counter == 26'd0) begin
        second_counter <= second_counter + 1'b1;
        one_second_pulse <= 1'b0;
        measure_enable <= 1'b1;
      end else begin
        second_counter <= second_counter + 1'b1;
        one_second_pulse <= 1'b0;
        measure_enable <= measure_enable;
      end
    end
  end

  ov5640_clock_meter u_meter_2 (
      .clk_50M(clk_50M), .locked(locked), .measure_enable(measure_enable),
      .one_second_pulse(one_second_pulse), .lane_pclk(cmos2_pclk),
      .lane_init_done(cmos2_init_done), .lane_pclk_count_reg(cmos2_pclk_count)
  );
  ov5640_clock_meter u_meter_5 (
      .clk_50M(clk_50M), .locked(locked), .measure_enable(measure_enable),
      .one_second_pulse(one_second_pulse), .lane_pclk(cmos5_pclk),
      .lane_init_done(cmos5_init_done), .lane_pclk_count_reg(cmos5_pclk_count)
  );
  ov5640_clock_meter u_meter_6 (
      .clk_50M(clk_50M), .locked(locked), .measure_enable(measure_enable),
      .one_second_pulse(one_second_pulse), .lane_pclk(cmos6_pclk),
      .lane_init_done(cmos6_init_done), .lane_pclk_count_reg(cmos6_pclk_count)
  );
endmodule

// 在像素域锁存计数，再用参考时钟域的秒脉冲逐级取回结果。
module ov5640_clock_meter (
    input clk_50M, locked, measure_enable, one_second_pulse,
    input lane_pclk, lane_init_done,
    output reg [31:0] lane_pclk_count_reg
);
  reg [2:0] lane_enable_sync;
  reg [2:0] lane_pulse_sync;

  always @(posedge lane_pclk) begin
    if (!lane_init_done) begin
      lane_enable_sync <= 3'b000;
      lane_pulse_sync <= 3'b000;
    end else begin
      lane_enable_sync <= {lane_enable_sync[1:0], measure_enable};
      lane_pulse_sync <= {lane_pulse_sync[1:0], one_second_pulse};
    end
  end

  wire lane_measure_en = lane_enable_sync[2];
  wire lane_pulse_edge = lane_pulse_sync[1] & ~lane_pulse_sync[2];

  reg [31:0] lane_temp_count;
  reg [31:0] lane_count_capture;

  always @(posedge lane_pclk) begin
    if (!lane_init_done) begin
      lane_temp_count <= 32'd0;
      lane_count_capture <= 32'd0;
    end else begin
      if (lane_pulse_edge) begin

        lane_count_capture <= lane_temp_count;
        lane_temp_count <= 32'd1;
      end else if (lane_measure_en) begin
        lane_temp_count <= lane_temp_count + 1'b1;
      end
    end
  end

  reg [31:0] lane_count_sync1, lane_count_sync2;

  always @(posedge clk_50M) begin
    if (!locked) begin
      lane_count_sync1 <= 32'd0;
      lane_count_sync2 <= 32'd0;
      lane_pclk_count_reg <= 32'd0;
    end else if (one_second_pulse) begin
      lane_count_sync1 <= lane_count_capture;
      lane_count_sync2 <= lane_count_sync1;
      lane_pclk_count_reg <= lane_count_sync2;
    end
  end

endmodule
