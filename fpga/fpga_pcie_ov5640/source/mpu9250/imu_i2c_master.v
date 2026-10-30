`timescale 1ns/1ps
// Independent open-drain register transaction engine. No camera bus is reused.
module imu_i2c_master #(
    parameter CLK_HZ=25000000, parameter I2C_HZ=100000,
    parameter TIMEOUT_CYCLES=1250000
)(
    input wire clk, rst_n, start, read_op,
    input wire [6:0] address,
    input wire [7:0] reg_address, write_value,
    input wire [4:0] read_length,
    inout wire scl, sda,
    output reg busy, done,
    output reg [7:0] error,
    output reg [119:0] read_data
);
    localparam DIV=(CLK_HZ+I2C_HZ*4-1)/(I2C_HZ*4);
    localparam IDLE=0, FREE=1, START=2, BITS=3, REP0=4, REP1=5,
               REP2=6, REP3=7, STOP0=8, STOP1=9, STOP2=10, RECOVER=11;
    reg [3:0] state, stage;
    reg [31:0] divider, timeout_count;
    reg scl_low, sda_low, reading, ack_bit;
    reg [1:0] phase;
    reg [2:0] bit_index;
    reg [4:0] byte_index, length_latched, recovery_count;
    reg [7:0] shift, register_latched, value_latched;
    reg [6:0] address_latched;
    reg read_latched;
    (* ASYNC_REG="TRUE" *) reg [1:0] scl_sync,sda_sync;
    assign scl=scl_low?1'b0:1'bz;
    assign sda=sda_low?1'b0:1'bz;
    always @(posedge clk or negedge rst_n)
      if(!rst_n)begin scl_sync<=3;sda_sync<=3;end
      else begin scl_sync<={scl_sync[0],scl};sda_sync<={sda_sync[0],sda};end
    always @(posedge clk or negedge rst_n) begin
      if(!rst_n) begin
        state<=IDLE;busy<=0;done<=0;error<=0;read_data<=0;
        scl_low<=0;sda_low<=0;divider<=0;timeout_count<=0;
        phase<=0;bit_index<=7;stage<=0;reading<=0;ack_bit<=0;
        byte_index<=0;length_latched<=0;shift<=0;register_latched<=0;
        value_latched<=0;address_latched<=0;read_latched<=0;recovery_count<=0;
      end else begin
        done<=0;
        if(!busy) begin
          divider<=0;timeout_count<=0;
          if(start) begin
            address_latched<=address;register_latched<=reg_address;
            value_latched<=write_value;length_latched<=read_length;
            read_latched<=read_op;busy<=1;error<=0;read_data<=0;
            scl_low<=0;sda_low<=0;state<=FREE;recovery_count<=0;
          end
        end else if(timeout_count>=TIMEOUT_CYCLES) begin
          // Releasing the pins is always possible, even if a slave holds SCL.
          busy<=0;done<=1;error<=2;state<=IDLE;scl_low<=0;sda_low<=0;
        end else begin
          timeout_count<=timeout_count+1'b1;
          if(divider<DIV-1)divider<=divider+1'b1;
          else begin
            divider<=0;
            case(state)
              FREE:if(scl_sync[1])begin
                if(sda_sync[1])begin sda_low<=1;state<=START;end
                else begin phase<=0;state<=RECOVER;end
              end
              RECOVER:begin
                sda_low<=0;
                case(phase)
                  0:begin scl_low<=1;phase<=1;end
                  1:begin scl_low<=0;phase<=2;end
                  2:if(scl_sync[1])phase<=3;
                  3:begin
                    scl_low<=1;phase<=0;
                    if(recovery_count==8)begin error<=3;state<=STOP0;end
                    else recovery_count<=recovery_count+1'b1;
                  end
                endcase
              end
              START:begin
                scl_low<=1;shift<={address_latched,1'b0};stage<=0;
                reading<=0;ack_bit<=0;phase<=0;bit_index<=7;state<=BITS;
              end
              BITS:case(phase)
                0:begin
                  scl_low<=1;
                  sda_low<=reading ? (ack_bit && byte_index+1<length_latched) :
                                      (!ack_bit && !shift[bit_index]);
                  phase<=1;
                end
                1:begin scl_low<=0;phase<=2;end
                2:if(scl_sync[1])begin
                  if(reading&&!ack_bit)shift[bit_index]<=sda_sync[1];
                  if(!reading&&ack_bit&&sda_sync[1])error<=1;
                  phase<=3;
                end
                3:begin
                  scl_low<=1;phase<=0;
                  if(!ack_bit)begin
                    if(bit_index==0)ack_bit<=1;
                    else bit_index<=bit_index-1'b1;
                  end else begin
                    ack_bit<=0;bit_index<=7;
                    if(error!=0)state<=STOP0;
                    else if(reading)begin
                      read_data[119-byte_index*8 -: 8]<=shift;
                      if(byte_index+1>=length_latched)state<=STOP0;
                      else begin byte_index<=byte_index+1'b1;shift<=0;end
                    end else case(stage)
                      0:begin shift<=register_latched;stage<=1;end
                      1:if(read_latched)state<=REP0;
                        else begin shift<=value_latched;stage<=2;end
                      2:state<=STOP0;
                      3:begin reading<=1;byte_index<=0;shift<=0;end
                      default:state<=STOP0;
                    endcase
                  end
                end
              endcase
              REP0:begin scl_low<=1;sda_low<=0;state<=REP1;end
              REP1:begin scl_low<=0;if(scl_sync[1])state<=REP2;end
              REP2:begin sda_low<=1;state<=REP3;end
              REP3:begin
                scl_low<=1;shift<={address_latched,1'b1};stage<=3;
                phase<=0;bit_index<=7;ack_bit<=0;state<=BITS;
              end
              STOP0:begin scl_low<=1;sda_low<=1;state<=STOP1;end
              STOP1:begin scl_low<=0;if(scl_sync[1])state<=STOP2;end
              STOP2:begin sda_low<=0;busy<=0;done<=1;state<=IDLE;end
              default:begin error<=2;state<=STOP0;end
            endcase
          end
        end
      end
    end
endmodule
