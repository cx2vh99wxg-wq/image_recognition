//****************************************Copyright (c)***********************************//
// 图像流水线：原子哥在线教学平台：www.yuanzige.com
// 图像流水线：技术支持：www.openedv.com
// 图像流水线：淘宝店铺：http://openedv.taobao.com
// 图像流水线：关注微信公众平台微信号："正点原子"，免费获取ZYNQ & FPGA & STM32 & LINUX资料。
//版权所有，盗版必究。
//Copyright(C) 正点原子 2018-2028
//All rights reserved
// ── 图像流水线 ──
// 图像流水线：File name: myram
// 图像流水线：Last modified Date: 2020/05/04 9:19:08
// 参数记录为 Last Version: V1.0。
// 图像流水线：Descriptions: 自定义RAM
// ── 图像流水线 ──
// ── 图像流水线 ──
// 图像流水线：Created by: 正点原子
// 参数记录为 Created date: 2019/05/04 9:19:08。
// 参数记录为 Version: V1.0。
// 图像流水线：Descriptions: The original version
// ── 图像流水线 ──
// ── 图像流水线 ──
// ── 图像流水线 ──

module myram #(
    parameter WIDTH = 1  ,               // 图像流水线：数据的位宽(位数)
    parameter DEPTH = 800,               // 图像流水线：数据的深度(个数)
    parameter DEPBIT= 10                 // 地址字段所占的位数。
)(
    // 图像流水线：module clock
    input                     clk  ,     // 模块采样使用的时钟。

    // 图像流水线：ram interface
    input                     we   ,
    input  [DEPBIT- 1'b1:0]   waddr,
    input  [DEPBIT- 1'b1:0]   raddr,
    input  [WIDTH - 1'b1:0]   dq_i ,
    output [WIDTH - 1'b1:0]   dq_o

    // 图像流水线：user interface
);

// 停用示例，仅供对照：reg define
reg [WIDTH - 1'b1:0] mem [DEPTH - 1'b1:0];

// ── 图像流水线 ──
// 图像流水线：** main code
// ── 图像流水线 ──

assign dq_o = mem[raddr];

always @ (posedge clk) begin
    if(we)
        mem[waddr-1] <= dq_i;
end

endmodule




