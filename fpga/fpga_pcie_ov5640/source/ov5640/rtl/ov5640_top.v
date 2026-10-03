module ov5640_top (
    input sys_clk,  //50Mhz
//    input    [2:0]                       select_cmos          ,//摄像头选择信号: 1=cmos1, 2=cmos2, 3=cmos3, 4=cmos4, 5=cmos5, 6=cmos6
    input [12:0] cmos_h_pixel,  //水平方向分辨率
    input [12:0] cmos_v_pixel,  //垂直方向分辨率
    input [12:0] total_h_pixel,  //水平总像素大小
    input [12:0] total_v_pixel,  //垂直总像素大小
    input [12:0] y_addr_st,
    input [12:0] y_addr_end,
    input [7:0] rate,
    output      clk_25M,
    output      clk_50M,
    //coms2
    inout cmos2_scl,  //cmos2 i2c 
    inout cmos2_sda,  //cmos2 i2c 
    input cmos2_vsync,  //cmos2 vsync
    input cmos2_href,  //cmos2 hsync refrence,data valid
    input cmos2_pclk,  //cmos2 pxiel clock
    input [7:0] cmos2_data,  //cmos2 data
    output cmos2_reset,  //cmos2 reset
    //coms5
    inout cmos5_scl,  //cmos5 i2c 
    inout cmos5_sda,  //cmos5 i2c 
    input cmos5_vsync,  //cmos5 vsync
    input cmos5_href,  //cmos5 hsync refrence,data valid
    input cmos5_pclk,  //cmos5 pxiel clock
    input [7:0] cmos5_data,  //cmos5 data
    output cmos5_reset,  //cmos5 reset
    //coms6
    inout cmos6_scl,  //cmos6 i2c 
    inout cmos6_sda,  //cmos6 i2c 
    input cmos6_vsync,  //cmos6 vsync
    input cmos6_href,  //cmos6 hsync refrence,data valid
    input cmos6_pclk,  //cmos6 pxiel clock
    input [7:0] cmos6_data,  //cmos6 data
    output cmos6_reset,  //cmos6 reset
    //output video stream
    output        video2_clk ,//video pixel clock
    output [ 9:0] video2_x   ,
    output [ 9:0] video2_y   ,
    output        video2_vs  ,//video vsync  
    output        video2_href,//video href
    output [15:0] cmos2_frame_data,//video data RGB56
    output        cmos2_frame_valid,

    output        video5_clk ,//video pixel clock
    output [ 9:0] video5_x   ,
    output [ 9:0] video5_y   ,
    output        video5_vs  ,//video vsync  
    output        video5_href,//video href
    output [15:0] cmos5_frame_data,//video data RGB565
    output        cmos5_frame_valid,

    output        video6_clk ,//video pixel clock
    output [ 9:0] video6_x   ,
    output [ 9:0] video6_y   ,
    output        video6_vs  ,//video vsync  
    output        video6_href,//video href
    output [15:0] cmos6_frame_data,//video data RGB565
    output        cmos6_frame_valid,
    
    // 频率计数器输出 - 用于测量pclk频率
    output [31:0] cmos2_pclk_count,  // CMOS2 像素时钟计数值
    output [31:0] cmos5_pclk_count,  // CMOS5 像素时钟计数值  
    output [31:0] cmos6_pclk_count   // CMOS6 像素时钟计数值
);

  wire        cmos2_init_done;
  wire        cmos5_init_done;
  wire        cmos6_init_done;
  reg  [15:0] rstn_1ms;
  wire        initial_en12;  
  wire        initial_en56;
  wire        cfg_clk;
