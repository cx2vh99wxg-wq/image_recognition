// 2026-11-03 注释校订。M 端传输适配层。共用 FSPI 引擎，并为状态返回追加锁存的 IMU 快照。
module	fspi_slave_m_imu
(
    input wire [511:0] imu_snapshot,
	input	wire			spi_clk			,
	input	wire			spi_cs			,
	inout	wire	[3:0]	spi_data		,
	input	wire			rst_n			,
	input	wire	[3:0]		select_rearview	,
    input   wire            auto_brake_enable,
    input   wire [5:0]      speed_status_async,
    input   wire [15:0]     control_status_async,

	output	wire	[1:0]	cmd				,
	output	wire	[7:0]	rx_data			,
	output	wire			rx_data_valid	,

	input	wire	[7:0]	tx_data			,
	output	wire			tx_data_valid

);

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
