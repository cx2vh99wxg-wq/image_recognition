`timescale 1ns/1ps
module tb_drive;
 reg clk=0,rst=0;always #20 clk=~clk;
 reg valid=0;reg [7:0] byte_in=0;wire [127:0] cfg;
 hmi_regs #(.TIMEOUT(1000)) hmi(clk,rst,valid,byte_in,cfg);
 reg sck=1,cs=1,host_oe=1;reg [3:0] host_data=0;wire [3:0] bus;
 assign bus=host_oe?host_data:4'bzzzz;
 wire go,back,left,right,stop;reg [2:0] cameras=7;
 safe_fspi #(.WATCHDOG(5000)) spi(clk,rst,sck,cs,bus,cfg[127:120],cameras,go,back,left,right,stop);
 task uart_byte(input [7:0] b);
  begin @(negedge clk);byte_in=b;valid=1;@(negedge clk);valid=0;end
 endtask
 task hmi_set(input [7:0] sel,input [7:0] value);
  begin uart_byte(8'h30);uart_byte(8'h90);uart_byte(sel);uart_byte(value);end
 endtask
 task spi_byte(input [7:0] b);
  begin
   #500;sck=0;host_data=b[7:4];#500;sck=1;
   #500;sck=0;host_data=b[3:0];#500;sck=1;
  end
 endtask
 task spi_write(input [7:0] addr,input [7:0] value);
  begin cs=0;host_oe=1;#200;spi_byte(0);spi_byte(addr);spi_byte(value);#500;cs=1;#500;end
 endtask
 reg [7:0] answer;
 task spi_read(input [7:0] addr);
  begin
   cs=0;host_oe=1;#200;spi_byte(8'h80);spi_byte(addr);
   // FPGA must release the shared wires for command/address/dummy.
   if(spi.out_enable!==0)$fatal(1,"read contention before dummy");
   spi_byte(0);#100;host_oe=0;
   #400;sck=0;#450;answer[7:4]=bus;#50;sck=1;
   #500;sck=0;#450;answer[3:0]=bus;#50;sck=1;
   #500;cs=1;#200;if(bus!==4'bzzzz)$fatal(1,"bus not released");#300;
  end
 endtask
 // Three independent pixel clocks/ISP instances, actual full top creates these.
 reg [2:0] pc=0;always #9 pc[0]=~pc[0];always #11 pc[1]=~pc[1];always #13 pc[2]=~pc[2];
 reg vs=0,de=0;reg [15:0] pixel=16'h4208;
 wire [2:0] ovs,ode;wire [15:0] rgb[0:2];
 genvar k;generate for(k=0;k<3;k=k+1)begin: isps
  night_isp isp(pc[k],rst,vs,de,pixel,cfg,ovs[k],ode[k],rgb[k]);
 end endgenerate
 wire health;
 camera_health #(.TIMEOUT(1000),.FRAME_PIXELS(16)) live(clk,rst,pc[0],vs,de,health);
 initial begin
  #300;rst=1;#300;
  if({go,back,left,right,stop}!==5'b11110)$fatal(1,"unsafe reset");
  hmi_set(15,2);if(cfg[127:120]!=2)$fatal(1,"HMI view");
  hmi_set(15,10);if(cfg[127:120]!=2)$fatal(1,"invalid HMI changes view");
  uart_byte(8'h30);#50000;uart_byte(8'h90);uart_byte(15);uart_byte(6);
  if(cfg[127:120]!=2)$fatal(1,"HMI timeout");
  hmi_set(4,8);if(cfg[39:32]!=8)$fatal(1,"enhancement selector");
  hmi_set(1,2);#200;vs=1;#200;vs=0;de=1;#1000;
  if(rgb[0]<=pixel||rgb[1]<=pixel||rgb[2]<=pixel)$fatal(1,"three night paths not enhanced");
  if(ode!==7)$fatal(1,"pixel enables lost");
  de=0;vs=1;#200;vs=0;#500;if(!health)$fatal(1,"valid frame not alive");
  #45000;if(health)$fatal(1,"camera stall not detected");
  hmi_set(1,0);#200;vs=1;#200;vs=0;de=1;#200;
  if(rgb[0]!==pixel||rgb[1]!==pixel||rgb[2]!==pixel)$fatal(1,"bypass pixel corruption");
  de=0;#200;if(ode!==0)$fatal(1,"enable delay mismatch");
  spi_read(8'h7f);if(answer!==8'ha6)$fatal(1,"FSPI version %h",answer);
  spi_read(8'h10);if(answer!==2)$fatal(1,"FSPI HMI view %h",answer);
  spi_read(8'h11);if(answer!==7)$fatal(1,"FSPI camera status %h",answer);
  spi_write(0,0);if(go!==1)$fatal(1,"motion before heartbeat");
  spi_write(5,8'ha5);spi_write(0,0);spi_write(4,1);#200;
  if(go!==0||stop!==1)$fatal(1,"valid GO refused");
  spi_write(4,0);if(go!==1||stop!==0)$fatal(1,"STOP failed");
  spi_write(4,1);cameras=6;#200;if(go!==1||stop!==0)$fatal(1,"camera loss failed");
  cameras=7;spi_write(5,8'ha5);spi_write(0,0);spi_write(4,1);
  #210000;if(go!==1||stop!==0)$fatal(1,"watchdog failed");
  spi_write(5,8'ha5);spi_write(4,1);if(go!==1)$fatal(1,"old motion resurrected");
  $display("PASS: HMI framing/bounds/timeout, 3 ISP clocks, camera health, FSPI turnaround/read/write/reset/watchdog");
  $finish;
 end
 initial begin #2000000;$fatal(1,"test timeout");end
endmodule
