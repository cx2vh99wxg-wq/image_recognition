// 2026-11-03 注释校订。用滑动字节窗识别屏幕报文。配置、挡位和牵引租期独立更新。
module uart_lcd #(

    parameter integer FRAME_GAP_CYCLES = 500_000,

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
    output     [7:0]  cr_max,
    output            auto_brake_enable,
    output reg        speed_slider_present,
    output reg        speed_request_level,
    output reg [1:0]  speed_target_gear,
    output reg        speed_target_toggle
);

    reg [7:0] mode_reg = 0;
    reg [7:0] blc_reg = 0;
    reg [7:0] awb_reg = 127;
    reg [7:0] liangdu_reg = 64;
    reg [7:0] baohedu_reg = 80;
    reg       awb_en_reg = 1'b1;
    reg [7:0] binarization_reg = 127;
    reg [3:0] cnn_level_reg = 0;

    reg       auto_brake_enable_reg = 1'b0;
    reg [7:0] sobel_reg = 40;
    reg [7:0] cb_min_reg, cb_max_reg, cr_min_reg, cr_max_reg;
    reg [0:0] isp_judge_reg;

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
    assign auto_brake_enable = auto_brake_enable_reg;

    reg [23:0] frame_history;
    wire [7:0] previous_3 = frame_history[23:16];
    wire [7:0] previous_2 = frame_history[15:8];
    wire [7:0] previous_1 = frame_history[7:0];
    reg [1:0] bytes_seen;
    reg [19:0] frame_gap_count;
    // 字节窗须同时满足帧头、长度与间隔条件，才更新对应命令寄存器。
    wire frame_complete = uart_rx_valid && (bytes_seen == 2'd3) &&
                          (frame_gap_count < FRAME_GAP_CYCLES) &&
                          (previous_3 == 8'h30) && (previous_2 == 8'h90);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            frame_history   <= 24'd0;
            bytes_seen     <= 2'd0;
            frame_gap_count <= 20'd0;
        end else if (uart_rx_valid) begin
            frame_history <= {frame_history[15:0], uart_rx};
            bytes_seen <= (frame_gap_count >= FRAME_GAP_CYCLES) ? 2'd1 :
                          (bytes_seen == 2'd3) ? 2'd3 : bytes_seen + 2'd1;
            frame_gap_count <= 20'd0;
        end else if (frame_gap_count < FRAME_GAP_CYCLES) begin
            frame_gap_count <= frame_gap_count + 20'd1;
        end else begin
            bytes_seen <= 2'd0;
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            mode_reg        <= 8'd8;
            blc_reg         <= 8'd0;
            awb_reg         <= 8'd127;
            liangdu_reg     <= 8'd64;
            baohedu_reg     <= 8'd80;
            awb_en_reg      <= 1'b1;
            binarization_reg <= 8'd127;
            auto_brake_enable_reg <= 1'b0;
            sobel_reg       <= 8'd40;
            isp_judge_reg   <= 1'b0;
            cb_min_reg      <= 8'd77;
            cb_max_reg      <= 8'd127;
            cr_min_reg      <= 8'd130;
            cr_max_reg      <= 8'd180;
        end else if (frame_complete) begin
            case (previous_1)
                8'h01: mode_reg         <= uart_rx;
                8'h02: blc_reg          <= uart_rx;
                8'h03: awb_reg          <= uart_rx;
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
                8'h0f: begin
                    if (uart_rx == 8'hff)
                        auto_brake_enable_reg <= 1'b1;
                    else if (uart_rx == 8'hee)
                        auto_brake_enable_reg <= 1'b0;
                end
                default: ;
            endcase
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            speed_slider_present <= 1'b0;
            speed_request_level <= 1'b0;
            speed_target_gear <= 2'd1;
            speed_target_toggle <= 1'b0;
        end else if (frame_complete) begin
            case (previous_1)
                8'h10: begin
                    if (uart_rx == 8'h01)
                        speed_request_level <= 1'b1;
                    else if (uart_rx == 8'h00)
                        speed_request_level <= 1'b0;
                end
                8'h11: begin

                    if (uart_rx == 0 || (uart_rx >= 1 && uart_rx <= 3))
                        speed_slider_present <= 1'b1;
                    else if (uart_rx == 8'hee)
                        speed_slider_present <= 1'b0;
                    if (uart_rx >= 1 && uart_rx <= 3) begin
                        speed_target_gear <= uart_rx[1:0];
                        speed_target_toggle <= ~speed_target_toggle;
                    end
                end
                default: ;
            endcase
        end
    end

    // 方向命令可在租期到期的同一拍刷新状态，命令更新优先。
    wire traction_active = (cnn_level_reg == 4'ha) || (cnn_level_reg == 4'hb);
    reg [26:0] traction_lease_count;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cnn_level_reg <= 4'd0;
            traction_lease_count <= 27'd0;
        end else if (frame_complete && previous_1 == 8'h04) begin
            cnn_level_reg <= uart_rx[3:0];
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

endmodule
