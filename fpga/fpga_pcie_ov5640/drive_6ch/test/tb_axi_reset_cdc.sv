`timescale 1ns/1ps
// FIFO data paths are stubbed by test_stereo_axi.sv. This test checks reset
// behavior with unrelated clocks, a stopped sensor and an in-flight restart.
module tb_axi_reset_cdc;
    reg axi_clk=0, rd_clk=0, reset=0, run_camera2=0;
    reg [2:0] pixel_clk=0;
    always #3.8095 axi_clk=~axi_clk;
    always #8 rd_clk=~rd_clk;
    always #9 pixel_clk[0]=~pixel_clk[0];
    always #11 pixel_clk[1]=~pixel_clk[1];
    always #13 if(run_camera2) pixel_clk[2]=~pixel_clk[2];
    wire capture_rst_n;
    drive_reset_sync capture_reset(pixel_clk[2], !reset, capture_rst_n);
    stereo_axi_ctrl dut (
        .axi_clk(axi_clk),.axi_reset(reset),.axi_awready(1'b0),.axi_wready(1'b0),
        .axi_bid(4'd0),.axi_bresp(2'd0),.axi_bvalid(1'b0),
        .axi_arready(1'b0),.axi_rid(4'd0),.axi_rdata(128'd0),
        .axi_rresp(2'd0),.axi_rlast(1'b0),.axi_rvalid(1'b0),
        .wframe_pclk(pixel_clk),.wframe_vsync(3'd0),.wframe_data_en(3'b111),
        .wframe_data0(16'd0),.wframe_data1(16'd0),.wframe_data2(16'd0),
        .rframe_pclk(rd_clk),.rframe_vsync(1'b0),.rframe_data_en(1'b0)
    );
    initial begin
        #2; reset=1; #1;
        if(dut.wr_local_reset!==3'b111) $fatal(1,"asynchronous assertion failed");
        if(capture_rst_n!==0) $fatal(1,"capture reset assertion failed");
        #34; reset=0;
        #100;
        if(dut.wr_local_reset!==3'b100) $fatal(1,"stopped PCLK released reset");
        if(capture_rst_n!==0) $fatal(1,"stopped capture reset released");
        if(dut.pix_h[2]!==0) $fatal(1,"stopped camera counter was not reset");
        run_camera2=1;
        @(posedge pixel_clk[2]); #1;
        if(!dut.wr_local_reset[2]) $fatal(1,"reset released after only one edge");
        if(capture_rst_n!==0) $fatal(1,"capture released after one edge");
        @(posedge pixel_clk[2]); #1;
        if(dut.wr_local_reset[2]) $fatal(1,"reset did not release after second edge");
        if(capture_rst_n!==1) $fatal(1,"capture did not release after two edges");
        #1000;
        if(dut.r_rfifo_rst_axi!==0 || dut.w_rfifo_rst!==0) $fatal(1,"read FIFO stuck in reset");
        #3; reset=1; #1;
        if(dut.wr_local_reset!==3'b111 || dut.r_rfifo_rst_axi!==1) $fatal(1,"restart assertion failed");
        #25; reset=0;
        #1000;
        if(dut.wr_local_reset!==0 || dut.r_rfifo_rst_axi!==0) $fatal(1,"restart release failed");
        $display("PASS: AXI CDC reset (independent clocks, stopped sensor, two-edge release, restart)");
        $finish;
    end
    initial begin #10000; $fatal(1,"reset test timed out"); end
endmodule
