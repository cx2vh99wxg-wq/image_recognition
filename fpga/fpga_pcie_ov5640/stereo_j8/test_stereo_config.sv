`timescale 1ns/1ps
module test_stereo_config;
    reg clk=0, reset_n=0, initial_en=0, acknowledge=0;
    always #20 clk=~clk;
    tri1 sda;
    wire [8:0] index;
    wire done;
    stereo_reg_config dut (
        .clk_25M(clk),.camera_rstn(reset_n),.initial_en(initial_en),
        .cmos_h_pixel(13'd640),.cmos_v_pixel(13'd480),
        .total_h_pixel(13'h768),.total_v_pixel(13'h3d8),.rate(8'h41),
        .y_addr_st(13'd4),.y_addr_end(13'd1947),.reg_conf_done(done),
        .i2c_sclk(),.i2c_sdat(sda),.clock_20k(),.reg_index(index)
    );
    wire ack_cycle = (dut.u1.cyc_count==11 || dut.u1.cyc_count==12 ||
                      dut.u1.cyc_count==20 || dut.u1.cyc_count==21 ||
                      dut.u1.cyc_count==29 || dut.u1.cyc_count==30 ||
                      dut.u1.cyc_count==38 || dut.u1.cyc_count==39);
    assign sda=(acknowledge && ack_cycle) ? 1'b0 : 1'bz;
    initial begin
        #200; reset_n=1;
        #6000000;
        if(index !== 0 || dut.start !== 0) $fatal(1,"power-on delay ignored");
        initial_en=1;
        #10000000;
        if(index !== 0 || done !== 0) $fatal(1,"NACK accepted as a configured camera");
        acknowledge=1;
        #16000000;
        if(index < 2 || index === 9'bx) $fatal(1,"ACK did not advance configuration");
        reset_n=0; #1;
        if(index !== 0 || dut.u1.cyc_count !== 63) $fatal(1,"async reset failed");
        $display("test_stereo_config: PASS (power delay, NACK retry, ACK progression, reset)");
        $finish;
    end
endmodule
