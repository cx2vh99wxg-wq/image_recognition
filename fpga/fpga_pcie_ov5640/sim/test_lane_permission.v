`timescale 1ns/1ps
module test_lane_permission;
  reg clk=0;always #20 clk=~clk;
  reg rst_n=0;reg [7:0] permission=0;
  reg [3:0] hmi=0,fspi=15;reg stop=0;
  wire deny_l,deny_r,go,back,left,right;
  lane_permission_guard #(.TIMEOUT_CYCLES(200)) guard(
    .clk(clk),.rst_n(rst_n),.async_permission(permission),.left_denied(deny_l),.right_denied(deny_r));
  remote_direction_arbiter arbiter(.clk(clk),.rst_n(rst_n),.hmi_code(hmi),.fspi_released(fspi),
    .fspi_direction_armed(1'b1),.movement_blocked(stop),.hmi_left_denied(deny_l),.hmi_right_denied(deny_r),
    .go_released(go),.back_released(back),.left_released(left),.right_released(right));
  task step;begin repeat(22)@(posedge clk);#1;end endtask
  initial begin
    step;rst_n=1;step;
    hmi=10;step;hmi=0;step;if(go!==0||back!==1)$fatal(1,"GO latch baseline");
    permission=8'hc1;step;if(!deny_l||deny_r)$fatal(1,"left permission byte");
    hmi=12;step;if(!left||!right||go)$fatal(1,"solid left must block without clearing GO");
    hmi=13;step;if(!left||right||go)$fatal(1,"dashed right still allowed");
    permission=8'h43;step;hmi=12;step;if(!left||!right||go)$fatal(1,"both solid");
    hmi=0;fspi=15;step;fspi=4'b1011;step;if(left)$fatal(1,"curve following must bypass crossing permission");
    stop=1;step;if(!left||!right||!go||!back)$fatal(1,"STOP highest priority");
    stop=0;fspi=15;hmi=0;step;if(!go)$fatal(1,"STOP clears traction intent");
    repeat(250)@(posedge clk);#1;if(deny_l||deny_r)$fatal(1,"heartbeat timeout");
    hmi=12;step;if(left)$fatal(1,"unknown legacy manual fallback");
    permission=8'hc2;step;if(deny_l||!deny_r)$fatal(1,"fresh heartbeat right");
    $display("lane permission, HMI/FSPI separation, traction, STOP, timeout PASS");$finish;
  end
endmodule
