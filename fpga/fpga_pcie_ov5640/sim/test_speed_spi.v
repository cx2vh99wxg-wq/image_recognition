`timescale 1ns/1ps
module test_speed_spi;
 reg rst_n=0,clk=1,cs=1,master=1;reg[3:0] m=15,hmi=12;reg[5:0] status=6'b100001;
 reg [15:0] controls=16'ha56b;
 tri[3:0] bus;assign bus=master?m:4'bz;wire[1:0] cmd;wire[7:0] rx;wire valid,txdone;
 fspi_slave dut(.spi_clk(clk),.spi_cs(cs),.spi_data(bus),.rst_n(rst_n),.select_rearview(hmi),
  .auto_brake_enable(1'b0),.speed_status_async(status),.control_status_async(controls),.cmd(cmd),.rx_data(rx),.rx_data_valid(valid),.tx_data(8'h00),.tx_data_valid(txdone));
 // Production SMDT runs mode 3 at 24 MHz (41.666 ns per clock).
 task nibble(input[3:0] d);begin clk=0;m=d;#20.833;clk=1;#20.833;end endtask
 task readn(output[3:0] v);begin clk=0;#20.833;clk=1;#1;v=bus;#19.833;end endtask
 task readbyte(output[7:0] v);reg[3:0] a,b;begin readn(a);readn(b);v={a,b};end endtask
 reg[7:0] a,b,c,d,e,f,g,h;reg[3:0] v;
 function [7:0] update_crc(input[7:0] old,input[7:0] value);
 reg[7:0] t;integer j;begin t=old^value;for(j=0;j<8;j=j+1)t=t[7]?(t<<1)^7:t<<1;update_crc=t;end endfunction
 function[7:0] crc(input[7:0] x);reg[7:0] t;integer i;begin t=8'ha7;for(i=0;i<8;i=i+1)t=t[7]?(t<<1)^7:t<<1;t=t^x;for(i=0;i<8;i=i+1)t=t[7]?(t<<1)^7:t<<1;crc=t;end endfunction
 task startread;begin cs=1;master=1;#100;cs=0;nibble(8'h80>>4);nibble(0);master=0;end endtask
 task finish;begin cs=1;master=1;#100;end endtask
 initial begin
  #100;rst_n=1;#100;
  startread;readbyte(a);readbyte(b);readbyte(c);readbyte(d);finish;
  if(a!=8'hcc||b!=8'ha7||c!=8'hc2||d!=crc(c))$fatal(1,"native 80 extension %h %h %h %h",a,b,c,d);
  startread;readbyte(a);controls=0;readbyte(b);readbyte(c);readbyte(d);
  readbyte(e);readbyte(f);readbyte(g);readbyte(h);finish;
  if(e!=8'hd7||f!=8'h6b||g!=8'ha5||h!=update_crc(update_crc(update_crc(0,e),f),g))
   $fatal(1,"diagnostic snapshot/magic/CRC %h %h %h %h",e,f,g,h);
  // Snapshot must not tear when gear/pressed status changes mid-read.
  status=6'b111011;hmi=13;
  startread;readbyte(a);status=6'b100000;readbyte(b);readbyte(c);readbyte(d);finish;
  if(a!=8'hdd||b!=8'ha7||c!=8'hf3||d!=crc(c))$fatal(1,"coherent status snapshot %h %h %h %h",a,b,c,d);
  // All six Gray phases decode 1,2,3,1,2,3.
  for(integer i=0;i<6;i=i+1)begin
   case(i)0:status=6'b100000;1:status=6'b100001;2:status=6'b100011;3:status=6'b100010;4:status=6'b100110;5:status=6'b100100;endcase
   startread;readbyte(a);readbyte(b);readbyte(c);readbyte(d);finish;
   if(c!=(8'hc1+i%3)||d!=crc(c))$fatal(1,"gray phase %d decoding %h",i,c);
  end
  hmi=10;startread;readbyte(a);finish;if(a!=8'haa)$fatal(1,"legacy one-byte HMI unchanged");
  startread;readbyte(a);finish;if(a!=8'haa)$fatal(1,"CS resets extension index");
  $display("SPEED SPI: native auto-80, CRC, coherent snapshot, six Gray phases, legacy HMI and CS reset PASS");$finish;
 end
endmodule
