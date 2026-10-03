
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
    output wire       left,
    output wire       right,
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

  // stop脉冲生成器：1秒间隔的0101脉冲
  reg [25:0] stop_pulse_cnt;            // 计数器，用于1秒计时 (25MHz × 1s = 25,000,000)
  reg stop_pulse;                       // 脉冲信号
  reg [1:0] pulse_step;                 // 脉冲步骤：0=空闲, 1=第1个0, 2=第1个1, 3=第2个0
  reg mem4_out_d1;                      // mem4_out[0]的延迟一拍
  wire mem4_negedge;                    // mem4_out[0]的下降沿检测
  parameter STOP_PULSE_1S = 26'd25_000_000;  // 1秒计数值 (free_clk为25MHz)

  // 下降沿检测：从1变到0
  assign mem4_negedge = (mem4_out_d1 == 1'b1) && (mem4_out[0] == 1'b0);

  always @(posedge free_clk or negedge board_rst_n) begin
    if (!board_rst_n) begin
      mem4_out_d1 <= 1'b1;               // 初始化为1
    end else begin
      mem4_out_d1 <= mem4_out[0];        // 延迟一拍
    end
  end

  always @(posedge free_clk or negedge board_rst_n) begin
    if (!board_rst_n) begin
      stop_pulse_cnt <= 26'd0;
      stop_pulse <= 1'b1;                // 默认输出1
      pulse_step <= 2'd0;                // 空闲状态
    end else begin
      case (pulse_step)
        2'd0: begin  // 空闲状态，默认输出1，等待触发
          stop_pulse <= 1'b1;
          stop_pulse_cnt <= 26'd0;
          if (mem4_negedge) begin        // 检测到下降沿触发
            pulse_step <= 2'd1;          // 启动脉冲序列
            stop_pulse <= 1'b0;          // 输出第1个0
            stop_pulse_cnt <= 26'd0;
          end
          else
            pulse_step <= 2'd0;
        end

        2'd1: begin  // 第1个0 (持续1秒)
          if (stop_pulse_cnt >= STOP_PULSE_1S - 1) begin
            stop_pulse_cnt <= 26'd0;
            stop_pulse <= 1'b1;          // 切换到1
            pulse_step <= 2'd2;
          end else begin
            stop_pulse_cnt <= stop_pulse_cnt + 1'b1;
          end
        end

        2'd2: begin  // 第1个1 (持续1秒)
          if (stop_pulse_cnt >= STOP_PULSE_1S - 1) begin
            stop_pulse_cnt <= 26'd0;
            stop_pulse <= 1'b0;          // 切换到0
            pulse_step <= 2'd3;
          end else begin
            stop_pulse_cnt <= stop_pulse_cnt + 1'b1;
          end
        end

        2'd3: begin  // 第2个0 (持续1秒)
          if (stop_pulse_cnt >= STOP_PULSE_1S - 1) begin
            stop_pulse_cnt <= 26'd0;
            stop_pulse <= 1'b1;          // 切换到1
            pulse_step <= 2'd0;          // 完成，回到空闲状态
          end else begin
            stop_pulse_cnt <= stop_pulse_cnt + 1'b1;
          end
        end

        default: begin
          pulse_step <= 2'd0;
          stop_pulse <= 1'b1;
        end
      endcase
    end
  end

  assign go = (select_rearview == 4'ha) ? 1'b0 : mem0_out[0];                    // aa=前进启动(0), 否则停止(1)
  assign back = (select_rearview == 4'hb) ? 1'b0 : mem1_out[0];                  // bb=后退启动(0), 否则停止(1)
  assign left = (select_rearview == 4'hc) ? 1'b0 : mem2_out[0];           // cc=左转启动(0), 或由mem2_out[0]控制
  assign right = (select_rearview == 4'hd) ? 1'b0 : mem3_out[0];          // dd=右转启动(0), 或由mem3_out[0]控制
  assign stop = stop_pulse;                                                 // stop由脉冲生成器控制

  fspi_slave_top u_fspi_slave_top (
      .sys_clk        (free_clk),
      .rst_n          (board_rst_n),
      .spi_clk        (spi_clk),
      .spi_cs         (spi_cs),
      .spi_data       (spi_data),
      .select_rearview(select_rearview),             // 传递select_rearview信号
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
      .cnn_level    (select_rearview)
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