//  wire        clk_25M;
//  wire        clk_50M;
  wire        locked;
  wire        rstn_out;

  // cmos2 signals
  reg  [ 7:0] cmos2_d_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos2_href_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos2_vsync_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  wire        cmos2_frame_vsync;
  wire        cmos2_frame_href;
  wire [ 9:0] cmos2_frame_x;
  wire [ 9:0] cmos2_frame_y;

  // cmos5 signals
  reg  [ 7:0] cmos5_d_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos5_href_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos5_vsync_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  wire        cmos5_frame_vsync;
  wire        cmos5_frame_href;
  wire [ 9:0] cmos5_frame_x;
  wire [ 9:0] cmos5_frame_y;

  // cmos6 signals
  reg  [ 7:0] cmos6_d_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos6_href_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  reg         cmos6_vsync_d0  /*synthesis PAP_MARK_DEBUG="1"*/;
  wire        cmos6_frame_vsync;
  wire        cmos6_frame_href;
  wire [ 9:0] cmos6_frame_x;
  wire [ 9:0] cmos6_frame_y;

  clk_1080p_gen u_pll (
      .clkin1 (sys_clk),  //50MHz
      .clkout0(cfg_clk),  //10MHz
      .clkout1(clk_25M),  //25M
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
      .clk_50M     (clk_50M),      //input
      .reset_n     (1'b1),         //input	
      .camera1_rstn(cmos1_reset),  //output
      .camera2_rstn(cmos2_reset),  //output	
      .camera_pwnd (),             //output
      .initial_en  (initial_en12)  //output		
  );

  power_on_delay cmos5cmos6_power_on_delay (
      .clk_50M     (clk_50M),      //input
      .reset_n     (1'b1),         //input	
      .camera1_rstn(cmos5_reset),  //output
      .camera2_rstn(cmos6_reset),  //output	
      .camera_pwnd (),             //output
      .initial_en  (initial_en56)  //output		
  );

  //==============================================================================
  //  CMOS2 Configuration and Data Capture
  //==============================================================================

  // cmos2 register configuration - 只在选中时配置
  reg_config cmos2_reg_config (
      .clk_25M      (clk_25M),          //input
      .camera_rstn  (cmos2_reset),      //input
      .initial_en   (initial_en12),     //input - 避免I2C冲突		
      .cmos_h_pixel (cmos_h_pixel),     //input [12:0]
      .cmos_v_pixel (cmos_v_pixel),     //input [12:0]
      .total_h_pixel(total_h_pixel),    //input [12:0]
      .total_v_pixel(total_v_pixel),    //input [12:0]
      .rate         (rate),             //input [7:0]
      .y_addr_st    (y_addr_st),        //input [12:0]
      .y_addr_end   (y_addr_end),       //input [12:0]
      .i2c_sclk     (cmos2_scl),        //output
      .i2c_sdat     (cmos2_sda),        //inout
      .reg_conf_done(cmos2_init_done),  //output config_finished
      .reg_index    (),                 //output reg [8:0]
      .clock_20k    ()                  //output reg
  );

  // cmos2 data pipeline
  always @(posedge cmos2_pclk) begin
    cmos2_d_d0     <= cmos2_data;
    cmos2_href_d0  <= cmos2_href;
    cmos2_vsync_d0 <= cmos2_vsync;
  end

  // cmos2 capture data
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

  //==============================================================================
  //  CMOS5 Configuration and Data Capture
  //==============================================================================

  // cmos5 register configuration - 只在选中时配置
  reg_config cmos5_reg_config (
      .clk_25M      (clk_25M),          //input
      .camera_rstn  (cmos5_reset),      //input
      .initial_en   (initial_en56),       //input - 避免I2C冲突		
      .cmos_h_pixel (cmos_h_pixel),     //input [12:0]
      .cmos_v_pixel (cmos_v_pixel),     //input [12:0]
      .total_h_pixel(total_h_pixel),    //input [12:0]
      .total_v_pixel(total_v_pixel),    //input [12:0]
      .rate         (rate),             //input [7:0]
      .y_addr_st    (y_addr_st),        //input [12:0]
      .y_addr_end   (y_addr_end),       //input [12:0]
      .i2c_sclk     (cmos5_scl),        //output
      .i2c_sdat     (cmos5_sda),        //inout
      .reg_conf_done(cmos5_init_done),  //output config_finished
      .reg_index    (),                 //output reg [8:0]
      .clock_20k    ()                  //output reg
  );

  // cmos5 data pipeline
  always @(posedge cmos5_pclk) begin
    cmos5_d_d0     <= cmos5_data;
    cmos5_href_d0  <= cmos5_href;
    cmos5_vsync_d0 <= cmos5_vsync;
  end

  // cmos5 capture data
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

  //==============================================================================
  //  CMOS6 Configuration and Data Capture
  //==============================================================================

  // cmos6 register configuration - 只在选中时配置
  reg_config cmos6_reg_config (
      .clk_25M      (clk_25M),          //input
      .camera_rstn  (cmos6_reset),      //input
      .initial_en   (initial_en56),       //input - 避免I2C冲突
      .cmos_h_pixel (cmos_h_pixel),     //input [12:0]
      .cmos_v_pixel (cmos_v_pixel),     //input [12:0]
      .total_h_pixel(total_h_pixel),    //input [12:0]
      .total_v_pixel(total_v_pixel),    //input [12:0]
      .rate         (rate),             //input [7:0]
      .y_addr_st    (y_addr_st),        //input [12:0]
      .y_addr_end   (y_addr_end),       //input [12:0]
      .i2c_sclk     (cmos6_scl),        //output
      .i2c_sdat     (cmos6_sda),        //inout
      .reg_conf_done(cmos6_init_done),  //output config_finished
      .reg_index    (),                 //output reg [8:0]
      .clock_20k    ()                  //output reg
  );

  // cmos6 data pipeline
  always @(posedge cmos6_pclk) begin
    cmos6_d_d0     <= cmos6_data;
    cmos6_href_d0  <= cmos6_href;
    cmos6_vsync_d0 <= cmos6_vsync;
  end

  // cmos6 capture data
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

  //==============================================================================
  //  Video Output Selection
  //==============================================================================

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

  //==============================================================================
  //  频率计数器 - 优化版本，解决跨时钟域问题
  //  使用1秒计时窗口精确测量pclk频率
  //==============================================================================
  
  // 1秒计时器 (基于50MHz系统时钟)
  reg [25:0] second_counter;    // 50,000,000计数 = 1秒
  reg        one_second_pulse;  // 1秒脉冲信号
  reg        measure_enable;    // 测量使能信号
  
  always @(posedge clk_50M) begin
    if (!locked) begin
      second_counter <= 26'd0;
      one_second_pulse <= 1'b0;
      measure_enable <= 1'b0;
    end else begin
      if (second_counter >= 26'd49999999) begin  // 50MHz - 1
        second_counter <= 26'd0;
        one_second_pulse <= 1'b1;
        measure_enable <= 1'b0;  // 停止测量
      end else if (second_counter == 26'd0) begin
        second_counter <= second_counter + 1'b1;
        one_second_pulse <= 1'b0;
        measure_enable <= 1'b1;  // 开始测量
      end else begin
        second_counter <= second_counter + 1'b1;
        one_second_pulse <= 1'b0;
        measure_enable <= measure_enable;
      end
    end
  end
  
  //==============================================================================
  //  CMOS2 像素时钟频率计数器
  //==============================================================================
  
  // 跨时钟域同步器 - 将测量控制信号同步到cmos2_pclk域
  reg [2:0] cmos2_enable_sync;
  reg [2:0] cmos2_pulse_sync;
  
  always @(posedge cmos2_pclk) begin
    if (!cmos2_init_done) begin
      cmos2_enable_sync <= 3'b000;
      cmos2_pulse_sync <= 3'b000;
    end else begin
      cmos2_enable_sync <= {cmos2_enable_sync[1:0], measure_enable};
      cmos2_pulse_sync <= {cmos2_pulse_sync[1:0], one_second_pulse};
    end
  end
  
  wire cmos2_measure_en = cmos2_enable_sync[2];
  wire cmos2_pulse_edge = cmos2_pulse_sync[1] & ~cmos2_pulse_sync[2];
  
  // CMOS2 计数器
  reg [31:0] cmos2_temp_count;
  reg [31:0] cmos2_count_capture;
  
  always @(posedge cmos2_pclk) begin
    if (!cmos2_init_done) begin
      cmos2_temp_count <= 32'd0;
      cmos2_count_capture <= 32'd0;
    end else begin
      if (cmos2_pulse_edge) begin
        // 捕获计数值并重新开始
        cmos2_count_capture <= cmos2_temp_count;
        cmos2_temp_count <= 32'd1;
      end else if (cmos2_measure_en) begin
        cmos2_temp_count <= cmos2_temp_count + 1'b1;
      end
    end
  end
  
  // 将计数值同步回clk_50M域
  reg [31:0] cmos2_count_sync1, cmos2_count_sync2;
  reg [31:0] cmos2_pclk_count_reg;
  
  always @(posedge clk_50M) begin
    if (!locked) begin
      cmos2_count_sync1 <= 32'd0;
      cmos2_count_sync2 <= 32'd0;
      cmos2_pclk_count_reg <= 32'd0;
    end else if (one_second_pulse) begin
      cmos2_count_sync1 <= cmos2_count_capture;
      cmos2_count_sync2 <= cmos2_count_sync1;
      cmos2_pclk_count_reg <= cmos2_count_sync2;
    end
  end
  
  //==============================================================================
  //  CMOS5 像素时钟频率计数器
  //==============================================================================
  
  // 跨时钟域同步器 - 将测量控制信号同步到cmos5_pclk域
  reg [2:0] cmos5_enable_sync;
  reg [2:0] cmos5_pulse_sync;
  
  always @(posedge cmos5_pclk) begin
    if (!cmos5_init_done) begin
      cmos5_enable_sync <= 3'b000;
      cmos5_pulse_sync <= 3'b000;
    end else begin
      cmos5_enable_sync <= {cmos5_enable_sync[1:0], measure_enable};
      cmos5_pulse_sync <= {cmos5_pulse_sync[1:0], one_second_pulse};
    end
  end
  
  wire cmos5_measure_en = cmos5_enable_sync[2];
  wire cmos5_pulse_edge = cmos5_pulse_sync[1] & ~cmos5_pulse_sync[2];
  
  // CMOS5 计数器
  reg [31:0] cmos5_temp_count;
  reg [31:0] cmos5_count_capture;
  
  always @(posedge cmos5_pclk) begin
    if (!cmos5_init_done) begin
      cmos5_temp_count <= 32'd0;
      cmos5_count_capture <= 32'd0;
    end else begin
      if (cmos5_pulse_edge) begin
        // 捕获计数值并重新开始
        cmos5_count_capture <= cmos5_temp_count;
        cmos5_temp_count <= 32'd1;
      end else if (cmos5_measure_en) begin
        cmos5_temp_count <= cmos5_temp_count + 1'b1;
      end
    end
  end
  
  // 将计数值同步回clk_50M域
  reg [31:0] cmos5_count_sync1, cmos5_count_sync2;
  reg [31:0] cmos5_pclk_count_reg;
  
  always @(posedge clk_50M) begin
    if (!locked) begin
      cmos5_count_sync1 <= 32'd0;
      cmos5_count_sync2 <= 32'd0;
      cmos5_pclk_count_reg <= 32'd0;
    end else if (one_second_pulse) begin
      cmos5_count_sync1 <= cmos5_count_capture;
      cmos5_count_sync2 <= cmos5_count_sync1;
      cmos5_pclk_count_reg <= cmos5_count_sync2;
    end
  end
  
  //==============================================================================
  //  CMOS6 像素时钟频率计数器
  //==============================================================================
  
  // 跨时钟域同步器 - 将测量控制信号同步到cmos6_pclk域
  reg [2:0] cmos6_enable_sync;
  reg [2:0] cmos6_pulse_sync;
  
  always @(posedge cmos6_pclk) begin
    if (!cmos6_init_done) begin
      cmos6_enable_sync <= 3'b000;
      cmos6_pulse_sync <= 3'b000;
    end else begin
      cmos6_enable_sync <= {cmos6_enable_sync[1:0], measure_enable};
      cmos6_pulse_sync <= {cmos6_pulse_sync[1:0], one_second_pulse};
    end
  end
  
  wire cmos6_measure_en = cmos6_enable_sync[2];
  wire cmos6_pulse_edge = cmos6_pulse_sync[1] & ~cmos6_pulse_sync[2];
  
  // CMOS6 计数器
  reg [31:0] cmos6_temp_count;
  reg [31:0] cmos6_count_capture;
  
  always @(posedge cmos6_pclk) begin
    if (!cmos6_init_done) begin
      cmos6_temp_count <= 32'd0;
      cmos6_count_capture <= 32'd0;
    end else begin
      if (cmos6_pulse_edge) begin
        // 捕获计数值并重新开始
        cmos6_count_capture <= cmos6_temp_count;
        cmos6_temp_count <= 32'd1;
      end else if (cmos6_measure_en) begin
        cmos6_temp_count <= cmos6_temp_count + 1'b1;
      end
    end
  end
  
  // 将计数值同步回clk_50M域
  reg [31:0] cmos6_count_sync1, cmos6_count_sync2;
  reg [31:0] cmos6_pclk_count_reg;
  
  always @(posedge clk_50M) begin
    if (!locked) begin
      cmos6_count_sync1 <= 32'd0;
      cmos6_count_sync2 <= 32'd0;
      cmos6_pclk_count_reg <= 32'd0;
    end else if (one_second_pulse) begin
      cmos6_count_sync1 <= cmos6_count_capture;
      cmos6_count_sync2 <= cmos6_count_sync1;
      cmos6_pclk_count_reg <= cmos6_count_sync2;
    end
  end
  
  // 输出计数值 - 即为实际的pclk频率(Hz)
  assign cmos2_pclk_count = cmos2_pclk_count_reg;
  assign cmos5_pclk_count = cmos5_pclk_count_reg;
  assign cmos6_pclk_count = cmos6_pclk_count_reg;

endmodule
