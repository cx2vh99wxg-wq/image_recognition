`timescale 1ns/1ps

// Exercise the same UART receiver, HMI decoder, clock-domain crossing, and
// direction arbiter used by the board. The weak pull-ups model the original
// remote's 3.3 V button inputs: a released FPGA pin reads high.
module test_hmi_serial_release;
  localparam integer UART_BIT_NS = 8680; // 50 MHz / 115200 = 434 clocks/bit
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
  wire [3:0] hmi_code;
  wire [3:0] hmi_code_sync;
  wire rearm_command;
  wire go_released, back_released, left_released, right_released;
  reg [3:0] fspi_released = 4'hf;
  tri1 go_pad;
  tri1 back_pad;

  uart_rx #(.CLK_FREQ(50_000_000), .UART_BPS(115200)) u_uart_rx (
      .clk(clk_50m), .rst_n(rst_n), .uart_rxd(uart_rxd),
      .uart_rx_done(uart_rx_valid), .uart_rx_data(uart_rx_data)
  );

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
      .fspi_released(fspi_released), .fspi_direction_armed(1'b1),
      .movement_blocked(1'b0), .go_released(go_released),
      .back_released(back_released), .left_released(left_released),
      .right_released(right_released)
  );

  assign go_pad = go_released ? 1'bz : 1'b0;
  assign back_pad = back_released ? 1'bz : 1'b0;

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
      repeat (8) @(posedge clk_25m);
      #1;
    end
  endtask

  task expect_contacts(input [3:0] expected_code,
                       input expected_go, input expected_back,
                       input [8*100-1:0] label);
    begin
      settle();
      if (hmi_code !== expected_code || hmi_code_sync !== expected_code ||
          go_pad !== expected_go || back_pad !== expected_back ||
          left_released !== 1'b1 || right_released !== 1'b1) begin
        $fatal(1, "FAIL %0s: HMI=%h sync=%h GO=%b BACK=%b LEFT_RELEASED=%b RIGHT_RELEASED=%b",
               label, hmi_code, hmi_code_sync, go_pad, back_pad,
               left_released, right_released);
      end
    end
  endtask

  initial begin
    #200 rst_n = 1'b1;
    #100;
    expect_contacts(4'h0, 1'b1, 1'b1, "reset releases both contacts");

    send_direction(8'h0a);
    expect_contacts(4'ha, 1'b0, 1'b1, "GO serial press sinks GO pad");
    // A stale single 30 must not consume the new complete release frame.
    send_byte(8'h30);
    send_direction(8'h00);
    expect_contacts(4'h0, 1'b1, 1'b1, "GO release after stray header");

    send_direction(8'h0b);
    expect_contacts(4'hb, 1'b1, 1'b0, "BACK serial press sinks BACK pad");
    // An incomplete 30 90 prefix previously made the release's 30 vanish.
    send_byte(8'h30);
    send_byte(8'h90);
    send_direction(8'h00);
    expect_contacts(4'h0, 1'b1, 1'b1, "BACK release after partial prefix");

    send_direction(8'h0a);
    expect_contacts(4'ha, 1'b0, 1'b1, "GO press before incomplete other command");
    send_byte(8'h30);
    send_byte(8'h90);
    send_byte(8'h02); // No data byte: discard this frame after a gap.
    repeat (FRAME_GAP_CYCLES + 10) @(posedge clk_50m);
    send_direction(8'h00);
    expect_contacts(4'h0, 1'b1, 1'b1, "release after timed-out partial command");

    // No release frame arrives. The traction lease must bound GO motion.
    // Stale FSPI GO/BACK lows must remain masked after the lease expires.
    fspi_released = 4'hc;
    send_direction(8'h0a);
    expect_contacts(4'ha, 1'b0, 1'b1, "GO press before lost release");
    repeat (TRACTION_LEASE_CYCLES + 10) @(posedge clk_50m);
    expect_contacts(4'h0, 1'b1, 1'b1, "GO lease releases lost-release press");

    fspi_released = 4'hf;
    settle();
    send_direction(8'h0b);
    expect_contacts(4'hb, 1'b1, 1'b0, "BACK press before lost release");
    repeat (TRACTION_LEASE_CYCLES + 10) @(posedge clk_50m);
    expect_contacts(4'h0, 1'b1, 1'b1, "BACK lease releases lost-release press");

    $display("PASS: serial GO/BACK releases after stray frames and missing release");
    $finish;
  end

  initial begin
    #20_000_000;
    $fatal(1, "FAIL serial release test timed out");
  end
endmodule
