`timescale 1ns / 1ps

module hmi_camera_view #(
    parameter integer PREVIEW_CYCLES = 500_000_000
) (
    input  wire       clk,
    input  wire       rst_n,
    input  wire [3:0] hmi_code,
    output reg  [3:0] view_code
);
  localparam [3:0] OVERVIEW = 4'h9;
  localparam integer COUNTER_WIDTH = $clog2(PREVIEW_CYCLES + 1);
  localparam [COUNTER_WIDTH-1:0] RELOAD = PREVIEW_CYCLES[COUNTER_WIDTH-1:0];
  reg [3:0] previous_code;
  reg [COUNTER_WIDTH-1:0] cycles_left;

  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      previous_code <= 0;
      view_code <= OVERVIEW;
      cycles_left <= 0;
    end else begin
      previous_code <= hmi_code;
      if (cycles_left != 0) begin
        cycles_left <= cycles_left - 1'b1;
        if (cycles_left == 1)
          view_code <= OVERVIEW;
      end

      // Release and held-key frames leave the countdown running.
      if (hmi_code != previous_code) begin
        case (hmi_code)
          4'hc, 4'hd: begin
            view_code <= (hmi_code == 4'hc) ? 4'h2 : 4'h3;
            cycles_left <= RELOAD;
          end
          4'h1, 4'h2, 4'h3, 4'h4, 4'h5, 4'h6, 4'h7, 4'h8, 4'h9: begin
            view_code <= hmi_code;
            cycles_left <= 0;
          end
          default: ;
        endcase
      end
    end
  end
endmodule
