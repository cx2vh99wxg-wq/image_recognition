// 2026-11-03 注释校订。M 端板级连接。在相机、DDR 和 PCIe 数据通路旁接入 IMU 与车控。
`timescale 1ns / 1ps

module image_pcie_capture_m_imu #(

    parameter MEM_ROW_WIDTH = 15,

    parameter MEM_COLUMN_WIDTH = 10,

    parameter MEM_BANK_WIDTH = 3,

    parameter MEM_DQ_WIDTH = 16,

    parameter MEM_DQS_WIDTH = 2

) (
    inout wire mpu_scl,
    inout wire mpu_sda,
    input free_clk,
    input board_rst_n,

    inout        cmos2_scl,
    inout        cmos2_sda,
    input        cmos2_vsync,
    input        cmos2_href,
    input        cmos2_pclk,
    input  [7:0] cmos2_data,
    output       cmos2_reset,

    inout        cmos5_scl,
    inout        cmos5_sda,
    input        cmos5_vsync,
    input        cmos5_href,
    input        cmos5_pclk,
    input  [7:0] cmos5_data,
    output       cmos5_reset,

    inout        cmos6_scl,
    inout        cmos6_sda,
    input        cmos6_vsync,
    input        cmos6_href,
    input        cmos6_pclk,
    input  [7:0] cmos6_data,
    output       cmos6_reset,

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

    input             ref_clk_p,
    input             ref_clk_n,
    input             perst_n,
    input       [1:0] rxn,
    input       [1:0] rxp,
    output wire [1:0] txn,
    output wire [1:0] txp,

    output reg        heart_beat_led,
    output reg        pclk_led,
    output reg        ref_led,

    input  wire       spi_clk,
    input  wire       spi_cs,
    inout  wire [3:0] spi_data,
    output wire       go,
    output wire       back,
    inout  wire       left,
    inout  wire       right,
    output wire       stop,
    inout  wire       speed_key_n,

    input RXD,
    output wire hmi_status_txd

);

  wire [31:0] cmos2_pclk_count;
  wire [31:0] cmos5_pclk_count;
  wire [31:0] cmos6_pclk_count;
  parameter CTRL_ADDR_WIDTH = MEM_ROW_WIDTH + MEM_BANK_WIDTH + MEM_COLUMN_WIDTH;
  parameter TH_1S = 27'd33000000;
  parameter REM_DQS_WIDTH = 9 - MEM_DQS_WIDTH;

  parameter V_CMOS_DISP = 13'd480;

  parameter H_CMOS_DISP = 13'd640;

  parameter TOTAL_H_PIXEL = 13'h768;

  parameter TOTAL_V_PIXEL = 13'h3d8;

  parameter RATE = 8'h41;
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

  localparam DEVICE_TYPE = 3'b000;
  localparam AXIS_SLAVE_NUM = 3;

  wire pcie_cfg_ctrl_en;
  wire axis_master_tready_cfg;

  wire cfg_axis_slave0_tvalid;
  wire [127:0] cfg_axis_slave0_tdata;
  wire cfg_axis_slave0_tlast;
  wire cfg_axis_slave0_tuser;

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

  wire sync_button_rst_n;
  wire ref_core_rst_n;
  wire sync_perst_n;
  wire s_pclk_rstn;

  wire			pclk_div2/*synthesis PAP_MARK_DEBUG="1"*/;
  wire			pclk/*synthesis PAP_MARK_DEBUG="1"*/;
  wire ref_clk;
  wire core_rst_n;

  wire axis_master_tvalid;
  wire axis_master_tready;
  wire [127:0] axis_master_tdata;
  wire [3:0] axis_master_tkeep;
  wire axis_master_tlast;
  wire [7:0] axis_master_tuser;

  wire axis_slave0_tready;
  wire axis_slave0_tvalid;
  wire [127:0] axis_slave0_tdata;
  wire axis_slave0_tlast;
  wire axis_slave0_tuser;

  wire axis_slave1_tready;
  wire axis_slave1_tvalid;
  wire [127:0] axis_slave1_tdata;
  wire axis_slave1_tlast;
  wire axis_slave1_tuser;

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

  wire [4:0] smlh_ltssm_state  /*synthesis PAP_MARK_DEBUG="1"*/;

  reg [22:0] ref_led_cnt;
  reg [26:0] pclk_led_cnt;
  wire smlh_link_up;
  wire rdlh_link_up  /*synthesis PAP_MARK_DEBUG="1"*/;

  wire uart_p_sel;
  wire [3:0] uart_p_strb;
  wire [15:0] uart_p_addr;
  wire [31:0] uart_p_wdata;
  wire uart_p_ce;
  wire uart_p_we;
  wire uart_p_rdy;
  wire [31:0] uart_p_rdata;

  wire [3:0] p_strb;
  wire [15:0] p_addr;
  wire [31:0] p_wdata;
  wire p_ce;
  wire p_we;

  wire p_sel_pcie;
  wire p_sel_cfg;
  wire p_sel_dma;

  wire [31:0] p_rdata_pcie;
  wire [31:0] p_rdata_cfg;
  wire [31:0] p_rdata_dma;

  wire p_rdy_pcie;
  wire p_rdy_cfg;
  wire p_rdy_dma;

  wire [7:0] uart_rx_data;
  wire uart_rx_valid;
  wire [7:0] mode;
  wire [3:0] select_rearview;
  wire hmi_auto_brake_enable;

  assign cfg_ido_req_en = 1'b0;
  assign cfg_ido_cpl_en = 1'b0;
  assign xadm_ph_cdts   = 8'b0;
  assign xadm_pd_cdts   = 12'b0;
  assign xadm_nph_cdts  = 8'b0;
  assign xadm_npd_cdts  = 12'b0;
  assign xadm_cplh_cdts = 8'b0;
  assign xadm_cpld_cdts = 12'b0;

  wire          ch0_rframe_req;
  wire         ch0_rframe_req_ack;
  wire         ch0_rframe_data_en;
  wire [127:0] ch0_rframe_data;
  wire         ch0_rframe_data_valid;

  wire         o_dma_write_data_req;
  wire [ 11:0] o_dma_write_addr;
  wire [127:0] i_dma_write_data;

  wire         axi_dma_write_data_req;
  wire [127:0] axi_dma_write_data;

  wire [  7:0] mem0_out;
  wire [  7:0] mem1_out;
  wire [  7:0] mem2_out;
  wire [  7:0] mem3_out;
  wire [  7:0] mem4_out;
  wire [  7:0] mem5_out;
  wire [  7:0] mem6_out;
  wire hmi_speed_slider_present;
  wire hmi_speed_level,hmi_speed_target_toggle;
  wire [1:0] hmi_speed_target;
  wire [2:0] speed_phase_gray;
  wire speed_ready, speed_pressed, speed_drive;
  wire [3:0] hmi_select_sync;

  wire hmi_speed_manual = (hmi_select_sync == 4'hf) | hmi_speed_level;
  remote_speed_controller u_remote_speed_controller (
      .clk(free_clk), .rst_n(board_rst_n), .key_n(speed_key_n),
      .hmi_level_async(hmi_speed_manual), .sync_command_async(mem6_out),
      .target_toggle_async(hmi_speed_target_toggle), .target_gear_async(hmi_speed_target),
      .phase_gray(speed_phase_gray), .ready(speed_ready),
      .pressed(speed_pressed), .drive_active(speed_drive)
  );

  assign speed_key_n = speed_drive ? 1'b0 : 1'bz;
  wire hmi_left_denied, hmi_right_denied, traffic_go_denied;
  lane_permission_guard u_lane_permission_guard (
      .clk(free_clk), .rst_n(board_rst_n), .async_permission(mem5_out),
      .left_denied(hmi_left_denied), .right_denied(hmi_right_denied),
      .traffic_go_denied(traffic_go_denied)
  );

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

  wire hmi_rearm;
  hmi_control_synchronizer u_hmi_control_synchronizer (
      .clk          (free_clk),
      .rst_n        (board_rst_n),
      .async_code   (select_rearview),
      .code_sync    (hmi_select_sync),
      .rearm_command(hmi_rearm)
  );

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
      .hmi_left_denied     (hmi_left_denied),
      .hmi_right_denied    (hmi_right_denied),
      .go_released         (go_released),
      .back_released       (back_released),
      .left_released       (left_released),
      .right_released      (right_released)
  );

  assign go    = go_released | traffic_go_denied;
  assign back  = back_released;
  assign left  = left_released  ? 1'bz : 1'b0;
  assign right = right_released ? 1'bz : 1'b0;
  assign stop  = ~stop_pressed;

  wire [511:0] imu_snapshot;
  mpu9250_sampler imu_sampler(.clk(free_clk),.rst_n(board_rst_n),
    .spi_clk(spi_clk),.spi_cs(spi_cs),.spi_data(spi_data),
    .mpu_scl(mpu_scl),.mpu_sda(mpu_sda),.snapshot(imu_snapshot));

  fspi_slave_top_m_imu u_fspi_slave_top (
      .imu_snapshot(imu_snapshot),
      .sys_clk        (free_clk),
      .rst_n          (board_rst_n),
      .spi_clk        (spi_clk),
      .spi_cs         (spi_cs),
      .spi_data       (spi_data),
      .select_rearview(select_rearview),
      .auto_brake_enable(auto_brake_enabled),
      .speed_status_async({speed_ready,speed_pressed,speed_drive,speed_phase_gray}),
      .control_status_async({board_rst_n,speed_ready,fspi_direction_armed,traffic_go_denied,
          hmi_right_denied,hmi_left_denied,movement_blocked,auto_brake_enabled,
          hmi_speed_manual,speed_key_n,speed_drive,~stop_pressed,
          right_released,left_released,back_released,(go_released|traffic_go_denied)}),
      .mem0_out       (mem0_out),
      .mem1_out       (mem1_out),
      .mem2_out       (mem2_out),
      .mem3_out       (mem3_out),
      .mem4_out       (mem4_out),
      .mem5_out       (mem5_out),
      .mem6_out       (mem6_out)
  );

  wire true_50M;
  pllaaa the_instance_name (
      .clkout0(true_50M),
      .lock   (),
      .clkin1 (free_clk)
  );

  hmi_gear_feedback u_hmi_gear_feedback (
      .clk(true_50M), .rst_n(board_rst_n),
      .gear_async({speed_ready,speed_phase_gray}),
      .slider_present(hmi_speed_slider_present), .txd(hmi_status_txd)
  );
  uart_loopback u_uart_loopback (
      .sys_clk     (true_50M),
      .sys_rst_n   (board_rst_n),
      .uart_rxd    (RXD),
      .uart_txd    (),
      .uart_rx_done(uart_rx_valid),
      .uart_rx_data(uart_rx_data)
  );

  uart_lcd u_uart_lcd (
      .clk          (true_50M),
      .rst_n        (board_rst_n),
      .uart_rx      (uart_rx_data),
      .uart_rx_valid(uart_rx_valid),
      .mode         (mode),
      .cnn_level    (select_rearview),
      .auto_brake_enable(hmi_auto_brake_enable),
      .speed_slider_present(hmi_speed_slider_present),
      .speed_request_level(hmi_speed_level),
      .speed_target_gear(hmi_speed_target),
      .speed_target_toggle(hmi_speed_target_toggle)
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

  ov5640_top u_ov5640_top (
      .sys_clk      (free_clk),
      .cmos_h_pixel (H_CMOS_DISP),
      .cmos_v_pixel (V_CMOS_DISP),
      .total_h_pixel(TOTAL_H_PIXEL),
      .total_v_pixel(TOTAL_V_PIXEL),
      .y_addr_st    (Y_ADDR_ST),
      .y_addr_end   (Y_ADDR_END),
      .rate         (RATE),
      .clk_25M      (clk_25M),
      .clk_50M      (clk_50M),

      .cmos2_scl    (cmos2_scl),
      .cmos2_sda    (cmos2_sda),
      .cmos2_vsync  (cmos2_vsync),
      .cmos2_href   (cmos2_href),
      .cmos2_pclk   (cmos2_pclk),
      .cmos2_data   (cmos2_data),
      .cmos2_reset  (cmos2_reset),

      .cmos5_scl    (cmos5_scl),
      .cmos5_sda    (cmos5_sda),
      .cmos5_vsync  (cmos5_vsync),
      .cmos5_href   (cmos5_href),
      .cmos5_pclk   (cmos5_pclk),
      .cmos5_data   (cmos5_data),
      .cmos5_reset  (cmos5_reset),

      .cmos6_scl    (cmos6_scl),
      .cmos6_sda    (cmos6_sda),
      .cmos6_vsync  (cmos6_vsync),
      .cmos6_href   (cmos6_href),
      .cmos6_pclk   (cmos6_pclk),
      .cmos6_data   (cmos6_data),
      .cmos6_reset  (cmos6_reset),

      .video2_clk   (video2_clk),
      .video2_x     (video2_x),
      .video2_y     (video2_y),
      .video2_vs    (video2_vs),
      .video2_href  (video2_href),
      .cmos2_frame_valid    (video2_de),
      .cmos2_frame_data  (video2_data),

      .video5_clk (video5_clk),
      .video5_x   (video5_x),
      .video5_y   (video5_y),
      .video5_vs  (video5_vs),
      .video5_href(video5_href),
      .cmos5_frame_valid  (video5_de),
      .cmos5_frame_data(video5_data),

      .video6_clk (video6_clk),
      .video6_x   (video6_x),
      .video6_y   (video6_y),
      .video6_vs  (video6_vs),
      .video6_href(video6_href),
      .cmos6_frame_valid  (video6_de),
      .cmos6_frame_data(video6_data),

      .cmos2_pclk_count(cmos2_pclk_count),
      .cmos5_pclk_count(cmos5_pclk_count),
      .cmos6_pclk_count(cmos6_pclk_count)
  );

  wire        cnn_frame_vsync;
  wire        cnn_frame_de;
  wire [23:0] cnn_rgb_24bit;
  wire [15:0] cnn_rgb_16bit;
  wire [23:0] image_process_rgb_24bit;
  wire        image_process_vsync;
  wire        image_process_de;

  assign image_process_rgb_24bit = {
    video2_data[15:11], 3'b0, video2_data[10:5], 2'b0, video2_data[4:0], 3'b0
  };
  assign image_process_vsync = video2_vs;
  assign image_process_de = video2_de;

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
      .level(level)
  );

  assign cnn_rgb_16bit = {cnn_rgb_24bit[23:19], cnn_rgb_24bit[15:10], cnn_rgb_24bit[7:3]};

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

  ips2l_pcie_dma #(
      .DEVICE_TYPE   (DEVICE_TYPE),
      .AXIS_SLAVE_NUM   (AXIS_SLAVE_NUM)
  ) u_ips2l_pcie_dma (
      .clk  (pclk_div2),
      .rst_n(core_rst_n),

      .i_cfg_pbus_num   (cfg_pbus_num),
      .i_cfg_pbus_dev_num  (cfg_pbus_dev_num),
      .i_cfg_max_rd_req_size (cfg_max_rd_req_size),
      .i_cfg_max_payload_size (cfg_max_payload_size),

      .i_axis_master_tvld (axis_master_tvalid_mem),
      .o_axis_master_trdy (axis_master_tready_mem),
      .i_axis_master_tdata(axis_master_tdata_mem),
      .i_axis_master_tkeep(axis_master_tkeep_mem),

      .i_axis_master_tlast(axis_master_tlast_mem),
      .i_axis_master_tuser(axis_master_tuser_mem),

      .i_axis_slave0_trdy (axis_slave0_tready),
      .o_axis_slave0_tvld (dma_axis_slave0_tvalid),
      .o_axis_slave0_tdata(dma_axis_slave0_tdata),
      .o_axis_slave0_tlast(dma_axis_slave0_tlast),
      .o_axis_slave0_tuser(dma_axis_slave0_tuser),

      .i_axis_slave1_trdy (axis_slave1_tready),
      .o_axis_slave1_tvld (axis_slave1_tvalid),
      .o_axis_slave1_tdata(axis_slave1_tdata),
      .o_axis_slave1_tlast(axis_slave1_tlast),
      .o_axis_slave1_tuser(axis_slave1_tuser),

      .i_axis_slave2_trdy (axis_slave2_tready),
      .o_axis_slave2_tvld (axis_slave2_tvalid),
      .o_axis_slave2_tdata(axis_slave2_tdata),
      .o_axis_slave2_tlast(axis_slave2_tlast),
      .o_axis_slave2_tuser(axis_slave2_tuser),

      .i_cfg_ido_req_en(cfg_ido_req_en),
      .i_cfg_ido_cpl_en(cfg_ido_cpl_en),
      .i_xadm_ph_cdts  (xadm_ph_cdts),
      .i_xadm_pd_cdts  (xadm_pd_cdts),
      .i_xadm_nph_cdts (xadm_nph_cdts),
      .i_xadm_npd_cdts (xadm_npd_cdts),
      .i_xadm_cplh_cdts(xadm_cplh_cdts),
      .i_xadm_cpld_cdts(xadm_cpld_cdts),

      .i_apb_psel          (p_sel_dma),
      .i_apb_paddr         (p_addr[8:0]),
      .i_apb_pwdata        (p_wdata),
      .i_apb_pstrb         (p_strb),
      .i_apb_pwrite        (p_we),
      .i_apb_penable       (p_ce),
      .o_apb_prdy          (p_rdy_dma),
      .o_apb_prdata        (p_rdata_dma),
      .o_cross_4kb_boundary(cross_4kb_boundary),

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

  pcie_test u_ips2l_pcie_wrap (
      .button_rst_n  (1'b1),
      .power_up_rst_n(1'b1),
      .perst_n       (1'b1),

      .pclk      (pclk),
      .pclk_div2 (pclk_div2),
      .ref_clk   (ref_clk),
      .ref_clk_n (ref_clk_n),
      .ref_clk_p (ref_clk_p),
      .core_rst_n(core_rst_n),

      .p_sel  (p_sel_pcie),
      .p_strb (uart_p_strb),
      .p_addr (uart_p_addr),
      .p_wdata(uart_p_wdata),
      .p_ce   (uart_p_ce),
      .p_we   (uart_p_we),
      .p_rdy  (p_rdy_pcie),
      .p_rdata(p_rdata_pcie),

      .rxn              (rxn),
      .rxp              (rxp),
      .txn              (txn),
      .txp              (txp),
      .pcs_nearend_loop ({4{1'b0}}),
      .pma_nearend_ploop({4{1'b0}}),
      .pma_nearend_sloop({4{1'b0}}),

      .axis_master_tvalid(axis_master_tvalid),
      .axis_master_tready(axis_master_tready),
      .axis_master_tdata (axis_master_tdata),
      .axis_master_tkeep (axis_master_tkeep),

      .axis_master_tlast(axis_master_tlast),
      .axis_master_tuser(axis_master_tuser),

      .axis_slave0_tready(axis_slave0_tready),
      .axis_slave0_tvalid(axis_slave0_tvalid),
      .axis_slave0_tdata (axis_slave0_tdata),
      .axis_slave0_tlast (axis_slave0_tlast),
      .axis_slave0_tuser (axis_slave0_tuser),

      .axis_slave1_tready(axis_slave1_tready),
      .axis_slave1_tvalid(axis_slave1_tvalid),
      .axis_slave1_tdata (axis_slave1_tdata),
      .axis_slave1_tlast (axis_slave1_tlast),
      .axis_slave1_tuser (axis_slave1_tuser),

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

      .radm_cpl_timeout(),

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

      .smlh_link_up    (smlh_link_up),
      .rdlh_link_up    (rdlh_link_up),
      .smlh_ltssm_state(smlh_ltssm_state)
  );

  // DMA 使用两端共用的行填充逻辑，IMU 快照走独立 FSPI 返回路径。
  video_dma_line_formatter u_dma_line_formatter (
      .pclk_div2(pclk_div2),
      .ddr_init_done(ddr_init_done),
      .o_dma_write_addr(o_dma_write_addr),
      .o_dma_write_data_req(o_dma_write_data_req),
      .axi_dma_write_data(axi_dma_write_data),
      .ch0_rframe_req(ch0_rframe_req),
      .axi_dma_write_data_req(axi_dma_write_data_req),
      .i_dma_write_data(i_dma_write_data)
  );
endmodule
