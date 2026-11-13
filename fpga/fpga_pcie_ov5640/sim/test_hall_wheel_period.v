`timescale 1ns / 1ps
module test_hall_wheel_period;
    reg clk = 0;
    reg rst_n = 0;
    reg hall_in = 1;
    wire [31:0] report;
    always #5 clk = ~clk;

    hall_wheel_period #(.SAMPLE_DIV(4), .STABLE_SAMPLES(3), .STOP_TICKS(32)) dut (
        .clk(clk), .rst_n(rst_n), .hall_in(hall_in), .report(report)
    );

    task hold(input value, input integer ticks);
        begin
            hall_in = value;
            repeat(ticks*4) @(negedge clk);
        end
    endtask

    initial begin
        repeat(4) @(negedge clk);
        rst_n = 1;
        hold(1, 8);
        if (report[31:8] != 24'hb6ffff) $fatal(1, "initial high made a reading");
        hold(0, 8);
        hold(1, 2);
        hold(0, 6);
        if (report[23:8] != 16'hffff) $fatal(1, "glitch made a reading");
        hold(1, 8);
        hold(0, 8);
        hold(1, 8);
        if (report[31:8] != 24'hb60010) $fatal(1, "wrong interval: %h", report);
        hold(1, 30);
        if (report[31:8] != 24'hb60000) $fatal(1, "old interval survived timeout");
        hold(0, 8);
        hold(1, 8);
        if (report[23:8] != 0) $fatal(1, "first edge after stop used old timestamp");
        hold(0, 8);
        hold(1, 8);
        if (report[23:8] != 16) $fatal(1, "failed to resume measurement");
        $display("PASS Hall synchronization, glitch rejection, period, timeout, restart");
        $finish;
    end
endmodule
