`timescale 1ns/1ps
module test_fspi_control;
    reg wr_clk = 0;
    always #1.25 wr_clk = ~wr_clk;  // 400 MHz register clock from clk_fspi

    reg rst_n = 0;
    reg spi_clk = 1;
    reg spi_cs = 1;
    reg [3:0] master_data = 4'hf;
    reg master_drive = 1;
    tri [3:0] spi_data;
    assign spi_data = master_drive ? master_data : 4'bzzzz;

    wire [1:0] cmd;
    wire [7:0] rx_data;
    wire rx_data_valid;
    wire [7:0] tx_data;
    wire tx_data_valid;
    wire [7:0] mem0_out, mem1_out, mem2_out, mem3_out, mem4_out;

    fspi_slave u_slave (
        .spi_clk(spi_clk), .spi_cs(spi_cs), .spi_data(spi_data),
        .rst_n(rst_n), .select_rearview(4'ha), .cmd(cmd),
        .rx_data(rx_data), .rx_data_valid(rx_data_valid),
        .tx_data(tx_data), .tx_data_valid(tx_data_valid)
    );

    spi_ram_crtl u_regs (
        .wr_clk(wr_clk), .rst_n(rst_n), .spi_cs(spi_cs),
        .cmd(cmd), .tx_done(tx_data_valid), .wr_data(rx_data),
        .rx_valid(rx_data_valid), .select_rearview(4'ha),
        .rd_data(tx_data), .mem0_out(mem0_out), .mem1_out(mem1_out),
        .mem2_out(mem2_out), .mem3_out(mem3_out), .mem4_out(mem4_out)
    );

    task send_nibble(input [3:0] value);
    begin
        master_data = value;
        #10 spi_clk = 0;
        #10 spi_clk = 1;
        #2;
    end
    endtask

    task send_byte(input [7:0] value);
    begin
        send_nibble(value[7:4]);
        send_nibble(value[3:0]);
    end
    endtask

    task write_reg(input [7:0] addr, input [7:0] value);
    begin
        spi_cs = 0;
        #20;
        send_byte(8'h00);
        send_byte(8'h00);
        send_byte(addr);
        send_byte(value);
        send_byte(8'h00);  // the S-side driver appends this commit byte
        #20 spi_cs = 1;
        #100;
    end
    endtask

    task expect_byte(input [7:0] actual, input [7:0] expected, input [127:0] label);
    begin
        if (actual !== expected) begin
            $display("FAIL %s: actual=%02h expected=%02h", label, actual, expected);
            $fatal(1);
        end
    end
    endtask

    initial begin
        #20 rst_n = 1;
        #50;
        expect_byte(mem0_out, 8'h01, "reset go released");
        expect_byte(mem1_out, 8'h01, "reset back released");
        expect_byte(mem2_out, 8'h01, "reset left released");
        expect_byte(mem3_out, 8'h01, "reset right released");
        expect_byte(mem4_out, 8'h01, "reset stop released");
        write_reg(8'h04, 8'h01);
        expect_byte(mem4_out, 8'h01, "release stop");
        write_reg(8'h00, 8'h00);
        expect_byte(mem0_out, 8'h00, "press go");
        expect_byte(mem1_out, 8'h01, "back still released");
        write_reg(8'h00, 8'h01);
        expect_byte(mem0_out, 8'h01, "release go");
        write_reg(8'h01, 8'h00);
        expect_byte(mem1_out, 8'h00, "press back");
        expect_byte(mem0_out, 8'h01, "go still released");
        write_reg(8'h01, 8'h01);
        expect_byte(mem1_out, 8'h01, "release back");
        write_reg(8'h02, 8'h00);
        expect_byte(mem2_out, 8'h00, "press left");
        expect_byte(mem3_out, 8'h01, "right still released");
        write_reg(8'h02, 8'h01);
        expect_byte(mem2_out, 8'h01, "release left");
        write_reg(8'h03, 8'h00);
        expect_byte(mem3_out, 8'h00, "press right");
        expect_byte(mem2_out, 8'h01, "left still released");
        write_reg(8'h03, 8'h01);
        expect_byte(mem3_out, 8'h01, "release right");
        write_reg(8'h04, 8'h00);
        expect_byte(mem4_out, 8'h00, "assert stop");
        write_reg(8'h04, 8'h01);
        expect_byte(mem4_out, 8'h01, "release again");
        $display("PASS: quad SPI writes update all five control registers");
        $finish;
    end
endmodule
