module remote_direction_arbiter_previous (
    input  wire       clk,
    input  wire       rst_n,
    input  wire [3:0] hmi_code,
    input  wire [3:0] fspi_released,
    input  wire       fspi_direction_armed,
    input  wire       movement_blocked,
    output wire       go_released,
    output wire       back_released,
    output wire       left_released,
    output wire       right_released
);
  wire hmi_go    = (hmi_code == 4'ha);
  wire hmi_back  = (hmi_code == 4'hb);
  wire hmi_left  = (hmi_code == 4'hc);
  wire hmi_right = (hmi_code == 4'hd);
  wire hmi_neutral = (hmi_code == 4'he);
  wire hmi_traction = hmi_go | hmi_back;
  wire hmi_steering = hmi_left | hmi_right;

  reg hmi_traction_hold;
  reg hmi_steering_hold;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      hmi_traction_hold <= 1'b0;
      hmi_steering_hold <= 1'b0;
    end else begin
      if (hmi_traction | hmi_neutral)
        hmi_traction_hold <= 1'b1;
      else if (&fspi_released[1:0])
        hmi_traction_hold <= 1'b0;
      if (hmi_steering | hmi_neutral)
        hmi_steering_hold <= 1'b1;
      else if (&fspi_released[3:2])
        hmi_steering_hold <= 1'b0;
    end
  end

  wire hmi_owns_traction = hmi_traction | hmi_neutral | hmi_traction_hold;
  wire hmi_owns_steering = hmi_steering | hmi_neutral | hmi_steering_hold;
  wire fspi_blocked = ~fspi_direction_armed;
  assign go_released    = movement_blocked | (hmi_owns_traction ? ~hmi_go    : (fspi_blocked | fspi_released[0]));
  assign back_released  = movement_blocked | (hmi_owns_traction ? ~hmi_back  : (fspi_blocked | fspi_released[1]));
  assign left_released  = movement_blocked | (hmi_owns_steering ? ~hmi_left  : (fspi_blocked | fspi_released[2]));
  assign right_released = movement_blocked | (hmi_owns_steering ? ~hmi_right : (fspi_blocked | fspi_released[3]));
endmodule
