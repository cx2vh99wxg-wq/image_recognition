//======================================================================
// fspi_slave_top.v — FSPI 从机顶层（【人员 C · 控制与 FPGA】）
//
// 例化三个子模块：
//   clk_fspi      : 由 sys_clk(25MHz) 4 倍频得到 wr_clk（SPI 100MHz 时需 400MHz，
//                   用于内部 4 线并行收发的字节组装时钟）；
//   fspi_slave    : 四线 QUAD SPI 物理层（字节组装 + 命令解码）；
//   spi_ram_crtl  : 把字节流解析为 5 个寄存器 mem[0..4] 的读写。
//
// 端口与 image_pcie_capture.v 保持一致，未改动。
//======================================================================
module fspi_slave_top
(
    input    wire             sys_clk         ,    // 25MHz
    input    wire             rst_n           ,

    input    wire             spi_clk         ,
    input    wire             spi_cs          ,
    inout    wire    [3:0]    spi_data        ,
    input    wire    [3:0]    select_rearview ,    // GPIO 值（历史沿用，实为 cnn_level）
    output   wire    [7:0]    mem0_out        ,
    output   wire    [7:0]    mem1_out        ,
    output   wire    [7:0]    mem2_out        ,
    output   wire    [7:0]    mem3_out        ,
    output   wire    [7:0]    mem4_out
);

wire    [1:0]   cmd;
wire            tx_data_valid;
wire    [7:0]   tx_data;
wire            rx_data_valid;
wire    [7:0]   rx_data;
wire            wr_clk;    // 4 倍频时钟（SPI 通道 100MHz 时需 400MHz）

clk_fspi clk_gen_inst (
  .clkout0(wr_clk),    // output
  .lock(),             // output
  .clkin1(sys_clk)     // input
);

// 读写 ram 控制
spi_ram_crtl u_spi_ram_crtl(
    .wr_clk         ( wr_clk         ),
    .rst_n          ( rst_n          ),
    .spi_cs         ( spi_cs         ),
    .cmd            ( cmd            ),
    .tx_done        ( tx_data_valid  ),
    .wr_data        ( rx_data        ),
    .rx_valid       ( rx_data_valid  ),
    .select_rearview( select_rearview),
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
