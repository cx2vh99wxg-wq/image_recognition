
//Ŀǰram�������2048
//��дʱ����400MHZ
//��� RK�����д��ͨ������ҪС��100mhz ��д��СҪС��2048
module fspi_slave_top
(
    input    wire             sys_clk    ,    //25mhz
    input    wire             rst_n      ,
                              
    input    wire             spi_clk    ,
    input    wire             spi_cs     ,
    inout    wire    [3:0]    spi_data    ,
    input    wire    [3:0]         select_rearview,  // GPIO接收到的一位值
    output   wire    [7:0]    mem0_out,
    output   wire    [7:0]    mem1_out,
    output   wire    [7:0]    mem2_out,
    output   wire    [7:0]    mem3_out,
    output   wire    [7:0]    mem4_out
);


wire    [1:0]    cmd;
wire             tx_data_valid;
wire    [7:0]    tx_data;
wire             rx_data_valid;
wire    [7:0]    rx_data;
wire             wr_clk    ;    //��Ҫ�������ʵ�4������������spiͨ������ 100MHZʱ����ʱ��������Ҫ400mhz ��Ϊ��4��

clk_fspi clk_gen_inst (
  .clkout0(wr_clk),    // output
  .lock(),          // output
  .clkin1(sys_clk)       // input
);

//读写ram控制
spi_ram_crtl u_spi_ram_crtl(
    .wr_clk         ( wr_clk         ),
    .rst_n          ( rst_n          ),
    .spi_cs         ( spi_cs         ),
    .cmd            ( cmd            ),
    .tx_done        ( tx_data_valid  ),
    .wr_data        ( rx_data        ),
    .rx_valid       ( rx_data_valid  ),
    .select_rearview( select_rearview),  // 传递select_rearview信号
    .rd_data        ( tx_data        ),
    .mem0_out       ( mem0_out       ),
    .mem1_out       ( mem1_out       ),
    .mem2_out       ( mem2_out       ),
    .mem3_out       ( mem3_out       ),
    .mem4_out       ( mem4_out       )
);

fspi_slave u_fspi_slave(
    .spi_clk           ( spi_clk           ),
    .spi_cs            ( spi_cs            ),
    .spi_data          ( spi_data          ),
    .rst_n             ( rst_n             ),
    .select_rearview   ( select_rearview   ),
    .cmd               ( cmd               ),
    .rx_data           ( rx_data           ),
    .rx_data_valid     ( rx_data_valid     ),
    .tx_data           ( tx_data           ),
    .tx_data_valid     ( tx_data_valid     )
);



endmodule
