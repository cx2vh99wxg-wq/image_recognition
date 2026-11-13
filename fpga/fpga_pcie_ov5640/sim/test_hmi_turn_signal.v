`timescale 1ns / 1ps

module test_hmi_turn_signal;
  localparam integer CLOCK_HZ = 16;
  reg clk = 1'b0;
  always #10 clk = ~clk;

  reg rst_n = 1'b0;
  reg [7:0] uart_rx = 8'h00;
  reg uart_rx_valid = 1'b0;
  wire [3:0] hmi_code;
  wire [1:0] lamps;

  uart_lcd u_uart_lcd (
      .clk(clk), .rst_n(rst_n), .uart_rx(uart_rx),
      .uart_rx_valid(uart_rx_valid), .cnn_level(hmi_code)
  );
  hmi_turn_signal #(.CLOCK_HZ(CLOCK_HZ)) dut (
      .clk(clk), .rst_n(rst_n), .hmi_code(hmi_code),
      .lamp_left(lamps[0]), .lamp_right(lamps[1])
  );

  task expect_lamps(input [1:0] expected);
    if (lamps !== expected)
      $fatal(1, "FAIL at %0t: lamps=%b, expected %b", $time, lamps, expected);
  endtask

  task tick;
    begin
      @(posedge clk);
      #1;
    end
  endtask

  task send_byte(input [7:0] value);
    begin
      @(negedge clk);
      uart_rx = value;
      uart_rx_valid = 1'b1;
      @(negedge clk);
      uart_rx_valid = 1'b0;
    end
  endtask

  task send_key(input [7:0] key);
    begin
      send_byte(8'h30);
      send_byte(8'h90);
      send_byte(8'h04);
      send_byte(key);
      tick();
    end
  endtask

  // Called on the first lit cycle. Check both pins on every subsequent cycle.
  task expect_five_flashes(input [1:0] side);
    integer elapsed;
    begin
      for (elapsed = 0; elapsed < 10 * CLOCK_HZ; elapsed = elapsed + 1) begin
        expect_lamps(((elapsed / CLOCK_HZ) % 2 == 0) ? side : 2'b00);
        tick();
      end
      repeat (2 * CLOCK_HZ) begin
        expect_lamps(2'b00);
        tick();
      end
    end
  endtask

  initial begin
    repeat (2) tick();
    @(negedge clk);
    rst_n = 1'b1;
    tick();
    expect_lamps(2'b00);

    // Non-turn commands and unrelated command types cannot start a sequence.
    send_key(8'h0a);
    expect_lamps(2'b00);
    send_byte(8'h30);
    send_byte(8'h90);
    send_byte(8'h02);
    send_byte(8'h0c);
    tick();
    expect_lamps(2'b00);

    send_key(8'h0c);
    fork
      expect_five_flashes(2'b01);
      send_key(8'h0c); // Duplicate during a held press must not reset the timer.
    join
    send_key(8'h0c);
    repeat (CLOCK_HZ) begin
      expect_lamps(2'b00);
      tick();
    end

    send_key(8'h0d);
    fork
      expect_five_flashes(2'b10);
      send_key(8'h00); // Release must not cut the sequence short.
    join

    send_key(8'h0c);
    expect_lamps(2'b01);
    send_key(8'h0d);
    expect_five_flashes(2'b10);

    send_key(8'h00);
    send_key(8'h0c);
    repeat (CLOCK_HZ) tick();
    expect_lamps(2'b00);
    send_key(8'h00);
    send_key(8'h0c); // A new press during OFF starts from ON immediately.
    expect_five_flashes(2'b01);

    send_key(8'h0d);
    expect_lamps(2'b10);
    rst_n = 1'b0;
    #1;
    expect_lamps(2'b00);
    $display("PASS HMI turn signals");
    $finish;
  end
endmodule
