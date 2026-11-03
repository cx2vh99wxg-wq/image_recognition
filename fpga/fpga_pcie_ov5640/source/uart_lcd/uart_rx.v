// 2026-11-03 注释校订。同步串口输入，在位周期中点采样，结束时更新并行字节与完成标志。
module uart_rx(
    input               clk         ,
    input               rst_n       ,

    input               uart_rxd    ,
    output  reg         uart_rx_done,
    output  reg  [7:0]  uart_rx_data
    );

parameter CLK_FREQ = 50000000;
parameter UART_BPS = 115200  ;
localparam BAUD_CNT_MAX = CLK_FREQ/UART_BPS;

reg          uart_rxd_d0;
reg          uart_rxd_d1;
reg          uart_rxd_d2;
reg          rx_flag    ;
wire  [3:0 ]  rx_cnt     ;
wire  [15:0]  baud_cnt   ;
reg  [7:0 ]  rx_data_t  ;

wire        start_en;

assign start_en = uart_rxd_d2 & (~uart_rxd_d1) & (~rx_flag);

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        uart_rxd_d0 <= 1'b0;
        uart_rxd_d1 <= 1'b0;
        uart_rxd_d2 <= 1'b0;
    end
    else begin
        uart_rxd_d0 <= uart_rxd;
        uart_rxd_d1 <= uart_rxd_d0;
        uart_rxd_d2 <= uart_rxd_d1;
    end
end

always @(posedge clk or negedge rst_n) begin
    if(!rst_n)
        rx_flag <= 1'b0;
    else if(start_en)
        rx_flag <= 1'b1;

    else if((rx_cnt == 4'd9) && (baud_cnt == BAUD_CNT_MAX/2 - 1'b1))
        rx_flag <= 1'b0;
    else
        rx_flag <= rx_flag;
end

uart_bit_timer #(.BAUD_CYCLES(BAUD_CNT_MAX)) u_bit_timer (
    .clk(clk), .rst_n(rst_n), .active(rx_flag),
    .baud_count(baud_cnt), .bit_index(rx_cnt)
);

always @(posedge clk or negedge rst_n) begin
    if(!rst_n)
        rx_data_t <= 8'b0;
    else if(rx_flag) begin
        if(baud_cnt == BAUD_CNT_MAX/2 - 1'b1) begin
           case(rx_cnt)
               4'd1, 4'd2, 4'd3, 4'd4, 4'd5, 4'd6, 4'd7, 4'd8:
                   rx_data_t[rx_cnt - 4'd1] <= uart_rxd_d2;
               default : ;
            endcase
        end
        else
            rx_data_t <= rx_data_t;
    end
    else
        rx_data_t <= 8'b0;
end

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        uart_rx_done <= 1'b0;
        uart_rx_data <= 8'b0;
    end

    else if(rx_cnt == 4'd9 && baud_cnt == BAUD_CNT_MAX/2 - 1'b1) begin
        uart_rx_done <= 1'b1     ;
        uart_rx_data <= rx_data_t;
    end
    else begin
        uart_rx_done <= 1'b0;
        uart_rx_data <= uart_rx_data;
    end
end

endmodule