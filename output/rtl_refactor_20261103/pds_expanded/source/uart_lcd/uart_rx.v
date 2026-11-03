
module uart_rx(
    input               clk         ,  //系统时钟
    input               rst_n       ,  //系统复位，低有效

    input               uart_rxd    ,  //UART接收端口
    output  reg         uart_rx_done,  //UART接收完成信号
    output  reg  [7:0]  uart_rx_data   //UART接收到的数据
    );

//parameter define
parameter CLK_FREQ = 50000000;               //系统时钟频率
parameter UART_BPS = 115200  ;               //串口波特率
localparam BAUD_CNT_MAX = CLK_FREQ/UART_BPS; //为得到指定波特率，对系统时钟计数BPS_CNT次

//reg define
reg          uart_rxd_d0;
reg          uart_rxd_d1;
reg          uart_rxd_d2;
reg          rx_flag    ;  //接收过程标志信号
wire  [3:0 ]  rx_cnt     ;  //接收数据计数器
wire  [15:0]  baud_cnt   ;  //波特率计数器
reg  [7:0 ]  rx_data_t  ;  //接收数据寄存器

//wire define
wire        start_en;

//*****************************************************
//**                    main code
//*****************************************************
//捕获接收端口下降沿(起始位)，得到一个时钟周期的脉冲信号
assign start_en = uart_rxd_d2 & (~uart_rxd_d1) & (~rx_flag);

//针对异步信号的同步处理
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

//给接收标志赋值
always @(posedge clk or negedge rst_n) begin
    if(!rst_n) 
        rx_flag <= 1'b0;
    else if(start_en)    //检测到起始位
        rx_flag <= 1'b1; //接收过程中，标志信号rx_flag拉高
    //在停止位一半的时候，即接收过程结束，标志信号rx_flag拉低
    else if((rx_cnt == 4'd9) && (baud_cnt == BAUD_CNT_MAX/2 - 1'b1))
        rx_flag <= 1'b0;
    else
        rx_flag <= rx_flag;
end        

//波特率的计数器赋值
// 2026-11-03: transmit and receive use the same bit-period counter.
uart_bit_timer #(.BAUD_CYCLES(BAUD_CNT_MAX)) u_bit_timer (
    .clk(clk), .rst_n(rst_n), .active(rx_flag),
    .baud_count(baud_cnt), .bit_index(rx_cnt)
);

//根据rx_cnt来寄存rxd端口的数据
always @(posedge clk or negedge rst_n) begin
    if(!rst_n) 
        rx_data_t <= 8'b0;
    else if(rx_flag) begin                           //系统处于接收过程时
        if(baud_cnt == BAUD_CNT_MAX/2 - 1'b1) begin  //判断baud_cnt是否计数到数据位的中间
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

//给接收完成信号和接收到的数据赋值
always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        uart_rx_done <= 1'b0;
        uart_rx_data <= 8'b0;
    end
    //当接收数据计数器计数到停止位，且baud_cnt计数到停止位的中间时
    else if(rx_cnt == 4'd9 && baud_cnt == BAUD_CNT_MAX/2 - 1'b1) begin
        uart_rx_done <= 1'b1     ;  //拉高接收完成信号
        uart_rx_data <= rx_data_t;  //并对UART接收到的数据进行赋值
    end    
    else begin
        uart_rx_done <= 1'b0;
        uart_rx_data <= uart_rx_data;
    end
end

endmodule