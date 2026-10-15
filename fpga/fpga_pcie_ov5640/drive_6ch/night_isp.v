`timescale 1ns/1ps
// One pixel per clock, 11 registered stages (sample at N, output at N+10).
// VS/DE and the per-frame configuration travel with each pixel. No border crop.
// Preserve legacy 32-bit arithmetic, chroma shifts and final signedness.
module night_isp (
    input clk, rst_n, vs, de, input [15:0] rgb, input [127:0] cfg,
    output reg ovs, ode, output reg [15:0] out
);
    localparam PIPE_STAGES = 11;
    (* ASYNC_REG="TRUE" *) reg [127:0] cfg1, cfg2;
    reg [127:0] frame_cfg;
    reg oldvs;
    reg [9:0] vs_pipe, de_pipe;
    reg [127:0] config_pipe [0:9];
    reg [7:0] red [0:9], green [0:9], blue [0:9];
    reg [7:0] raw_red1, raw_blue1;
    reg [15:0] raw_luma1, luma7;
    integer cb_product2, cr_product2;
    integer cb_pipe [3:9], cr_pipe [3:9];
    reg [15:0] wb_red2;
    reg [16:0] wb_blue2;
    reg [15:0] curve_red4, curve_green4, curve_blue4;
    reg [23:0] curve_red5, curve_green5, curve_blue5;
    reg [20:0] gain_red4, gain_green4, gain_blue4;
    reg [20:0] gain_red5, gain_green5, gain_blue5;
    reg signed [8:0] diff_red8, diff_green8, diff_blue8;
    reg signed [17:0] saturation_red9, saturation_green9, saturation_blue9;
    reg [7:0] y8, y9, previous_y;
    reg mask9;
    integer k;

    function [7:0] sat;
        input integer value;
        begin
            if (value < 0) sat = 0;
            else if (value > 255) sat = 255;
            else sat = value[7:0];
        end
    endfunction

    // Bound multiplier operands before multiplication to avoid 32x32 DSP
    // cascades. Extend results explicitly where legacy signedness requires it.
    wire signed [8:0] raw_y = {1'b0, raw_luma1[15:8]};
    wire signed [8:0] lum = {1'b0, luma7[15:8]};
    wire signed [8:0] sg = {1'b0, config_pipe[8][47:40]};
    wire [12:0] gain = 13'd128 + {1'b0, config_pipe[3][39:32], 4'b0};
    wire [7:0] inv_red3 = 8'd255 - red[3];
    wire [7:0] inv_green3 = 8'd255 - green[3];
    wire [7:0] inv_blue3 = 8'd255 - blue[3];
    wire signed [8:0] raw_cb_diff = $signed({1'b0, raw_blue1}) - raw_y;
    wire signed [8:0] raw_cr_diff = $signed({1'b0, raw_red1}) - raw_y;
    wire signed [17:0] cb_mul = raw_cb_diff * 9'sd144;
    wire signed [17:0] cr_mul = raw_cr_diff * 9'sd183;
    wire signed [31:0] sat_red_ext = {{14{saturation_red9[17]}}, saturation_red9};
    wire signed [31:0] sat_green_ext = {{14{saturation_green9[17]}}, saturation_green9};
    wire signed [31:0] sat_blue_ext = {{14{saturation_blue9[17]}}, saturation_blue9};
    wire [7:0] edge8 = (y8 > previous_y) ? y8 - previous_y : previous_y - y8;
    wire [7:0] result_red = sat(y9 + (sat_red_ext >>> 7) + config_pipe[9][55:48] - 128);
    wire [7:0] result_green = sat(y9 + (sat_green_ext >>> 7) + config_pipe[9][55:48] - 128);
    wire [7:0] result_blue = sat(y9 + (sat_blue_ext >>> 7) + config_pipe[9][55:48] - 128);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cfg1 <= 0; cfg2 <= 0;
            frame_cfg <= 128'h00_f0_10_f0_10_01_40_80_00_80_80_02_80_00_01_00;
            oldvs <= 0; vs_pipe <= 0; de_pipe <= 0;
            ovs <= 0; ode <= 0; out <= 0;
            for (k = 0; k < 10; k = k + 1) begin
                config_pipe[k] <= 0;
                red[k] <= 0; green[k] <= 0; blue[k] <= 0;
            end
            for (k = 3; k < 10; k = k + 1) begin
                cb_pipe[k] <= 0; cr_pipe[k] <= 0;
            end
            raw_red1 <= 0; raw_blue1 <= 0; raw_luma1 <= 0; luma7 <= 0;
            cb_product2 <= 0; cr_product2 <= 0; wb_red2 <= 0; wb_blue2 <= 0;
            curve_red4 <= 0; curve_green4 <= 0; curve_blue4 <= 0;
            curve_red5 <= 0; curve_green5 <= 0; curve_blue5 <= 0;
            gain_red4 <= 0; gain_green4 <= 0; gain_blue4 <= 0;
            gain_red5 <= 0; gain_green5 <= 0; gain_blue5 <= 0;
            diff_red8 <= 0; diff_green8 <= 0; diff_blue8 <= 0;
            saturation_red9 <= 0; saturation_green9 <= 0; saturation_blue9 <= 0;
            y8 <= 0; y9 <= 0; previous_y <= 0; mask9 <= 0;
        end else begin
            cfg1 <= cfg; cfg2 <= cfg1; oldvs <= vs;
            if (vs && !oldvs) frame_cfg <= cfg2;
            vs_pipe <= {vs_pipe[8:0], vs};
            de_pipe <= {de_pipe[8:0], de};
            config_pipe[0] <= frame_cfg;
            red[0] <= {rgb[15:11], rgb[15:13]};
            green[0] <= {rgb[10:5], rgb[10:9]};
            blue[0] <= {rgb[4:0], rgb[4:2]};
            for (k = 1; k < 10; k = k + 1) begin
                config_pipe[k] <= config_pipe[k-1];
                red[k] <= red[k-1]; green[k] <= green[k-1]; blue[k] <= blue[k-1];
            end
            for (k = 4; k < 10; k = k + 1) begin
                cb_pipe[k] <= cb_pipe[k-1]; cr_pipe[k] <= cr_pipe[k-1];
            end

            // 1: Original luma for chroma, plus black-level subtraction.
            raw_red1 <= red[0]; raw_blue1 <= blue[0];
            raw_luma1 <= red[0]*16'd77 + green[0]*16'd150 + blue[0]*16'd29;
            if (config_pipe[0][15:8] != 0) begin
                red[1] <= sat({24'd0, red[0]} - config_pipe[0][23:16]);
                green[1] <= sat({24'd0, green[0]} - config_pipe[0][23:16]);
                blue[1] <= sat({24'd0, blue[0]} - config_pipe[0][23:16]);
            end

            // 2-3: White balance; chroma retains the legacy 32-bit format.
            cb_product2 <= cb_mul;
            cr_product2 <= cr_mul;
            wb_red2 <= red[1] * config_pipe[1][31:24];
            wb_blue2 <= blue[1] * (9'd256 - {1'b0, config_pipe[1][31:24]});
            cb_pipe[3] <= 128 + (cb_product2 >> 8);
            cr_pipe[3] <= 128 + (cr_product2 >> 8);
            if (config_pipe[2][15:8] != 0 && config_pipe[2][63:56] != 0) begin
                red[3] <= sat(wb_red2 >> 7); blue[3] <= sat(wb_blue2 >> 7);
            end

            // 4-6: Separate the two multiplications of the shadow curve.
            curve_red4 <= red[3] * inv_red3;
            curve_green4 <= green[3] * inv_green3;
            curve_blue4 <= blue[3] * inv_blue3;
            gain_red4 <= red[3] * gain;
            gain_green4 <= green[3] * gain;
            gain_blue4 <= blue[3] * gain;
            curve_red5 <= curve_red4 * config_pipe[4][39:32];
            curve_green5 <= curve_green4 * config_pipe[4][39:32];
            curve_blue5 <= curve_blue4 * config_pipe[4][39:32];
            gain_red5 <= gain_red4; gain_green5 <= gain_green4; gain_blue5 <= gain_blue4;
            if (config_pipe[5][15:8] == 2) begin
                if (config_pipe[5][87:80] == 1) begin
                    red[6] <= sat(red[5] + (curve_red5 >> 10));
                    green[6] <= sat(green[5] + (curve_green5 >> 10));
                    blue[6] <= sat(blue[5] + (curve_blue5 >> 10));
                end else begin
                    red[6] <= sat(gain_red5 >> 7);
                    green[6] <= sat(gain_green5 >> 7);
                    blue[6] <= sat(gain_blue5 >> 7);
                end
            end

            // 7-9: Luma, signed channel differences, saturation products.
            luma7 <= red[6]*16'd77 + green[6]*16'd150 + blue[6]*16'd29;
            y8 <= luma7[15:8];
            diff_red8 <= $signed({1'b0, red[7]}) - lum;
            diff_green8 <= $signed({1'b0, green[7]}) - lum;
            diff_blue8 <= $signed({1'b0, blue[7]}) - lum;
            saturation_red9 <= diff_red8 * sg;
            saturation_green9 <= diff_green8 * sg;
            saturation_blue9 <= diff_blue8 * sg;
            y9 <= y8;
            previous_y <= de_pipe[8] ? y8 : 8'd0;
            mask9 <= y8 >= config_pipe[8][71:64] && edge8 >= config_pipe[8][79:72] &&
                cb_pipe[8] >= config_pipe[8][95:88] && cb_pipe[8] <= config_pipe[8][103:96] &&
                cr_pipe[8] >= config_pipe[8][111:104] && cr_pipe[8] <= config_pipe[8][119:112];

            // 10: RGB, VS and DE have exactly the same output latency.
            ovs <= vs_pipe[9]; ode <= de_pipe[9];
            if (config_pipe[9][15:8] == 0)
                out <= {red[9][7:3], green[9][7:2], blue[9][7:3]};
            else if (config_pipe[9][15:8] == 3)
                out <= {16{mask9}};
            else
                out <= {result_red[7:3], result_green[7:2], result_blue[7:3]};
        end
    end
endmodule
