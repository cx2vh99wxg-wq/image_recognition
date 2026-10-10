`timescale 1ns/1ps
// Three independent instances per board. Fixed one pixel-clock output latency;
// never remove border pixels. This is bounded gain/curve ISP, not the paper CNN.
module night_isp(input clk,rst_n,vs,de,input [15:0] rgb,
 input [127:0] cfg,output reg ovs,ode,output reg [15:0] out);
 (* ASYNC_REG="TRUE" *) reg [127:0] cfg1,cfg2;
 reg [127:0] frame_cfg;reg oldvs;reg [7:0] previous_y;
 integer r,g,b,y,cb,cr,gain,v,edgeval,sg;
 function [7:0] sat;
  input integer n;begin if(n<0)sat=0;else if(n>255)sat=255;else sat=n;end
 endfunction
 always @*begin
  r={rgb[15:11],rgb[15:13]};g={rgb[10:5],rgb[10:9]};b={rgb[4:0],rgb[4:2]};
  y=(r*77+g*150+b*29)>>8;cb=128+((b-y)*144>>8);cr=128+((r-y)*183>>8);
  sg=frame_cfg[47:40];
  gain=128+frame_cfg[39:32]*16;
  if(frame_cfg[15:8]!=0)begin
   r=sat(r-frame_cfg[23:16]);g=sat(g-frame_cfg[23:16]);b=sat(b-frame_cfg[23:16]);
   if(frame_cfg[63:56]!=0)begin r=sat(r*frame_cfg[31:24]>>7);b=sat(b*(256-frame_cfg[31:24])>>7);end
   if(frame_cfg[15:8]==2)begin
    if(frame_cfg[87:80]==1)begin
     // Shadow-selective curve; whites stay bounded, zero stays black.
     r=sat(r+((r*(255-r)*frame_cfg[39:32])>>10));
     g=sat(g+((g*(255-g)*frame_cfg[39:32])>>10));
     b=sat(b+((b*(255-b)*frame_cfg[39:32])>>10));
    end else begin r=sat(r*gain>>7);g=sat(g*gain>>7);b=sat(b*gain>>7);end
   end
   y=(r*77+g*150+b*29)>>8;
   r=sat(y+(((r-y)*sg)>>>7)+frame_cfg[55:48]-128);
   g=sat(y+(((g-y)*sg)>>>7)+frame_cfg[55:48]-128);
   b=sat(y+(((b-y)*sg)>>>7)+frame_cfg[55:48]-128);
  end
  edgeval=y>previous_y?y-previous_y:previous_y-y;
  v=(y>=frame_cfg[71:64] && edgeval>=frame_cfg[79:72] &&
     cb>=frame_cfg[95:88] && cb<=frame_cfg[103:96] && cr>=frame_cfg[111:104] && cr<=frame_cfg[119:112])?255:0;
  if(frame_cfg[15:8]==3)begin r=v;g=v;b=v;end
 end
 always @(posedge clk or negedge rst_n)begin
  if(!rst_n)begin
   cfg1<=0;cfg2<=0;frame_cfg<=128'h00_f0_10_f0_10_01_40_80_00_80_80_02_80_00_01_00;
   oldvs<=0;ovs<=0;ode<=0;out<=0;previous_y<=0;
  end else begin
   cfg1<=cfg;cfg2<=cfg1;oldvs<=vs;
   if(vs&&!oldvs)frame_cfg<=cfg2;
   ovs<=vs;ode<=de;out<={r[7:3],g[7:2],b[7:3]};
   if(de)previous_y<=y[7:0];else previous_y<=0;
  end
 end
endmodule
