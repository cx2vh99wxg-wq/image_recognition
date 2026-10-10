`timescale 1ns/1ps
// VSYNC liveness alone can pass a broken data bus. Require a full valid frame
// before toggling each completed-frame event, then watchdog in free_clk.
module camera_health #(parameter TIMEOUT=12500000,parameter FRAME_PIXELS=307200)(
 input clk,rst_n,pclk,vs,de,output reg alive
);
 reg oldvs,toggle;reg [19:0] pixels;
 always @(posedge pclk or negedge rst_n)begin
  if(!rst_n)begin oldvs<=0;toggle<=0;pixels<=0;end
  else begin
   oldvs<=vs;
   if(vs&&!oldvs)begin if(pixels>=FRAME_PIXELS)toggle<=~toggle;pixels<=0;end
   else if(de && pixels<524287)pixels<=pixels+1'b1;
  end
 end
 (* ASYNC_REG="TRUE" *) reg [2:0] sync;
 reg [31:0] age;
 always @(posedge clk or negedge rst_n)begin
  if(!rst_n)begin sync<=0;age<=TIMEOUT;alive<=0;end
  else begin
   sync<={sync[1:0],toggle};
   if(sync[2]!=sync[1])begin age<=0;alive<=1;end
   else if(age<TIMEOUT)age<=age+1'b1;
   else alive<=0;
  end
 end
endmodule
