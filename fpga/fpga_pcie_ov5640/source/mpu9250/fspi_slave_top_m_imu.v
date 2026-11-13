// 2026-11-03 注释校订。连接 M 端 FSPI、控制寄存器和 IMU 快照输入，保留板级时钟路径。
module fspi_slave_top_m_imu
    (input wire [511 : 0] imu_snapshot,
     input wire sys_clk,

     input wire rst_n,

     input wire spi_clk,
     input wire spi_cs,
     inout wire [3 : 0] spi_data,
     input wire [3 : 0] select_rearview,

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

    wire wr_clk;
    clk_fspi clk_gen_inst(.clkout0(wr_clk), .lock(), .clkin1(sys_clk));
    fspi_register_endpoint #(.HAS_IMU(1'b1)) u_register_endpoint (
        .wr_clk(wr_clk),
        .rst_n(rst_n),
        .spi_clk(spi_clk),
        .spi_cs(spi_cs),
        .spi_data(spi_data),
        .select_rearview(select_rearview),
        .camera_view_async(4'd0),
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
        .imu_snapshot(imu_snapshot)
    );
endmodule
