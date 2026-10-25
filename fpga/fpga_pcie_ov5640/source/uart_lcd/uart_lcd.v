
/* 解析串口屏送来的 0x30、0x90 帧头、功能编号和一个配置字节。
 * 寄存器在 uart_rx_valid 有效时更新，输出直接反映已保存的配置值。 */
module uart_lcd #(
    // Four bytes at 115200 baud take about 0.35 ms. Discard an incomplete
    // frame after 10 ms so a later touch release cannot complete old data.
    parameter integer FRAME_GAP_CYCLES = 500_000,
    // page0 sends one press and one release frame, without a held-key
    // heartbeat. Zero disables the optional lease so GO/BACK remain pressed
    // until release, just like LEFT/RIGHT. A nonzero value enables a timeout.
    parameter integer TRACTION_LEASE_CYCLES = 0
) (
    input             clk,
    input             rst_n,
    input      [7:0]  uart_rx,
    input             uart_rx_valid,
    output     [7:0]  mode,
    output     [7:0]  blc,
    output     [7:0]  awb,
    output     [7:0]  liangdu,
    output     [7:0]  baohedu,
    output            awb_en,
    output     [7:0]  binarization,
    output     [3:0]  cnn_level,
    output     [7:0]  sobel,
    output            isp_judge,
    output     [7:0]  cb_min,
    output     [7:0]  cb_max,
    output     [7:0]  cr_min,
    output     [7:0]  cr_max
);

    reg [7:0] mode_reg = 0;
    reg [7:0] blc_reg = 0;
    reg [7:0] awb_reg = 127;
    reg [7:0] liangdu_reg = 64;
    reg [7:0] baohedu_reg = 80;
    reg       awb_en_reg = 1'b1;
    reg [7:0] binarization_reg = 127;
    reg [3:0] cnn_level_reg = 0;
    reg [7:0] sobel_reg = 40;
    reg [7:0] cb_min_reg, cb_max_reg, cr_min_reg, cr_max_reg;
    reg [0:0] isp_judge_reg;

    /* 各输出端口与配置寄存器一一对应。 */
    assign mode = mode_reg;
    assign blc = blc_reg;
    assign awb = awb_reg;
    assign liangdu = liangdu_reg;
    assign baohedu = baohedu_reg;
    assign awb_en = awb_en_reg;
    assign binarization = binarization_reg;
    assign cnn_level = cnn_level_reg;
    assign sobel = sobel_reg;
    assign isp_judge = isp_judge_reg;
    assign cb_min = cb_min_reg;
    assign cb_max = cb_max_reg;
    assign cr_min = cr_min_reg;
    assign cr_max = cr_max_reg;

    // Match the complete 30 90 <function> <value> sequence using a sliding
    // window. A stray 30, 30 90, or partial command cannot consume the next
    // complete release frame as it did in the old state machine.
    reg [7:0] previous_3, previous_2, previous_1;
    reg [1:0] bytes_seen;
    reg [19:0] frame_gap_count;
    wire frame_complete = uart_rx_valid && (bytes_seen == 2'd3) &&
                          (frame_gap_count < FRAME_GAP_CYCLES) &&
                          (previous_3 == 8'h30) && (previous_2 == 8'h90);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            previous_3     <= 8'd0;
            previous_2     <= 8'd0;
            previous_1     <= 8'd0;
            bytes_seen     <= 2'd0;
            frame_gap_count <= 20'd0;
        end else if (uart_rx_valid) begin
            previous_3 <= previous_2;
            previous_2 <= previous_1;
            previous_1 <= uart_rx;
            bytes_seen <= (frame_gap_count >= FRAME_GAP_CYCLES) ? 2'd1 :
                          (bytes_seen == 2'd3) ? 2'd3 : bytes_seen + 2'd1;
            frame_gap_count <= 20'd0;
        end else if (frame_gap_count < FRAME_GAP_CYCLES) begin
            frame_gap_count <= frame_gap_count + 20'd1;
        end else begin
            bytes_seen <= 2'd0;
        end
    end

    wire traction_active = (cnn_level_reg == 4'ha) || (cnn_level_reg == 4'hb);
    reg [26:0] traction_lease_count;

    // Only complete frames update configuration. A received release 00
    // releases the remote contacts. The default has no GO/BACK-only timeout:
    // holding either key follows the same press/release protocol as steering.
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            mode_reg        <= 8'd8;
            blc_reg         <= 8'd0;
            awb_reg         <= 8'd127;
            liangdu_reg     <= 8'd64;
            baohedu_reg     <= 8'd80;
            awb_en_reg      <= 1'b1;
            binarization_reg <= 8'd127;
            cnn_level_reg   <= 4'd0;
            sobel_reg       <= 8'd40;
            isp_judge_reg   <= 1'b0;
            cb_min_reg      <= 8'd77;
            cb_max_reg      <= 8'd127;
            cr_min_reg      <= 8'd130;
            cr_max_reg      <= 8'd180;
            traction_lease_count <= 27'd0;
        end else begin
            if (frame_complete) begin
                case (previous_1)
                    8'h01: mode_reg         <= uart_rx;
                    8'h02: blc_reg          <= uart_rx;
                    8'h03: awb_reg          <= uart_rx;
                    8'h04: cnn_level_reg    <= uart_rx[3:0];
                    8'h05: baohedu_reg      <= uart_rx;
                    8'h06: liangdu_reg      <= uart_rx;
                    8'h07: awb_en_reg       <= uart_rx[0];
                    8'h08: binarization_reg <= uart_rx;
                    8'h09: sobel_reg        <= uart_rx;
                    8'h0a: isp_judge_reg    <= uart_rx[0];
                    8'h0b: cb_min_reg       <= uart_rx;
                    8'h0c: cb_max_reg       <= uart_rx;
                    8'h0d: cr_min_reg       <= uart_rx;
                    8'h0e: cr_max_reg       <= uart_rx;
                    default: ;
                endcase
            end

            if (frame_complete && (previous_1 == 8'h04)) begin
                traction_lease_count <= 27'd0;
            end else if ((TRACTION_LEASE_CYCLES > 0) && traction_active) begin
                if (traction_lease_count >= TRACTION_LEASE_CYCLES - 1) begin
                    cnn_level_reg <= 4'h0;
                    traction_lease_count <= 27'd0;
                end else begin
                    traction_lease_count <= traction_lease_count + 27'd1;
                end
            end else begin
                traction_lease_count <= 27'd0;
            end
        end
    end

endmodule
