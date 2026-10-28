`timescale 1ns/1ps
module test_traffic_go;
  reg clk=0;always #20 clk=~clk;
  reg rst_n=0;reg [7:0] permission=0;
  reg [3:0] hmi=0,fspi=15;reg stop=0;
  wire deny_l,deny_r,hold_go,raw_go,go,back,left,right;
  lane_permission_guard #(.TIMEOUT_CYCLES(200)) guard(
    .clk(clk),.rst_n(rst_n),.async_permission(permission),
    .left_denied(deny_l),.right_denied(deny_r),.traffic_go_denied(hold_go));
  remote_direction_arbiter arbiter(.clk(clk),.rst_n(rst_n),.hmi_code(hmi),.fspi_released(fspi),
    .fspi_direction_armed(1'b1),.movement_blocked(stop),.hmi_left_denied(deny_l),.hmi_right_denied(deny_r),
    .go_released(raw_go),.back_released(back),.left_released(left),.right_released(right));
  assign go=raw_go|hold_go; // exact production final output gate
  task step;begin repeat(22)@(posedge clk);#1;end endtask
  initial begin
    step;rst_n=1;step;
    if(!go||hold_go)$fatal(1,"startup idle, no unsolicited GO");
    fspi=14;step;if(go)$fatal(1,"FSPI GO baseline");
    permission=8'hc4;step;if(!go||raw_go||!hold_go)$fatal(1,"red masks FSPI GO only");
    fspi=15;hmi=10;step;hmi=0;step;
    if(!go||raw_go||!back)$fatal(1,"red masks HMI latched GO without erasing it");
    permission=8'h40;step; // no confirmed color / legacy software is not a clear
    repeat(250)@(posedge clk);#1;
    if(!hold_go||!go||deny_l||deny_r)$fatal(1,"red survives heartbeat loss; old lane timeout preserved");
    permission=8'hc7;step;hmi=12;step;
    if(!left||!right||!go||!deny_l||!deny_r)$fatal(1,"lane permissions compose with red");
    hmi=0;step;fspi=11;step;
    if(left||!go)$fatal(1,"automatic curve steering unchanged while GO held");
    fspi=15;hmi=11;step;hmi=0;step;
    if(back||!go)$fatal(1,"red permits latched BACK, replaces previous GO");
    permission=8'h48;step;if(hold_go||back||!go)$fatal(1,"green keeps BACK intent");
    hmi=10;step;hmi=0;step;if(go)$fatal(1,"GO latch restored under green");
    permission=8'hc4;step;if(!go)$fatal(1,"second red masks GO");
    permission=8'h48;step;if(go||hold_go)$fatal(1,"green resumes retained GO");
    permission=8'hc4;step;stop=1;step;stop=0;step;
    if(raw_go!==1||!go)$fatal(1,"STOP still clears original GO intent");
    permission=8'h48;step;if(!go)$fatal(1,"green cannot bypass STOP-cleared GO");
    permission=8'hcc;step;if(!hold_go)$fatal(1,"simultaneous SET and CLEAR: SET wins");
    permission=8'h00;step;if(!hold_go)$fatal(1,"invalid protocol cannot clear red");
    permission=8'h48;step;hmi=10;step;hmi=0;step;if(go)$fatal(1,"fresh green permits new GO");
    rst_n=0;step;rst_n=1;permission=0;fspi=15;hmi=0;step;
    if(!go||hold_go)$fatal(1,"reset releases all contacts without restarting traction");
    $display("traffic GO gate: FSPI/HMI, latched intent, green resume, STOP/BACK, lane gate, watchdog PASS");$finish;
  end
endmodule
