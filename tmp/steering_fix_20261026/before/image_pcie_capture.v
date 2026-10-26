
`timescale 1ns / 1ps

module image_pcie_capture #(

    parameter MEM_ROW_WIDTH = 15,

    parameter MEM_COLUMN_WIDTH = 10,

    parameter MEM_BANK_WIDTH = 3,

    parameter MEM_DQ_WIDTH = 16,

    parameter MEM_DQS_WIDTH = 2

) (
    input free_clk,
    input board_rst_n,

    //OV5640 CMOS interface
    //coms2
    inout        cmos2_scl,    //cmos2 i2c 
    inout        cmos2_sda,    //cmos2 i2c 
    input        cmos2_vsync,  //cmos2 vsync
    input        cmos2_href,   //cmos2 hsync refrence,data valid
    input        cmos2_pclk,   //cmos2 pxiel clock
    input  [7:0] cmos2_data,   //cmos2 data
    output       cmos2_reset,  //cmos2 reset
    //coms5
    inout        cmos5_scl,    //cmos5 i2c 
    inout        cmos5_sda,    //cmos5 i2c 
    input        cmos5_vsync,  //cmos5 vsync
    input        cmos5_href,   //cmos5 hsync refrence,data valid
    input        cmos5_pclk,   //cmos5 pxiel clock
    input  [7:0] cmos5_data,   //cmos5 data
    output       cmos5_reset,  //cmos5 reset
    //coms6
    inout        cmos6_scl,    //cmos6 i2c 
    inout        cmos6_sda,    //cmos6 i2c 
    input        cmos6_vsync,  //cmos6 vsync
    input        cmos6_href,   //cmos6 hsync refrence,data valid
    input        cmos6_pclk,   //cmos6 pxiel clock
    input  [7:0] cmos6_data,   //cmos6 data
    output       cmos6_reset,  //cmos6 reset

    //DDR3 interface
    output                      mem_cs_n,
    output                      mem_rst_n,
    output                      mem_ck,
    output                      mem_ck_n,
    output                      mem_cke,
    output                      mem_ras_n,
    output                      mem_cas_n,
    output                      mem_we_n,
    output                      mem_odt,
    output [ MEM_ROW_WIDTH-1:0] mem_a,
    output [MEM_BANK_WIDTH-1:0] mem_ba,
    inout  [MEM_DQ_WIDTH/8-1:0] mem_dqs,
    inout  [MEM_DQ_WIDTH/8-1:0] mem_dqs_n,
    inout  [  MEM_DQ_WIDTH-1:0] mem_dq,
    output [MEM_DQ_WIDTH/8-1:0] mem_dm,

    // PCIe interface
    input             ref_clk_p,  // 输入参考时钟 
    input             ref_clk_n,  // 
    input             perst_n,    //pcie复位
    input       [1:0] rxn,        //pcie接收
    input       [1:0] rxp,        //
    output wire [1:0] txn,        //pcie发送
    output wire [1:0] txp,        // 

    //LED signals
    output reg        heart_beat_led,
    output reg        pclk_led,
    output reg        ref_led,
    //fspi
    input  wire       spi_clk,
    input  wire       spi_cs,
    inout  wire [3:0] spi_data,
    output wire       go,
    output wire       back,
    inout  wire       left,
    inout  wire       right,
    output wire       stop,
    //test
    //output wire test,

    //UART interface
    input RXD  //UART receive data

    //Debug signals for waveform capture
    //output  wire    [7:0]   debug_mode,     //Mode signal for debug
    //output  wire    [5:0]   debug_cnn_data  //CNN data signal for debug
    // 频率计数器输出 - 用于测量pclk频率
    // output wire [31:0] cmos2_pclk_count,  // CMOS2 像素时钟计数值
    // output wire [31:0] cmos5_pclk_count,  // CMOS5 像素时钟计数值  
    // output wire [31:0] cmos6_pclk_count   // CMOS6 像素时钟计数值
);
  //assign test = 0;

  wire [31:0] cmos2_pclk_count;  // CMOS2 像素时钟计数值
  wire [31:0] cmos5_pclk_count;  // CMOS5 像素时钟计数值  
  wire [31:0] cmos6_pclk_count;  // CMOS6 像素时钟计数值
  parameter CTRL_ADDR_WIDTH = MEM_ROW_WIDTH + MEM_BANK_WIDTH + MEM_COLUMN_WIDTH;
  parameter TH_1S = 27'd33000000;
  parameter REM_DQS_WIDTH = 9 - MEM_DQS_WIDTH;

  // OV5640 配置参数
  //   parameter V_CMOS_DISP = 13'd720;  //CMOS垂直分辨率
  //   parameter V_CMOS_DISP = 13'd768;  //CMOS垂直分辨率
  parameter V_CMOS_DISP = 13'd480;  //CMOS垂直分辨率
  //   parameter H_CMOS_DISP = 13'd1280;  //CMOS水平分辨率
  //   parameter H_CMOS_DISP = 13'd1024;  //CMOS水平分辨率
  parameter H_CMOS_DISP = 13'd640;  //CMOS水平分辨率
  // localparam  TOTAL_H_PIXEL = 13'ha3f;  //水平总像素
  //   parameter TOTAL_H_PIXEL = H_CMOS_DISP + 13'd1216;//CMOS水平总像素
  parameter TOTAL_H_PIXEL = 13'h768;  //CMOS水平总像素
  //   localparam TOTAL_V_PIXEL = 13'h790;  //垂直总像素
  //   parameter TOTAL_V_PIXEL = V_CMOS_DISP + 13'd504;
  parameter TOTAL_V_PIXEL = 13'h3d8;

  parameter RATE = 8'h41;  //41:15fps, 21:30Fps, 11:60Fps
  parameter Y_ADDR_END = 13'd1947;
  parameter Y_ADDR_ST = 13'd4;

  wire                        ddrphy_cpd_lock;
  wire                        ddr_init_done;
  wire                        pll_lock;
  wire                        core_clk;
  wire [ CTRL_ADDR_WIDTH-1:0] axi_awaddr;
  wire                        axi_awuser_ap;
  wire [                 3:0] axi_awuser_id;
  wire [                 3:0] axi_awlen;
  wire                        axi_awready;
  wire                        axi_awvalid;
  wire [  MEM_DQ_WIDTH*8-1:0] axi_wdata;
  wire [MEM_DQ_WIDTH*8/8-1:0] axi_wstrb;
  wire                        axi_wready;
  wire [                 3:0] axi_wusero_id;
  wire                        axi_wusero_last;
  wire [ CTRL_ADDR_WIDTH-1:0] axi_araddr;
  wire                        axi_aruser_ap;
  wire [                 3:0] axi_aruser_id;
  wire [                 3:0] axi_arlen;
  wire                        axi_arready;
  wire                        axi_arvalid;
  wire [  MEM_DQ_WIDTH*8-1:0] axi_rdata  /* synthesis syn_keep = 1 */;
  wire                        axi_rvalid  /* synthesis syn_keep = 1 */;
  wire [                 3:0] axi_rid;
  wire                        axi_rlast;
  wire                        resetn;
  reg  [                26:0] cnt;
  wire [                 7:0] err_cnt;
  wire                        free_clk_g;


  //pcie 相关信号定义
  localparam DEVICE_TYPE = 3'b000;  // @IPC enum 3'b000, 3'b001, 3'b100
  localparam AXIS_SLAVE_NUM = 3;  // @IPC enum 1 2 3

  //reg             ref_led;

  // Test unit mode signals
  wire pcie_cfg_ctrl_en;
  wire axis_master_tready_cfg;

  wire cfg_axis_slave0_tvalid;
  wire [127:0] cfg_axis_slave0_tdata;
  wire cfg_axis_slave0_tlast;
  wire cfg_axis_slave0_tuser;

  // For mux
  wire axis_master_tready_mem;
  wire axis_master_tvalid_mem;
  wire [127:0] axis_master_tdata_mem;
  wire [3:0] axis_master_tkeep_mem;

  wire axis_master_tlast_mem;
  wire [7:0] axis_master_tuser_mem;

  wire cross_4kb_boundary;

  wire dma_axis_slave0_tvalid;
  wire [127:0] dma_axis_slave0_tdata;
  wire dma_axis_slave0_tlast;
  wire dma_axis_slave0_tuser;

  // Reset debounce and sync
  wire sync_button_rst_n;
  wire ref_core_rst_n;
  wire sync_perst_n;
  wire s_pclk_rstn;

  // Internal signal
  wire			pclk_div2/*synthesis PAP_MARK_DEBUG="1"*/;  	// 用户时钟，x2 5gt/s时，为125MHZ 2.5gt/s时为62.5
  wire			pclk/*synthesis PAP_MARK_DEBUG="1"*/;			// 用户时钟，x2 5gt/s时，为125MHZ 2.5gt/s时为62.5			
  wire ref_clk;
  wire core_rst_n;

  wire axis_master_tvalid;
  wire axis_master_tready;
  wire [127:0] axis_master_tdata;
  wire [3:0] axis_master_tkeep;
  wire axis_master_tlast;
  wire [7:0] axis_master_tuser;

  // AXI4-Stream slave 0 interface
  wire axis_slave0_tready;
  wire axis_slave0_tvalid;
  wire [127:0] axis_slave0_tdata;
  wire axis_slave0_tlast;
  wire axis_slave0_tuser;
  // AXI4-Stream slave 1 interface
  wire axis_slave1_tready;
  wire axis_slave1_tvalid;
  wire [127:0] axis_slave1_tdata;
  wire axis_slave1_tlast;
  wire axis_slave1_tuser;
  // AXI4-Stream slave 2 interface
  wire axis_slave2_tready;
  wire axis_slave2_tvalid;
  wire [127:0] axis_slave2_tdata;
  wire axis_slave2_tlast;
  wire axis_slave2_tuser;

  wire [7:0] cfg_pbus_num;
  wire [4:0] cfg_pbus_dev_num;
  wire [2:0] cfg_max_rd_req_size;
  wire [2:0] cfg_max_payload_size;
  wire cfg_rcb;

  wire cfg_ido_req_en;
  wire cfg_ido_cpl_en;
  wire [7:0] xadm_ph_cdts;
  wire [11:0] xadm_pd_cdts;
  wire [7:0] xadm_nph_cdts;
  wire [11:0] xadm_npd_cdts;
  wire [7:0] xadm_cplh_cdts;
  wire [11:0] xadm_cpld_cdts;

  wire [4:0] smlh_ltssm_state  /*synthesis PAP_MARK_DEBUG="1"*/;  //link状态机

  // Led lights up signal
  reg [22:0] ref_led_cnt;
  reg [26:0] pclk_led_cnt;
  wire smlh_link_up;
  wire rdlh_link_up  /*synthesis PAP_MARK_DEBUG="1"*/;

  // Uart to APB 32bits
  wire uart_p_sel;
  wire [3:0] uart_p_strb;
  wire [15:0] uart_p_addr;
  wire [31:0] uart_p_wdata;
  wire uart_p_ce;
  wire uart_p_we;
  wire uart_p_rdy;
  wire [31:0] uart_p_rdata;

  // APB signal
  wire [3:0] p_strb;
  wire [15:0] p_addr;
  wire [31:0] p_wdata;
  wire p_ce;
  wire p_we;

  // APB MUX signal
  // 0~5: HSSTLP 6: Reserved 7: PCIe
  // 8: config
  // 9: DMA
  wire p_sel_pcie;
  wire p_sel_cfg;
  wire p_sel_dma;

  wire [31:0] p_rdata_pcie;
  wire [31:0] p_rdata_cfg;
  wire [31:0] p_rdata_dma;

  wire p_rdy_pcie;
  wire p_rdy_cfg;
  wire p_rdy_dma;

  // UART signals
  wire [7:0] uart_rx_data;
  wire uart_rx_valid;
  wire [7:0] mode;
  wire [3:0] select_rearview;
  wire hmi_auto_brake_enable;
  //wire [5:0] cnn_data;

  // Debug signal assignments for waveform capture
  //assign debug_mode     = mode;
  //assign debug_cnn_data = cnn_data;

  assign cfg_ido_req_en = 1'b0;
  assign cfg_ido_cpl_en = 1'b0;
  assign xadm_ph_cdts   = 8'b0;
  assign xadm_pd_cdts   = 12'b0;
  assign xadm_nph_cdts  = 8'b0;
  assign xadm_npd_cdts  = 12'b0;
  assign xadm_cplh_cdts = 8'b0;
  assign xadm_cpld_cdts = 12'b0;

  //axi
  reg          ch0_rframe_req;
  wire         ch0_rframe_req_ack;
  wire         ch0_rframe_data_en;
  wire [127:0] ch0_rframe_data;
  wire         ch0_rframe_data_valid;

  //dma
  // DMA CTRL      BASE ADDR = 0x8000
  wire         o_dma_write_data_req;
  wire [ 11:0] o_dma_write_addr;
  wire [127:0] i_dma_write_data;

  // 数据拼接相关信号
  wire         axi_dma_write_data_req;
  wire [127:0] axi_dma_write_data;
  reg  [127:0] concatenated_data;
  reg          concatenated_data_valid;
  reg  [  4:0] black_sent_counter;  // 已发送的黑色块计数(0-14)
  reg          is_sending_black;  // 标志是否正在发送黑色像素
  reg          line_start_flag;  // 行开始标志寄存
  reg          waiting_for_line;  // 等待新行标志
  reg  [  6:0] pixel_block_counter;  // 当前行已传输的128位块计数(0-94)
  //===================================================================================================================
  //fspi
  //===================================================================================================================
  wire [  7:0] mem0_out;
  wire [  7:0] mem1_out;
  wire [  7:0] mem2_out;
  wire [  7:0] mem3_out;
  wire [  7:0] mem4_out;

  // FSPI registers are written on its faster wr_clk. Their button levels
  // are held for many free_clk periods, so two flip-flops safely transfer
  // them into the 25 MHz pad-control domain before any decision is made.
  reg [4:0] fspi_button_meta;
  reg [4:0] fspi_button_sync;   always @(posedge free_clk or negedge board_rst_n) begin
    if (!board_rst_n) begin
      fspi_button_meta <= 5'b11111;
      fspi_button_sync <= 5'b11111;
    end else begin
      fspi_button_meta <= {mem4_out[0], mem3_out[0], mem2_out[0], mem1_out[0], mem0_out[0]};
      fspi_button_sync <= fspi_button_meta;
    end
  end

  // UART commands originate on true_50M. A stable-value filter prevents a
  // multi-bit direction change from looking like a momentary E request.
  wire [3:0] hmi_select_sync;
  wire hmi_rearm;
  hmi_control_synchronizer u_hmi_control_synchronizer (
      .clk          (free_clk),
      .rst_n        (board_rst_n),
      .async_code   (select_rearview),
      .code_sync    (hmi_select_sync),
      .rearm_command(hmi_rearm)
  );

  // A pedestrian request latches STOP and all direction contacts until E.
  // HMI E remains a momentary manual STOP and can force-clear that latch.
  wire stop_pressed;
  wire movement_blocked;
  wire fspi_direction_armed;
  wire auto_brake_enabled;
  wire auto_brake_request;
  auto_brake_enable_gate u_auto_brake_enable_gate (
      .clk                  (free_clk),
      .rst_n                (board_rst_n),
      .async_enable         (hmi_auto_brake_enable),
      .brake_request_in     (~fspi_button_sync[4]),
      .enable_sync          (auto_brake_enabled),
      .brake_request_out    (auto_brake_request)
  );
  remote_stop_controller u_remote_stop_controller (
      .clk                  (free_clk),
      .rst_n                (board_rst_n),
      .brake_request        (auto_brake_request),
      .rearm_command        (hmi_rearm),
      .fspi_directions_idle (&fspi_button_sync[3:0]),
      .stop_pressed         (stop_pressed),
      .movement_blocked     (movement_blocked),
      .fspi_direction_armed (fspi_direction_armed)
  );

  // A released HMI direction must not expose an older active FSPI register.
  // Return control to FSPI only after all four direction registers are idle.
  wire go_released;
  wire back_released;
  wire left_released;
  wire right_released;
  remote_direction_arbiter u_remote_direction_arbiter (
      .clk                 (free_clk),
      .rst_n               (board_rst_n),
      .hmi_code            (hmi_select_sync),
      .fspi_released       (fspi_button_sync[3:0]),
      .fspi_direction_armed(fspi_direction_armed),
      .movement_blocked    (movement_blocked),
      .go_released         (go_released),
      .back_released       (back_released),
      .left_released       (left_released),
      .right_released      (right_released)
  );

  // GO/BACK/STOP actively drive the requested idle-high/pressed-low levels.
  // LEFT/RIGHT retain their existing open-drain contact wiring.
  assign go    = go_released;
  assign back  = back_released;
  assign left  = left_released  ? 1'bz : 1'b0;
  assign right = right_released ? 1'bz : 1'b0;
  assign stop  = ~stop_pressed;

  fspi_slave_top u_fspi_slave_top (
      .sys_clk        (free_clk),
      .rst_n          (board_rst_n),
      .spi_clk        (spi_clk),
      .spi_cs         (spi_cs),
      .spi_data       (spi_data),
      .select_rearview(select_rearview),             // 传递select_rearview信号
      .auto_brake_enable(auto_brake_enabled),
      .mem0_out       (mem0_out),
      .mem1_out       (mem1_out),
      .mem2_out       (mem2_out),
      .mem3_out       (mem3_out),
      .mem4_out       (mem4_out)
  );

  //===================================================================================================================
  //UART 通信控制
  //===================================================================================================================

  wire true_50M;
  pllaaa the_instance_name (
      .clkout0(true_50M),  // output
      .lock   (),          // output
      .clkin1 (free_clk)   // input
  );




  //////////////////////UART通信控制///////////////////////////
  uart_loopback u_uart_loopback (
      .sys_clk     (true_50M),
      .sys_rst_n   (board_rst_n),
      .uart_rxd    (RXD),
      .uart_txd    (),               // 不连接外部引脚
      .uart_rx_done(uart_rx_valid),
      .uart_rx_data(uart_rx_data)
  );

  ////////////////////////UART屏幕控制///////////////////////////
  uart_lcd u_uart_lcd (
      .clk          (true_50M),
      .rst_n        (board_rst_n),
      .uart_rx      (uart_rx_data),
      .uart_rx_valid(uart_rx_valid),
      .mode         (mode),
      .cnn_level    (select_rearview),
      .auto_brake_enable(hmi_auto_brake_enable)
  );

  wire [5:0] level;
  assign level          = (mode == 8'd8) ? 6'd0 :
                          (mode == 8'd9) ? 6'd1 :  
                          (mode == 8'd10) ? 6'd2 : 
                          (mode == 8'd11) ? 6'd3 : 
                          (mode == 8'd12) ? 6'd4 : 
                          (mode == 8'd13) ? 6'd5 : 
                          (mode == 8'd14) ? 6'd6 : 
                          (mode == 8'd15) ? 6'd7 : 
                          (mode == 8'd16) ? 6'd8 :
                          6'd0;

  //=================================================================================================================
  // OV5640 video stream
  //=================================================================================================================
  wire        clk_25M;
  wire        clk_50M;

  wire        video2_clk;
  wire [ 9:0] video2_x;
  wire [ 9:0] video2_y;
  wire        video2_vs;
  wire        video2_href;
  wire        video2_de;
  wire [15:0] video2_data;

  wire        video5_clk;
  wire [ 9:0] video5_x;
  wire [ 9:0] video5_y;
  wire        video5_vs;
  wire        video5_href;
  wire        video5_de;
  wire [15:0] video5_data;

  wire        video6_clk;
  wire [ 9:0] video6_x;
  wire [ 9:0] video6_y;
  wire        video6_vs;
  wire        video6_href;
  wire        video6_de;
  wire [15:0] video6_data;


  // OV5640 top module instantiation
  ov5640_top u_ov5640_top (
      .sys_clk      (free_clk),       //50Mhz
      .cmos_h_pixel (H_CMOS_DISP),    //水平方向分辨率
      .cmos_v_pixel (V_CMOS_DISP),    //垂直方向分辨率
      .total_h_pixel(TOTAL_H_PIXEL),  //水平总像素大小
      .total_v_pixel(TOTAL_V_PIXEL),  //垂直总像素大小
      .y_addr_st    (Y_ADDR_ST),
      .y_addr_end   (Y_ADDR_END),
      .rate         (RATE),           //帧率配置
      .clk_25M      (clk_25M),
      .clk_50M      (clk_50M),
      //coms2
      .cmos2_scl    (cmos2_scl),      //cmos2 i2c 
      .cmos2_sda    (cmos2_sda),      //cmos2 i2c 
      .cmos2_vsync  (cmos2_vsync),    //cmos2 vsync
      .cmos2_href   (cmos2_href),     //cmos2 hsync refrence,data valid
      .cmos2_pclk   (cmos2_pclk),     //cmos2 pxiel clock
      .cmos2_data   (cmos2_data),     //cmos2 data
      .cmos2_reset  (cmos2_reset),    //cmos2 reset
      //coms5
      .cmos5_scl    (cmos5_scl),      //cmos5 i2c 
      .cmos5_sda    (cmos5_sda),      //cmos5 i2c 
      .cmos5_vsync  (cmos5_vsync),    //cmos5 vsync
      .cmos5_href   (cmos5_href),     //cmos5 hsync refrence,data valid
      .cmos5_pclk   (cmos5_pclk),     //cmos5 pxiel clock
      .cmos5_data   (cmos5_data),     //cmos5 data
      .cmos5_reset  (cmos5_reset),    //cmos5 reset
      //coms6
      .cmos6_scl    (cmos6_scl),      //cmos6 i2c 
      .cmos6_sda    (cmos6_sda),      //cmos6 i2c 
      .cmos6_vsync  (cmos6_vsync),    //cmos6 vsync
      .cmos6_href   (cmos6_href),     //cmos6 hsync refrence,data valid
      .cmos6_pclk   (cmos6_pclk),     //cmos6 pxiel clock
      .cmos6_data   (cmos6_data),     //cmos6 data
      .cmos6_reset  (cmos6_reset),    //cmos6 reset
      //output video stream
      .video2_clk   (video2_clk),     //video pixel clock
      .video2_x     (video2_x),
      .video2_y     (video2_y),
      .video2_vs    (video2_vs),      //video vsync  
      .video2_href  (video2_href),    //video href
      .cmos2_frame_valid    (video2_de),      //video data enable
      .cmos2_frame_data  (video2_data),    //video data RGB565

      .video5_clk (video5_clk),   //video pixel clock
      .video5_x   (video5_x),
      .video5_y   (video5_y),
      .video5_vs  (video5_vs),    //video vsync  
      .video5_href(video5_href),  //video href
      .cmos5_frame_valid  (video5_de),    //video data enable
      .cmos5_frame_data(video5_data),  //video data RGB565

      .video6_clk (video6_clk),   //video pixel clock
      .video6_x   (video6_x),
      .video6_y   (video6_y),
      .video6_vs  (video6_vs),    //video vsync  
      .video6_href(video6_href),  //video href
      .cmos6_frame_valid  (video6_de),    //video data enable
      .cmos6_frame_data(video6_data),  //video data RGB565

      // 频率计数器连接
      .cmos2_pclk_count(cmos2_pclk_count),  // CMOS2 像素时钟计数值
      .cmos5_pclk_count(cmos5_pclk_count),  // CMOS5 像素时钟计数值  
      .cmos6_pclk_count(cmos6_pclk_count)   // CMOS6 像素时钟计数值
  );

  //===================================================================================================================
  //CNN 神经网络夜视算法处理
  //===================================================================================================================
  // CNN 相关信号定义
  wire        cnn_frame_vsync;
  wire        cnn_frame_de;
  wire [23:0] cnn_rgb_24bit;
  wire [15:0] cnn_rgb_16bit;
  wire [23:0] image_process_rgb_24bit;
  wire        image_process_vsync;
  wire        image_process_de;

  // 将RGB565转换为RGB888用于CNN处理
  assign image_process_rgb_24bit = {
    video2_data[15:11], 3'b0, video2_data[10:5], 2'b0, video2_data[4:0], 3'b0
  };
  assign image_process_vsync = video2_vs;
  assign image_process_de = video2_de;

  // CNN夜视算法模块实例化
  cnn_top u_cnn_top (
      .clk(video2_clk),
      .rst_n(board_rst_n),
      .per_img_r_1(image_process_rgb_24bit[23:16]),
      .per_img_g_1(image_process_rgb_24bit[15:8]),
      .per_img_b_1(image_process_rgb_24bit[7:0]),
      .post_img_r(cnn_rgb_24bit[23:16]),
      .post_img_g(cnn_rgb_24bit[15:8]),
      .post_img_b(cnn_rgb_24bit[7:0]),
      .per_img_vsync_1(image_process_vsync),
      .per_img_href_1(image_process_de),
      .post_img_vsync(cnn_frame_vsync),
      .post_img_href(cnn_frame_de),
      .level(level)  // 来自UART的控制信号
  );

  // 将CNN输出的RGB888转换回RGB565
  assign cnn_rgb_16bit = {cnn_rgb_24bit[23:19], cnn_rgb_24bit[15:10], cnn_rgb_24bit[7:3]};
  /*
wire rdreq;
wire vga_hs, vga_vs, vga_de;
wire [15:0] vga_rgb;

vga_ctrl u_vga_ctrl
(
    .vga_clk        (clk_54M    ),   
    .sys_rst_n      (board_rst_n),   
    .pix_data       (),   //输入像素点色彩信息
                    
    .pix_data_req   (rdreq      ),  
    .hsync          (vga_hs     ),  //输出行同步信号
    .vsync          (vga_vs     ),  //输出场同步信号
    .rgb_valid      (vga_de     ),  
    .rgb            (vga_rgb    )
);
*/

  //------------- image_recognition -------------

  // wire [15:0] vga_data;
  //    assign vga_data = {video_data[4:0],video_data[10:5],video_data[15:11]};
  // 使用CNN处理后的图像数据

  // wire isp_de, isp_vs;
  // wire [15:0] isp_rgb;

  // isp_ctrl #(
  //     .H_PIXEL(640),
  //     .V_PIXEL(480)
  // ) isp_ctrl (
  //     .clk    (video2_clk),
  //     .sys_clk(clk_50M),
  //     .rst_n  (board_rst_n),
  //     .i_rgb  (vga_data),
  //     .i_vsync(a_vs),
  //     .i_de   (a_de),
  //     .x      (video2_x),
  //     .y      (video2_y),

  //     .VGA_rgb(isp_rgb),
  //     .VGA_vsync(isp_vs),
  //     .VGA_de(isp_de)
  // );

  //*==============================================================================
  //ddr3 IP例化
  //*==============================================================================
  always @(posedge core_clk or negedge ddr_init_done) begin
    if (!ddr_init_done) cnt <= 27'd0;
    else if (cnt >= TH_1S) cnt <= 27'd0;
    else cnt <= cnt + 27'd1;
  end

  always @(posedge core_clk or negedge ddr_init_done) begin
    if (!ddr_init_done) heart_beat_led <= 1'd1;
    else if (cnt >= TH_1S) heart_beat_led <= ~heart_beat_led;
  end

  ddr3 #(
      .MEM_ROW_WIDTH   (MEM_ROW_WIDTH),
      .MEM_COLUMN_WIDTH(MEM_COLUMN_WIDTH),
      .MEM_BANK_WIDTH  (MEM_BANK_WIDTH),
      .MEM_DQ_WIDTH    (MEM_DQ_WIDTH),
      .MEM_DM_WIDTH    (MEM_DQS_WIDTH),
      .MEM_DQS_WIDTH   (MEM_DQS_WIDTH),
      .CTRL_ADDR_WIDTH (CTRL_ADDR_WIDTH)
  ) I_ips_ddr_top (
      .ref_clk        (free_clk),
      .resetn         (board_rst_n),
      .core_clk       (core_clk),
      .pll_lock       (pll_lock),
      .phy_pll_lock   (phy_pll_lock),
      .gpll_lock      (gpll_lock),
      .rst_gpll_lock  (rst_gpll_lock),
      .ddrphy_cpd_lock(ddrphy_cpd_lock),
      .ddr_init_done  (ddr_init_done),

      .axi_awaddr   (axi_awaddr),
      .axi_awuser_ap(axi_awuser_ap),
      .axi_awuser_id(axi_awuser_id),
      .axi_awlen    (axi_awlen),
      .axi_awready  (axi_awready),
      .axi_awvalid  (axi_awvalid),

      .axi_wdata      (axi_wdata),
      .axi_wstrb      (axi_wstrb),
      .axi_wready     (axi_wready),
      .axi_wusero_id  (axi_wusero_id),
      .axi_wusero_last(axi_wusero_last),

      .axi_araddr   (axi_araddr),
      .axi_aruser_ap(axi_aruser_ap),
      .axi_aruser_id(axi_aruser_id),
      .axi_arlen    (axi_arlen),
      .axi_arready  (axi_arready),
      .axi_arvalid  (axi_arvalid),

      .axi_rdata (axi_rdata),
      .axi_rid   (axi_rid),
      .axi_rlast (axi_rlast),
      .axi_rvalid(axi_rvalid),

      .apb_clk   (1'b0),
      .apb_rst_n (1'b0),
      .apb_sel   (1'b0),
      .apb_enable(1'b0),
      .apb_addr  (8'd0),
      .apb_write (1'b0),
      .apb_ready (),
      .apb_wdata (16'd0),
      .apb_rdata (),


      .mem_cs_n(mem_cs_n),

      .mem_rst_n(mem_rst_n),
      .mem_ck   (mem_ck),
      .mem_ck_n (mem_ck_n),
      .mem_cke  (mem_cke),
      .mem_ras_n(mem_ras_n),
      .mem_cas_n(mem_cas_n),
      .mem_we_n (mem_we_n),
      .mem_odt  (mem_odt),
      .mem_a    (mem_a),
      .mem_ba   (mem_ba),
      .mem_dqs  (mem_dqs),
      .mem_dqs_n(mem_dqs_n),
      .mem_dq   (mem_dq),
      .mem_dm   (mem_dm),

      //debug
      .dbg_gate_start   (1'b0),
      .dbg_cpd_start    (1'b0),
      .dbg_ddrphy_rst_n (1'b1),
      .dbg_gpll_scan_rst(1'b0),

      .samp_position_dyn_adj  (1'b0),
      .init_samp_position_even(16'd0),
      .init_samp_position_odd (16'd0),

      .wrcal_position_dyn_adj(1'b0),
      .init_wrcal_position   (16'd0),

      .force_read_clk_ctrl(1'b0),
      .init_slip_step     (8'd0),
      .init_read_clk_ctrl (6'd0),

      .debug_calib_ctrl    (),
      .dbg_dll_upd_state   (),
      .dbg_slice_status    (),
      .dbg_slice_state     (),
      .debug_data          (),
      .debug_gpll_dps_phase(),

      .dbg_rst_dps_state   (),
      .dbg_tran_err_rst_cnt(),
      .dbg_ddrphy_init_fail(),

      .debug_cpd_offset_adj(1'b0),
      .debug_cpd_offset_dir(1'b0),
      .debug_cpd_offset    (10'd0),
      .debug_dps_cnt_dir0  (),
      .debug_dps_cnt_dir1  (),

      .ck_dly_en       (1'b0),
      .init_ck_dly_step(8'd0),
      .ck_dly_set_bin  (),

      .align_error    (),
      .debug_rst_state(),
      .debug_cpd_state()

  );

  //*==============================================================================
  //axi控制器例化
  //*==============================================================================
  axi4_ctrl_3ch #(
      .C_RD_END_ADDR(640 * 480 * 2),
      .C_W_WIDTH(16),
      .C_R_WIDTH(128),
      .C_ID_LEN(4)
  ) u_axi4_ctrl (

      .axi_clk  (core_clk),
      .axi_reset(~ddr_init_done),

      .axi_awaddr (axi_awaddr),
      .axi_awlen  (axi_awlen),
      .axi_awvalid(axi_awvalid),
      .axi_awready(axi_awready),

      .axi_wdata (axi_wdata),
      .axi_wstrb (axi_wstrb),
      .axi_wlast (axi_wusero_last),
      .axi_wvalid(),
      .axi_wready(axi_wready),

      .axi_bid   (0),
      .axi_bresp (0),
      .axi_bvalid(1),

      .axi_arid   (axi_aruser_id),
      .axi_araddr (axi_araddr),
      .axi_arlen  (axi_arlen),
      .axi_arvalid(axi_arvalid),
      .axi_arready(axi_arready),

      .axi_rid   (axi_rid),
      .axi_rdata (axi_rdata),
      .axi_rresp (0),
      .axi_rlast (axi_rlast),
      .axi_rvalid(axi_rvalid),
      .axi_rready(),

      .wframe_pclk({video6_clk, video5_clk, video2_clk}),
      .wframe_vsync({~video6_vs, ~video5_vs, ~cnn_frame_vsync}),
      .wframe_data_en({video6_de, video5_de, cnn_frame_de}),
      .wframe_data0(cnn_rgb_16bit),
      .wframe_data1(video5_data),
      .wframe_data2(video6_data),

      .rframe_pclk(pclk_div2),
      .rframe_vsync(ch0_rframe_req),
      .rframe_data_en(axi_dma_write_data_req),
      .rframe_data(axi_dma_write_data),

      .tp_o()
  );

  //*==============================================================================
  // pcie
  //*==============================================================================
  // Rst debounce
  hsst_rst_cross_sync_v1_0 #(
`ifdef IPS2L_PCIE_SPEEDUP_SIM
      .RST_CNTR_VALUE(16'h10)
`else
      .RST_CNTR_VALUE(16'hC000)
`endif
  ) u_refclk_buttonrstn_debounce (
      .clk     (ref_clk),
      .rstn_in (board_rst_n),
      .rstn_out(sync_button_rst_n)
  );

  hsst_rst_cross_sync_v1_0 #(
