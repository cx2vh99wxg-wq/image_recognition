module cmos_capture_data_reference(
    input                 rst_n            ,  // 模块状态复位输入。
    // 连接图像传感器的端口。
    input                 cam_pclk         ,  // 摄像头时序：cmos 数据像素时钟
    input                 cam_vsync        ,  // 本信号用于cmos 场同步。
    input                 cam_href         ,  // 本信号用于cmos 行同步。
    input  [7:0]          cam_data         ,                      
    // 供上层逻辑连接的接口。
	output reg   [9:0]    active_x         ,
	output reg   [9:0]    active_y         ,            
    output                cmos_frame_vsync ,  // 指示当前帧数据有效。
    output                cmos_frame_href  ,  // 指示当前行数据有效。
    output                cmos_frame_valid ,  // 用于决定数据有效是否执行。
    output       [15:0]   cmos_frame_data     // 表示本周期数据的有效状态。

    );

// 摄像头时序：寄存器全部配置完成后，先等待10帧数据
// 摄像头时序：待寄存器配置生效后再开始采集图像
parameter  WAIT_FRAME = 4'd10    ;            // 摄像头时序：寄存器数据稳定等待的帧个数

// 停用示例，仅供对照：reg define
reg             cam_vsync_d0     ;
reg             cam_vsync_d1     ;
reg             cam_href_d0      ;
reg             cam_href_d1      ;
reg    [3:0]    cmos_ps_cnt      ;            // 对等待帧数稳定进行累计计数。
reg    [7:0]    cam_data_d0      ;            
reg    [15:0]   cmos_data_t      ;            // 用来8位转16位的临时寄存器
reg             byte_flag        ;            // 本信号用于16位RGB数据转换完成的标志。
reg             byte_flag_d0     ;
reg             frame_val_flag   ;            // 该标志跟踪帧有效区间。

wire            pos_vsync        ;            // 摄像头时序：采输入场同步信号的上升沿

// ── 摄像头时序 ──
// 摄像头时序：** main code
// ── 摄像头时序 ──

parameter H_PIXEL = 640;
parameter V_PIXEL = 480;
 
always @(posedge cam_pclk or negedge rst_n)
    if(!rst_n)
        active_x <= 0;
    else if(active_x == H_PIXEL-1 & cmos_frame_valid)
        active_x <= 0;
    else if(cmos_frame_valid)
        active_x <= active_x + 1;

always @(posedge cam_pclk or negedge rst_n)
    if(!rst_n)
        active_y <= 0;
    else if(active_y == V_PIXEL-1 & active_x == H_PIXEL-1 & cmos_frame_valid)
        active_y <= 0;
    else if(active_x == H_PIXEL-1 & cmos_frame_valid) 
        active_y <= active_y + 1;

// 摄像头时序：采输入场同步信号的上升沿
assign pos_vsync = (~cam_vsync_d1) & cam_vsync_d0; 

// 向后级传递帧有效标志。
assign  cmos_frame_vsync = frame_val_flag  ?  cam_vsync_d1  :  1'b0; 

// 向后级传递行有效标志。
assign  cmos_frame_href  = frame_val_flag  ?  cam_href_d1   :  1'b0; 

// 指示输出数据使能是否处于有效区间。
assign  cmos_frame_valid = frame_val_flag  ?  byte_flag_d0  :  1'b0; 

// 送往下级的数据。
assign  cmos_frame_data  = frame_val_flag  ?  cmos_data_t   :  1'b0; 
       
always @(posedge cam_pclk or negedge rst_n) begin
    if(!rst_n) begin
        cam_vsync_d0 <= 1'b0;
        cam_vsync_d1 <= 1'b0;
        cam_href_d0 <= 1'b0;
        cam_href_d1 <= 1'b0;
    end
    else begin
        cam_vsync_d0 <= cam_vsync;
        cam_vsync_d1 <= cam_vsync_d0;
        cam_href_d0 <= cam_href;
        cam_href_d1 <= cam_href_d0;
    end
end

// 每帧完成后推进帧计数。
always @(posedge cam_pclk or negedge rst_n) begin
    if(!rst_n)
        cmos_ps_cnt <= 4'd0;
    else if(pos_vsync && (cmos_ps_cnt < WAIT_FRAME))
        cmos_ps_cnt <= cmos_ps_cnt + 4'd1;
end

// 该标志表示帧有效状态。
always @(posedge cam_pclk or negedge rst_n) begin
    if(!rst_n)
        frame_val_flag <= 1'b0;
    else if((cmos_ps_cnt == WAIT_FRAME) && pos_vsync)
        frame_val_flag <= 1'b1;
    else;    
end            

// 摄像头时序：8位数据转16位RGB565数据
always @(posedge cam_pclk or negedge rst_n) begin
    if(!rst_n) begin
        cmos_data_t <= 16'd0;
        cam_data_d0 <= 8'd0;
        byte_flag <= 1'b0;
    end
    else if(cam_href) begin
        byte_flag <= ~byte_flag;
        cam_data_d0 <= cam_data;
        if(byte_flag)
            cmos_data_t <= {cam_data_d0,cam_data};
        else;   
    end
    else begin
        byte_flag <= 1'b0;
        cam_data_d0 <= 8'b0;
    end    
end        

// 摄像头时序：产生输出数据有效信号(cmos_frame_valid)
always @(posedge cam_pclk or negedge rst_n) begin
    if(!rst_n)
        byte_flag_d0 <= 1'b0;
    else
        byte_flag_d0 <= byte_flag;	
end 
       
endmodule