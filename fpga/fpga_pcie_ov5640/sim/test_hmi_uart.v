`timescale 1ns/1ps
module test_hmi_uart;
    reg clk = 0;
    always #10 clk = ~clk;  // 50 MHz UART parser clock
    reg rst_n = 0;
    reg [7:0] uart_rx = 0;
    reg uart_rx_valid = 0;
    wire [3:0] cnn_level;

    uart_lcd u_hmi (
        .clk(clk), .rst_n(rst_n), .uart_rx(uart_rx),
        .uart_rx_valid(uart_rx_valid), .cnn_level(cnn_level)
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

    task send_direction(input [7:0] value);
    begin
        send_byte(8'h30);
        send_byte(8'h90);
        send_byte(8'h04);
        send_byte(value);
        @(negedge clk);
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
        $display("PASS: HMI frame 30 90 04 xx decodes A-E and release");
        $finish;
    end
endmodule
