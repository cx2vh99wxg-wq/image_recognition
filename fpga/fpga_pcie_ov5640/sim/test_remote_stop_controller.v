`timescale 1ns / 1ps

module test_remote_stop_controller;
  reg clk = 1'b0;
  always #20 clk = ~clk;  // free_clk: 25 MHz

  reg rst_n = 1'b0;
  reg brake_request = 1'b0;
  reg rearm_command = 1'b0;
  reg fspi_directions_idle = 1'b1;
  wire stop_pressed, movement_blocked, fspi_direction_armed;
  reg [3:0] hmi_raw_code = 4'h0;
  wire [3:0] hmi_code_sync;
  wire hmi_rearm_filtered;
  wire cdc_stop_pressed, cdc_movement_blocked;

  hmi_control_synchronizer hmi_cdc (
      .clk(clk), .rst_n(rst_n), .async_code(hmi_raw_code),
      .code_sync(hmi_code_sync), .rearm_command(hmi_rearm_filtered)
  );

  remote_stop_controller dut (
      .clk(clk), .rst_n(rst_n), .brake_request(brake_request),
      .rearm_command(rearm_command), .fspi_directions_idle(fspi_directions_idle),
      .stop_pressed(stop_pressed), .movement_blocked(movement_blocked),
      .fspi_direction_armed(fspi_direction_armed)
  );

  // Also prove that a rejected CDC glitch cannot clear an actual brake latch.
  remote_stop_controller cdc_dut (
      .clk(clk), .rst_n(rst_n), .brake_request(brake_request),
      .rearm_command(hmi_rearm_filtered), .fspi_directions_idle(1'b1),
      .stop_pressed(cdc_stop_pressed), .movement_blocked(cdc_movement_blocked),
      .fspi_direction_armed()
  );

  task step;
    begin
      @(posedge clk);
      #1;
    end
  endtask

  task check(input condition, input [8*120-1:0] description);
    begin
      if (condition !== 1'b1)
        $fatal(1, "FAIL: %0s at t=%0t STOP=%b BLOCK=%b FSPI_ARMED=%b",
               description, $time, stop_pressed, movement_blocked, fspi_direction_armed);
    end
  endtask

  integer i;
  initial begin
    step();
    check(!stop_pressed && !movement_blocked && fspi_direction_armed,
          "reset releases STOP and directions");
    @(negedge clk);
    rst_n = 1'b1;
    step();

    // Manual E remains a momentary STOP when no alarm is latched.
    @(negedge clk);
    rearm_command = 1'b1;
    #1;
    check(stop_pressed && movement_blocked, "manual E presses STOP immediately");
    step();
    check(!fspi_direction_armed, "manual E disarms FSPI");
    repeat (12) begin
      step();
      check(stop_pressed && movement_blocked, "manual E stays active while held");
    end
    @(negedge clk);
    rearm_command = 1'b0;
    #1;
    check(!stop_pressed && !movement_blocked, "manual E release has no timed tail");
    step();
    check(fspi_direction_armed, "idle FSPI can rearm after manual E release");

    // STOP must stay active beyond the old 250 ms automatic pulse timer.
    @(negedge clk);
    brake_request = 1'b1;
    fspi_directions_idle = 1'b0;
    step();
    check(stop_pressed && movement_blocked && !fspi_direction_armed,
          "first alarm latches STOP and rejects stale directions");
    repeat (6_250_010) begin
      step();
      check(stop_pressed && movement_blocked, "automatic STOP holds beyond 250 ms");
    end
    @(negedge clk);
    brake_request = 1'b0;
    repeat (12) begin
      step();
      check(stop_pressed && movement_blocked, "alarm disappearing never unlocks STOP");
    end
    @(negedge clk);
    rearm_command = 1'b1;
    step();
    check(stop_pressed && movement_blocked, "fresh E clears latch but still presses manual STOP");
    @(negedge clk);
    rearm_command = 1'b0;
    #1;
    check(!stop_pressed && !movement_blocked, "E release releases automatically latched STOP");
    repeat (8) begin
      step();
      check(!fspi_direction_armed, "stale FSPI command remains disarmed after unlock");
    end
    @(negedge clk);
    fspi_directions_idle = 1'b1;
    step();
    check(fspi_direction_armed, "all-idle FSPI rearms after unlock");

    // The user chose forced unlock while the same person alarm remains high.
    @(negedge clk);
    brake_request = 1'b1;
    step();
    check(stop_pressed && movement_blocked, "new alarm locks again");
    @(negedge clk);
    rearm_command = 1'b1;
    step();
    @(negedge clk);
    rearm_command = 1'b0;
    #1;
    check(!stop_pressed && !movement_blocked, "forced unlock works with alarm still high");
    repeat (16) begin
      step();
      check(!stop_pressed && !movement_blocked, "same high alarm does not immediately relock");
    end
    @(negedge clk);
    brake_request = 1'b0;
    step();
    @(negedge clk);
    brake_request = 1'b1;
    step();
    check(stop_pressed && movement_blocked, "clear then re-detect creates another brake event");

    // Holding E before a new alarm cannot acknowledge that future event.
    @(negedge clk);
    rearm_command = 1'b1;
    step(); // Acknowledge the currently latched event first.
    @(negedge clk);
    brake_request = 1'b0;
    step();
    @(negedge clk);
    brake_request = 1'b1;
    step();
    check(stop_pressed && movement_blocked, "alarm arriving during held E latches");
    @(negedge clk);
    rearm_command = 1'b0;
    repeat (8) begin
      step();
      check(stop_pressed && movement_blocked, "old E release cannot unlock a later alarm");
    end
    @(negedge clk);
    rearm_command = 1'b1;
    step();
    @(negedge clk);
    rearm_command = 1'b0;
    step();
    check(!stop_pressed && !movement_blocked, "fresh E recovers from alarm during E hold");

    // A first E edge coincident with a new alarm cannot acknowledge a latch
    // that was not yet present when the operator pressed the button.
    @(negedge clk);
    brake_request = 1'b0;
    step();
    @(negedge clk);
    brake_request = 1'b1;
    rearm_command = 1'b1;
    step();
    @(negedge clk);
    rearm_command = 1'b0;
    step();
    check(stop_pressed && movement_blocked, "simultaneous first E and new alarm retain brake latch");

    // Reset explicitly clears the latch and restarts event detection.
    @(negedge clk);
    rst_n = 1'b0;
    #1;
    check(!stop_pressed && !movement_blocked, "reset clears latched brake");
    step();
    @(negedge clk);
    rst_n = 1'b1;
    step();
    check(stop_pressed && movement_blocked, "active request after reset is detected again");

    // Direction CDC can briefly pass through E; glitches must not unlock.
    @(negedge clk);
    hmi_raw_code = 4'ha;
    repeat (4) step();
    @(negedge clk);
    hmi_raw_code = 4'he;
    step();
    @(negedge clk);
    hmi_raw_code = 4'hc;
    for (i = 0; i < 12; i = i + 1) begin
      step();
      check(!hmi_rearm_filtered && cdc_stop_pressed && cdc_movement_blocked,
            "A-to-C transient E cannot unlock brake");
    end
    @(negedge clk);
    hmi_raw_code = 4'hb;
    repeat (4) step();
    @(negedge clk);
    hmi_raw_code = 4'he;
    step();
    @(negedge clk);
    hmi_raw_code = 4'hd;
    for (i = 0; i < 12; i = i + 1) begin
      step();
      check(!hmi_rearm_filtered && cdc_stop_pressed && cdc_movement_blocked,
            "B-to-D transient E cannot unlock brake");
    end
    @(negedge clk);
    hmi_raw_code = 4'he;
    repeat (12) step();
    check(hmi_rearm_filtered && cdc_stop_pressed, "stable E accepted while alarm remains high");
    @(negedge clk);
    hmi_raw_code = 4'h0;
    repeat (5) step();
    check(!hmi_rearm_filtered && !cdc_stop_pressed && !cdc_movement_blocked,
          "filtered E release completes forced unlock");

    $display("PASS: persistent automatic STOP, forced REARM, re-detection, stale FSPI and CDC");
    $finish;
  end

  initial begin
    #300_000_000;
    $fatal(1, "FAIL STOP controller test timed out");
  end
endmodule