`ifdef IPS2L_PCIE_SPEEDUP_SIM
      .RST_CNTR_VALUE(16'h10)
`else
      .RST_CNTR_VALUE(16'hC000)
`endif
  ) u_refclk_perstn_debounce (
      .clk     (ref_clk),
      .rstn_in (perst_n),
      .rstn_out(sync_perst_n)
  );

  hsst_rst_sync_v1_0 u_ref_core_rstn_sync (
      .clk       (ref_clk),
      .rst_n     (core_rst_n),
      .sig_async (1'b1),
      .sig_synced(ref_core_rst_n)
  );

  hsst_rst_sync_v1_0 u_pclk_core_rstn_sync (
      .clk       (pclk),
      .rst_n     (core_rst_n),
      .sig_async (1'b1),
      .sig_synced(s_pclk_rstn)
  );

  always @(posedge ref_clk or negedge sync_perst_n) begin
    if (!sync_perst_n) begin
      ref_led_cnt <= 23'd0;
      ref_led <= 1'b1;
    end else if (smlh_link_up & rdlh_link_up) begin
      ref_led_cnt <= ref_led_cnt + 23'd1;
      if (&ref_led_cnt) ref_led <= ~ref_led;
    end
  end

  always @(posedge pclk or negedge s_pclk_rstn) begin
    if (!s_pclk_rstn) begin
      pclk_led_cnt <= 27'd0;
      pclk_led <= 1'b1;
    end else if (smlh_link_up & rdlh_link_up) begin
      pclk_led_cnt <= pclk_led_cnt + 27'd1;
      if (&pclk_led_cnt) pclk_led <= ~pclk_led;
    end
  end

  //===========================================================================
  //pcie dma
  //===========================================================================
  // DMA CTRL      BASE ADDR = 0x8000
  ips2l_pcie_dma #(
      .DEVICE_TYPE   (DEVICE_TYPE),
      .AXIS_SLAVE_NUM   (AXIS_SLAVE_NUM)
  ) u_ips2l_pcie_dma (
      .clk  (pclk_div2),
      .rst_n(core_rst_n),

      // Num
      .i_cfg_pbus_num   (cfg_pbus_num),
      .i_cfg_pbus_dev_num  (cfg_pbus_dev_num),
      .i_cfg_max_rd_req_size (cfg_max_rd_req_size),
      .i_cfg_max_payload_size (cfg_max_payload_size),

      // AXI4-Stream master interface
      .i_axis_master_tvld (axis_master_tvalid_mem),
      .o_axis_master_trdy (axis_master_tready_mem),
      .i_axis_master_tdata(axis_master_tdata_mem),
      .i_axis_master_tkeep(axis_master_tkeep_mem),

      .i_axis_master_tlast(axis_master_tlast_mem),
      .i_axis_master_tuser(axis_master_tuser_mem),

      // AXI4-Stream slave0 interface
      .i_axis_slave0_trdy (axis_slave0_tready),
      .o_axis_slave0_tvld (dma_axis_slave0_tvalid),
      .o_axis_slave0_tdata(dma_axis_slave0_tdata),
      .o_axis_slave0_tlast(dma_axis_slave0_tlast),
      .o_axis_slave0_tuser(dma_axis_slave0_tuser),

      // AXI4-Stream slave1 interface
      .i_axis_slave1_trdy (axis_slave1_tready),
      .o_axis_slave1_tvld (axis_slave1_tvalid),
      .o_axis_slave1_tdata(axis_slave1_tdata),
      .o_axis_slave1_tlast(axis_slave1_tlast),
      .o_axis_slave1_tuser(axis_slave1_tuser),

      // AXI4-Stream slave2 interface
      .i_axis_slave2_trdy (axis_slave2_tready),
      .o_axis_slave2_tvld (axis_slave2_tvalid),
      .o_axis_slave2_tdata(axis_slave2_tdata),
      .o_axis_slave2_tlast(axis_slave2_tlast),
      .o_axis_slave2_tuser(axis_slave2_tuser),

      // From pcie
      .i_cfg_ido_req_en(cfg_ido_req_en),
      .i_cfg_ido_cpl_en(cfg_ido_cpl_en),
      .i_xadm_ph_cdts  (xadm_ph_cdts),
      .i_xadm_pd_cdts  (xadm_pd_cdts),
      .i_xadm_nph_cdts (xadm_nph_cdts),
      .i_xadm_npd_cdts (xadm_npd_cdts),
      .i_xadm_cplh_cdts(xadm_cplh_cdts),
      .i_xadm_cpld_cdts(xadm_cpld_cdts),

      // APB interface
      .i_apb_psel          (p_sel_dma),
      .i_apb_paddr         (p_addr[8:0]),
      .i_apb_pwdata        (p_wdata),
      .i_apb_pstrb         (p_strb),
      .i_apb_pwrite        (p_we),
      .i_apb_penable       (p_ce),
      .o_apb_prdy          (p_rdy_dma),
      .o_apb_prdata        (p_rdata_dma),
      .o_cross_4kb_boundary(cross_4kb_boundary),    //4k边界
      //**********************************************************************
      // dma write interface
      .o_dma_write_data_req(o_dma_write_data_req),
      .o_dma_write_addr    (o_dma_write_addr),
      .i_dma_write_data    (i_dma_write_data)
  );

  assign p_rdy_cfg              = 1'b0;
  assign p_rdata_cfg            = 32'b0;

  assign axis_slave0_tvalid     = dma_axis_slave0_tvalid;
  assign axis_slave0_tlast      = dma_axis_slave0_tlast;
  assign axis_slave0_tuser      = dma_axis_slave0_tuser;
  assign axis_slave0_tdata      = dma_axis_slave0_tdata;

  assign axis_master_tvalid_mem = axis_master_tvalid;
  assign axis_master_tdata_mem  = axis_master_tdata;
  assign axis_master_tkeep_mem  = axis_master_tkeep;
  assign axis_master_tlast_mem  = axis_master_tlast;
  assign axis_master_tuser_mem  = axis_master_tuser;

  assign axis_master_tready     = axis_master_tready_mem;

  // PCIe IP TOP : HSSTLP : 0x0000~6000 PCIe BASE ADDR : 0x7000
  pcie_test u_ips2l_pcie_wrap (
      .button_rst_n  (1'b1),
      .power_up_rst_n(1'b1),
      .perst_n       (1'b1),

      // The clock and reset signals
      .pclk      (pclk),
      .pclk_div2 (pclk_div2),
      .ref_clk   (ref_clk),
      .ref_clk_n (ref_clk_n),
      .ref_clk_p (ref_clk_p),
      .core_rst_n(core_rst_n),

      // APB interface to DBI config
      .p_sel  (p_sel_pcie),
      .p_strb (uart_p_strb),
      .p_addr (uart_p_addr),
      .p_wdata(uart_p_wdata),
      .p_ce   (uart_p_ce),
      .p_we   (uart_p_we),
      .p_rdy  (p_rdy_pcie),
      .p_rdata(p_rdata_pcie),

      // PHY diff signals
      .rxn              (rxn),
      .rxp              (rxp),
      .txn              (txn),
      .txp              (txp),
      .pcs_nearend_loop ({4{1'b0}}),
      .pma_nearend_ploop({4{1'b0}}),
      .pma_nearend_sloop({4{1'b0}}),

      // AXI4-Stream master interface
      .axis_master_tvalid(axis_master_tvalid),
      .axis_master_tready(axis_master_tready),
      .axis_master_tdata (axis_master_tdata),
      .axis_master_tkeep (axis_master_tkeep),

      .axis_master_tlast(axis_master_tlast),
      .axis_master_tuser(axis_master_tuser),

      // AXI4-Stream slave 0 interface
      .axis_slave0_tready(axis_slave0_tready),
      .axis_slave0_tvalid(axis_slave0_tvalid),
      .axis_slave0_tdata (axis_slave0_tdata),
      .axis_slave0_tlast (axis_slave0_tlast),
      .axis_slave0_tuser (axis_slave0_tuser),

      // AXI4-Stream slave 1 interface
      .axis_slave1_tready(axis_slave1_tready),
      .axis_slave1_tvalid(axis_slave1_tvalid),
      .axis_slave1_tdata (axis_slave1_tdata),
      .axis_slave1_tlast (axis_slave1_tlast),
      .axis_slave1_tuser (axis_slave1_tuser),

      // AXI4-Stream slave 2 interface
      .axis_slave2_tready(axis_slave2_tready),
      .axis_slave2_tvalid(axis_slave2_tvalid),
      .axis_slave2_tdata (axis_slave2_tdata),
      .axis_slave2_tlast (axis_slave2_tlast),
      .axis_slave2_tuser (axis_slave2_tuser),

      .pm_xtlh_block_tlp(),

      .cfg_send_cor_err_mux(),
      .cfg_send_nf_err_mux (),
      .cfg_send_f_err_mux  (),
      .cfg_sys_err_rc      (),
      .cfg_aer_rc_err_mux  (),

      // The radm timeout
      .radm_cpl_timeout(),

      // Configuration signals
      .cfg_max_rd_req_size (cfg_max_rd_req_size),
      .cfg_bus_master_en   (),
      .cfg_max_payload_size(cfg_max_payload_size),
      .cfg_ext_tag_en      (),
      .cfg_rcb             (cfg_rcb),
      .cfg_mem_space_en    (),
      .cfg_pm_no_soft_rst  (),
      .cfg_crs_sw_vis_en   (),
      .cfg_no_snoop_en     (),
      .cfg_relax_order_en  (),
      .cfg_tph_req_en      (),
      .cfg_pf_tph_st_mode  (),
      .rbar_ctrl_update    (),
      .cfg_atomic_req_en   (),

      .cfg_pbus_num    (cfg_pbus_num),
      .cfg_pbus_dev_num(cfg_pbus_dev_num),

      // Debug signals
      .radm_idle                (),
      .radm_q_not_empty         (),
      .radm_qoverflow           (),
      .diag_ctrl_bus            (2'b0),
      .cfg_link_auto_bw_mux     (),
      .cfg_bw_mgt_mux           (),
      .cfg_pme_mux              (),
      .app_ras_des_sd_hold_ltssm(1'b0),
      .app_ras_des_tba_ctrl     (2'b0),

      .dyn_debug_info_sel(4'b0),
      .debug_info_mux    (),

      // System signal
      .smlh_link_up    (smlh_link_up),     //link状态
      .rdlh_link_up    (rdlh_link_up),     //link状态
      .smlh_ltssm_state(smlh_ltssm_state)
  );



  //=======================
  reg [11:0] o_dma_write_addr_dly1;
  reg [11:0] o_dma_write_addr_dly2;
  reg [11:0] dma_write_cnt;  // 计数dma_write的次数

  always @(posedge pclk_div2) begin
    if (!ddr_init_done) begin
      o_dma_write_addr_dly1 <= 12'd0;
      o_dma_write_addr_dly2 <= 12'd0;
    end else begin
      o_dma_write_addr_dly1 <= o_dma_write_addr;
      o_dma_write_addr_dly2 <= o_dma_write_addr_dly1;
    end
  end

  always @(posedge pclk_div2) begin
    if (!ddr_init_done) begin
      dma_write_cnt <= 12'd0;
    end else if (o_dma_write_addr_dly1 == 12'h5E && o_dma_write_addr_dly2 == 12'h5D) begin
      dma_write_cnt <= dma_write_cnt + 1'b1;
    end else if (dma_write_cnt == 12'd480) begin
      dma_write_cnt <= 12'd0;
    end else begin
      dma_write_cnt <= dma_write_cnt;
    end
  end

  always @(posedge pclk_div2) begin
    if (!ddr_init_done) begin
      ch0_rframe_req <= 1'b0;
    end else if (dma_write_cnt == 12'd480) begin
      ch0_rframe_req <= 1'b1;
    end else begin
      ch0_rframe_req <= 1'b0;
    end
  end

  //=======================
  // 数据拼接模块: 在每行前面添加120个黑色像素(15个128位数据块)
  // 图像尺寸: 760×480 (原640×480 + 120黑色像素)
  // DMA地址范围: 0x000~0x5E (95个128位传输 = 1520字节)
  //=======================
  reg  [11:0] o_dma_write_addr_last;
  wire        line_start;

  // 记录上一个地址,用于检测行开始
  always @(posedge pclk_div2) begin
    if (!ddr_init_done) begin
      o_dma_write_addr_last <= 12'd0;
    end else if (o_dma_write_data_req) begin
      o_dma_write_addr_last <= o_dma_write_addr;
    end
  end

  // 检测行开始:地址从0x5E或其他值跳转到0x000
  assign line_start = (o_dma_write_addr == 12'd0) && 
                    (o_dma_write_addr_last != 12'd0) && 
                    o_dma_write_data_req;

  // 状态机控制
  always @(posedge pclk_div2) begin
    if (!ddr_init_done) begin
      black_sent_counter <= 5'd0;
      is_sending_black <= 1'b0;
      line_start_flag <= 1'b0;
      waiting_for_line <= 1'b1;  // 初始等待第一行
      pixel_block_counter <= 7'd0;
    end else begin
      // 检测到行开始
      if (line_start || (waiting_for_line && o_dma_write_addr == 12'd0 && o_dma_write_data_req)) begin
        line_start_flag <= 1'b1;
        is_sending_black <= 1'b1;
        black_sent_counter <= 5'd0;
        waiting_for_line <= 1'b0;
        pixel_block_counter <= 7'd0;  // 重置块计数器
      end  // 正在发送黑色像素
      else if (is_sending_black && o_dma_write_data_req) begin
        if (black_sent_counter < 5'd14) begin
          black_sent_counter  <= black_sent_counter + 1'b1;
          pixel_block_counter <= pixel_block_counter + 7'd1;  // 增加块计数
        end else begin
          // 发送完15个黑色块
          black_sent_counter <= 5'd0;
          is_sending_black <= 1'b0;
          line_start_flag <= 1'b0;
          pixel_block_counter <= pixel_block_counter + 7'd1;  // 最后一个黑色块
        end
      end  // 发送实际数据时增加块计数
      else if (!is_sending_black && o_dma_write_data_req) begin
        pixel_block_counter <= pixel_block_counter + 7'd1;
      end
      // 检测当前行结束,准备下一行 - 确保发送完所有95个块(15黑+80实际数据)
      // 由于640像素÷8像素/块=80块,所以实际数据是80块,加上15个黑色块=95块
      if (!is_sending_black && pixel_block_counter >= 7'd94 && o_dma_write_data_req) begin
        waiting_for_line <= 1'b1;
      end
    end
  end

  // AXI数据请求控制:只有在发送实际数据时才请求AXI
  // 添加额外的保护,确保黑色像素发送期间不会提前读取数据
  assign axi_dma_write_data_req = o_dma_write_data_req && (!is_sending_black) && (!line_start_flag);

  // 数据选择和输出
  always @(posedge pclk_div2) begin
    if (!ddr_init_done) begin
      concatenated_data <= 128'd0;
      concatenated_data_valid <= 1'b0;
    end else if (o_dma_write_data_req) begin
      if (is_sending_black || line_start_flag) begin
        // 发送白色像素(RGB565格式:0xFFFF表示白色)
        concatenated_data <= 128'hFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF;
        concatenated_data_valid <= 1'b1;
      end else begin
        // 发送实际AXI读取的数据
        concatenated_data <= axi_dma_write_data;
        concatenated_data_valid <= 1'b1;
      end
    end else begin
      concatenated_data_valid <= 1'b0;
    end
  end

  // 输出拼接后的数据
  assign i_dma_write_data = concatenated_data;

endmodule

// HMI directions have priority while pressed. Their release frame is 0, so a
// previously asserted FSPI bit must not immediately press the same contact
// again. Wait for an all-idle FSPI state before accepting a fresh FSPI press.
module remote_direction_arbiter (
    input  wire       clk,
    input  wire       rst_n,
    input  wire [3:0] hmi_code,
    input  wire [3:0] fspi_released,
    input  wire       fspi_direction_armed,
    input  wire       movement_blocked,
    output wire       go_released,
    output wire       back_released,
    output wire       left_released,
    output wire       right_released
);
  wire hmi_go    = (hmi_code == 4'ha);
  wire hmi_back  = (hmi_code == 4'hb);
  wire hmi_left  = (hmi_code == 4'hc);
  wire hmi_right = (hmi_code == 4'hd);
  wire hmi_neutral = (hmi_code == 4'he);
  wire hmi_direction_valid = hmi_go | hmi_back | hmi_left | hmi_right;
  wire fspi_idle = &fspi_released;

  reg hmi_release_hold;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
      hmi_release_hold <= 1'b0;
    else if (hmi_direction_valid)
      hmi_release_hold <= 1'b1;
    else if (fspi_idle)
      hmi_release_hold <= 1'b0;
  end

  wire hmi_owns_direction = hmi_direction_valid | hmi_neutral | hmi_release_hold;
  wire fspi_blocked = ~fspi_direction_armed;
  assign go_released    = movement_blocked | (hmi_owns_direction ? ~hmi_go    : (fspi_blocked | fspi_released[0]));
  assign back_released  = movement_blocked | (hmi_owns_direction ? ~hmi_back  : (fspi_blocked | fspi_released[1]));
  assign left_released  = movement_blocked | (hmi_owns_direction ? ~hmi_left  : (fspi_blocked | fspi_released[2]));
  assign right_released = movement_blocked | (hmi_owns_direction ? ~hmi_right : (fspi_blocked | fspi_released[3]));
endmodule

// Only the new persistent enable bit crosses into the existing STOP domain.
module auto_brake_enable_gate (
    input  wire clk,
    input  wire rst_n,
    input  wire async_enable,
    input  wire brake_request_in,
    output wire enable_sync,
    output wire brake_request_out
);
  (* ASYNC_REG = "TRUE" *) reg [1:0] enable_pipe;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      enable_pipe <= 2'b00;
    end else begin
      enable_pipe <= {enable_pipe[0], async_enable};
    end
  end
  // Disabling only prevents new requests. Existing STOP latch state remains
  // in remote_stop_controller and still requires the original REARM action.
  assign enable_sync = enable_pipe[1];
  assign brake_request_out = enable_sync && brake_request_in;
endmodule

// A new automatic brake request holds STOP low until a fresh manual E press.
// E may force-clear the latch while the request is still high. That same
// request is then consumed until it clears; a subsequent rise brakes again.
// Manual E itself holds STOP low, so STOP returns high when E is released.
module remote_stop_controller (
    input  wire clk,
    input  wire rst_n,
    input  wire brake_request,
    input  wire rearm_command,
    input  wire fspi_directions_idle,
    output wire stop_pressed,
    output wire movement_blocked,
    output wire fspi_direction_armed
);
  reg brake_latched;
  reg brake_event_armed;
  reg rearm_previous;
  reg fspi_armed;
  wire rearm_rising = rearm_command & ~rearm_previous;
  wire new_brake_event = brake_request & brake_event_armed;

  assign stop_pressed = rst_n && (brake_latched | rearm_command);
  assign movement_blocked = brake_latched | rearm_command;
  assign fspi_direction_armed = fspi_armed;

  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      brake_latched  <= 1'b0;
      brake_event_armed <= 1'b1;
      rearm_previous <= 1'b0;
      fspi_armed     <= 1'b1;
    end else begin
      rearm_previous <= rearm_command;

      // A held pre-brake direction must not resume after manual unlock.
      // Require all four FSPI registers to be idle before accepting a new one.
      if (brake_latched || new_brake_event || rearm_command)
        fspi_armed <= 1'b0;
      else if (fspi_directions_idle)
        fspi_armed <= 1'b1;

      if (!brake_request)
        brake_event_armed <= 1'b1;

      if (rearm_rising && brake_latched) begin
        // Explicit operator override. Consume a still-present alarm so the
        // following cycle cannot immediately undo this manual unlock.
        brake_latched <= 1'b0;
        if (brake_request)
          brake_event_armed <= 1'b0;
      end else if (new_brake_event) begin
        brake_latched <= 1'b1;
        brake_event_armed <= 1'b0;
      end
    end
  end
endmodule

// The UART decoder and the remote-button controller use different clocks.
// A held E code must survive eight 25 MHz samples before it can toggle STOP;
// this rejects the short intermediate values possible during a 4-bit CDC.
module hmi_control_synchronizer (
    input  wire       clk,
    input  wire       rst_n,
    input  wire [3:0] async_code,
    output reg  [3:0] code_sync,
    output wire       rearm_command
);
  reg [3:0] code_meta;
  reg [2:0] e_stable_count;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      code_meta      <= 4'h0;
      code_sync      <= 4'h0;
      e_stable_count <= 3'd0;
    end else begin
      code_meta <= async_code;
      code_sync <= code_meta;
      if (code_sync == 4'he) begin
        if (e_stable_count != 3'd7)
          e_stable_count <= e_stable_count + 3'd1;
      end else begin
        e_stable_count <= 3'd0;
      end
    end
  end
  assign rearm_command = (code_sync == 4'he) && (e_stable_count == 3'd7);
endmodule
