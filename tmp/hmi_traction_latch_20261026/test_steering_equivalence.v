`timescale 1ns/1ps
module test_steering_equivalence;
  reg clk=0; always #20 clk=~clk;
  reg rst_n=0;
  reg [3:0] hmi_code=0, fspi_released=15;
  reg armed=1, blocked=0;
  wire old_l,old_r,new_l,new_r,new_go,new_back;
  remote_direction_arbiter_previous previous(
    .clk(clk),.rst_n(rst_n),.hmi_code(hmi_code),.fspi_released(fspi_released),
    .fspi_direction_armed(armed),.movement_blocked(blocked),
    .go_released(),.back_released(),.left_released(old_l),.right_released(old_r));
  remote_direction_arbiter current(
    .clk(clk),.rst_n(rst_n),.hmi_code(hmi_code),.fspi_released(fspi_released),
    .fspi_direction_armed(armed),.movement_blocked(blocked),
    .go_released(new_go),.back_released(new_back),.left_released(new_l),.right_released(new_r));
  integer checks=0,i,hold_cycles;
  reg [31:0] rng=32'h8b65e410;
  task check;
    begin
      if ({old_l,old_r} !== {new_l,new_r})
        $fatal(1,"steering changed at %0t HMI=%h FSPI=%h",$time,hmi_code,fspi_released);
      if (current.hmi_owns_traction && !new_go && !new_back)
        $fatal(1,"mutually exclusive HMI traction outputs both asserted");
      checks=checks+1;
    end
  endtask
  initial begin
    repeat(3) @(negedge clk);
    rst_n=1;
    // All HMI codes, FSPI bit combinations, enable and brake combinations.
    for(i=0;i<1024;i=i+1) begin
      @(negedge clk);
      hmi_code=i[3:0]; fspi_released=i[7:4]; armed=i[8]; blocked=i[9];
      #1; check();
      repeat(12) begin @(posedge clk); #1; check(); end
    end
    // Stateful transitions with short/long code dwell and varying FSPI bits.
    for(i=0;i<20000;i=i+1) begin
      @(negedge clk);
      rng=rng^(rng<<13); rng=rng^(rng>>17); rng=rng^(rng<<5);
      hmi_code=rng[3:0]; fspi_released=rng[7:4]; armed=rng[8]; blocked=rng[9];
      hold_cycles=1+int'(rng[15:12]);
      #1; check();
      repeat(hold_cycles) begin @(posedge clk); #1; check(); end
    end
    $display("PASS: steering identical to prior working arbiter over 21024 scenarios, %0d samples; HMI traction mutually exclusive",checks);
    $finish;
  end
endmodule
