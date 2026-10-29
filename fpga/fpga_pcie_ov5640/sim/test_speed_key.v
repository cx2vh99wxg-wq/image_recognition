`timescale 1ns/1ps
module test_speed_key;
 reg clk=0;always #20 clk=~clk;
 reg rst_n=0,physical=0;tri1 key_n;wire drive,ready,pressed;wire [2:0] phase;
 assign key_n=physical?1'b0:1'bz;
 assign key_n=drive?1'b0:1'bz;
 reg [7:0] sync_command=0,data=0;reg valid=0;
 wire slider_present,level,target_toggle;wire [1:0] target;wire [3:0] hmi;wire go,back,left,right;
 wire [3:0] hmi_sync;
 hmi_control_synchronizer cdc(.clk(clk),.rst_n(rst_n),.async_code(hmi),.code_sync(hmi_sync),.rearm_command());
 uart_lcd #(.FRAME_GAP_CYCLES(64)) uart(.clk(clk),.rst_n(rst_n),.uart_rx(data),.uart_rx_valid(valid),
  .cnn_level(hmi),.speed_slider_present(slider_present),.speed_request_level(level),
  .speed_target_gear(target),.speed_target_toggle(target_toggle));
 remote_speed_controller #(.DEBOUNCE_CYCLES(8),.PULSE_CYCLES(40),.GAP_CYCLES(30)) speed(
  .clk(clk),.rst_n(rst_n),.key_n(key_n),.hmi_level_async(level||(hmi_sync==4'hf)),
  .target_toggle_async(target_toggle),.target_gear_async(target),.sync_command_async(sync_command),
  .phase_gray(phase),.ready(ready),.pressed(pressed),.drive_active(drive));
 remote_direction_arbiter direction(.clk(clk),.rst_n(rst_n),.hmi_code(hmi),.fspi_released(4'hf),.fspi_direction_armed(1'b1),
  .movement_blocked(1'b0),.hmi_left_denied(1'b0),.hmi_right_denied(1'b0),.go_released(go),.back_released(back),.left_released(left),.right_released(right));
 function [1:0] gear(input[2:0] g);case(g)0,2:gear=1;1,6:gear=2;3,4:gear=3;default:gear=0;endcase endfunction
 task step(input integer n);begin repeat(n)@(negedge clk);#1;end endtask
 task octet(input[7:0] b);begin @(negedge clk);data=b;valid=1;@(negedge clk);valid=0;end endtask
 task command(input[7:0] f,input[7:0] v);begin octet(8'h30);octet(8'h90);octet(f);octet(v);step(3);end endtask
 integer drives=0,saved;reg previous_drive=0;
 always @(posedge clk)begin if(drive&&!previous_drive)drives=drives+1;previous_drive=drive;end
 initial begin
  step(4);rst_n=1;step(22);if(!ready||drive||pressed||gear(phase)!=1)$fatal(1,"released startup gear1");
  if(slider_present)$fatal(1,"legacy TFT must not receive unsupported variable commands");
  command(17,0);step(20);if(!slider_present||drive||gear(phase)!=1)$fatal(1,"slider handshake does not shift");
  command(17,8'hee);step(20);if(slider_present||drive)$fatal(1,"slider departure disables numeric feedback only");
  repeat(4)begin physical=1;step(3);physical=0;step(3);end
  step(15);if(gear(phase)!=1)$fatal(1,"bounce must not count");
  physical=1;step(20);if(gear(phase)!=2)$fatal(1,"physical 1->2");
  step(100);if(gear(phase)!=2)$fatal(1,"physical hold counts once");
  physical=0;step(20);physical=1;step(20);if(gear(phase)!=3)$fatal(1,"physical 2->3");
  physical=0;step(20);physical=1;step(20);if(gear(phase)!=1)$fatal(1,"physical 3->1");physical=0;step(25);
  step(20);if(key_n!==1'b1)$fatal(1,"remote pull-up restores released HIGH");
  // New TFT: same function-04 path and release frame as RIGHT, unique F code.
  repeat(3)begin
    saved=gear(phase);command(4,15);step(25);
    if(!drive||key_n||gear(phase)!=(saved==3?1:saved+1)||!right||!left||!go||!back)
      $fatal(1,"04 F SPEED contact, one gear, no direction contact");
    saved=gear(phase);step(150);command(4,15);step(25);
    if(!drive||gear(phase)!=saved)$fatal(1,"hold/duplicate F must not repeat");
    command(4,0);step(25);if(drive||!key_n||pressed)$fatal(1,"04 00 releases SPEED");
  end
  command(4,10);step(15);command(4,0);step(15);if(go)$fatal(1,"GO latch baseline");
  command(16,1);step(20);if(!drive||key_n||gear(phase)!=2||go)$fatal(1,"manual SPEED low, preserve GO");
  step(150);if(!drive||key_n||gear(phase)!=2)$fatal(1,"manual SPEED must stay low beyond old fixed pulse");
  command(16,1);step(30);if(!drive||gear(phase)!=2)$fatal(1,"duplicate press is not another gear");
  command(16,0);step(20);if(drive||!key_n||pressed||gear(phase)!=2)$fatal(1,"release actually releases pad");
  command(16,1);step(20);if(!drive||gear(phase)!=3)$fatal(1,"no artificial 300ms dead zone, 2->3");
  command(16,0);step(20);command(16,1);step(20);if(gear(phase)!=1)$fatal(1,"HMI 3->1 rollover");
  command(16,0);step(20);
  command(4,12);step(15);if(left||go)$fatal(1,"LEFT and GO independent");
  command(16,1);step(20);if(left||go||gear(phase)!=2)$fatal(1,"SPEED does not alter LEFT/GO");
  command(16,0);step(20);command(4,0);step(15);if(!left||go)$fatal(1,"LEFT release preserves GO");
  command(4,11);step(15);command(4,0);step(15);if(back||!go)$fatal(1,"BACK replaces GO");
  command(4,13);step(15);if(right||back)$fatal(1,"RIGHT independent of BACK");command(4,0);step(15);
  command(17,3);step(200);if(gear(phase)!=3||drive||back)$fatal(1,"slider one pulse to3, BACK intact");
  saved=drives;command(17,3);step(150);if(drives!=saved||gear(phase)!=3)$fatal(1,"same target does not shift");
  command(17,1);step(200);if(gear(phase)!=1||drive)$fatal(1,"slider 3->1");
  command(17,3);step(250);if(gear(phase)!=3||drive)$fatal(1,"slider 1->3 uses two contacts");
  saved=drives;command(17,0);command(17,4);step(100);if(drives!=saved)$fatal(1,"invalid target ignored");
  sync_command=8'ha5;step(20);sync_command=0;step(20);if(gear(phase)!=1||drive||back)$fatal(1,"calibration idle only, BACK unchanged");
  command(17,3);step(20);command(16,1);step(100);
  if(!drive||gear(phase)!=2)$fatal(1,"manual hold takes over slider without duplicate edge");
  sync_command=8'ha5;step(20);if(gear(phase)!=2)$fatal(1,"calibration rejected while pressed");sync_command=0;
  command(16,0);step(150);if(drive||gear(phase)!=2)$fatal(1,"manual release cancels pending slider pulse");
  rst_n=0;physical=1;step(4);rst_n=1;step(25);if(ready||drive||gear(phase)!=1)$fatal(1,"held at reset must not shift/drive");
  command(4,15);step(25);if(ready||!drive)$fatal(1,"unready feedback must not block manual SPEED");
  command(4,0);step(25);if(drive)$fatal(1,"unready manual release");
  physical=0;step(25);physical=1;step(20);if(!ready||gear(phase)!=2)$fatal(1,"fresh press after startup release");
  $display("SPEED: RIGHT-style 04 F/00, open-drain, unready manual output, exact hold/release, 3->1, physical key, slider and legacy independence PASS");$finish;
 end
 initial begin #1000000;$fatal(1,"speed test timeout");end
endmodule
