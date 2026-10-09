`timescale 1ns/1ps
// Unit simulation only: FIFO/DDR/PCIe IP internals are not modeled here.
module W_FIFO_16i_128o(input wr_clk,wr_rst,wr_en, input [15:0] wr_data,
    input rd_clk,rd_rst,rd_en, output [127:0] rd_data, output rd_empty,almost_empty);
    assign rd_data=0; assign rd_empty=1; assign almost_empty=1;
endmodule
module R_FIFO_128i_128o(input wr_clk,wr_rst,wr_en,input [127:0] wr_data,
    output almost_full,input rd_clk,rd_rst,rd_en,output [127:0] rd_data);
    assign almost_full=0; assign rd_data=0;
endmodule
module test_stereo_axi;
    reg clk=0, reset=1;
    always #5 clk=~clk;
    reg [2:0] vs=0;
    wire [31:0] address;
    stereo_axi_ctrl dut (
        .axi_clk(clk),.axi_reset(reset),.axi_awready(1'b0),.axi_wready(1'b0),
        .axi_bid(4'd0),.axi_bresp(2'd0),.axi_bvalid(1'b0),
        .axi_arready(1'b0),.axi_rid(4'd0),.axi_rdata(128'd0),
        .axi_rresp(2'd0),.axi_rlast(1'b0),.axi_rvalid(1'b0),.axi_araddr(address),
        .wframe_pclk({3{clk}}),.wframe_vsync(vs),.wframe_data_en(3'b0),
        .wframe_data0(16'hf800),.wframe_data1(16'h0000),.wframe_data2(16'h001f),
        .rframe_pclk(clk),.rframe_vsync(1'b0),.rframe_data_en(1'b0)
    );
    task pulse(input [2:0] channels);
        begin @(negedge clk); vs=channels; repeat(5) @(negedge clk);
        vs=0; repeat(5) @(negedge clk); end
    endtask
    task snapshot;
        begin force dut.r_rfifo_rst_axi=1'b1; repeat(3) @(negedge clk);
        release dut.r_rfifo_rst_axi; repeat(3) @(negedge clk); end
    endtask
    initial begin
        #32; reset=0; repeat(40) @(negedge clk);
        // Left advances without a third physical sensor or a right sensor.
        repeat(4) pulse(3'b011);
        snapshot();
        if (dut.read_ready !== 3'b011 || dut.read_bank[0] !== 2'd2)
            $fatal(1,"left must progress without a third sensor");
        // Right starts later; independent banks are intentionally different.
        repeat(3) pulse(3'b100);
        snapshot();
        if(dut.read_bank[0] !== 2'd2 || dut.read_bank[2] !== 2'd1)
            $fatal(1,"independent camera bank selection failed");
        force dut.araddr_offset=22'd0; #1;
        if(address !== 32'd1228800) $fatal(1,"left DDR bank address");
        force dut.araddr_offset=22'd640; #1;
        if(address !== 32'd1229440) $fatal(1,"empty center bank address");
        force dut.araddr_offset=22'd307200; #1;
        if(address !== 32'd921600) $fatal(1,"right DDR bank address");
        release dut.araddr_offset;
        pulse(3'b011);
        if(dut.read_bank[0] !== 2'd2) $fatal(1,"bank changed mid-read");
        snapshot();
        if(dut.read_bank[0] !== 2'd3 || dut.read_bank[2] !== 2'd1)
            $fatal(1,"snapshot did not retain independent banks");
        $display("test_stereo_axi: PASS (missing sensor, independent banks, tile addresses, stable read snapshot)");
        $finish;
    end
endmodule
