//****************************************Copyright (c)***********************************//
// 摄像头时序：原子哥在线教学平台：www.yuanzige.com
// 摄像头时序：技术支持：http://www.openedv.com/forum.php
// 摄像头时序：淘宝店铺：https://zhengdianyuanzi.tmall.com
// 摄像头时序：关注微信公众平台微信号："正点原子"，免费获取ZYNQ & FPGA & STM32 & LINUX资料。
//版权所有，盗版必究。
//Copyright(C) 正点原子 2023-2033
//All rights reserved
// ── 摄像头时序 ──
// 参数记录为 File name: ov5640_dri。
// 摄像头时序：Last modified Date: 2020/05/04 9:19:08
// 参数记录为 Last Version: V1.0。
// 摄像头时序：Descriptions: OV5640摄像头驱动
// ── 摄像头时序 ──
// ── 摄像头时序 ──
// 摄像头时序：Created by: 正点原子
// 参数记录为 Created date: 2019/05/04 9:19:08。
// 参数记录为 Version: V1.0。
// 摄像头时序：Descriptions: The original version
// ── 摄像头时序 ──
// ── 摄像头时序 ──
// ── 摄像头时序 ──

module ov5640_dri (
    input           clk             ,  // 本模块采用的时钟输入。
    input           rst_n           ,  // 摄像头时序：复位信号,低电平有效
    // 连接图像传感器的端口。
    input           cam_pclk        ,  // 摄像头时序：cmos 数据像素时钟
    input           cam_vsync       ,  // 本信号用于cmos 场同步。
    input           cam_href        ,  // 本信号用于cmos 行同步。
    input    [7:0]  cam_data        ,  // 摄像头时序：cmos 数据
    output          cam_rst_n       ,  // 摄像头时序：cmos 复位信号，低电平有效
    output          cam_pwdn        ,  // 本信号用于cmos 电源休眠模式选择。
    output          cam_scl         ,  // 摄像头时序：cmos SCCB_SCL线
    inout           cam_sda         ,  // 摄像头时序：cmos SCCB_SDA线
    
    // 用于连接摄像头分辨率配置的端口。
    input     [7:0] rate,               // 此参数决定摄像头输出帧率。
    input    [12:0] cmos_h_pixel    ,  // 有效图像的水平像素数。
    input    [12:0] cmos_v_pixel    ,  // 有效图像的垂直像素数。
    input    [12:0] total_h_pixel   ,  // 一行水平方向的总计数尺寸。
    input    [12:0] total_v_pixel   ,  // 一帧垂直方向的总计数尺寸。
    input    [12:0] y_addr_st       ,
    input    [12:0] y_addr_end      ,
    input           capture_start   ,  // 本信号用于图像采集开始。
    output          cam_init_done   ,  // 摄像头时序：摄像头初始化完成
    
    // 供上层逻辑连接的接口。
    output          cmos_frame_vsync,  // 指示当前帧数据有效。
    output          cmos_frame_href ,  // 指示当前行数据有效。
    output          cmos_frame_valid,  // 用于决定数据有效是否执行。
    output  [15:0]  cmos_frame_data    // 表示本周期数据的有效状态。
);

// 摄像头时序：parameter define
parameter SLAVE_ADDR = 7'h78          ; // 摄像头时序：OV5640的器件地址7'h3c
parameter BIT_CTRL   = 1'b1           ; // 摄像头时序：OV5640的字节地址为16位 0:8位 1:16位
parameter CLK_FREQ   = 27'd50_000_000 ; // i2c_dri模块的驱动时钟频率：本节对应的标识或作用域。
parameter I2C_FREQ   = 18'd250_000    ; // 摄像头时序：I2C的SCL时钟频率,不超过400KHz

// 停用示例，仅供对照：wire difine
wire        i2c_exec       ;  // I2C触发执行信号：本节对应的标识或作用域。
wire [23:0] i2c_data       ;  // 摄像头时序：I2C要配置的地址与数据(高8位地址,低8位数据)
wire        i2c_done       ;  // I2C寄存器配置完成信号：本节对应的标识或作用域。
wire        i2c_dri_clk    ;  // I2C操作时钟：本节对应的标识或作用域。
wire [ 7:0] i2c_data_r     ;  // I2C读出的数据：本节对应的标识或作用域。
wire        i2c_rh_wl      ;  // I2C读写控制信号：本节对应的标识或作用域。

// ── 摄像头时序 ──
// 摄像头时序：** main code
// ── 摄像头时序 ──

// 摄像头时序：电源休眠模式选择 0：正常模式 1：电源休眠模式
assign  cam_pwdn  = 1'b0;
assign  cam_rst_n = 1'b1;

// I2C配置模块：本节对应的标识或作用域。
i2c_ov5640_rgb565_cfg u_i2c_cfg(
    .clk                (i2c_dri_clk  ),
    .rst_n              (rst_n        ),
    .rate               (rate     ),        
    .i2c_exec           (i2c_exec     ),
    .i2c_data           (i2c_data     ),
    .i2c_rh_wl          (i2c_rh_wl    ),    // I2C读写控制信号：本节对应的标识或作用域。
    .i2c_done           (i2c_done     ),
    .i2c_data_r         (i2c_data_r   ),
                
    .cmos_h_pixel       (cmos_h_pixel ),    // CMOS水平方向像素个数：本节对应的标识或作用域。
    .cmos_v_pixel       (cmos_v_pixel ),    // CMOS垂直方向像素个数：本节对应的标识或作用域。
    .total_h_pixel      (total_h_pixel),    // 一行水平方向的总计数尺寸。
    .total_v_pixel      (total_v_pixel),    // 一帧垂直方向的总计数尺寸。
    .y_addr_st          (y_addr_st    ),
    .y_addr_end         (y_addr_end   ),
    .init_done          (cam_init_done) 
    );    

// I2C驱动模块：本节对应的标识或作用域。
i2c_dri #(
    .SLAVE_ADDR         (SLAVE_ADDR   ),    // 向子模块转交参数。
    .CLK_FREQ           (CLK_FREQ     ),
    .I2C_FREQ           (I2C_FREQ     ) 
    )
u_i2c_dri(
    .clk                (clk          ),
    .rst_n              (rst_n        ),

    .i2c_exec           (i2c_exec     ),
    .bit_ctrl           (BIT_CTRL     ),
    .i2c_rh_wl          (  1'b0  ),    // 摄像头时序：固定为0，只用到了IIC驱动的写操作
    .i2c_addr           (i2c_data[23:8]),
    .i2c_data_w         (i2c_data[7:0]),
    .i2c_data_r         (i2c_data_r   ),
    .i2c_done           (i2c_done     ),
    
    .scl                (cam_scl      ),
    .sda                (cam_sda      ),

    .dri_clk            (i2c_dri_clk  )     // I2C操作时钟：本节对应的标识或作用域。
    );

// CMOS图像数据采集模块：本节对应的标识或作用域。
cmos_capture_data u_cmos_capture_data(      // 系统初始化完成以后再开始采集数据
    .rst_n              (rst_n & capture_start),

    .cam_pclk           (cam_pclk     ),
    .cam_vsync          (cam_vsync    ),
    .cam_href           (cam_href     ),
    .cam_data           (cam_data     ),

    .cmos_frame_vsync   (cmos_frame_vsync),
    .cmos_frame_href    (cmos_frame_href ),
    .cmos_frame_valid   (cmos_frame_valid), // 用于决定数据有效是否执行。
    .cmos_frame_data    (cmos_frame_data )  // 表示本周期数据的有效状态。
    );

endmodule 