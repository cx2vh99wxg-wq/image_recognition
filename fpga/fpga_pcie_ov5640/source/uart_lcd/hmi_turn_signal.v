`timescale 1ns / 1ps

module hmi_turn_signal #(
    parameter integer CLOCK_HZ = 50_000_000
) (
    input  wire       clk,
    input  wire       rst_n,
    input  wire [3:0] hmi_code,
    output wire       lamp_left,
    output wire       lamp_right
);
  localparam [1:0] IDLE = 2'b00, LEFT = 2'b01, RIGHT = 2'b10;
  localparam integer TIMER_WIDTH = (CLOCK_HZ > 1) ? $clog2(CLOCK_HZ) : 1;
  localparam [TIMER_WIDTH-1:0] LAST_TICK = CLOCK_HZ - 1;

  wire [1:0] requested_direction =
      (hmi_code == 4'hc) ? LEFT : (hmi_code == 4'hd) ? RIGHT : IDLE;
  reg [1:0] previous_direction;
  reg [1:0] active_direction;
  reg [TIMER_WIDTH-1:0] ticks;
  reg [2:0] flashes_remaining;
  reg lit;

  assign lamp_left  = lit && active_direction == LEFT;
  assign lamp_right = lit && active_direction == RIGHT;

  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      previous_direction <= IDLE;
      active_direction <= IDLE;
      ticks <= 0;
      flashes_remaining <= 0;
      lit <= 1'b0;
    end else begin
      previous_direction <= requested_direction;
      // A new press starts at ON. Release only allows the next press to be seen.
      if (requested_direction != IDLE && requested_direction != previous_direction) begin
        active_direction <= requested_direction;
        ticks <= 0;
        flashes_remaining <= 3'd5;
        lit <= 1'b1;
      end else if (active_direction != IDLE) begin
        if (ticks != LAST_TICK) begin
          ticks <= ticks + 1'b1;
        end else begin
          ticks <= 0;
          if (lit) begin
            lit <= 1'b0;
          end else if (flashes_remaining == 3'd1) begin
            active_direction <= IDLE;
            flashes_remaining <= 0;
          end else begin
            flashes_remaining <= flashes_remaining - 1'b1;
            lit <= 1'b1;
          end
        end
      end
    end
  end
endmodule
