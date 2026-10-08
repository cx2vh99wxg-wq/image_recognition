//======================================================================
// uart_loopback.v — 串口屏 UART 收发封装（【人员 C · 控制与 FPGA】）
//
// 例化 uart_rx + uart_tx，把 RXD 收到的字节回环到 TXD，同时把 rx 字节
// （uart_rx_data / uart_rx_done）引出给 uart_lcd.v 做命令解析。
// 顶层 image_pcie_capture.v 中 uart_txd 未接外部引脚，仅使用接收侧。
//======================================================================
module uart_loopback(
    input                               sys_clk                    ,//外部50MHz时钟
    input                               sys_rst_n                  ,//系外部复位信号，低有效
    
    //UART端口    
    input                               uart_rxd                   ,//UART接收端口
    output                              uart_txd                   ,//UART发送端口
    output                              uart_rx_done               ,//UART接收完成信号
    output             [   7: 0]        uart_rx_data                //UART接收数据
    );

//parameter define
parameter CLK_FREQ = 50000000;    //定义系统时钟频率
parameter UART_BPS = 115200  ;    //定义串口波特率

//wire define


//*****************************************************
//**                    main code
//*****************************************************

//串口接收模块
uart_rx #(
    .CLK_FREQ  (CLK_FREQ),
    .UART_BPS  (UART_BPS)
    )    
    u_uart_rx(
    .clk           (sys_clk     ),
    .rst_n         (sys_rst_n   ),
    .uart_rxd      (uart_rxd    ),
    .uart_rx_done  (uart_rx_done),
    .uart_rx_data  (uart_rx_data)
    );

//串口发送模块
uart_tx #(
    .CLK_FREQ  (CLK_FREQ),
    .UART_BPS  (UART_BPS)
    )    
    u_uart_tx(
    .clk          (sys_clk     ),
    .rst_n        (sys_rst_n   ),
    .uart_tx_en   (uart_rx_done),
    .uart_tx_data (uart_rx_data),
    .uart_txd     (uart_txd    ),
    .uart_tx_busy (            )
    );
    
endmodule
