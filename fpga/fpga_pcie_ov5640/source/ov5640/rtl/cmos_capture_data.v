// 2026-11-03 注释校订。采集 OV5640 的 RGB565 字节。启动帧门控、像素有效和坐标计数分别处理。
// 保持两级同步输出和奇数字节行尾的原有效脉冲行为。
module cmos_capture_data(
    input                 rst_n            ,

    input                 cam_pclk         ,
    input                 cam_vsync        ,
    input                 cam_href         ,
    input  [7:0]          cam_data         ,

	output reg   [9:0]    active_x         ,
	output reg   [9:0]    active_y         ,
    output                cmos_frame_vsync ,
    output                cmos_frame_href  ,
    output                cmos_frame_valid ,
    output       [15:0]   cmos_frame_data

    );

parameter  WAIT_FRAME = 4'd10    ;

reg             cam_vsync_d0     ;
reg             cam_vsync_d1     ;
reg             cam_href_d0      ;
reg             cam_href_d1      ;
reg    [3:0]    cmos_ps_cnt      ;
reg    [7:0]    cam_data_d0      ;
reg    [15:0]   cmos_data_t      ;
reg             byte_flag        ;
reg             byte_flag_d0     ;
reg             frame_val_flag   ;

wire            pos_vsync        ;

parameter H_PIXEL = 640;
parameter V_PIXEL = 480;

wire last_column = active_x == H_PIXEL-1;
always @(posedge cam_pclk or negedge rst_n) begin
    if (!rst_n) begin
        active_x <= 0;
        active_y <= 0;
    end else if (cmos_frame_valid) begin
        if (last_column) begin
            active_x <= 0;
            if (active_y == V_PIXEL-1)
                active_y <= 0;
            else
                active_y <= active_y + 1;
        end else active_x <= active_x + 1;
    end
end

assign pos_vsync = (~cam_vsync_d1) & cam_vsync_d0;

assign  cmos_frame_vsync = frame_val_flag  ?  cam_vsync_d1  :  1'b0;

assign  cmos_frame_href  = frame_val_flag  ?  cam_href_d1   :  1'b0;

assign  cmos_frame_valid = frame_val_flag  ?  byte_flag_d0  :  1'b0;

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

always @(posedge cam_pclk or negedge rst_n) begin
    if (!rst_n) begin
        cmos_ps_cnt <= 4'd0;
        frame_val_flag <= 1'b0;
    end else if (pos_vsync) begin
        if (cmos_ps_cnt < WAIT_FRAME) cmos_ps_cnt <= cmos_ps_cnt + 4'd1;
        if (cmos_ps_cnt == WAIT_FRAME) frame_val_flag <= 1'b1;
    end
end

always @(posedge cam_pclk or negedge rst_n) begin
    if (!rst_n) begin
        cmos_data_t <= 16'd0;
        cam_data_d0 <= 8'd0;
        byte_flag <= 1'b0;
        byte_flag_d0 <= 1'b0;
    end else begin
        byte_flag_d0 <= byte_flag;
        if (cam_href) begin
            byte_flag <= ~byte_flag;
            cam_data_d0 <= cam_data;
            if (byte_flag) cmos_data_t <= {cam_data_d0,cam_data};
        end else begin
            byte_flag <= 1'b0;
            cam_data_d0 <= 8'd0;
        end
    end
end

endmodule
