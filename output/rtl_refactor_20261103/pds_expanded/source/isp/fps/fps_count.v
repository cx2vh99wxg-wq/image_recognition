module fps_count(
    input wire pixelclk,    // VGA像素时钟域
    input wire clk,         // 基准时钟（50MHz）
    input wire rst_n,            // 低电平复位
    input wire vsync,            // VGA垂直同步信号
    output wire [8:0] fps   // 帧率输出
);

//--------------------------------------------------
// 时钟域：clk（50MHz）
//--------------------------------------------------
reg [26:0] cnt;
wire clear_flag;

// 生成1秒定时脉冲（50MHz → 50,000,000周期）
reg [1:0] clear_flag_hold;  // 新增寄存器，用于延长脉冲
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        cnt <= 27'd0;
        clear_flag_hold <= 2'b00;
    end else if (cnt == 27'd49_999_999) begin
        cnt <= 27'd0;
        clear_flag_hold <= 2'b11;  // 保持高电平2个周期
    end else if (clear_flag_hold != 2'b00) begin
        clear_flag_hold <= clear_flag_hold - 1;  // 递减至0
    end else begin
        cnt <= cnt + 1;
    end
end

assign clear_flag = (clear_flag_hold != 2'b00);  // 脉冲宽度为2个周期
//--------------------------------------------------
// 时钟域：pixelclk（VGA像素时钟）
//--------------------------------------------------
// 2026-11-03: shift both synchronized inputs in one pixel-clock process.
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

wire clear_pulse = (clear_flag_sync2 && !clear_flag_sync3); // 上升沿检测

// 帧计数器（带清零）
reg [9:0] fps_out;  // 扩展为10位
always @(posedge pixelclk or negedge rst_n) begin
    if (!rst_n) 
        fps_out <= 10'd0;
    else if (clear_pulse) 
        fps_out <= 10'd0;        // 优先清零
    else if (vsync_pos) 
        fps_out <= fps_out + 1;  // 其次计数
end
// 输出锁存
reg [8:0] fps_reg;
always @(posedge pixelclk or negedge rst_n) begin
    if (!rst_n) 
        fps_reg <= 0;
    else if (clear_flag) 
        fps_reg <= fps_out; // 每秒结束时锁存稳定值
end

assign fps = fps_reg;

endmodule