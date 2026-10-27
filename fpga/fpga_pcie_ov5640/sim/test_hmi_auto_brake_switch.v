`timescale 1ns/1ps

// Exercise the independent UART switch, both CDC hops, real quad-SPI status
// pins and the unchanged manual-direction / persistent STOP controllers.
module test_hmi_auto_brake_switch;
  reg uart_clk = 1'b0;
  reg pad_clk = 1'b0;
  always #10 uart_clk = ~uart_clk;
  always #20 pad_clk = ~pad_clk;
  reg rst_n = 1'b0;
  reg [7:0] uart_data = 8'd0;
  reg uart_valid = 1'b0;
  wire [7:0] mode;
  wire [3:0] hmi_code, hmi_code_sync;
  wire auto_enable, auto_enable_sync, gated_request;
  reg raw_brake_request = 1'b1; // Old software can initialize register 4 low.
  reg use_spi_brake_request = 1'b0;
  wire stop_pad = ~stop_pressed;
  wire rearm_command, stop_pressed, movement_blocked, fspi_direction_armed;
  wire go_released, back_released, left_released, right_released;
  wire [3:0] released = {right_released, left_released, back_released, go_released};

  uart_lcd #(.FRAME_GAP_CYCLES(32)) u_hmi (
      .clk(uart_clk), .rst_n(rst_n), .uart_rx(uart_data),
      .uart_rx_valid(uart_valid), .mode(mode), .cnn_level(hmi_code),
      .auto_brake_enable(auto_enable)
  );
  auto_brake_enable_gate u_enable_gate (
      .clk(pad_clk), .rst_n(rst_n), .async_enable(auto_enable),
      .brake_request_in(use_spi_brake_request ? ~fspi_button_sync[4] : raw_brake_request),
      .enable_sync(auto_enable_sync),
      .brake_request_out(gated_request)
  );
  hmi_control_synchronizer u_hmi_cdc (
      .clk(pad_clk), .rst_n(rst_n), .async_code(hmi_code),
      .code_sync(hmi_code_sync), .rearm_command(rearm_command)
  );
  remote_stop_controller u_stop (
      .clk(pad_clk), .rst_n(rst_n), .brake_request(gated_request),
      .rearm_command(rearm_command), .fspi_directions_idle(1'b1),
      .stop_pressed(stop_pressed), .movement_blocked(movement_blocked),
      .fspi_direction_armed(fspi_direction_armed)
  );
  remote_direction_arbiter u_directions (
      .clk(pad_clk), .rst_n(rst_n), .hmi_code(hmi_code_sync),
      .fspi_released(4'hf), .fspi_direction_armed(fspi_direction_armed),
      .hmi_left_denied(1'b0), .hmi_right_denied(1'b0), .movement_blocked(movement_blocked), .go_released(go_released),
      .back_released(back_released), .left_released(left_released),
      .right_released(right_released)
  );

  reg spi_clk = 1'b1;
  reg spi_cs = 1'b1;
  reg master_drive = 1'b1;
  reg [3:0] master_data = 4'hf;
  tri [3:0] spi_data;
  assign spi_data = master_drive ? master_data : 4'bzzzz;
  wire [1:0] legacy_cmd;
  wire legacy_rx_valid;
  wire [7:0] legacy_rx_data, register_read_data;
  wire legacy_tx_done;
  wire [7:0] mem0_out, mem1_out, mem2_out, mem3_out, mem4_out;
  reg register_clk = 1'b0;
  always #1.25 register_clk = ~register_clk;
  reg [4:0] fspi_button_meta;
  reg [4:0] fspi_button_sync;
  // Match the production register-to-pad CDC before the enable gate.
  always @(posedge pad_clk or negedge rst_n) begin
    if (!rst_n) begin
      fspi_button_meta <= 5'b11111;
      fspi_button_sync <= 5'b11111;
    end else begin
      fspi_button_meta <= {mem4_out[0], mem3_out[0], mem2_out[0], mem1_out[0], mem0_out[0]};
      fspi_button_sync <= fspi_button_meta;
    end
  end
  fspi_slave u_fspi (
      .spi_clk(spi_clk), .spi_cs(spi_cs), .spi_data(spi_data),
      .rst_n(rst_n), .select_rearview(hmi_code),
      .auto_brake_enable(auto_enable_sync), .cmd(legacy_cmd),
      .rx_data(legacy_rx_data), .rx_data_valid(legacy_rx_valid),
      .tx_data(register_read_data), .tx_data_valid(legacy_tx_done)
  );
  spi_ram_crtl u_spi_registers (
      .wr_clk(register_clk), .rst_n(rst_n), .spi_cs(spi_cs),
      .cmd(legacy_cmd), .tx_done(legacy_tx_done), .wr_data(legacy_rx_data),
      .rx_valid(legacy_rx_valid), .select_rearview(hmi_code),
      .rd_data(register_read_data), .mem0_out(mem0_out), .mem1_out(mem1_out),
      .mem2_out(mem2_out), .mem3_out(mem3_out), .mem4_out(mem4_out)
  );

  task settle;
    begin
      repeat (16) @(negedge pad_clk);
      #1;
    end
  endtask
  task uart_byte(input [7:0] value);
    begin
      @(negedge uart_clk);
      uart_data = value;
      uart_valid = 1'b1;
      @(negedge uart_clk);
      uart_valid = 1'b0;
    end
  endtask
  task uart_command(input [7:0] function_code, input [7:0] value);
    begin
      uart_byte(8'h30); uart_byte(8'h90);
      uart_byte(function_code); uart_byte(value);
      settle();
    end
  endtask
  task check(input condition, input [8*120-1:0] description);
    begin
      if (condition !== 1'b1)
        $fatal(1, "FAIL %0s: auto=%b sync=%b selector=%h directions=%h STOP=%b",
               description, auto_enable, auto_enable_sync, hmi_code, released, stop_pressed);
    end
  endtask
  task spi_nibble(input [3:0] value);
    begin
      master_data = value;
      #10 spi_clk = 1'b0;
      #10 spi_clk = 1'b1;
      #2;
    end
  endtask
  task spi_byte(input [7:0] value);
    begin
      spi_nibble(value[7:4]); spi_nibble(value[3:0]);
    end
  endtask
  task receive_nibble(output [3:0] value);
    begin
      #10 spi_clk = 1'b0;
      #1;
      #9 spi_clk = 1'b1;
      #1 value = spi_data;
      #1;
    end
  endtask
  task read_status(input [15:0] address, input [7:0] expected,
                   input change_switch_between_nibbles);
    reg [7:0] received;
    begin
      master_drive = 1'b1;
      spi_cs = 1'b0;
      spi_byte(8'h81); spi_byte(address[15:8]); spi_byte(address[7:0]);
      // The new command must not enter the existing RAM write protocol.
      check(legacy_cmd == 2'd0 && !legacy_rx_valid,
            "independent status command never becomes a legacy write");
      check(!u_fspi.status_tx_started, "slave releases bus throughout address phase");
      master_drive = 1'b0;
      receive_nibble(received[7:4]);
      if (change_switch_between_nibbles)
        uart_command(8'h0f, 8'hee);
      receive_nibble(received[3:0]);
      if (received !== expected)
        $fatal(1, "FAIL SPI81 address=%h received=%h expected=%h", address, received, expected);
      #10 spi_cs = 1'b1;
      #10 master_drive = 1'b1;
    end
  endtask
  task check_direction(input [7:0] code, input [3:0] expected, input [3:0] after_release);
    begin
      uart_command(8'h04, code);
      check(released == expected && !stop_pressed && !auto_enable,
            "legacy direction press remains independent while auto is disabled");
      uart_command(8'h04, 8'h00);
      check(released == after_release && !stop_pressed && !auto_enable,
            "traction persists while steering release remains momentary");
    end
  endtask
  task write_spi_register(input [15:0] address, input [7:0] value);
    begin
      master_drive = 1'b1;
      spi_cs = 1'b0;
      #20;
      spi_byte(8'h00); spi_byte(address[15:8]); spi_byte(address[7:0]);
      spi_byte(value); spi_byte(8'h00); // Production trailing commit byte.
      #20 spi_cs = 1'b1;
      #10;
      settle();
    end
  endtask

  reg [7:0] legacy_read;
  initial begin
    #25 rst_n = 1'b1;
    settle();
    check(!auto_enable && !auto_enable_sync && !gated_request && !stop_pressed,
          "reset defaults OFF and masks an already active software brake register");
    read_status(16'h0005, 8'ha0, 1'b0);

    uart_command(8'h04, 8'h0a);
    check(hmi_code == 4'ha && released == 4'he && !stop_pressed,
          "GO works with an active raw brake request while switch is OFF");
    uart_command(8'h0f, 8'h01);
    uart_command(8'h0f, 8'h00);
    check(!auto_enable && hmi_code == 4'ha && released == 4'he,
          "only FF and EE are accepted; invalid switch payloads never change GO");
    uart_command(8'h0f, 8'hff);
    check(auto_enable && gated_request && stop_pressed && released == 4'hf && hmi_code == 4'ha,
          "enabling with an already active brake request latches STOP without changing selector");
    read_status(16'h0005, 8'ha1, 1'b0);
    uart_command(8'h0f, 8'hee);
    check(!auto_enable && !gated_request && stop_pressed,
          "disabling masks new requests without clearing the existing STOP latch");
    read_status(16'h0005, 8'ha0, 1'b0);
    uart_command(8'h04, 8'h0e);
    check(stop_pressed && !auto_enable, "REARM keeps its original manual STOP hold");
    uart_command(8'h04, 8'h00);
    check(!stop_pressed && released == 4'hf && !auto_enable,
          "REARM release unlocks while switch remains OFF");

    check_direction(8'h0a, 4'he, 4'he);
    check_direction(8'h0b, 4'hd, 4'hd);
    check_direction(8'h0c, 4'h9, 4'hd);
    check_direction(8'h0d, 4'h5, 4'hd);
    uart_command(8'h04, 8'h0e);
    check(stop_pressed && released == 4'hf && !auto_enable,
          "manual E still brakes when automatic braking is disabled");
    uart_command(8'h04, 8'h00);
    check(!stop_pressed && !auto_enable, "manual E release remains unchanged");

    raw_brake_request = 1'b0;
    settle();
    uart_command(8'h04, 8'h0a);
    uart_command(8'h0f, 8'hff);
    check(auto_enable && !stop_pressed && released == 4'he && hmi_code == 4'ha,
          "enabling without a brake request leaves an existing GO unchanged");
    uart_command(8'h04, 8'h00);
    check(auto_enable && !stop_pressed && released == 4'he,
          "release retains GO without switching automatic braking OFF");
    raw_brake_request = 1'b1;
    settle();
    check(stop_pressed, "fresh request latches while enabled");
    uart_command(8'h04, 8'h0e);
    uart_command(8'h04, 8'h00);
    check(auto_enable && !stop_pressed && gated_request,
          "original forced REARM works with an enabled request still active");
    uart_command(8'h04, 8'h0b);
    check(auto_enable && !stop_pressed && released == 4'hd,
          "BACK remains available after the original forced unlock");
    uart_command(8'h04, 8'h00);

    raw_brake_request = 1'b0;
    settle();
    read_status(16'h0005, 8'ha1, 1'b1);
    check(!auto_enable && !stop_pressed, "mid-read switch change is accepted independently");
    read_status(16'h0005, 8'ha0, 1'b0);
    read_status(16'h0006, 8'hff, 1'b0);
    read_status(16'h0105, 8'hff, 1'b0);

    // Incomplete commands cannot consume the next complete switch frame.
    uart_byte(8'h30); uart_byte(8'h90);
    uart_command(8'h0f, 8'hff);
    check(auto_enable, "switch frame resynchronizes after an incomplete prefix");
    uart_byte(8'h30); uart_byte(8'h90); uart_byte(8'h0f);
    repeat (40) @(negedge uart_clk);
    uart_command(8'h0f, 8'hee);
    check(!auto_enable, "switch frame resynchronizes after a truncated-command timeout");

    // Existing 80 read still repeats the unchanged four-bit selector.
    uart_command(8'h04, 8'h0c);
    master_drive = 1'b1; spi_cs = 1'b0;
    spi_byte(8'h80);
    master_drive = 1'b0;
    receive_nibble(legacy_read[7:4]); receive_nibble(legacy_read[3:0]);
    check(legacy_read == 8'hcc, "legacy 80 mode read remains unchanged");
    #10 spi_cs = 1'b1;
    #10 master_drive = 1'b1;
    read_status(16'h0005, 8'ha0, 1'b0);

    // CS can abort a byte-aligned partial status header without affecting
    // the next transaction or the persistent switch state.
    spi_cs = 1'b0; spi_byte(8'h81); spi_byte(8'h00);
    #10 spi_cs = 1'b1;
    #10;
    read_status(16'h0005, 8'ha0, 1'b0);

    // Full joint path: UART enable -> same-CS status read -> actual 00 write
    // -> real register 4 -> register CDC -> enable gate -> latched STOP pad.
    // No injected request is used in this section.
    uart_command(8'h04, 8'h00);
    use_spi_brake_request = 1'b1;
    settle();
    check(mem4_out == 8'h01 && !gated_request && stop_pad == 1'b1,
          "real register chain starts released with automatic braking OFF");
    uart_command(8'h0f, 8'hff);
    read_status(16'h0005, 8'ha1, 1'b0);
    check(stop_pad == 1'b1, "ON status read alone never presses STOP");
    write_spi_register(16'h0004, 8'h00);
    check(mem4_out == 8'h00 && !fspi_button_sync[4] && gated_request &&
          stop_pad == 1'b0 && movement_blocked,
          "SPI00 register4 write0 reaches the physical active-low STOP output");
    read_status(16'h0005, 8'ha1, 1'b0);
    check(mem4_out == 8'h00 && stop_pad == 1'b0,
          "SPI81 status read does not alter the active STOP register");
    write_spi_register(16'h0004, 8'h01);
    check(mem4_out == 8'h01 && fspi_button_sync[4] && !gated_request && stop_pad == 1'b0,
          "real SPI register4 release does not clear the STOP latch");
    uart_command(8'h0f, 8'hee);
    read_status(16'h0005, 8'ha0, 1'b0);
    check(stop_pad == 1'b0 && !auto_enable,
          "UART EE and its OFF readback still cannot clear an existing STOP latch");
    uart_command(8'h04, 8'h0e);
    check(stop_pad == 1'b0, "manual E holds the STOP pad during acknowledgment");
    uart_command(8'h04, 8'h00);
    check(stop_pad == 1'b1 && !movement_blocked,
          "manual E release alone completes unlock after the full SPI braking path");

    // An OFF status transaction also must not poison the following legacy
    // write. The register changes, but the disabled gate keeps STOP released.
    read_status(16'h0005, 8'ha0, 1'b0);
    write_spi_register(16'h0004, 8'h00);
    check(mem4_out == 8'h00 && !fspi_button_sync[4] && !gated_request && stop_pad == 1'b1,
          "SPI81 OFF read followed by SPI00 write0 preserves both protocols");
    write_spi_register(16'h0004, 8'h01);
    check(mem4_out == 8'h01 && stop_pad == 1'b1 &&
          mem0_out == 8'h01 && mem1_out == 8'h01 && mem2_out == 8'h01 && mem3_out == 8'h01,
          "joint read/write operations preserve all four direction registers");
    @(negedge uart_clk); rst_n = 1'b0;
    #1;
    check(!auto_enable && !auto_enable_sync && !stop_pressed,
          "FPGA reset restores the automatic switch to OFF");
    $display("PASS: UART FF/EE + SPI81 read + SPI00 register4 write + CDC + latched STOP pad + REARM, with legacy controls");
    $finish;
  end
  initial begin
    #1_000_000;
    $fatal(1, "FAIL independent automatic-brake switch test timed out");
  end
endmodule
