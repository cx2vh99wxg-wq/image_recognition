// 2026-11-03 注释校订。在参考时钟域生成统计窗口，在像素时钟域统计帧沿并登记显示值。
module fps_count(
    input wire pixelclk,
    input wire clk,
    input wire rst_n,
    input wire vsync,
    output wire [8:0] fps
);

reg [26:0] cnt;
wire clear_flag;

reg [1:0] clear_flag_hold;
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        cnt <= 27'd0;
        clear_flag_hold <= 2'b00;
    end else if (cnt == 27'd49_999_999) begin
        cnt <= 27'd0;
        clear_flag_hold <= 2'b11;
    end else if (clear_flag_hold != 2'b00) begin
        clear_flag_hold <= clear_flag_hold - 1;
    end else begin
        cnt <= cnt + 1;
    end
end

assign clear_flag = (clear_flag_hold != 2'b00);

reg [1:0] vsync_history;
reg [2:0] clear_history;
always @(posedge pixelclk or negedge rst_n) begin
    if (!rst_n) begin
        vsync_history <= 2'b00;
        clear_history <= 3'b000;
    end else begin
        vsync_history <= {vsync_history[0], vsync};
        clear_history <= {clear_history[1:0], clear_flag};
    end
end
wire vsync_pos = !vsync_history[1] && vsync_history[0];
wire clear_flag_sync2 = clear_history[1];
wire clear_flag_sync3 = clear_history[2];

// 计数清零取像素域同步后的上升沿，显示寄存器仍按原清除电平取样。
wire clear_pulse = (clear_flag_sync2 && !clear_flag_sync3);

reg [9:0] fps_out;
always @(posedge pixelclk or negedge rst_n) begin
    if (!rst_n)
        fps_out <= 10'd0;
    else if (clear_pulse)
        fps_out <= 10'd0;
    else if (vsync_pos)
        fps_out <= fps_out + 1;
end

reg [8:0] fps_reg;
always @(posedge pixelclk or negedge rst_n) begin
    if (!rst_n)
        fps_reg <= 0;
    else if (clear_flag)
        fps_reg <= fps_out;
end

assign fps = fps_reg;

endmodule