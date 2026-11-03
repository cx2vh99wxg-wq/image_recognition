// 2026-11-03 注释校订。按起始位、低位优先的数据与停止位串行发送，沿用旧 BUSY 释放时刻。
module uart_tx(
    input               clk         ,
    input               rst_n       ,
    input               uart_tx_en  ,
    input     [7:0]     uart_tx_data,
    output  reg         uart_txd    ,
    output  reg         uart_tx_busy
    );

parameter CLK_FREQ = 50000000;
parameter UART_BPS = 115200  ;
localparam BAUD_CNT_MAX = CLK_FREQ/UART_BPS;

reg  [7:0]  tx_data_t;
wire  [3:0]  tx_cnt   ;
wire  [15:0] baud_cnt ;

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        tx_data_t <= 8'b0;
        uart_tx_busy <= 1'b0;
    end

    else if(uart_tx_en) begin
        tx_data_t <= uart_tx_data;
        uart_tx_busy <= 1'b1;
    end

    else if(tx_cnt == 4'd9 && baud_cnt == BAUD_CNT_MAX - BAUD_CNT_MAX/16) begin
        tx_data_t <= 8'b0;
        uart_tx_busy <= 1'b0;
    end
    else begin
        tx_data_t <= tx_data_t;
        uart_tx_busy <= uart_tx_busy;
    end
end

uart_bit_timer #(.BAUD_CYCLES(BAUD_CNT_MAX)) u_bit_timer (
    .clk(clk), .rst_n(rst_n), .active(uart_tx_busy),
    .baud_count(baud_cnt), .bit_index(tx_cnt)
);

always @(posedge clk or negedge rst_n) begin
    if(!rst_n)
        uart_txd <= 1'b1;
    else if(uart_tx_busy) begin
        case(tx_cnt)
            4'd0 : uart_txd <= 1'b0        ;
            4'd1, 4'd2, 4'd3, 4'd4, 4'd5, 4'd6, 4'd7, 4'd8:
                uart_txd <= tx_data_t[tx_cnt - 4'd1];
            4'd9 : uart_txd <= 1'b1        ;
            default : uart_txd <= 1'b1;
        endcase
    end
    else
        uart_txd <= 1'b1;
end

endmodule

module uart_bit_timer #(parameter BAUD_CYCLES = 1) (
    input clk, rst_n, active,
    output reg [15:0] baud_count,
    output reg [3:0] bit_index
);
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            baud_count <= 16'd0;
            bit_index <= 4'd0;
        end else if (active) begin
            if (baud_count < BAUD_CYCLES - 1'b1)
                baud_count <= baud_count + 16'd1;
            else
                baud_count <= 16'd0;
            if (baud_count == BAUD_CYCLES - 1'b1)
                bit_index <= bit_index + 1'b1;
        end else begin
            baud_count <= 16'd0;
            bit_index <= 4'd0;
        end
    end
endmodule
