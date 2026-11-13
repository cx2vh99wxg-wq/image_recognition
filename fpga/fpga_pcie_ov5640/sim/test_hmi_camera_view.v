`timescale 1ns / 1ps

module test_hmi_camera_view;
  localparam integer PREVIEW_CYCLES = 256;
  reg clk = 0;
  always #5 clk = ~clk;
  reg rst_n = 0;
  reg uart_rx_valid = 0;
  reg [7:0] uart_rx = 0;
  wire [3:0] hmi_code, view_code;

  uart_lcd decoder (
      .clk(clk), .rst_n(rst_n), .uart_rx(uart_rx),
      .uart_rx_valid(uart_rx_valid), .cnn_level(hmi_code)
  );
  hmi_camera_view #(.PREVIEW_CYCLES(PREVIEW_CYCLES)) preview (
      .clk(clk), .rst_n(rst_n), .hmi_code(hmi_code), .view_code(view_code)
  );

  reg spi_clk = 1, spi_cs = 1, host_drives = 0;
  reg [3:0] host_data = 0;
  tri [3:0] bus;
  assign bus = host_drives ? host_data : 4'bzzzz;
  fspi_transport #(.HAS_CAMERA_VIEW(1'b1)) transport (
      .spi_clk(spi_clk), .spi_cs(spi_cs), .spi_data(bus), .rst_n(rst_n),
      .select_rearview(hmi_code), .camera_view_async(view_code),
      .auto_brake_enable(1'b0), .speed_status_async(6'd0),
      .control_status_async(16'h1234), .tx_data(8'd0), .imu_snapshot(512'd0)
  );

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
      uart_rx_valid = 1;
      @(negedge clk);
      uart_rx_valid = 0;
    end
  endtask

  task send_key(input [7:0] value);
    begin
      send_byte(8'h30);
      send_byte(8'h90);
      send_byte(8'h04);
      send_byte(value);
      tick();
    end
  endtask

  task expect_view(input [3:0] expected);
    if (view_code !== expected)
      $fatal(1, "FAIL view %h, expected %h at %0t", view_code, expected, $time);
  endtask

  // Production RX-only FSPI read has a controller-generated 80 prefix.
  task spi_write_nibble(input [3:0] value);
    begin
      spi_clk = 0;
      host_data = value;
      #20.833 spi_clk = 1;
      #20.833;
    end
  endtask

  task spi_read_nibble(output [3:0] value);
    begin
      spi_clk = 0;
      #20.833 spi_clk = 1;
      #1 value = bus;
      #19.833;
    end
  endtask

  task expect_spi(input [7:0] expected_hmi, input [7:0] expected_view);
    reg [7:0] packet [0:8];
    reg [3:0] hi, lo;
    begin
      host_drives = 1;
      spi_cs = 0;
      #20;
      spi_write_nibble(4'h8);
      spi_write_nibble(4'h0);
      host_drives = 0;
      for (integer i = 0; i < 9; i = i + 1) begin
        spi_read_nibble(hi);
        spi_read_nibble(lo);
        packet[i] = {hi, lo};
      end
      spi_cs = 1;
      #20;
      if (packet[0] !== expected_hmi || packet[1] !== 8'ha7 ||
          packet[4] !== 8'hd7 || packet[5] !== 8'h34 ||
          packet[6] !== 8'h12 || packet[8] !== expected_view)
        $fatal(1, "FAIL FSPI raw %h, view %h, expected %h/%h",
               packet[0], packet[8], expected_hmi, expected_view);
    end
  endtask

  initial begin
    repeat (3) tick();
    @(negedge clk);
    rst_n = 1;
    expect_view(4'h9);
    send_key(8'h0c);
    for (integer i = 0; i < PREVIEW_CYCLES; i = i + 1) begin
      expect_view(4'h2);
      tick();
    end
    expect_view(4'h9);
    send_key(8'h0c);
    expect_view(4'h9); // Holding/repeating a press does not retrigger.

    send_key(8'h0d);
    expect_spi(8'hdd, 8'h33);
    send_key(8'h00);
    expect_spi(8'h00, 8'h33); // Releasing raw steering keeps the view.
    send_key(8'h0c);
    expect_spi(8'hcc, 8'h22); // Opposite direction starts its own timer.
    send_key(8'h09);
    expect_spi(8'h99, 8'h99); // Manual overview cancels immediately.

    send_key(8'h02);
    repeat (PREVIEW_CYCLES + 1) tick();
    expect_view(4'h2); // Existing manual m2 remains persistent.
    send_key(8'h0d);
    send_key(8'h0a); // GO remains independent of the timed preview.
    expect_spi(8'haa, 8'h33);
    repeat (PREVIEW_CYCLES) tick();
    expect_spi(8'haa, 8'h99);
    $display("PASS HMI timed camera view, unchanged raw keys, and S-only FSPI view byte");
    $finish;
  end
endmodule
