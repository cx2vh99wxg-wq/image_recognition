`timescale 1ns/1ps
// 30 90 selector data, 115200 8N1. One writer, timeout and bounded values.
module hmi_regs #(parameter TIMEOUT=250000)(
 input clk,rst_n,valid,input [7:0] data,output reg [127:0] cfg
);
 reg [1:0] state;reg [7:0] selector;reg [31:0] timer;
 always @(posedge clk or negedge rst_n) begin
  if(!rst_n) begin
   state<=0;selector<=0;timer<=0;
   // bytes 0..15; mode=1, black=0, white gain=128, enhancement=2.
   cfg<=128'h00_f0_10_f0_10_01_40_80_00_80_80_02_80_00_01_00;
  end else begin
   if(state!=0 && timer<TIMEOUT)timer<=timer+1'b1;
   if(timer>=TIMEOUT)begin state<=0;timer<=0;end
   if(valid)begin
    timer<=0;
    case(state)
     0:if(data==8'h30)state<=1;
     1:if(data==8'h90)state<=2;else state<=data==8'h30 ? 1 : 0;
     2:begin selector<=data;state<=3;end
     3:begin
      state<=0;
      if(selector>=1 && selector<=15)begin
       if(selector==1)begin
        if(data<=3)cfg[15:8]<=data;
        // Compatibility with old enhancement buttons 8..16.
        else if(data>=8 && data<=16)begin cfg[15:8]<=2;cfg[39:32]<=data-8;end
       end
       else if(selector==4)begin if(data<=8)cfg[39:32]<=data;end
       else if(selector==7 || selector==10)begin if(data<=1)cfg[selector*8+:8]<=data;end
       else if(selector==15)begin if(data<=6)cfg[127:120]<=data;end
       else cfg[selector*8+:8]<=data;
      end
     end
    endcase
   end
  end
 end
endmodule
