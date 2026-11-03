
// 旧说明含编码损坏，原记录为：Ŀǰram�������2048
// 旧说明含编码损坏，原记录为：��дʱ����400MHZ
// 旧说明含编码损坏，原记录为：��� RK�����д��ͨ������ҪС��100mhz ��д��СҪС��2048
module fspi_slave_top
    (input wire sys_clk,
     // 参数记录为 25mhz。
     input wire rst_n,

     input wire spi_clk,
     input wire spi_cs,
     inout wire [3 : 0] spi_data,
     input wire [3 : 0] select_rearview,
     // GPIO接收到的一位值：本节对应的标识或作用域。
     input wire auto_brake_enable,
     input wire [5 : 0] speed_status_async,
     input wire [15 : 0] control_status_async,
     output wire [7 : 0] mem0_out,
     output wire [7 : 0] mem1_out,
     output wire [7 : 0] mem2_out,
     output wire [7 : 0] mem3_out,
     output wire [7 : 0] mem4_out,
     output wire [7 : 0] mem5_out,
     output wire [7 : 0] mem6_out);

    // 2026-11-03: share the register endpoint and keep the PLL at board level.
    wire wr_clk;
    clk_fspi clk_gen_inst(.clkout0(wr_clk), .lock(), .clkin1(sys_clk));
    fspi_register_endpoint #(.HAS_IMU(1'b0)) u_register_endpoint (
        .wr_clk(wr_clk),
        .rst_n(rst_n),
        .spi_clk(spi_clk),
        .spi_cs(spi_cs),
        .spi_data(spi_data),
        .select_rearview(select_rearview),
        .auto_brake_enable(auto_brake_enable),
        .speed_status_async(speed_status_async),
        .control_status_async(control_status_async),
        .mem0_out(mem0_out),
        .mem1_out(mem1_out),
        .mem2_out(mem2_out),
        .mem3_out(mem3_out),
        .mem4_out(mem4_out),
        .mem5_out(mem5_out),
        .mem6_out(mem6_out),
        .imu_snapshot(512'd0)
    );
endmodule

module fspi_register_endpoint #(parameter HAS_IMU = 1'b0)
    (input wire wr_clk,
     input wire [511:0] imu_snapshot,
     // 参数记录为 25mhz。
     input wire rst_n,

     input wire spi_clk,
     input wire spi_cs,
     inout wire [3 : 0] spi_data,
     input wire [3 : 0] select_rearview,
     // GPIO接收到的一位值：本节对应的标识或作用域。
     input wire auto_brake_enable,
     input wire [5 : 0] speed_status_async,
     input wire [15 : 0] control_status_async,
     output wire [7 : 0] mem0_out,
     output wire [7 : 0] mem1_out,
     output wire [7 : 0] mem2_out,
     output wire [7 : 0] mem3_out,
     output wire [7 : 0] mem4_out,
     output wire [7 : 0] mem5_out,
     output wire [7 : 0] mem6_out);


    wire [1:0] cmd;
    wire tx_data_valid, rx_data_valid;
    wire [7:0] tx_data, rx_data;

    spi_ram_crtl u_spi_ram_crtl (
        .wr_clk(wr_clk), .rst_n(rst_n), .spi_cs(spi_cs), .cmd(cmd),
        .tx_done(tx_data_valid), .wr_data(rx_data), .rx_valid(rx_data_valid),
        .select_rearview(select_rearview), .rd_data(tx_data),
        .mem0_out(mem0_out), .mem1_out(mem1_out), .mem2_out(mem2_out),
        .mem3_out(mem3_out), .mem4_out(mem4_out), .mem5_out(mem5_out),
        .mem6_out(mem6_out)
    );

    fspi_transport #(.HAS_IMU(HAS_IMU)) u_transport (
        .spi_clk(spi_clk), .spi_cs(spi_cs), .spi_data(spi_data), .rst_n(rst_n),
        .select_rearview(select_rearview), .auto_brake_enable(auto_brake_enable),
        .speed_status_async(speed_status_async), .control_status_async(control_status_async),
        .cmd(cmd), .rx_data(rx_data), .rx_data_valid(rx_data_valid),
        .tx_data(tx_data), .tx_data_valid(tx_data_valid), .imu_snapshot(imu_snapshot)
    );
endmodule
