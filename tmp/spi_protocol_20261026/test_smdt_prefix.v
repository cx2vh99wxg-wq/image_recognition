`timescale 1ns/1ps
// Compare host payloads when the controller prefixes write opcode 00.
// This is a transport hypothesis test, not a measurement of the SMDT driver.
module test_smdt_prefix;
  reg wr_clk=0; always #1.25 wr_clk=~wr_clk;
  reg rst_n=0, spi_clk=1, spi_cs=1;
  reg [3:0] master_data=0;
  tri [3:0] spi_data;
  assign spi_data=master_data;
  wire [1:0] cmd;
  wire [7:0] rx_data,tx_data;
  wire rx_valid,tx_valid;
  wire [7:0] m0,m1,m2,m3,m4;
  fspi_slave slave(.spi_clk(spi_clk),.spi_cs(spi_cs),.spi_data(spi_data),.rst_n(rst_n),
    .select_rearview(4'h0),.auto_brake_enable(1'b0),.cmd(cmd),.rx_data(rx_data),
    .rx_data_valid(rx_valid),.tx_data(tx_data),.tx_data_valid(tx_valid));
  spi_ram_crtl regs(.wr_clk(wr_clk),.rst_n(rst_n),.spi_cs(spi_cs),.cmd(cmd),
    .tx_done(tx_valid),.wr_data(rx_data),.rx_valid(rx_valid),.select_rearview(4'h0),
    .rd_data(tx_data),.mem0_out(m0),.mem1_out(m1),.mem2_out(m2),.mem3_out(m3),.mem4_out(m4));
  task byte_out(input [7:0] b);
    begin
      master_data=b[7:4]; #20 spi_clk=0; #20 spi_clk=1; #2;
      master_data=b[3:0]; #20 spi_clk=0; #20 spi_clk=1; #2;
    end
  endtask
  task host_write(input [7:0] addr,input [7:0] data,input redundant_opcode);
    begin
      spi_cs=0; #20;
      byte_out(8'h00); // controller-generated command
      if(redundant_opcode)byte_out(8'h00);
      byte_out(8'h00); byte_out(addr); byte_out(data);
      if(redundant_opcode)byte_out(8'h00);
      #20 spi_cs=1; #100;
    end
  endtask
  initial begin
    #20 rst_n=1; #50;
    host_write(4,1,1);
    if(m0!==4||m4!==1)$fatal(1,"redundant opcode reproduction mismatch");
    $display("REPRO: host reg4=1 becomes reg0=4, GO active-low bit becomes 0");
    host_write(0,1,0);
    if(m0!==1)$fatal(1,"SDK release GO failed");
    host_write(2,0,1);
    if(m0!==2||m2!==1)$fatal(1,"redundant LEFT reproduction mismatch");
    $display("REPRO: host LEFT=0 becomes reg0=2; LEFT remains released");
    host_write(3,0,1);
    if(m0!==3||m3!==1)$fatal(1,"redundant RIGHT reproduction mismatch");
    $display("REPRO: host RIGHT=0 becomes reg0=3; GO released and RIGHT remains released");
    host_write(0,1,0); host_write(2,0,0);
    if(m0!==1||m2!==0)$fatal(1,"SDK LEFT failed");
    host_write(2,1,0); host_write(3,0,0);
    if(m0!==1||m2!==1||m3!==0)$fatal(1,"SDK RIGHT failed");
    $display("PASS: prefixed-controller model reproduces unintended GO and absent steering; SDK payload selects correct registers");
    $finish;
  end
endmodule
