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
      .hmi_left_denied(1'b0), .hmi_right_denied(1'b0), .movement_blocked(movement_blocked),
      .go_released(go_released),
      .back_released(back_released),
      .left_released(left_released),
      .right_released(right_released)
  );

  task step;
    begin
      repeat (16) @(posedge clk);
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

    // HMI commands persist after 00 and override stale FSPI traction.
    @(negedge clk);
    fspi_released = 4'hc;
    hmi_code = 4'ha;
    step();
    expect_released(4'he, "HMI GO overrides both FSPI directions");
    @(negedge clk);
    hmi_code = 4'h0;
    #1;
    expect_released(4'he, "GO remains latched after HMI release");
    repeat (3) step();
    expect_released(4'he, "stale FSPI BACK cannot override latched GO");

    @(negedge clk);
    hmi_code = 4'hb;
    step();
    expect_released(4'hd, "HMI BACK press");
    @(negedge clk);
    hmi_code = 4'h0;
    #1;
    expect_released(4'hd, "BACK remains latched and replaces GO");

    // LEFT and RIGHT use the same handoff rule.
    @(negedge clk);
    hmi_code = 4'hc;
    step();
    expect_released(4'h9, "HMI LEFT press preserves latched BACK");
    @(negedge clk);
    hmi_code = 4'h0;
    #1;
    expect_released(4'hd, "HMI LEFT release preserves latched BACK");
    @(negedge clk);
    hmi_code = 4'hd;
    step();
    expect_released(4'h5, "HMI RIGHT press preserves latched BACK");
    @(negedge clk);
    hmi_code = 4'h0;
    #1;
    expect_released(4'hd, "HMI RIGHT release preserves latched BACK");

    // Only an all-idle FSPI state hands authority back. Its next press works.
    @(negedge clk);
    fspi_released = 4'hf;
    step();
    expect_released(4'hd, "FSPI idle never clears the HMI BACK latch");
    @(negedge clk);
    hmi_code = 4'he;
    movement_blocked = 1'b1;
    step();
    expect_released(4'hf, "manual STOP clears traction latch");
    @(negedge clk);
    hmi_code = 4'h0;
    movement_blocked = 1'b0;
    step();
    expect_released(4'hf, "STOP release cannot restart traction");
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
    fspi_released = 4'hf;
    hmi_code = 4'h0;
    step();
    expect_released(4'he, "GO persists with released HMI and FSPI");
    @(negedge clk);
    hmi_code = 4'he;
    @(posedge clk); #1;
    @(negedge clk);
    hmi_code = 4'h0;
    step();
    expect_released(4'he, "unfiltered transient E cannot erase latched GO");
    @(negedge clk);
    hmi_code = 4'hc;
    step();
    expect_released(4'ha, "momentary HMI LEFT preserves latched GO");
    @(negedge clk);
    hmi_code = 4'h0;
    step();
    expect_released(4'he, "HMI LEFT release preserves latched GO");
    @(negedge clk);
    hmi_code = 4'hd;
    step();
    expect_released(4'h6, "momentary HMI RIGHT preserves latched GO");
    @(negedge clk);
    hmi_code = 4'h0;
    step();
    expect_released(4'he, "HMI RIGHT release preserves latched GO");
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
    expect_released(4'h9, "released HMI retains BACK and lane LEFT");
    @(negedge clk);
    hmi_code = 4'hd;
    step();
    expect_released(4'h5, "manual RIGHT overrides automatic LEFT while BACK persists");
    @(negedge clk);
    hmi_code = 4'h0;
    step();
    expect_released(4'hd, "steering release masks old LEFT while BACK persists");
    @(negedge clk);
    fspi_released = 4'hc; // steering idle while stale traction still held
    step();
    @(negedge clk);
    fspi_released = 4'h8;
    step();
    expect_released(4'h9, "fresh lane LEFT rearms independently while BACK persists");
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
    step();
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

    // A key observed during braking cannot reappear when braking clears.
    @(negedge clk);
    movement_blocked = 1'b1;
    hmi_code = 4'ha;
    step();
    @(negedge clk);
    movement_blocked = 1'b0;
    step();
    expect_released(4'hf, "held GO from a blocked interval cannot restart traction");
    @(negedge clk);
    hmi_code = 4'h0;
    step();
    @(negedge clk);
    hmi_code = 4'ha;
    step();
    expect_released(4'he, "a fresh GO press is accepted after unlock");
    @(negedge clk);
    hmi_code = 4'he;
    movement_blocked = 1'b1;
    step();
    @(negedge clk);
    hmi_code = 4'h0;
    movement_blocked = 1'b0;
    step();
    expect_released(4'hf, "manual STOP cancels GO until another explicit command");

    // One-clock intermediate A/B during a multi-bit CDC is not a press.
    @(negedge clk);
    hmi_code = 4'h2;
    step();
    @(negedge clk);
    hmi_code = 4'ha;
    @(posedge clk); #1;
    @(negedge clk);
    hmi_code = 4'h8;
    step();
    expect_released(4'hf, "transient A between camera selections cannot latch GO");
    @(negedge clk);
    hmi_code = 4'hb;
    @(posedge clk); #1;
    @(negedge clk);
    hmi_code = 4'h0;
    step();
    expect_released(4'hf, "transient B cannot latch BACK");

    $display("PASS: latched HMI traction, momentary independent steering, opposite switch, STOP clear and fresh-press rearm");
    $finish;
  end
endmodule
