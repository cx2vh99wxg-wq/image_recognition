`timescale 1ns / 1ps

module test_remote_stop_controller;
  reg clk = 1'b0;
  always #20 clk = ~clk;  // free_clk: 25 MHz

  reg rst_n = 1'b0;
  reg brake_request = 1'b0;
  reg rearm_command = 1'b0;
  reg fspi_directions_idle = 1'b1;
  wire stop_pressed;
  wire movement_blocked;
  wire fspi_direction_armed;
  reg [3:0] hmi_raw_code = 4'h0;
  wire [3:0] hmi_code_sync;
  wire hmi_rearm_filtered;

  hmi_control_synchronizer hmi_cdc (
      .clk(clk),
      .rst_n(rst_n),
      .async_code(hmi_raw_code),
      .code_sync(hmi_code_sync),
      .rearm_command(hmi_rearm_filtered)
  );

  remote_stop_controller #(
      .STOP_PULSE_CYCLES(5),
      .STOP_GAP_CYCLES  (3)
  ) dut (
      .clk(clk),
      .rst_n(rst_n),
      .brake_request(brake_request),
      .rearm_command(rearm_command),
      .fspi_directions_idle(fspi_directions_idle),
      .stop_pressed(stop_pressed),
      .movement_blocked(movement_blocked),
      .fspi_direction_armed(fspi_direction_armed)
  );

  task step;
    begin
      @(posedge clk);
      #1;
    end
  endtask

  task check;
    input condition;
    input [8*100-1:0] description;
    begin
      if (condition !== 1'b1) begin
        $display("FAIL: %0s at t=%0t state=%0d", description, $time, dut.state);
        $finish;
      end
    end
  endtask

  integer i;
  initial begin
    step();
    @(negedge clk);
    rst_n = 1'b1;
    step();
    check(!stop_pressed && !movement_blocked && fspi_direction_armed,
          "released and direction-ready after reset");

    // Startup's STOP=0 and a detected person have the same one-shot action.
    @(negedge clk);
    brake_request = 1'b1;
    #1;
    check(movement_blocked && !stop_pressed, "direction blocks immediately");
    step();
    check(stop_pressed, "first brake press begins");
    for (i = 0; i < 4; i = i + 1) begin
      step();
      check(stop_pressed, "brake press duration");
    end
    step();
    check(!stop_pressed && movement_blocked, "brake press releases into lock");
    repeat (12) begin
      step();
      check(!stop_pressed && movement_blocked, "persistent alarm must not toggle STOP again");
    end

    // An E press while a person remains detected is ignored even after the
    // detection later clears; release and press E anew to rearm.
    @(negedge clk);
    rearm_command = 1'b1;
    step();
    check(!stop_pressed, "E ignored during active brake request");
    @(negedge clk);
    brake_request = 1'b0;
    repeat (3) step();
    check(!stop_pressed && movement_blocked, "old held E must not rearm");
    @(negedge clk);
    rearm_command = 1'b0;
    fspi_directions_idle = 1'b0; // stale FSPI direction remains asserted
    step();
    @(negedge clk);
    rearm_command = 1'b1;
    step();
    check(stop_pressed && movement_blocked && !fspi_direction_armed,
          "fresh E starts rearm pulse and discards stale direction");
    repeat (4) begin
      step();
      check(stop_pressed, "rearm press duration");
    end
    step();
    check(!stop_pressed && movement_blocked, "rearm release gap");
    repeat (3) step();
    check(!stop_pressed && movement_blocked, "held E blocks motion after gap");
    repeat (8) begin
      step();
      check(!stop_pressed, "held E must never retrigger STOP");
    end
    @(negedge clk);
    rearm_command = 1'b0;
    step();
    check(!movement_blocked && !fspi_direction_armed,
          "stale FSPI direction cannot move car after E release");
    @(negedge clk);
    fspi_directions_idle = 1'b1;
    step();
    check(fspi_direction_armed, "fresh idle FSPI state rearms its directions");

    // If detection recurs during an E pulse, finish that press, release for
    // the debounce gap, and issue exactly one new braking press.
    @(negedge clk);
    brake_request = 1'b1;
    step();
    check(stop_pressed, "second independent brake event");
    repeat (5) step();
    check(!stop_pressed && movement_blocked, "second event release gap");
    repeat (3) step();
    check(!stop_pressed && movement_blocked, "second event locked");
    @(negedge clk);
    brake_request = 1'b0;
    rearm_command = 1'b1;
    step();
    check(stop_pressed, "second E begins rearm");
    @(negedge clk);
    brake_request = 1'b1;
    step();
    @(negedge clk);
    brake_request = 1'b0;
    repeat (3) step();
    check(stop_pressed, "rearm pulse completes after transient detection");
    step();
    check(!stop_pressed && movement_blocked, "rearm release starts");
    repeat (2) begin
      step();
      check(!stop_pressed, "minimum release gap before renewed braking");
    end
    step();
    check(stop_pressed && movement_blocked, "transient person requires a new brake press");
    repeat (5) step();
    repeat (3) step();
    check(!stop_pressed && movement_blocked, "remain locked after renewed brake");

    // A multi-bit direction change may transiently pass through E after
    // synchronization; a single E sample must never become a rearm event.
    @(negedge clk);
    hmi_raw_code = 4'ha;
    repeat (4) step();
    @(negedge clk);
    hmi_raw_code = 4'he;
    step();
    @(negedge clk);
    hmi_raw_code = 4'hc;
    for (i = 0; i < 10; i = i + 1) begin
      step();
      check(!hmi_rearm_filtered, "A-to-C transient E rejected");
    end

    @(negedge clk);
    hmi_raw_code = 4'hb;
    repeat (4) step();
    @(negedge clk);
    hmi_raw_code = 4'he;
    step();
    @(negedge clk);
    hmi_raw_code = 4'hd;
    for (i = 0; i < 10; i = i + 1) begin
      step();
      check(!hmi_rearm_filtered, "B-to-D transient E rejected");
    end

    @(negedge clk);
    hmi_raw_code = 4'he;
    repeat (12) step();
    check(hmi_rearm_filtered, "held E accepted after stable sampling");
    @(negedge clk);
    hmi_raw_code = 4'h0;
    repeat (3) step();
    check(!hmi_rearm_filtered, "E release clears rearm command");

    $display("PASS: STOP one-shot, E rearm, stale-direction gate, detection race and HMI CDC");
    $finish;
  end
endmodule
