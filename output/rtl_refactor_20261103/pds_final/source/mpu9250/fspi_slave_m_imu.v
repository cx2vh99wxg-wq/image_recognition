// 采用四线 SPI 接口。
module	fspi_slave_m_imu
(
    input wire [511:0] imu_snapshot,
	input	wire			spi_clk			,
	input	wire			spi_cs			,
	inout	wire	[3:0]	spi_data		,
	input	wire			rst_n			,
	input	wire	[3:0]		select_rearview	,	// 本信号用于从顶层传入的select_rearview。
    input   wire            auto_brake_enable,
    input   wire [5:0]      speed_status_async,
    input   wire [15:0]     control_status_async,

	// user：本节对应的标识或作用域。
	output	wire	[1:0]	cmd				,	// IMU 硬件：2'b10:写 2'b01:读
	output	wire	[7:0]	rx_data			,
	output	wire			rx_data_valid	,

	input	wire	[7:0]	tx_data			,
	output	wire			tx_data_valid	

);
// 2026-11-03: S and M share one quad-SPI command and response engine.
    fspi_transport #(.HAS_IMU(1'b1)) u_transport (
        .spi_clk(spi_clk),
        .spi_cs(spi_cs),
        .spi_data(spi_data),
        .rst_n(rst_n),
        .select_rearview(select_rearview),
        .auto_brake_enable(auto_brake_enable),
        .speed_status_async(speed_status_async),
        .control_status_async(control_status_async),
        .cmd(cmd),
        .rx_data(rx_data),
        .rx_data_valid(rx_data_valid),
        .tx_data(tx_data),
        .tx_data_valid(tx_data_valid),
        .imu_snapshot(imu_snapshot), .status_tx_started_dbg()
    );
endmodule
