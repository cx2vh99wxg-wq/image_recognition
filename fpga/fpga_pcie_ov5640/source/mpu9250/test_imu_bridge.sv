`timescale 1ns/1ps
module test_imu_bridge;
  reg clk=0,rst_n=0,spi_clk=1,spi_cs=1;
  always #20 clk=~clk;
  reg [3:0] host_nibble=0;
  reg host_drive=1;
  tri [3:0] spi_data;
  assign spi_data=host_drive?host_nibble:4'bzzzz;
  tri1 scl,sda;
  reg slave_low=0,present=1;
  assign sda=slave_low?1'b0:1'bz;
  wire [511:0] packet;
  wire [1:0] cmd;wire [7:0] rx;wire rx_valid,tx_valid;
  mpu9250_sampler dut(.clk(clk),.rst_n(rst_n),.spi_clk(spi_clk),.spi_cs(spi_cs),
    .spi_data(spi_data),.mpu_scl(scl),.mpu_sda(sda),.snapshot(packet));
  fspi_slave_m_imu spi(.imu_snapshot(packet),.spi_clk(spi_clk),.spi_cs(spi_cs),
    .spi_data(spi_data),.rst_n(rst_n),.select_rearview(4'h9),.auto_brake_enable(1'b0),
    .speed_status_async(6'b100000),.control_status_async(16'h800f),.cmd(cmd),
    .rx_data(rx),.rx_data_valid(rx_valid),.tx_data(8'd0),.tx_data_valid(tx_valid));
  byte registers[256];
  integer writes=0,reads=0;
  reg [7:0] addr,regptr,value,rxbyte;
  task automatic receive_byte(output reg[7:0] v);
    integer k;
    begin
      v=0;
      for(k=7;k>=0;k=k-1)begin @(posedge scl);v[k]=sda;end
      @(negedge scl);slave_low=present;
      @(negedge scl);slave_low=0;
    end
  endtask
  task automatic send_byte(input reg[7:0] v,output reg acknowledged);
    integer k;
    begin
      for(k=7;k>=0;k=k-1)begin
        slave_low=!v[k];@(posedge scl);@(negedge scl);
      end
      slave_low=0;@(posedge scl);acknowledged=!sda;@(negedge scl);
    end
  endtask
  reg ack;
  integer j;
  // Independent bus-level slave: verify address, register pointer, repeated
  // START, all ACKs, sequential read bytes and the master's final NACK.
  initial forever begin
    @(negedge sda);
    if(scl&&rst_n&&present&&!slave_low)begin
      receive_byte(addr);
      if(addr!=8'hd0)$fatal(1,"wrong write address %x",addr);
      receive_byte(regptr);
      if(dut.read_op)begin
        @(negedge sda);
        if(!scl)$fatal(1,"missing repeated START");
        receive_byte(addr);
        if(addr!=8'hd1)$fatal(1,"wrong read address");
        for(j=0;j<dut.read_length;j=j+1)begin
          send_byte(registers[regptr+j],ack);
          if(ack!=(j+1<dut.read_length))$fatal(1,"read ACK/NACK index %d",j);
        end
        reads=reads+1;
      end else begin
        receive_byte(value);registers[regptr]=value;writes=writes+1;
      end
      @(posedge sda);
    end
  end
  task automatic quad_byte(input reg[7:0] b);
    begin
      host_nibble=b[7:4];#100;spi_clk=0;#100;spi_clk=1;#100;
      host_nibble=b[3:0];#100;spi_clk=0;#100;spi_clk=1;#100;
    end
  endtask
  task automatic write_command(input reg[15:0] a,input reg[7:0] v);
    begin
      host_drive=1;spi_cs=0;#100;quad_byte(0);quad_byte(a[15:8]);quad_byte(a[7:0]);quad_byte(v);
      #100;spi_cs=1;#5000;
    end
  endtask
  reg [7:0] wire_bytes[72];
  task automatic read_packet;
    integer n;
    begin
      host_drive=1;spi_cs=0;#100;quad_byte(8'h80);host_drive=0;
      for(n=0;n<144;n=n+1)begin
        #100;spi_clk=0;#100;spi_clk=1;#20;
        if(n%2==0)wire_bytes[n/2][7:4]=spi_data;
        else wire_bytes[n/2][3:0]=spi_data;
        #80;
      end
      spi_cs=1;host_drive=1;#100;
    end
  endtask
  function automatic [31:0] crc32(input reg[479:0] p);
    reg[31:0] c;integer b,k;
    begin
      c=32'hffffffff;
      for(b=0;b<60;b=b+1)begin
        c=c^{24'd0,p[479-8*b -:8]};
        for(k=0;k<8;k=k+1)c=c[0]?(c>>1)^32'hedb88320:c>>1;
      end
      crc32=c^32'hffffffff;
    end
  endfunction
  reg [511:0] received;
  reg [31:0] first_seq;
  initial begin
    for(integer k=0;k<256;k=k+1)registers[k]=0;
    registers[8'h75]=8'h71;registers[8'h3a]=1;
    registers[8'h3b]=8'h12;registers[8'h3c]=8'h34;
    registers[8'h3f]=8'h40;registers[8'h47]=8'hff;registers[8'h48]=8'h7d;
    #200;rst_n=1;#10000;
    if(packet[511:480]!=32'h494d5531||packet[463:456]!=0)$fatal(1,"boot header/off");
    if(packet[31:0]!=crc32(packet[511:32]))$fatal(1,"boot CRC");
    write_command(16'h4d00,8'ha5);
    #250000000;
    write_command(16'h4d01,8'ha5);#10000;
    if(packet[459:456]!=7||packet[447:440]!=8'h71)$fatal(1,"identity/status %h",packet[463:440]);
    if(writes!=10||registers[8'h19]!=9||registers[8'h1a]!=3)$fatal(1,"configuration");
    if(packet[255:240]!=16'h1234||packet[159:144]!=16'hff7d)$fatal(1,"coherent sensor bytes");
    first_seq=packet[415:384];
    read_packet();
    if(wire_bytes[0]!=8'h99||wire_bytes[1]!=8'ha7||wire_bytes[4]!=8'hd7)$fatal(1,"legacy prefix changed");
    received=0;
    for(integer k=0;k<64;k=k+1)received[511-8*k -:8]=wire_bytes[k+8];
    if(received!==packet)$fatal(1,"SPI snapshot differs %h / %h",received[511:448],packet[511:448]);
    if(received[31:0]!=crc32(received[511:32]))$fatal(1,"wire CRC");
    #15000000;
    if(packet[415:384]!=first_seq)$fatal(1,"snapshot changed without capture");
    write_command(16'h4d01,8'ha5);#10000;
    if(packet[415:384]<=first_seq)$fatal(1,"new sequence missing");
    // No device: must clear validity, retain real failure, never fabricate 0deg.
    present=0;#60000000;write_command(16'h4d01,8'ha5);#10000;
    if(packet[458]||packet[455:448]==0)$fatal(1,"missing sensor accepted");
    write_command(16'h4d00,0);#10000;write_command(16'h4d01,8'ha5);#10000;
    if(packet[456])$fatal(1,"disable failed");
    $display("PASS IMU RTL: bus ACK/NACK, repeated START, init, coherent raw bytes, CRC, frozen snapshot, legacy prefix, disconnect, disable");
    $finish;
  end
  initial begin #600000000;$fatal(1,"test timeout");end
endmodule
