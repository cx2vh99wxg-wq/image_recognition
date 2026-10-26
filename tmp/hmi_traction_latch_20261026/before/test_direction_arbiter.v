`timescale 1ns/1ps

module test_direction_arbiter;
  reg clk = 1'b0;
  always #20 clk = ~clk;

  reg rst_n = 1'b0;
  reg [3:0] hmi_code = 4'h0;
  reg [3:0] fspi_released = 4'hf;
  reg fspi_direction_armed = 1'b1;
  reg movement_blocked = 1'b0;
  wire go_released, back_released, left_released, right_released;
  wire [3:0] released = {right_released, left_released, back_released, go_released};

  remote_direction_arbiter dut (
      .clk(clk),
      .rst_n(rst_n),
      .hmi_code(hmi_code),
      .fspi_released(fspi_released),
      .fspi_direction_armed(fspi_direction_armed),
      .movement_blocked(movement_blocked),
      .go_released(go_released),
      .back_released(back_released),
      .left_released(left_released),
      .right_released(right_released)
  );

  task step;
    begin
      @(posedge clk);
      #1;
    end
  endtask

  task expect_released;
    input [3:0] expected;
    input [8*80-1:0] label;
    begin
      if (released !== expected) begin
        $fatal(1, "FAIL %0s: released=%b expected=%b at %0t", label, released, expected, $time);
      end
    end
  endtask

  initial begin
    step();
    @(negedge clk);
    rst_n = 1'b1;
    step();
    expect_released(4'hf, "all contacts released after reset");

    // FSPI keeps working until the HMI takes over.
    @(negedge clk);
    fspi_released = 4'he;
    #1;
    expect_released(4'he, "fresh FSPI GO press");

    // Simulate stale GO and BACK FSPI writes. HMI release 0 must not reveal
    // those old lows, even though the HMI decoder itself has released.
    @(negedge clk);
    fspi_released = 4'hc;
    hmi_code = 4'ha;
    step();
    expect_released(4'he, "HMI GO overrides both FSPI directions");
    @(negedge clk);
    hmi_code = 4'h0;
    #1;
    expect_released(4'hf, "HMI GO release opens all contacts immediately");
    repeat (3) step();
    expect_released(4'hf, "stale FSPI GO and BACK remain suppressed");

    @(negedge clk);
    hmi_code = 4'hb;
    step();
    expect_released(4'hd, "HMI BACK press");
    @(negedge clk);
    hmi_code = 4'h0;
    #1;
    expect_released(4'hf, "HMI BACK release opens all contacts");

    // LEFT and RIGHT use the same handoff rule.
    @(negedge clk);
    hmi_code = 4'hc;
    step();
    expect_released(4'hb, "HMI LEFT press");
    @(negedge clk);
    hmi_code = 4'h0;
    #1;
    expect_released(4'hf, "HMI LEFT release");
    @(negedge clk);
    hmi_code = 4'hd;
    step();
    expect_released(4'h7, "HMI RIGHT press");
    @(negedge clk);
    hmi_code = 4'h0;
    #1;
    expect_released(4'hf, "HMI RIGHT release");

    // Only an all-idle FSPI state hands authority back. Its next press works.
    @(negedge clk);
    fspi_released = 4'hf;
    step();
    expect_released(4'hf, "FSPI idle clears stale-command hold");
    @(negedge clk);
    fspi_released = 4'hb;
    #1;
    expect_released(4'hb, "new FSPI LEFT press after handoff");

    // Lane steering and manual traction must coexist on independent axes.
    @(negedge clk);
    hmi_code = 4'ha;
    step();
    expect_released(4'ha, "HMI GO plus FSPI LEFT drives forward-left");
    @(negedge clk);
    fspi_released = 4'h7;
    step();
    expect_released(4'h6, "lane changes LEFT to RIGHT without releasing HMI GO");
    @(negedge clk);
    hmi_code = 4'hb;
    step();
    expect_released(4'h5, "HMI BACK plus FSPI RIGHT drives reverse-right");
    @(negedge clk);
    fspi_released = 4'hb;
    step();
    expect_released(4'h9, "HMI BACK plus FSPI LEFT drives reverse-left");
    @(negedge clk);
    fspi_released = 4'h8; // stale GO/BACK and active automatic LEFT
    hmi_code = 4'h0;
    step();
    expect_released(4'hb, "manual traction release masks stale traction but keeps lane steering");
    @(negedge clk);
    hmi_code = 4'hd;
    step();
    expect_released(4'h7, "manual RIGHT overrides automatic LEFT");
    @(negedge clk);
    hmi_code = 4'h0;
    step();
    expect_released(4'hf, "manual steering release cannot reveal old automatic LEFT");
    @(negedge clk);
    fspi_released = 4'hc; // steering idle while stale traction still held
    step();
    @(negedge clk);
    fspi_released = 4'h8;
    step();
    expect_released(4'hb, "fresh lane LEFT rearms independently of stale traction");
    @(negedge clk);
    fspi_released = 4'hf;
    step();
    @(negedge clk);
    hmi_code = 4'ha;
    fspi_released = 4'h7;
    step();
    expect_released(4'h6, "forward-right before brake");

    // Pedestrian braking and HMI E must always release movement contacts.
    @(negedge clk);
    movement_blocked = 1'b1;
    #1;
    expect_released(4'hf, "brake releases all directions");
    @(negedge clk);
    movement_blocked = 1'b0;
    hmi_code = 4'he;
    #1;
    expect_released(4'hf, "HMI E releases all directions");
    step();
    @(negedge clk);
    hmi_code = 4'h0;
    fspi_direction_armed = 1'b0;
    #1;
    expect_released(4'hf, "disarmed FSPI cannot move vehicle");

    $display("PASS: independent traction/steering, manual priority, stale release guards and STOP");
    $finish;
  end
endmodule
