`timescale 1ns/1ps
// M-only autonomous sensor sampler; disabled until 00 4D 00 A5 arrives.
// A snapshot request (00 4D 01 A5) freezes all fields + FPGA acquisition time.
module mpu9250_sampler #(parameter CLK_HZ=25000000)(
    input wire clk,rst_n,spi_clk,spi_cs,
    input wire [3:0] spi_data,
    inout wire mpu_scl,mpu_sda,
    output reg [511:0] snapshot
);
    reg [31:0] header;
    reg [3:0] nibble_count;
    reg command_toggle;
    reg [15:0] command_address;
    reg [7:0] command_value;
    always @(posedge spi_clk or negedge rst_n or posedge spi_cs)
      if(!rst_n||spi_cs)begin header<=0;nibble_count<=0;end
      else if(nibble_count<8)begin header<={header[27:0],spi_data};nibble_count<=nibble_count+1'b1;end
    always @(posedge spi_clk or negedge rst_n)
      if(!rst_n)begin command_toggle<=0;command_address<=0;command_value<=0;end
      else if(!spi_cs&&nibble_count==7&&header[27:20]==8'h00&&header[19:12]==8'h4d)begin
        command_address<=header[19:4];command_value<={header[3:0],spi_data};
        command_toggle<=~command_toggle;
      end
    (* ASYNC_REG="TRUE" *)reg [2:0] command_sync;
    always @(posedge clk or negedge rst_n)
      if(!rst_n)command_sync<=0;else command_sync<={command_sync[1:0],command_toggle};
    wire new_command=command_sync[2]^command_sync[1];
    reg enabled,identified,valid_sample,capture_pending;
    reg [7:0] who,error_code;
    reg [6:0] device_address;
    reg [31:0] sample_sequence,errors,generation;
    reg [111:0] raw_sample;
    reg [63:0] now_us,sample_us,transaction_us;
    reg [7:0] micro_div;
    reg [31:0] delay_count;
    reg [4:0] state,config_index;
    reg start,read_op;
    reg [7:0] reg_address,write_value;
    reg [4:0] read_length;
    wire busy,done;
    wire [7:0] bus_error;
    wire [119:0] read_data;
    localparam PROBE=0,PROBE_WAIT=1,CONFIG=2,CONFIG_WAIT=3,
               SAMPLE=4,SAMPLE_WAIT=5,RETRY=6;
    imu_i2c_master #(.CLK_HZ(CLK_HZ)) bus(
      .clk(clk),.rst_n(rst_n),.start(start),.read_op(read_op),.address(device_address),
      .reg_address(reg_address),.write_value(write_value),.read_length(read_length),
      .scl(mpu_scl),.sda(mpu_sda),.busy(busy),.done(done),.error(bus_error),.read_data(read_data));
    function [15:0] configuration(input [4:0] n);
      case(n)
        0:configuration=16'h6b80; // device reset; wait 100ms
        1:configuration=16'h6b01; // PLL X gyro; wait 100ms
        2:configuration=16'h6c00;
        3:configuration=16'h1a03; // 41Hz gyro DLPF, internal sample clock 1kHz
        4:configuration=16'h1909; // 100Hz
        5:configuration=16'h1b00; // +/-250 dps, 131 LSB/dps
        6:configuration=16'h1c00; // +/-2g, 16384 LSB/g
        7:configuration=16'h1d03; // 41Hz accel DLPF
        8:configuration=16'h3700; // clear data-ready by INT_STATUS read
        default:configuration=16'h3801;
      endcase
    endfunction
    always @(posedge clk or negedge rst_n)begin
      if(!rst_n)begin
        enabled<=0;identified<=0;valid_sample<=0;who<=8'hff;error_code<=0;
        device_address<=7'h68;sample_sequence<=0;errors<=0;generation<=0;raw_sample<=0;
        now_us<=0;sample_us<=0;transaction_us<=0;micro_div<=0;
        delay_count<=0;state<=PROBE;config_index<=0;start<=0;read_op<=0;
        reg_address<=0;write_value<=0;read_length<=1;
      end else begin
        start<=0;
        if(micro_div==CLK_HZ/1000000-1)begin micro_div<=0;now_us<=now_us+1'b1;end
        else micro_div<=micro_div+1'b1;
        if(new_command&&command_address==16'h4d00)begin
          enabled<=command_value==8'ha5;identified<=0;valid_sample<=0;
          sample_sequence<=0;generation<=generation+1'b1;who<=8'hff;error_code<=0;
          state<=PROBE;device_address<=7'h68;delay_count<=CLK_HZ/100;
        end else if(enabled&&!busy)begin
          if(delay_count!=0)delay_count<=delay_count-1'b1;
          else case(state)
            PROBE:begin
              read_op<=1;reg_address<=8'h75;read_length<=1;start<=1;state<=PROBE_WAIT;
            end
            CONFIG:begin
              {reg_address,write_value}<=configuration(config_index);
              read_op<=0;start<=1;state<=CONFIG_WAIT;
            end
            SAMPLE:begin
              reg_address<=8'h3a;read_length<=15;read_op<=1;start<=1;
              transaction_us<=now_us;state<=SAMPLE_WAIT;
            end
            RETRY:begin device_address<=7'h68;state<=PROBE;end
            default:begin end
          endcase
        end
        if(done&&enabled)begin
          if(bus_error!=0)begin
            error_code<=bus_error;errors<=errors+1'b1;valid_sample<=0;
            if(state==PROBE_WAIT&&device_address==7'h68)begin device_address<=7'h69;state<=PROBE;end
            else begin identified<=0;state<=RETRY;delay_count<=CLK_HZ/2;end
          end else case(state)
            PROBE_WAIT:begin
              who<=read_data[119:112];
              if(read_data[119:112]==8'h71)begin identified<=1;error_code<=0;config_index<=0;state<=CONFIG;end
              else begin error_code<=4;state<=RETRY;delay_count<=CLK_HZ/2;end
            end
            CONFIG_WAIT:begin
              if(config_index==9)begin state<=SAMPLE;delay_count<=CLK_HZ/100;end
              else begin
                config_index<=config_index+1'b1;state<=CONFIG;
                delay_count<=config_index<2?CLK_HZ/10:CLK_HZ/1000;
              end
            end
            SAMPLE_WAIT:begin
              if(read_data[112])begin // INT_STATUS.RAW_DATA_RDY_INT, not a repeat
                raw_sample<=read_data[111:0];sample_us<=transaction_us;
                sample_sequence<=sample_sequence+1'b1;valid_sample<=1;error_code<=0;
              end
              state<=SAMPLE;delay_count<=CLK_HZ/10000; // 0.1ms gap, ~2ms polling period
            end
            default:begin end
          endcase
        end
      end
    end
    // CRC is computed sequentially away from the SPI/400MHz datapath.
    reg [511:0] staging;
    reg [31:0] crc;
    reg [6:0] crc_index;
    reg crc_busy;
    function [31:0] crc_byte(input [31:0] old_crc,input [7:0] data);
      reg [31:0] c;integer k;
      begin c=old_crc^data;for(k=0;k<8;k=k+1)c=c[0]?(c>>1)^32'hedb88320:c>>1;crc_byte=c;end
    endfunction
    always @(posedge clk or negedge rst_n)begin
      if(!rst_n)begin
        snapshot<=0;staging<=0;crc<=32'hffffffff;crc_index<=0;crc_busy<=0;capture_pending<=1;
      end else begin
        if(new_command&&command_address==16'h4d01&&command_value==8'ha5)capture_pending<=1;
        if(capture_pending&&!crc_busy)begin
          staging<={32'h494d5531,8'd1,8'd64,{5'd0,valid_sample,identified,enabled},error_code,
            who,1'b0,device_address,8'd100,8'd0,sample_sequence,sample_us,now_us,raw_sample,
            16'd0,errors,generation,32'd0,32'd0};
          crc<=32'hffffffff;crc_index<=0;crc_busy<=1;capture_pending<=0;
        end else if(crc_busy)begin
          if(crc_index<60)begin crc<=crc_byte(crc,staging[511-crc_index*8 -: 8]);crc_index<=crc_index+1'b1;end
          else begin snapshot<={staging[511:32],crc^32'hffffffff};crc_busy<=0;end
        end
      end
    end
endmodule
