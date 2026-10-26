`timescale 1ns/1ps

// Exercise the real UART, HMI decoder, CDC, direction arbiter and STOP latch.
// GO/BACK/STOP have no testbench pull-ups: idle high is actively driven.
module test_hmi_serial_release;
  localparam integer UART_BIT_NS = 8680;
  localparam integer FRAME_GAP_CYCLES = 20_000;
  localparam integer TRACTION_LEASE_CYCLES = 100_000;

  reg clk_50m = 1'b0;
  reg clk_25m = 1'b0;
  always #10 clk_50m = ~clk_50m;
  always #20 clk_25m = ~clk_25m;

  reg rst_n = 1'b0;
  reg uart_rxd = 1'b1;
  wire uart_rx_valid;
  wire [7:0] uart_rx_data;
  wire [3:0] hmi_code, hmi_code_sync;
  wire rearm_command;
  wire stop_pressed, movement_blocked, fspi_direction_armed;
  reg brake_request = 1'b0;
  wire go_released, back_released, left_released, right_released;
  reg [3:0] fspi_released = 4'hf;
  wire go_pad = go_released;
  wire back_pad = back_released;
  wire stop_pad = ~stop_pressed;
  wire [3:0] direction_pad = {right_released, left_released, back_pad, go_pad};

  uart_rx #(.CLK_FREQ(50_000_000), .UART_BPS(115200)) u_uart_rx (
      .clk(clk_50m), .rst_n(rst_n), .uart_rxd(uart_rxd),
      .uart_rx_done(uart_rx_valid), .uart_rx_data(uart_rx_data)
  );
  // Preserve coverage of the optional lost-release lease. The board default
  // lease is disabled and is exercised separately in test_hmi_uart.v.
  uart_lcd #(
      .FRAME_GAP_CYCLES(FRAME_GAP_CYCLES),
      .TRACTION_LEASE_CYCLES(TRACTION_LEASE_CYCLES)
  ) u_uart_lcd (
      .clk(clk_50m), .rst_n(rst_n), .uart_rx(uart_rx_data),
      .uart_rx_valid(uart_rx_valid), .cnn_level(hmi_code)
  );
  hmi_control_synchronizer u_hmi_control_synchronizer (
      .clk(clk_25m), .rst_n(rst_n), .async_code(hmi_code),
      .code_sync(hmi_code_sync), .rearm_command(rearm_command)
  );
  remote_direction_arbiter u_remote_direction_arbiter (
      .clk(clk_25m), .rst_n(rst_n), .hmi_code(hmi_code_sync),
      .fspi_released(fspi_released), .fspi_direction_armed(fspi_direction_armed),
      .movement_blocked(movement_blocked), .go_released(go_released),
      .back_released(back_released), .left_released(left_released),
      .right_released(right_released)
  );
  remote_stop_controller u_stop (
      .clk(clk_25m), .rst_n(rst_n), .brake_request(brake_request),
      .rearm_command(rearm_command), .fspi_directions_idle(&fspi_released),
      .stop_pressed(stop_pressed), .movement_blocked(movement_blocked),
      .fspi_direction_armed(fspi_direction_armed)
  );

  task send_byte(input [7:0] value);
    integer bit_index;
    begin
      uart_rxd = 1'b0;
      #(UART_BIT_NS);
      for (bit_index = 0; bit_index < 8; bit_index = bit_index + 1) begin
        uart_rxd = value[bit_index];
        #(UART_BIT_NS);
      end
      uart_rxd = 1'b1;
      #(UART_BIT_NS);
    end
  endtask
  task send_direction(input [7:0] value);
    begin
      send_byte(8'h30);
      send_byte(8'h90);
      send_byte(8'h04);
      send_byte(value);
    end
  endtask
  task settle;
    begin
      repeat (16) @(posedge clk_25m);
      #1;
    end
  endtask
  task expect_contacts(input [3:0] expected_code,
                       input [3:0] expected_directions,
                       input expected_stop_pad,
                       input [8*120-1:0] label);
    begin
      settle();
      if (hmi_code !== expected_code || hmi_code_sync !== expected_code ||
          direction_pad !== expected_directions || stop_pad !== expected_stop_pad ||
          movement_blocked !== !expected_stop_pad)
        $fatal(1, "FAIL %0s: HMI=%h sync=%h DIRECTIONS=%h STOP=%b BLOCK=%b",
               label, hmi_code, hmi_code_sync, direction_pad, stop_pad, movement_blocked);
    end
  endtask

  initial begin
    #200 rst_n = 1'b1;
    #100;
    expect_contacts(4'h0, 4'hf, 1'b1, "reset releases every contact");

    send_direction(8'h0a);
    expect_contacts(4'ha, 4'he, 1'b1, "GO serial press drives GO low");
    send_byte(8'h30);
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "GO release after stray header");

    send_direction(8'h0b);
    expect_contacts(4'hb, 4'hd, 1'b1, "BACK serial press drives BACK low");
    send_byte(8'h30);
    send_byte(8'h90);
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "BACK release after partial prefix");

    send_direction(8'h0c);
    expect_contacts(4'hc, 4'hb, 1'b1, "LEFT remains momentary");
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "LEFT release");
    send_direction(8'h0d);
    expect_contacts(4'hd, 4'h7, 1'b1, "RIGHT remains momentary");
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "RIGHT release");

    send_direction(8'h0a);
    expect_contacts(4'ha, 4'he, 1'b1, "GO before incomplete other command");
    send_byte(8'h30);
    send_byte(8'h90);
    send_byte(8'h02);
    repeat (FRAME_GAP_CYCLES + 10) @(posedge clk_50m);
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "release after partial-command timeout");

    fspi_released = 4'hc;
    send_direction(8'h0a);
    expect_contacts(4'ha, 4'he, 1'b1, "GO before optional lost-release lease");
    repeat (TRACTION_LEASE_CYCLES + 10) @(posedge clk_50m);
    expect_contacts(4'h0, 4'hf, 1'b1, "GO lease releases and masks stale FSPI");
    fspi_released = 4'hf;
    settle();
    send_direction(8'h0b);
    expect_contacts(4'hb, 4'hd, 1'b1, "BACK before optional lost-release lease");
    repeat (TRACTION_LEASE_CYCLES + 10) @(posedge clk_50m);
    expect_contacts(4'h0, 4'hf, 1'b1, "BACK lease releases lost-release press");

    send_direction(8'h0e);
    expect_contacts(4'he, 4'hf, 1'b0, "manual E pulls STOP low in ready state");
    repeat (100) @(posedge clk_25m);
    expect_contacts(4'he, 4'hf, 1'b0, "held E has no pulse timeout");
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "manual E release actively drives STOP high");

    // An alarm interrupts a live GO command and remains latched after the
    // detection input clears. All HMI directions stay inactive while locked.
    send_direction(8'h0a);
    expect_contacts(4'ha, 4'he, 1'b1, "GO before automatic brake");
    @(negedge clk_25m);
    brake_request = 1'b1;
    expect_contacts(4'ha, 4'hf, 1'b0, "alarm releases GO and asserts STOP");
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b0, "normal HMI release cannot clear brake");
    @(negedge clk_25m);
    brake_request = 1'b0;
    expect_contacts(4'h0, 4'hf, 1'b0, "person disappearing cannot clear brake");
    send_direction(8'h0d);
    expect_contacts(4'hd, 4'hf, 1'b0, "RIGHT remains blocked by latched brake");
    send_direction(8'h0e);
    expect_contacts(4'he, 4'hf, 1'b0, "fresh E acknowledges latch and stays a manual STOP");
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "E release completes unlock");

    // Forced unlock consumes a currently high alarm until a clear/re-detect.
    @(negedge clk_25m);
    brake_request = 1'b1;
    expect_contacts(4'h0, 4'hf, 1'b0, "new alarm latches again");
    fspi_released = 4'he; // Old FSPI GO must not resume after forced unlock.
    send_direction(8'h0e);
    expect_contacts(4'he, 4'hf, 1'b0, "E acknowledges while alarm remains high");
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "forced unlock releases STOP and masks stale FSPI");
    if (fspi_direction_armed !== 1'b0)
      $fatal(1, "FAIL stale FSPI was armed after forced unlock");
    repeat (100) @(posedge clk_25m);
    expect_contacts(4'h0, 4'hf, 1'b1, "same high alarm never immediately relocks");
    send_direction(8'h0a);
    expect_contacts(4'ha, 4'he, 1'b1, "HMI GO available after forced unlock with person present");
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "GO release still masks stale FSPI after unlock");
    fspi_released = 4'hf;
    settle();
    if (fspi_direction_armed !== 1'b1)
      $fatal(1, "FAIL idle FSPI did not rearm after unlock");
    @(negedge clk_25m);
    brake_request = 1'b0;
    settle();
    @(negedge clk_25m);
    brake_request = 1'b1;
    expect_contacts(4'h0, 4'hf, 1'b0, "clear and re-detect brakes again");

    // A pre-existing E hold cannot acknowledge a later detection event.
    send_direction(8'h0e);
    expect_contacts(4'he, 4'hf, 1'b0, "E begins by acknowledging prior alarm");
    @(negedge clk_25m);
    brake_request = 1'b0;
    settle();
    @(negedge clk_25m);
    brake_request = 1'b1;
    settle();
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b0, "alarm during held E requires a fresh E press");
    send_direction(8'h0e);
    send_direction(8'h00);
    expect_contacts(4'h0, 4'hf, 1'b1, "fresh E recovers from alarm during held E");

    $display("PASS: serial GO/BACK/LEFT/RIGHT/STOP, persistent brake and forced REARM integration");
    $finish;
  end

  initial begin
    #40_000_000;
    $fatal(1, "FAIL serial release test timed out");
  end
endmodule
