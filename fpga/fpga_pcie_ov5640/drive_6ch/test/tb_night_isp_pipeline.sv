`timescale 1ns/1ps
module tb_night_isp_pipeline;
    reg clk = 0;
    always #8.9285 clk = ~clk; // 56 MHz target
    reg rst_n = 0, vs = 0, de = 0;
    reg [15:0] rgb = 0;
    reg [127:0] cfg = 0;
    wire ovs, ode, rvs, rde;
    wire [15:0] out, reference;
    night_isp dut(clk, rst_n, vs, de, rgb, cfg, ovs, ode, out);
    night_isp_reference golden(clk, rst_n, vs, de, rgb, cfg, rvs, rde, reference);
    reg [17:0] history [0:9];
    integer cycles = 0, checked = 0, valid_checked = 0, k;
    reg [31:0] random_state = 32'h2670cafe;

    always @(posedge clk) begin
        #1;
        if (!rst_n) begin
            cycles = 0;
            for (k = 0; k < 10; k = k + 1) history[k] = 0;
            if ({ovs, ode, out} !== 18'd0) $fatal(1, "reset did not clear outputs");
        end else begin
            if (cycles >= 10) begin
                if ({ovs, ode, out} !== history[9])
                    $fatal(1, "pixel/sync mismatch cycle=%0d got=%h expected=%h", cycles, {ovs,ode,out}, history[9]);
                checked = checked + 1;
                if (ode) valid_checked = valid_checked + 1;
            end
            for (k = 9; k > 0; k = k - 1) history[k] = history[k-1];
            history[0] = {rvs, rde, reference};
            cycles = cycles + 1;
        end
    end

    task pixel(input [15:0] value, input valid_pixel, input frame_sync);
        begin @(negedge clk); rgb = value; de = valid_pixel; vs = frame_sync; end
    endtask
    task new_frame(input [127:0] settings);
        begin
            @(negedge clk); cfg = settings; de = 0; vs = 0;
            repeat (5) @(negedge clk);
            vs = 1;
            @(negedge clk); vs = 0;
        end
    endtask
    integer mode, n, variant;
    reg [127:0] settings;
    initial begin
        // Exercise an actual reset edge, and later reset with data in flight.
        #2; rst_n = 1; #2; rst_n = 0;
        repeat (4) @(negedge clk);
        rst_n = 1;
        for (mode = 0; mode < 5; mode = mode + 1) begin
            settings = 128'h00_f0_10_f0_10_01_40_80_00_80_80_02_80_00_01_00;
            settings[15:8] = mode == 4 ? 2 : mode;
            settings[39:32] = 8;
            settings[63:56] = 1;
            settings[87:80] = mode == 4 ? 1 : 0;
            new_frame(settings);
            // Every RGB565 value, including black, white and channel boundaries.
            for (n = 0; n < 65536; n = n + 1) pixel(n[15:0], 1, 0);
            repeat (16) pixel(0, 0, 0);
        end
        for (variant = 0; variant < 40; variant = variant + 1) begin
            for (n = 0; n < 16; n = n + 1) begin
                random_state = {random_state[30:0], random_state[31]^random_state[21]^random_state[1]^random_state[0]};
                settings[n*8 +: 8] = random_state[7:0];
            end
            settings[15:8] = variant % 4;
            settings[87:80] = variant % 2;
            new_frame(settings);
            for (n = 0; n < 2000; n = n + 1) begin
                random_state = {random_state[30:0], random_state[31]^random_state[21]^random_state[1]^random_state[0]};
                pixel(random_state[15:0], n % 19 != 0, n % 307 == 0);
                // Mid-frame HMI changes must not affect earlier pipeline pixels.
                if (n == 991) cfg = ~settings;
            end
        end
        @(negedge clk); rst_n = 0;
        repeat (3) @(negedge clk);
        rst_n = 1;
        new_frame(128'd0);
        repeat (64) pixel(16'hf81f, 1, 0);
        repeat (20) pixel(0, 0, 0);
        $display("PASS: ISP equivalence checked=%0d valid_pixels=%0d; all RGB565 values, modes, bubbles, frame changes and reset", checked, valid_checked);
        $finish;
    end
    initial begin #20000000; $fatal(1, "ISP regression timed out"); end
endmodule
