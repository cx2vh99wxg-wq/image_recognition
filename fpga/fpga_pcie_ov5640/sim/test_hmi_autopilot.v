`timescale 1ns/1ps
module test_hmi_autopilot;
  reg clk = 0;
  always #5 clk = ~clk;
  reg reset_n = 0;
  reg [7:0] data = 0, heartbeat = 0;
  reg valid = 0, blocked = 0;
  reg [3:0] host_released = 4'hf;
  wire requested, active, brake;
  wire [1:0] sequence_code;
  wire [3:0] direction_code;
  wire go_n, back_n, left_n, right_n;
  uart_lcd uart_decoder (
    .clk(clk), .rst_n(reset_n), .uart_rx(data), .uart_rx_valid(valid),
    .auto_brake_enable(brake), .autopilot_requested(requested), .autopilot_sequence(sequence_code),
    .cnn_level(direction_code)
  );
  autopilot_lease #(.TIMEOUT_CYCLES(40), .REQUEST_CYCLES(80)) gate (
    .clk(clk), .rst_n(reset_n), .request_async({requested, sequence_code}),
    .heartbeat_async(heartbeat), .steering_released(&host_released[3:2]), .active(active)
  );
  remote_direction_arbiter directions (
    .clk(clk), .rst_n(reset_n), .hmi_code(direction_code),
    .fspi_released({host_released[3:2] | {2{!active}}, host_released[1:0]}),
    .fspi_direction_armed(1'b1), .movement_blocked(blocked),
    .hmi_left_denied(1'b0), .hmi_right_denied(1'b0),
    .go_released(go_n), .back_released(back_n), .left_released(left_n), .right_released(right_n)
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
    host_released = 4'b1010;
    #1;
    if (!left_n || !right_n || go_n) $fatal(1, "OFF masks automatic steering only");
    command(8'h04, 8'h0d);
    if (right_n || !left_n) $fatal(1, "manual right must still work while OFF");
    command(8'h04, 8'h00);
    command(8'h12, 8'hff);
    if (!requested || active || !brake) $fatal(1, "request alone must not arm");
    approve(1);
    if (active) $fatal(1, "previous contact must release before accepting");
    host_released = 4'hf;
    repeat (4) @(negedge clk);
    if (!active) $fatal(1, "approval should arm");
    host_released = 4'b1011;
    approve(2);
    if (!active || left_n) $fatal(1, "ON must forward valid automatic left");
    host_released = 4'b0111;
    #1;
    if (!active || right_n || !left_n) $fatal(1, "ON must forward valid automatic right");
    blocked = 1;
    approve(3);
    if (!active || !left_n || !right_n) $fatal(1, "STOP blocks pins without silently switching Autopilot off");
    blocked = 0;
    repeat (42) @(negedge clk);
    if (active || !right_n) $fatal(1, "expired heartbeat must release");
    host_released = 4'hf;
    approve(4);
    if (active) $fatal(1, "expired request must not rearm");
    command(8'h12, 8'hff);
    approve(5);
    if (!active) $fatal(1, "fresh approved request must rearm");
    host_released = 4'b1011;
    command(8'h12, 8'hee);
    if (active || requested || !brake || !left_n) $fatal(1, "OFF releases an active pulse");
    host_released = 4'hf;
    command(8'h12, 8'hff);
    repeat (83) @(negedge clk);
    approve(6);
    if (active) $fatal(1, "an unanswered ON request cannot arm after its window");
    command(8'h0f, 8'hee);
    if (brake) $fatal(1, "existing brake OFF command");
    $display("PASS: Autopilot OFF/ON, automatic left/right, manual steering, STOP priority and timeout");
    $finish;
  end
endmodule
