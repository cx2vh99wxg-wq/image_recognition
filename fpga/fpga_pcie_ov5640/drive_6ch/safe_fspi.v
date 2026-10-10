`timescale 1ns/1ps
// Mode 3 QUAD, maximum 1 MHz with 25 MHz free_clk. Entire slave in free_clk
// domain, avoiding multi-clock register writers. Read dummy byte permits turn-around.
module safe_fspi #(parameter WATCHDOG=12500000)(
 input clk,rst_n,spi_clk,spi_cs,inout [3:0] spi_data,
 input [7:0] view,input [2:0] camera_ok,
 output go,back,left,right,stop
);
 (* ASYNC_REG="TRUE" *) reg [2:0] sclk,cs;
 (* ASYNC_REG="TRUE" *) reg [3:0] dq1,dq2;
 reg [3:0] nibble;reg [7:0] command,address,value,reply;
 reg [3:0] out_data;reg out_enable;
 reg [7:0] motor[0:4];reg [31:0] watchdog;integer i;
 wire expired=watchdog>=WATCHDOG;
 wire halted=expired || !motor[4][0] || camera_ok!=3'b111;
 assign go=halted?1'b1:motor[0][0];
 assign back=halted?1'b1:motor[1][0];
 assign left=halted?1'b1:motor[2][0];
 assign right=halted?1'b1:motor[3][0];
 assign stop=halted?1'b0:1'b1;
 assign spi_data=(!spi_cs && out_enable)?out_data:4'bzzzz;
 always @(posedge clk or negedge rst_n)begin
  if(!rst_n)begin
   sclk<=7;cs<=7;dq1<=0;dq2<=0;nibble<=0;command<=0;address<=0;value<=0;reply<=0;
   out_enable<=0;out_data<=0;watchdog<=WATCHDOG;
   for(i=0;i<4;i=i+1)motor[i]<=1;motor[4]<=0;
  end else begin
   sclk<={sclk[1:0],spi_clk};cs<={cs[1:0],spi_cs};dq1<=spi_data;dq2<=dq1;
   if(!expired)watchdog<=watchdog+1'b1;
   // A lost heartbeat must never resurrect old commands when it resumes.
   if(expired || camera_ok!=7)begin
    for(i=0;i<4;i=i+1)motor[i]<=1;motor[4]<=0;
   end
   if(cs[2])begin nibble<=0;out_enable<=0;command<=0;end
   else begin
    if(sclk[2:1]==2'b10)begin // falling: set output before master's rising sample
     out_enable<=command==8'h80 && (nibble==6 || nibble==7);
     out_data<=nibble==6?reply[7:4]:reply[3:0];
    end
    if(sclk[2:1]==2'b01)begin
     if(nibble<8)nibble<=nibble+1'b1;
     case(nibble)
      0:command[7:4]<=dq2;
      1:command[3:0]<=dq2;
      2:address[7:4]<=dq2;
      3:address[3:0]<=dq2;
      4:value[7:4]<=dq2;
      5:begin
       if(command==8'h00)begin
        if(address<5 && !expired && camera_ok==7)motor[address]<={7'b0,dq2[0]};
        if(address==5 && {value[7:4],dq2}==8'ha5)watchdog<=0;
       end
       if(address<5)reply<=motor[address];
       else if(address==8'h10)reply<=view;
       else if(address==8'h11)reply<={5'b0,camera_ok};
       else if(address==8'h7f)reply<=8'ha6;
       else reply<=8'hff;
      end
     endcase
    end
   end
  end
 end
endmodule
