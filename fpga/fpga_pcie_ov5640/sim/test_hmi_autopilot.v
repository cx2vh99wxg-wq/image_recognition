`timescale 1ns/1ps
module test_hmi_autopilot;
  reg clk = 0;
  always #5 clk = ~clk;
  reg reset_n = 0;
  reg [7:0] data = 0, heartbeat = 0;
  reg valid = 0, released = 1, blocked = 0;
  wire requested, active, brake;
  wire [1:0] sequence_code;
  uart_lcd uart_decoder (
    .clk(clk), .rst_n(reset_n), .uart_rx(data), .uart_rx_valid(valid),
    .auto_brake_enable(brake), .autopilot_requested(requested), .autopilot_sequence(sequence_code)
  );
  autopilot_lease #(.TIMEOUT_CYCLES(40)) gate (
    .clk(clk), .rst_n(reset_n), .request_async({requested, sequence_code}),
    .heartbeat_async(heartbeat), .steering_released(released),
    .movement_blocked(blocked), .active(active)
  );
  task send_byte(input [7:0] value);
    begin
      @(negedge clk); data = value; valid = 1;
      @(negedge clk); valid = 0;
    end
  endtask
  task command(input [7:0] number, input [7:0] value);
    begin
      send_byte(8'h30); send_byte(8'h90); send_byte(number); send_byte(value);
      repeat (5) @(negedge clk);
    end
  endtask
  task approve(input [3:0] count);
    begin
      heartbeat = {2'b10, sequence_code, count};
      repeat (5) @(negedge clk);
    end
  endtask
  initial begin
    repeat (4) @(negedge clk);
    reset_n = 1;
    command(8'h0f, 8'hff);
    if (!brake || active || requested) $fatal(1, "brake independent, autopilot off");
    command(8'h12, 8'h01);
    if (requested || active) $fatal(1, "invalid payload");
    command(8'h12, 8'hff);
    if (!requested || active || !brake) $fatal(1, "request alone must not arm");
    released = 0;
    approve(1);
    if (active) $fatal(1, "previous contact must release before accepting");
    released = 1;
    repeat (4) @(negedge clk);
    if (!active) $fatal(1, "approval should arm");
    released = 0;
    approve(2);
    if (!active) $fatal(1, "renewal must allow a current pulse");
    repeat (42) @(negedge clk);
    if (active) $fatal(1, "expired heartbeat must release");
    released = 1;
    approve(3);
    if (active) $fatal(1, "expired request must not rearm");
    command(8'h12, 8'hff);
    approve(4);
    if (!active) $fatal(1, "fresh approved request must rearm");
    blocked = 1;
    repeat (4) @(negedge clk);
    if (active) $fatal(1, "STOP must release");
    blocked = 0;
    approve(5);
    if (active) $fatal(1, "STOP release must not rearm");
    command(8'h12, 8'hff);
    approve(6);
    if (!active) $fatal(1, "new request after STOP");
    command(8'h12, 8'hee);
    if (active || requested || !brake) $fatal(1, "explicit OFF and brake independence");
    command(8'h0f, 8'hee);
    if (brake) $fatal(1, "existing brake OFF command");
    $display("PASS: HMI autopilot request, approval, contact release, lease expiry and STOP");
    $finish;
  end
endmodule
