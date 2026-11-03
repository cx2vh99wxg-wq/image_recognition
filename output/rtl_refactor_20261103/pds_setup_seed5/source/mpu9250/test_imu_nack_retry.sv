`timescale 1ns/1ps
// IMU 硬件：Sampler policy unit test. The original bridge test separately exercises the
// IMU 硬件：real bit-level I2C engine and wire CRC. This stub injects transaction errors.
module imu_i2c_master #(parameter CLK_HZ=25000000)(
  input wire clk,rst_n,start,read_op,input wire[6:0] address,
  input wire[7:0] reg_address,write_value,input wire[4:0] read_length,
  inout wire scl,sda,output reg busy,done,output reg[7:0] error,
  output reg[119:0] read_data
);
  integer inject_nacks=0,inject_other=0,writes=0,reads=0;
  reg[7:0] captured_reg;reg captured_read;
  reg[3:0] cycles;
  assign scl=1'bz;assign sda=1'bz;
  always @(posedge clk or negedge rst_n)begin
    if(!rst_n)begin busy<=0;done<=0;error<=0;read_data<=0;cycles<=0;captured_reg<=0;captured_read<=0;end
    else begin
      done<=0;
      if(start&&!busy)begin
        busy<=1;cycles<=4;captured_reg<=reg_address;captured_read<=read_op;
      end else if(busy&&cycles!=0)cycles<=cycles-1'b1;
      else if(busy)begin
        busy<=0;done<=1;error<=0;
        if(captured_read&&captured_reg==8'h3a&&inject_nacks>0)begin
          error<=1;inject_nacks=inject_nacks-1;
        end else if(captured_read&&captured_reg==8'h3a&&inject_other!=0)begin
          error<=inject_other[7:0];inject_other=0;
        end else if(captured_read)begin
          reads=reads+1;
          read_data<=captured_reg==8'h75 ? {8'h71,112'd0} : {8'h01,112'h123400004000000000000000007d};
        end else writes=writes+1;
      end
    end
  end
endmodule

module test_imu_nack_retry;
  reg clk=0,rst_n=0,spi_clk=1,spi_cs=1;
  always #20 clk=~clk;
  reg[3:0] nibble=0;
  wire[511:0] packet;
  tri1 scl,sda;
  mpu9250_sampler dut(.clk(clk),.rst_n(rst_n),.spi_clk(spi_clk),.spi_cs(spi_cs),
    .spi_data(nibble),.mpu_scl(scl),.mpu_sda(sda),.snapshot(packet));
  task automatic quad_byte(input reg[7:0] value);
    begin
      nibble=value[7:4];#100;spi_clk=0;#100;spi_clk=1;#100;
      nibble=value[3:0];#100;spi_clk=0;#100;spi_clk=1;#100;
    end
  endtask
  task automatic command(input reg[15:0] address,input reg[7:0] value);
    begin
      spi_cs=0;#100;quad_byte(0);quad_byte(address[15:8]);quad_byte(address[7:0]);quad_byte(value);
      #100;spi_cs=1;#10000;
    end
  endtask
  integer initial_writes;
  reg[31:0] initial_generation,before_sequence,before_errors;
  reg[63:0] before_time,before_sample;
  task automatic retry_case(input integer n);
    begin
      before_sequence=dut.sample_sequence;before_errors=dut.errors;
      before_time=dut.now_us;before_sample=dut.sample_us;
      dut.bus.inject_nacks=n;
      wait(dut.error_code==1);#1;
      if(dut.valid_sample||!dut.identified)$fatal(1,"NACK must revoke sample without losing identity");
      command(16'h4d01,8'ha5);
      if(packet[459:456]!=3||packet[455:448]!=1)$fatal(1,"invalidity/error hidden in snapshot");
      if(dut.sample_sequence!=before_sequence||dut.sample_us!=before_sample)$fatal(1,"NACK fabricated a sample");
      wait(dut.valid_sample&&dut.error_code==0&&dut.sample_sequence>before_sequence);#1;
      if(dut.now_us-before_time>50000)$fatal(1,"bounded retry missed host freshness budget");
      if(dut.errors!=before_errors+n||dut.generation!=initial_generation||dut.bus.writes!=initial_writes)
        $fatal(1,"transient NACK reset/reconfigured device or lost error accounting");
      if(dut.sample_us<=before_sample)$fatal(1,"fresh timestamp missing");
    end
  endtask
  initial begin
    #200;rst_n=1;#10000;command(16'h4d00,8'ha5);
    wait(dut.sample_sequence>=3);
    initial_writes=dut.bus.writes;initial_generation=dut.generation;
    if(initial_writes!=10||!dut.identified)$fatal(1,"initial configuration failed");
    retry_case(1);retry_case(2);
    before_errors=dut.errors;dut.bus.inject_nacks=3;
    wait(dut.errors==before_errors+3);#1;
    if(dut.valid_sample||dut.identified||dut.state!=6||dut.delay_count==0)
      $fatal(1,"persistent NACK did not retain hard recovery");
    // IMU 硬件：Re-enable for an independent non-NACK fault: never apply fast retry.
    dut.bus.inject_nacks=0;command(16'h4d00,8'ha5);
    wait(dut.identified&&dut.valid_sample);
    before_errors=dut.errors;dut.bus.inject_other=2;
    wait(dut.errors>before_errors);#1;
    if(dut.valid_sample||dut.identified||dut.state!=6)$fatal(1,"timeout was incorrectly fast-retried");
    command(16'h4d00,0);
    if(dut.enabled||dut.valid_sample)$fatal(1,"disable failed");
    $display("PASS IMU NACK retry policy: one/two transient retries, no forged sample/zero/reset, persistent NACK and timeout fail closed, disable");
    $finish;
  end
  initial begin #650000000;$fatal(1,"policy test deadline");end
endmodule
