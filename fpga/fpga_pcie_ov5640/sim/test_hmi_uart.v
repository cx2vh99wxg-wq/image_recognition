`timescale 1ns/1ps
module test_hmi_uart;
    reg clk = 0;
    always #10 clk = ~clk;  // 50 MHz UART parser clock
    reg rst_n = 0;
    reg [7:0] uart_rx = 0;
    reg uart_rx_valid = 0;
    wire [3:0] cnn_level;
    wire [7:0] mode;
    wire [7:0] blc, awb, liangdu, baohedu, binarization, sobel;
    wire [7:0] cb_min, cb_max, cr_min, cr_max;
    wire awb_en, isp_judge;

    uart_lcd #(.FRAME_GAP_CYCLES(24), .TRACTION_LEASE_CYCLES(64)) u_hmi (
        .clk(clk), .rst_n(rst_n), .uart_rx(uart_rx),
        .uart_rx_valid(uart_rx_valid), .cnn_level(cnn_level), .mode(mode),
        .blc(blc), .awb(awb), .liangdu(liangdu), .baohedu(baohedu),
        .awb_en(awb_en), .binarization(binarization), .sobel(sobel),
        .isp_judge(isp_judge), .cb_min(cb_min), .cb_max(cb_max),
        .cr_min(cr_min), .cr_max(cr_max)
    );

    task send_byte(input [7:0] value);
    begin
        @(negedge clk);
        uart_rx = value;
        uart_rx_valid = 1;
        @(negedge clk);
        uart_rx_valid = 0;
    end
    endtask

    task send_config(input [7:0] function_code, input [7:0] value);
    begin
        send_byte(8'h30);
        send_byte(8'h90);
        send_byte(function_code);
        send_byte(value);
        @(negedge clk);
    end
    endtask

    task send_direction(input [7:0] value);
    begin
        send_config(8'h04, value);
    end
    endtask

    task expect_direction(input [3:0] expected);
    begin
        if (cnn_level !== expected) begin
            $display("FAIL HMI selector actual=%h expected=%h", cnn_level, expected);
            $finish;
        end
    end
    endtask

    initial begin
        #25 rst_n = 1;
        @(negedge clk);
        expect_direction(4'h0);
        send_direction(8'h0a);
        expect_direction(4'ha);
        send_direction(8'h0b);
        expect_direction(4'hb);
        send_direction(8'h0c);
        expect_direction(4'hc);
        send_direction(8'h0d);
        expect_direction(4'hd);
        send_direction(8'h0e);
        expect_direction(4'he);
        send_direction(8'h00);
        expect_direction(4'h0);

        // A truncated frame header must not consume the next complete
        // release frame. The old parser stayed at A/B forever in this case.
        send_direction(8'h0a);
        send_byte(8'h30);
        send_direction(8'h00);
        expect_direction(4'h0);

        send_direction(8'h0b);
        send_byte(8'h30);
        send_byte(8'h90);
        send_direction(8'h00);
        expect_direction(4'h0);

        // A missing data byte also must not eat a subsequent release frame.
        send_direction(8'h0a);
        send_byte(8'h30);
        send_byte(8'h90);
        send_byte(8'h01);
        send_direction(8'h00);
        expect_direction(4'h0);

        // Payload value 30 is legal for unrelated settings. The next
        // complete direction frame must still be parsed correctly.
        send_byte(8'h30);
        send_byte(8'h90);
        send_byte(8'h01);
        send_byte(8'h30);
        if (mode !== 8'h30) begin
            $display("FAIL mode payload 30 was not preserved");
            $finish;
        end
        send_direction(8'h0b);
        expect_direction(4'hb);
        send_direction(8'h00);
        expect_direction(4'h0);

        // A completely missing release cannot leave traction enabled
        // forever. Repeating a complete press frame renews the lease.
        send_direction(8'h0a);
        repeat (40) @(negedge clk);
        expect_direction(4'ha);
        send_direction(8'h0a);
        repeat (40) @(negedge clk);
        expect_direction(4'ha);
        repeat (70) @(negedge clk);
        expect_direction(4'h0);

        // The framing repair must preserve all camera-setting function IDs.
        send_config(8'h01, 8'h09);
        send_config(8'h02, 8'h21);
        send_config(8'h03, 8'h22);
        send_config(8'h05, 8'h23);
        send_config(8'h06, 8'h24);
        send_config(8'h07, 8'h01);
        send_config(8'h08, 8'h25);
        send_config(8'h09, 8'h26);
        send_config(8'h0a, 8'h01);
        send_config(8'h0b, 8'h27);
        send_config(8'h0c, 8'h28);
        send_config(8'h0d, 8'h29);
        send_config(8'h0e, 8'h2a);
        if (mode !== 8'h09 || blc !== 8'h21 || awb !== 8'h22 ||
            baohedu !== 8'h23 || liangdu !== 8'h24 || awb_en !== 1'b1 ||
            binarization !== 8'h25 || sobel !== 8'h26 || isp_judge !== 1'b1 ||
            cb_min !== 8'h27 || cb_max !== 8'h28 || cr_min !== 8'h29 ||
            cr_max !== 8'h2a) begin
            $display("FAIL camera configuration function mapping changed");
            $finish;
        end
        $display("PASS: HMI frame 30 90 04 xx decodes A-E and release");
        $finish;
    end
endmodule
