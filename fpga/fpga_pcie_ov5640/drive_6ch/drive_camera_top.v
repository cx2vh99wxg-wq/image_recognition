// DOUBLE-DVP-OV5640 J17 -> RK3568_MES2L100H baseboard J8.
// Module crystal supplies 24 MHz XCLK; FPGA free_clk is 25 MHz.
`timescale 1ns / 1ps
module drive_camera_top (
    input sys_clk, input reset_n,
    inout cmos2_scl, inout cmos2_sda,
    input cmos2_vsync, input cmos2_href, input cmos2_pclk,
    input [7:0] cmos2_data, output cmos2_reset,
    inout cmos5_scl, inout cmos5_sda,
    input cmos5_vsync, input cmos5_href, input cmos5_pclk,
    input [7:0] cmos5_data, output cmos5_reset,
    inout cmos6_scl, inout cmos6_sda,
    input cmos6_vsync, input cmos6_href, input cmos6_pclk,
    input [7:0] cmos6_data, output cmos6_reset,
    output video2_clk,output video2_vs,output video2_de,output [15:0] video2_data,
    output video5_clk, output video5_vs, output video5_de,
    output [15:0] video5_data,
    output video6_clk, output video6_vs, output video6_de,
    output [15:0] video6_data
);
    wire clk25, clk50, locked;
    clk_1080p_gen u_pll (
        .clkin1(sys_clk), .clkout0(), .clkout1(clk25),
        .clkout2(clk50), .lock(locked)
    );
    wire power_reset_n = reset_n && locked;
    reg [20:0] power_count;
    always @(posedge clk50 or negedge power_reset_n) begin
        if (!power_reset_n) power_count <= 0;
        else if (!(&power_count)) power_count <= power_count + 1'b1;
    end
    // PWDN is tied low by R5/R6 on the camera module.
    assign cmos5_reset = power_reset_n && (power_count >= 21'd262144);
    assign cmos6_reset = cmos5_reset;
    wire initial_en = (power_count >= 21'd1572864);
    wire init2, init5, init6, scl2, scl5, scl6;
    assign cmos2_reset=cmos5_reset;
    assign cmos2_scl=scl2?1'bz:1'b0;
    // Use the module's 2.8 V SCCB pull-ups instead of driving SCL high at 3.3 V.
    assign cmos5_scl = scl5 ? 1'bz : 1'b0;
    assign cmos6_scl = scl6 ? 1'bz : 1'b0;
    stereo_reg_config cfg2 (
        .clk_25M(clk25), .camera_rstn(cmos2_reset), .initial_en(initial_en),
        .cmos_h_pixel(13'd640), .cmos_v_pixel(13'd480),
        .total_h_pixel(13'h768), .total_v_pixel(13'h3d8),
        .rate(8'h41), .y_addr_st(13'd4), .y_addr_end(13'd1947),
        .reg_conf_done(init2), .i2c_sclk(scl2), .i2c_sdat(cmos2_sda),
        .clock_20k(), .reg_index()
    );
    stereo_reg_config cfg5 (
        .clk_25M(clk25), .camera_rstn(cmos5_reset), .initial_en(initial_en),
        .cmos_h_pixel(13'd640), .cmos_v_pixel(13'd480),
        .total_h_pixel(13'h768), .total_v_pixel(13'h3d8),
        .rate(8'h41), .y_addr_st(13'd4), .y_addr_end(13'd1947),
        .reg_conf_done(init5), .i2c_sclk(scl5), .i2c_sdat(cmos5_sda),
        .clock_20k(), .reg_index()
    );
    stereo_reg_config cfg6 (
        .clk_25M(clk25), .camera_rstn(cmos6_reset), .initial_en(initial_en),
        .cmos_h_pixel(13'd640), .cmos_v_pixel(13'd480),
        .total_h_pixel(13'h768), .total_v_pixel(13'h3d8),
        .rate(8'h41), .y_addr_st(13'd4), .y_addr_end(13'd1947),
        .reg_conf_done(init6), .i2c_sclk(scl6), .i2c_sdat(cmos6_sda),
        .clock_20k(), .reg_index()
    );
    // SCCB completion and power reset originate outside the sensor domain.
    // Keep asynchronous assertion, but release capture on two local PCLK edges.
    wire capture2_rst_n, capture5_rst_n, capture6_rst_n;
    drive_reset_sync reset2(cmos2_pclk, init2 && cmos2_reset, capture2_rst_n);
    drive_reset_sync reset5(cmos5_pclk, init5 && cmos5_reset, capture5_rst_n);
    drive_reset_sync reset6(cmos6_pclk, init6 && cmos6_reset, capture6_rst_n);
    // The outputs of each capture block remain in that sensor's PCLK domain.
    cmos_capture_data capture2 (
        .rst_n(capture2_rst_n), .cam_pclk(cmos2_pclk),
        .cam_vsync(cmos2_vsync), .cam_href(cmos2_href), .cam_data(cmos2_data),
        .active_x(), .active_y(), .cmos_frame_vsync(video2_vs),
        .cmos_frame_href(), .cmos_frame_valid(video2_de), .cmos_frame_data(video2_data)
    );
    cmos_capture_data capture5 (
        .rst_n(capture5_rst_n), .cam_pclk(cmos5_pclk),
        .cam_vsync(cmos5_vsync), .cam_href(cmos5_href), .cam_data(cmos5_data),
        .active_x(), .active_y(), .cmos_frame_vsync(video5_vs),
        .cmos_frame_href(), .cmos_frame_valid(video5_de), .cmos_frame_data(video5_data)
    );
    cmos_capture_data capture6 (
        .rst_n(capture6_rst_n), .cam_pclk(cmos6_pclk),
        .cam_vsync(cmos6_vsync), .cam_href(cmos6_href), .cam_data(cmos6_data),
        .active_x(), .active_y(), .cmos_frame_vsync(video6_vs),
        .cmos_frame_href(), .cmos_frame_valid(video6_de), .cmos_frame_data(video6_data)
    );
    assign video2_clk=cmos2_pclk;
    assign video5_clk = cmos5_pclk;
    assign video6_clk = cmos6_pclk;
endmodule

module drive_reset_sync(input clk, input async_rst_n, output rst_n);
    (* ASYNC_REG="TRUE" *) reg [1:0] sync_ff;
    always @(posedge clk or negedge async_rst_n) begin
        if (!async_rst_n) sync_ff <= 2'b00;
        else sync_ff <= {sync_ff[0], 1'b1};
    end
    assign rst_n = sync_ff[1];
endmodule
