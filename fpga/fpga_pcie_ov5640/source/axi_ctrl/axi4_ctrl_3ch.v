// 2026-11-03 注释校订。多路帧缓存控制。写通道轮询共享 AXI，位置计数在帧同步时优先清零。
module axi4_ctrl_3ch #(
    parameter C_ID_LEN      = 4,
    parameter C_DATA_LEN    = 128,
    parameter C_DATA_SIZE   = 3'b100,
    parameter C_BURST_LEN   = 10,
    parameter C_STRB_LEN    = C_DATA_LEN / 8,
    parameter C_ADDR_INC    = C_BURST_LEN * C_STRB_LEN,
    parameter C_BASE_ADDR   = 32'h00000000,
    parameter C_BUF_SIZE    = 22,
    parameter C_RD_END_ADDR = 640 * 480 * 2,
    parameter C_W_WIDTH     = 16,
    parameter C_R_WIDTH     = 128,

    parameter C_START_MASK  = 623
) (
    input wire axi_clk,
    input wire axi_reset,

    output      [C_ID_LEN-1:0] axi_awid,
    output      [        27:0] axi_awaddr,
    output      [         3:0] axi_awlen,
    output      [         2:0] axi_awsize,
    output      [         1:0] axi_awburst,
    output                     axi_awlock,
    output      [         3:0] axi_awcache,
    output      [         2:0] axi_awprot,
    output      [         3:0] axi_awqos,
    output reg                 axi_awvalid,
    input  wire                axi_awready,
    output      [C_DATA_LEN-1:0] axi_wdata,
    output      [C_STRB_LEN-1:0] axi_wstrb,
    output                       axi_wlast,
    output reg                   axi_wvalid,
    input  wire                  axi_wready,
    input  wire [C_ID_LEN-1:0] axi_bid,
    input  wire [         1:0] axi_bresp,
    input  wire                axi_bvalid,
    output reg                 axi_bready,
    output      [C_ID_LEN-1:0] axi_arid,
    output      [        31:0] axi_araddr,
    output      [         3:0] axi_arlen,
    output      [         2:0] axi_arsize,
    output      [         1:0] axi_arburst,
    output                     axi_arlock,
    output      [         3:0] axi_arcache,
    output      [         2:0] axi_arprot,
    output      [         3:0] axi_arqos,
    output reg                 axi_arvalid,
    input  wire                axi_arready,
    input  wire [  C_ID_LEN-1:0] axi_rid,
    input  wire [C_DATA_LEN-1:0] axi_rdata,
    input  wire [           1:0] axi_rresp,
    input  wire                  axi_rlast,
    input  wire                  axi_rvalid,
    output                       axi_rready,
    input wire [          2:0] wframe_pclk,
    input wire [          2:0] wframe_vsync,
    input wire [          2:0] wframe_data_en,
    input wire [C_W_WIDTH-1:0] wframe_data0,
    input wire [C_W_WIDTH-1:0] wframe_data1,
    input wire [C_W_WIDTH-1:0] wframe_data2,
    input  wire                 rframe_pclk,
    input  wire                 rframe_vsync,
    input  wire                 rframe_data_en,
    output reg                  rframe_data_valid,
    output wire [C_R_WIDTH-1:0] rframe_data,
    output [31:0] tp_o
);

  localparam FRAME_SIZE_BYTES = 640 * 480 * 2;
  localparam LINE_STRIDE = 640 * 2;

  assign axi_awid    = {C_ID_LEN{1'b0}};
  assign axi_awlen   = C_BURST_LEN - 1'b1;
  assign axi_awsize  = C_DATA_SIZE;
  assign axi_awburst = 2'b01;
  assign axi_awlock  = 1'b0;
  assign axi_awcache = 0;
  assign axi_awprot  = 3'b0;
  assign axi_awqos   = 4'b0;
  assign axi_wstrb   = {(C_STRB_LEN){1'b1}};
  assign axi_arid    = {C_ID_LEN{1'b0}};
  assign axi_arlen   = C_BURST_LEN - 1'b1;
  assign axi_arsize  = C_DATA_SIZE;
  assign axi_arburst = 2'b01;
  assign axi_arlock  = 1'b0;
  assign axi_arcache = 0;
  assign axi_arprot  = 3'b0;
  assign axi_arqos   = 4'b0;
  assign axi_rready  = 1;

  wire [3:0] vsync_pulse_axi;
  reg [2:0] vsync_sync_r[3:0];
  wire [3:0] raw_vsync_in;
  assign raw_vsync_in = {wframe_vsync[0], wframe_vsync[2], wframe_vsync[1], wframe_vsync[0]};

  genvar gi;
  generate
    for (gi = 0; gi < 4; gi = gi + 1) begin : vsync_cdc
      always @(posedge axi_clk or posedge axi_reset) begin
        if (axi_reset) vsync_sync_r[gi] <= 3'b0;
        else vsync_sync_r[gi] <= {vsync_sync_r[gi][1:0], raw_vsync_in[gi]};
      end
      assign vsync_pulse_axi[gi] = (vsync_sync_r[gi][2:1] == 2'b01);
    end
  endgenerate

  reg [4:0] fifo_wr_rst_cnt [2:0];
  reg [4:0] fifo_rd_rst_cnt [2:0];
  wire [2:0] fifo_wr_rst_out;
  wire [2:0] fifo_rd_rst_out;
  reg [1:0] r_wr_vsync_edge [2:0];

  generate
    for (gi = 0; gi < 3; gi = gi + 1) begin : rst_gen_loop
      always @(posedge wframe_pclk[gi] or posedge axi_reset) begin
        if (axi_reset) begin
            fifo_wr_rst_cnt[gi] <= 5'd31;
            r_wr_vsync_edge[gi] <= 2'b00;
        end else begin
            r_wr_vsync_edge[gi] <= {r_wr_vsync_edge[gi][0], wframe_vsync[gi]};
            if (r_wr_vsync_edge[gi] == 2'b01) fifo_wr_rst_cnt[gi] <= 5'd31;
            else if (fifo_wr_rst_cnt[gi] > 0) fifo_wr_rst_cnt[gi] <= fifo_wr_rst_cnt[gi] - 1;
        end
      end
      assign fifo_wr_rst_out[gi] = (fifo_wr_rst_cnt[gi] != 0);

      always @(posedge axi_clk or posedge axi_reset) begin
        if (axi_reset) fifo_rd_rst_cnt[gi] <= 5'd31;
        else begin
            if (vsync_pulse_axi[gi]) fifo_rd_rst_cnt[gi] <= 5'd31;
            else if (fifo_rd_rst_cnt[gi] > 0) fifo_rd_rst_cnt[gi] <= fifo_rd_rst_cnt[gi] - 1;
        end
      end
      assign fifo_rd_rst_out[gi] = (fifo_rd_rst_cnt[gi] != 0);
    end
  endgenerate

  reg [9:0] pix_h[2:0], pix_v[2:0];
  reg  [1:0] r_vsync_d[2:0];
  wire [2:0] wr_en_ds;

  wire [C_W_WIDTH-1:0] raw_data_in [2:0];
  assign raw_data_in[0] = wframe_data0;
  assign raw_data_in[1] = wframe_data1;
  assign raw_data_in[2] = wframe_data2;

  wire [C_W_WIDTH-1:0] final_fifo_data [2:0];

  generate
    for (gi = 0; gi < 3; gi = gi + 1) begin : pix_cnt_loop

      always @(posedge wframe_pclk[gi]) begin
        r_vsync_d[gi] <= {r_vsync_d[gi][0], wframe_vsync[gi]};
        if (r_vsync_d[gi] == 2'b10) begin
          pix_h[gi] <= 0;
          pix_v[gi] <= 0;
        end else if (wframe_data_en[gi]) begin
          if (pix_h[gi] == 639) begin
            pix_h[gi] <= 0;
            pix_v[gi] <= pix_v[gi] + 1;
          end else pix_h[gi] <= pix_h[gi] + 1;
        end
      end

      assign wr_en_ds[gi] = wframe_data_en[gi] && (pix_h[gi][0] == 0) && (pix_v[gi][0] == 0) && !fifo_wr_rst_out[gi];

      if (C_START_MASK > 0) begin : gen_shift

          reg [C_W_WIDTH-1:0] pipe_r [C_START_MASK-1:0];
          integer k;

          always @(posedge wframe_pclk[gi]) begin
              if (wframe_data_en[gi]) begin
                  pipe_r[0] <= raw_data_in[gi];
                  for (k = 1; k < C_START_MASK; k = k + 1) begin
                      pipe_r[k] <= pipe_r[k-1];
                  end
              end
          end

          wire [C_W_WIDTH-1:0] delayed_pixel = pipe_r[C_START_MASK-1];

          assign final_fifo_data[gi] = (pix_v[gi] == 0 && pix_h[gi] < C_START_MASK) ? {C_W_WIDTH{1'b0}} : delayed_pixel;

      end else begin : gen_no_shift
          assign final_fifo_data[gi] = raw_data_in[gi];
      end

    end
  endgenerate

  wire [3:0] w_fifo_empty, w_fifo_prog_empty;
  wire [3:0] w_fifo_ren;
  wire [C_DATA_LEN-1:0] w_fifo_rdata[3:0];

  // 第四个缓存仍使用相机 0 的写端，AXI 读端保留独立通道。
  genvar fifo_channel;
  generate
    for (fifo_channel = 0; fifo_channel < 4; fifo_channel = fifo_channel + 1) begin : camera_fifo
      localparam CAMERA = fifo_channel == 3 ? 0 : fifo_channel;
      W_FIFO_16i_128o u_fifo (
          .wr_clk(wframe_pclk[CAMERA]),
          .wr_rst(fifo_wr_rst_out[CAMERA]),
          .wr_en(wr_en_ds[CAMERA]),
          .wr_data(final_fifo_data[CAMERA]),
          .rd_clk(axi_clk),
          .rd_rst(fifo_rd_rst_out[CAMERA]),
          .rd_en(w_fifo_ren[fifo_channel]),
          .rd_data(w_fifo_rdata[fifo_channel]),
          .rd_empty(w_fifo_empty[fifo_channel]),
          .almost_empty(w_fifo_prog_empty[fifo_channel])
      );
    end
  endgenerate

  reg [ 1:0] wr_idx       [3:0];
  reg [63:0] abs_frame_cnt[3:0];
  reg [ 9:0] col_burst_cnt[3:0];
  reg [ 9:0] row_line_cnt [3:0];

  localparam S_IDLE = 0;
  localparam S_ADDR = 1;
  localparam S_DATA = 2;
  localparam S_RESP = 3;
  reg [2:0] ws_state;

  reg [1:0] cur_ch;
  reg [7:0] burst_cnt;

  wire [1:0] next_ch = (cur_ch == 3) ? 2'b0 : cur_ch + 1;
  wire ch_ready = !w_fifo_prog_empty[cur_ch] && !fifo_rd_rst_out[cur_ch != 3 ? cur_ch : 0];

  // 仅最后一个被接收的写拍推进列位置。同拍帧同步覆盖该更新。
  wire last_write_beat = (ws_state == S_DATA) && axi_wready &&
                         (burst_cnt == C_BURST_LEN - 1);
  genvar write_ch;
  generate
    for (write_ch = 0; write_ch < 4; write_ch = write_ch + 1) begin : write_position
      always @(posedge axi_clk or posedge axi_reset) begin
        if (axi_reset) begin
          wr_idx[write_ch] <= 0;
          abs_frame_cnt[write_ch] <= 0;
          col_burst_cnt[write_ch] <= 0;
          row_line_cnt[write_ch] <= 0;
        end else if (vsync_pulse_axi[write_ch]) begin
          wr_idx[write_ch] <= wr_idx[write_ch] + 1;
          abs_frame_cnt[write_ch] <= abs_frame_cnt[write_ch] + 1;
          col_burst_cnt[write_ch] <= 0;
          row_line_cnt[write_ch] <= 0;
        end else if (last_write_beat && cur_ch == write_ch && !vsync_pulse_axi[write_ch]) begin
          if (col_burst_cnt[write_ch] == 3) begin
            col_burst_cnt[write_ch] <= 0;
            row_line_cnt[write_ch] <= row_line_cnt[write_ch] + 1;
          end else begin
            col_burst_cnt[write_ch] <= col_burst_cnt[write_ch] + 1;
          end
        end
      end

      assign w_fifo_ren[write_ch] = (cur_ch == write_ch) && (ws_state == S_DATA) && axi_wready;
    end
  endgenerate

  always @(posedge axi_clk or posedge axi_reset) begin
    if (axi_reset) begin
      ws_state <= S_IDLE;
      cur_ch <= 0;
      axi_awvalid <= 0;
      axi_wvalid <= 0;
      axi_bready <= 0;
      burst_cnt <= 0;
    end else begin
      case (ws_state)
        S_IDLE: begin
          axi_bready <= 0;
          if (ch_ready) begin
            axi_awvalid <= 1;
            ws_state <= S_ADDR;
          end else begin
            cur_ch <= next_ch;
          end
        end

        S_ADDR: begin
          if (axi_awready) begin
            axi_awvalid <= 0;
            axi_wvalid <= 1;
            burst_cnt <= 0;
            ws_state <= S_DATA;
          end
        end

        S_DATA: begin
          if (axi_wready) begin
            burst_cnt <= burst_cnt + 1;
            if (burst_cnt == C_BURST_LEN - 1) begin
              axi_wvalid <= 0;
              axi_bready <= 1;
              ws_state   <= S_RESP;

            end
          end
        end

        S_RESP: begin
          if (axi_bvalid) begin
            axi_bready <= 0;
            cur_ch <= next_ch;
            ws_state <= S_IDLE;
          end
        end
        default: ws_state <= S_IDLE;
      endcase
    end
  end

  reg [31:0] addr_offset_ch, row_base_addr, col_offset_addr;
  always @(*) begin
    addr_offset_ch = wr_idx[cur_ch] * FRAME_SIZE_BYTES;
    if (cur_ch[1]) row_base_addr = (row_line_cnt[cur_ch] + 240) * LINE_STRIDE;
    else row_base_addr = row_line_cnt[cur_ch] * LINE_STRIDE;

    if (cur_ch[0]) col_offset_addr = (col_burst_cnt[cur_ch] * C_ADDR_INC) + 640;
    else col_offset_addr = (col_burst_cnt[cur_ch] * C_ADDR_INC);
  end
  assign axi_awaddr = C_BASE_ADDR + addr_offset_ch + row_base_addr + col_offset_addr;

  assign axi_wdata = w_fifo_rdata[cur_ch];
  assign axi_wlast = (burst_cnt == C_BURST_LEN - 1);

  reg [ 1:0] rc_rframe_index;
  reg [63:0] min_abs_cnt;
  always @(*) begin
    if (abs_frame_cnt[0] <= abs_frame_cnt[1] && abs_frame_cnt[0] <= abs_frame_cnt[2])
      min_abs_cnt = abs_frame_cnt[0];
    else if (abs_frame_cnt[1] <= abs_frame_cnt[0] && abs_frame_cnt[1] <= abs_frame_cnt[2])
      min_abs_cnt = abs_frame_cnt[1];
    else min_abs_cnt = abs_frame_cnt[2];
  end
  always @(posedge axi_clk or posedge axi_reset) begin
    if (axi_reset) rc_rframe_index <= 0;
    else if (min_abs_cnt >= 2) rc_rframe_index <= min_abs_cnt[1:0] - 2'd2;
    else rc_rframe_index <= 0;
  end

  reg  rframe_vsync_dly;
  wire rframe_vsync_neg;
  always @(posedge rframe_pclk) begin
    if (axi_reset) rframe_vsync_dly <= 0;
    else rframe_vsync_dly <= rframe_vsync;
  end
  assign rframe_vsync_neg = rframe_vsync_dly & ~rframe_vsync;

  reg [4:0] rfifo_cnt;
  reg rfifo_rst;
  always @(posedge rframe_pclk) begin
    if (axi_reset) rfifo_cnt <= 5'h1f;
    else if (rframe_vsync_neg) rfifo_cnt <= 5'h1f;
    else if (rfifo_cnt > 0) rfifo_cnt <= rfifo_cnt - 1;
  end
  always @(posedge rframe_pclk) begin
    if (axi_reset) rfifo_rst <= 1;
    else rfifo_rst <= (rfifo_cnt != 0);
  end
  wire w_rfifo_rst = axi_reset || rfifo_rst;
  reg rfifo_wenb;
  reg [C_DATA_LEN-1:0] rfifo_wdata;
  wire rfifo_wfull;

  R_FIFO_128i_128o u_R_FIFO (
      .wr_clk(axi_clk),
      .wr_rst(w_rfifo_rst),
      .wr_en(rfifo_wenb),
      .wr_data(rfifo_wdata),
      .almost_full(rfifo_wfull),
      .rd_clk(rframe_pclk),
      .rd_rst(w_rfifo_rst),
      .rd_en(rframe_data_en),
      .rd_data(rframe_data)
  );

  always @(posedge rframe_pclk) begin
    if (w_rfifo_rst) rframe_data_valid <= 0;
    else rframe_data_valid <= rframe_data_en;
  end

  reg [1:0] rd_state;
  reg [C_BUF_SIZE-1:0] araddr_offset;
  reg r_rd_pend;
  reg r_rfifo_rst_axi;
  always @(posedge axi_clk) r_rfifo_rst_axi <= w_rfifo_rst;
  reg [1:0] rfifo_rst_busy_sr;
  always @(posedge axi_clk) rfifo_rst_busy_sr <= {rfifo_rst_busy_sr[0], r_rfifo_rst_axi};

  always @(posedge axi_clk or posedge axi_reset) begin
    if (axi_reset) begin
      rd_state <= 0;
      axi_arvalid <= 0;
      r_rd_pend <= 0;
      araddr_offset <= 0;
    end else begin
      if (r_rfifo_rst_axi) begin
        rd_state <= 0;
        axi_arvalid <= 0;
        r_rd_pend <= 0;
        araddr_offset <= 0;
      end else begin
        if (axi_arready) axi_arvalid <= 0;
        if (axi_rvalid && axi_rlast) r_rd_pend <= 0;

        case (rd_state)
          0: begin
            if ((araddr_offset < C_RD_END_ADDR) && !rfifo_wfull) rd_state <= 1;
          end
          1: begin
            axi_arvalid <= 1;
            r_rd_pend <= 1;
            rd_state <= 2;
          end
          2: begin
            if (!axi_arvalid && !r_rd_pend) begin
              araddr_offset <= araddr_offset + C_ADDR_INC;
              rd_state <= 0;
            end
          end
        endcase
      end
    end
  end
  assign axi_araddr = C_BASE_ADDR + (rc_rframe_index * FRAME_SIZE_BYTES) + araddr_offset;
  always @(posedge axi_clk) begin
    rfifo_wenb  <= (axi_rvalid && axi_rready);
    rfifo_wdata <= axi_rdata;
  end

  assign tp_o = {ws_state, cur_ch, min_abs_cnt[3:0], wr_idx[0], wr_idx[1], wr_idx[2]};

endmodule