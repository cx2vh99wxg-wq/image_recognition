`timescale 1ns / 1ps
// Shared vehicle-control blocks. Ports and module bodies are preserved.
// FPGA -> HMI status only, separate from RX commands and all vehicle outputs.
// Screen uses GB2312: B5 B5 encodes Chinese 'gear'. Refresh also repairs the
// display after page changes or a screen-only reboot. No local HMI counter.
module hmi_gear_feedback #(
    parameter integer CLK_FREQ=50000000,
    parameter integer UART_BPS=115200,
    parameter integer REFRESH_CYCLES=12500000
) (
    input wire clk,rst_n,
    input wire [3:0] gear_async,
    input wire slider_present,
    output wire txd
);
  (* ASYNC_REG="TRUE" *) reg [3:0] gear_meta,gear_sync;
  reg [23:0] refresh_count;
  reg [15:0] byte_gap;
  reg [1:0] state;
  reg [5:0] index;
  reg [7:0] gear_character,tx_data;
  reg tx_enable;
  reg slider_snapshot;
  wire tx_busy;
  function [7:0] gear_ascii(input [3:0] value);
    if(!value[3])gear_ascii="?";
    else case(value[2:0])
      3'b000,3'b010:gear_ascii="1";
      3'b001,3'b110:gear_ascii="2";
      3'b011,3'b100:gear_ascii="3";
      default:gear_ascii="?";
    endcase
  endfunction
  function [7:0] frame_byte(input [5:0] n,input [7:0] gear);
    case(n)
        0:frame_byte=8'h70;
        1:frame_byte=8'h61;
        2:frame_byte=8'h67;
        3:frame_byte=8'h65;
        4:frame_byte=8'h30;
        5:frame_byte=8'h2e;
        6:frame_byte=8'h74;
        7:frame_byte=8'h47;
        8:frame_byte=8'h65;
        9:frame_byte=8'h61;
        10:frame_byte=8'h72;
        11:frame_byte=8'h2e;
        12:frame_byte=8'h74;
        13:frame_byte=8'h78;
        14:frame_byte=8'h74;
        15:frame_byte=8'h3d;
        16:frame_byte=8'h22;
        17:frame_byte=gear;
        18:frame_byte=8'hb5;
        19:frame_byte=8'hb5;
        20:frame_byte=8'h22;
        21:frame_byte=8'hff;
        22:frame_byte=8'hff;
        23:frame_byte=8'hff;
        24:frame_byte=8'h70;
        25:frame_byte=8'h61;
        26:frame_byte=8'h67;
        27:frame_byte=8'h65;
        28:frame_byte=8'h30;
        29:frame_byte=8'h2e;
        30:frame_byte=8'h76;
        31:frame_byte=8'h61;
        32:frame_byte=8'h47;
        33:frame_byte=8'h65;
        34:frame_byte=8'h61;
        35:frame_byte=8'h72;
        36:frame_byte=8'h41;
        37:frame_byte=8'h63;
        38:frame_byte=8'h74;
        39:frame_byte=8'h75;
        40:frame_byte=8'h61;
        41:frame_byte=8'h6c;
        42:frame_byte=8'h2e;
        43:frame_byte=8'h76;
        44:frame_byte=8'h61;
        45:frame_byte=8'h6c;
        46:frame_byte=8'h3d;
        47:frame_byte=(gear=="?" ? "0" : gear);
        48:frame_byte=8'hff;
        49:frame_byte=8'hff;
        50:frame_byte=8'hff;
        default:frame_byte=8'hff;
    endcase
  endfunction
  uart_tx #(.CLK_FREQ(CLK_FREQ),.UART_BPS(UART_BPS)) tx (
      .clk(clk),.rst_n(rst_n),.uart_tx_en(tx_enable),.uart_tx_data(tx_data),
      .uart_txd(txd),.uart_tx_busy(tx_busy)
  );
  always @(posedge clk or negedge rst_n) begin
    if(!rst_n)begin
      gear_meta<=0;gear_sync<=0;refresh_count<=REFRESH_CYCLES-1;
      byte_gap<=0;state<=0;index<=0;gear_character<="?";tx_data<=0;tx_enable<=0;
      slider_snapshot<=0;
    end else begin
      gear_meta<=gear_async;gear_sync<=gear_meta;tx_enable<=0;
      case(state)
        0:if(refresh_count>=REFRESH_CYCLES-1)begin
          refresh_count<=0;index<=0;gear_character<=gear_ascii(gear_sync);state<=1;
          slider_snapshot<=slider_present;
        end else refresh_count<=refresh_count+1'b1;
        1:if(!tx_busy)begin tx_data<=frame_byte(index,gear_character);tx_enable<=1;state<=2;end
        2:if(tx_busy)state<=3;
        3:if(!tx_busy)begin
          if(byte_gap>=CLK_FREQ/UART_BPS)begin
            byte_gap<=0;
            if(index==50||(index==23&&!slider_snapshot))state<=0;
            else begin index<=index+1'b1;state<=1;end
          end else byte_gap<=byte_gap+1'b1;
        end
      endcase
    end
  end
endmodule

// SPEED is a direct momentary active-low contact. Slider requests use
// bounded pulses, but a manual press is NEVER shortened to a fixed pulse.
module remote_speed_controller #(
    parameter integer DEBOUNCE_CYCLES=250000,
    parameter integer PULSE_CYCLES=12500000, // slider: 500 ms at 25 MHz
    parameter integer GAP_CYCLES=12500000
) (
    input wire clk,rst_n,key_n,
    input wire hmi_level_async,
    input wire target_toggle_async,
    input wire [1:0] target_gear_async,
    input wire [7:0] sync_command_async,
    output reg [2:0] phase_gray,
    output reg ready,pressed,
    output wire drive_active
);
    (* ASYNC_REG="TRUE" *) reg [1:0] key_pipe,hmi_pipe;
    (* ASYNC_REG="TRUE" *) reg [2:0] target_pipe;
    (* ASYNC_REG="TRUE" *) reg [1:0] target_meta,target_sync;
    (* ASYNC_REG="TRUE" *) reg [7:0] sync_meta,sync_value;
    reg target_previous;
    reg [7:0] previous_sync;
    reg [23:0] debounce_count,release_count,pulse_count;
    reg [1:0] auto_state,wanted,attempts_left;
    reg pending;
    reg [2:0] phase_at_press;
    localparam IDLE=0,PRESS=1,GAP=2;
    wire target_event=target_pipe[2]!=target_previous;
    wire sensed_press=!key_pipe[1]&&!pressed&&(debounce_count>=DEBOUNCE_CYCLES-1);
    // A contact press must not depend on the feedback input first reaching
    // HIGH. RIGHT has no such prerequisite. Only queued absolute-gear
    // pulses require a valid baseline; manual hold/release always works.
    assign drive_active=rst_n&&(hmi_pipe[1]||(ready&&auto_state==PRESS));
    function [2:0] next_phase(input [2:0] p);
        case(p)
          0:next_phase=1;1:next_phase=3;3:next_phase=2;
          2:next_phase=6;6:next_phase=4;default:next_phase=0;
        endcase
    endfunction
    function [1:0] gear(input [2:0] p);
        case(p)0,2:gear=1;1,6:gear=2;3,4:gear=3;default:gear=0;endcase
    endfunction
    always @(posedge clk or negedge rst_n) begin
        if(!rst_n)begin
            key_pipe<=3;hmi_pipe<=0;target_pipe<=0;target_meta<=1;target_sync<=1;
            sync_meta<=0;sync_value<=0;previous_sync<=0;target_previous<=0;
            phase_gray<=0;ready<=0;pressed<=0;
            debounce_count<=0;release_count<=0;pulse_count<=0;
            auto_state<=IDLE;wanted<=1;attempts_left<=0;pending<=0;phase_at_press<=0;
        end else begin
            key_pipe<={key_pipe[0],key_n};hmi_pipe<={hmi_pipe[0],hmi_level_async};
            target_pipe<={target_pipe[1:0],target_toggle_async};target_previous<=target_pipe[2];
            target_meta<=target_gear_async;target_sync<=target_meta;
            sync_meta<=sync_command_async;sync_value<=sync_meta;previous_sync<=sync_value;
            if(key_pipe[1])begin
                if(release_count<DEBOUNCE_CYCLES)release_count<=release_count+1'b1;
                if(release_count>=DEBOUNCE_CYCLES-1)ready<=1;
            end else release_count<=0;
            if(key_pipe[1]==!pressed)debounce_count<=0;
            else if(debounce_count>=DEBOUNCE_CYCLES-1)begin
                debounce_count<=0;pressed<=!key_pipe[1];
                if(!key_pipe[1]&&ready)phase_gray<=next_phase(phase_gray);
            end else debounce_count<=debounce_count+1'b1;

            case(auto_state)
              IDLE:if(pending&&ready&&key_pipe[1]&&!pressed&&release_count>=DEBOUNCE_CYCLES)begin
                if(gear(phase_gray)==wanted||attempts_left==0)pending<=0;
                else begin
                    auto_state<=PRESS;pulse_count<=0;phase_at_press<=phase_gray;
                    attempts_left<=attempts_left-1'b1;
                end
              end
              PRESS:if(pulse_count>=PULSE_CYCLES-1)begin
                auto_state<=GAP;pulse_count<=0;
                // No sensed contact edge: do not keep sending blind pulses.
                if(phase_gray==phase_at_press)pending<=0;
              end else pulse_count<=pulse_count+1'b1;
              GAP:if(pulse_count>=GAP_CYCLES-1)begin auto_state<=IDLE;pulse_count<=0;end
                  else pulse_count<=pulse_count+1'b1;
              default:auto_state<=IDLE;
            endcase
            if(target_event&&ready&&target_sync>=1&&target_sync<=3)begin
                wanted<=target_sync;pending<=1;attempts_left<=2;
            end
            // Human input takes priority over queued slider actions. Hold
            // manual low for its entire press, release immediately on 00.
            if(hmi_pipe[1]||(sensed_press&&auto_state!=PRESS))begin
                pending<=0;auto_state<=IDLE;pulse_count<=0;
            end
            if(sync_value==8'ha5&&previous_sync!=8'ha5&&ready&&key_pipe[1]&&!pressed&&!drive_active)begin
                phase_gray<=0;pending<=0;auto_state<=IDLE;
            end
        end
    end
endmodule


// Traction and steering arbitrate independently. HMI GO/BACK must not mask
// automatic LEFT/RIGHT. Each axis retains the original stale-command guard:
// after manual release, wait for that axis's FSPI bits to be idle.
// HMI traction is latched until STOP or a new opposite-direction press.
// Steering still follows its original momentary press/release behavior.
module remote_direction_arbiter (
    input  wire       clk,
    input  wire       rst_n,
    input  wire [3:0] hmi_code,
    input  wire [3:0] fspi_released,
    input  wire       fspi_direction_armed,
    input  wire       movement_blocked,
    input  wire       hmi_left_denied,
    input  wire       hmi_right_denied,
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

  localparam [1:0] TRACTION_IDLE = 2'b00;
  localparam [1:0] TRACTION_GO   = 2'b01;
  localparam [1:0] TRACTION_BACK = 2'b10;
  reg [1:0] hmi_traction_latched;
  reg [3:0] previous_hmi_code;
  reg [2:0] traction_stable_count;
  reg traction_press_consumed;
  wire hmi_code_changed = (hmi_code != previous_hmi_code);

  // STOP clears the stored intent, including automatic STOP. Tracking the
  // received code while blocked prevents a pre-STOP held key from restarting
  // traction after REARM; the operator must release and press GO/BACK again.
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      hmi_traction_latched <= TRACTION_IDLE;
      previous_hmi_code <= 4'h0;
      traction_stable_count <= 3'd0;
      traction_press_consumed <= 1'b0;
    end else begin
      previous_hmi_code <= hmi_code;
      if (hmi_code_changed)
        traction_stable_count <= 3'd0;
      else if (traction_stable_count != 3'd7)
        traction_stable_count <= traction_stable_count + 3'd1;

      // Use the existing filtered STOP signal. A transient intermediate E
      // during a multi-bit CDC must not erase a valid traction latch.
      if (movement_blocked) begin
        hmi_traction_latched <= TRACTION_IDLE;
        traction_press_consumed <= 1'b1;
      end else if (hmi_code_changed) begin
        traction_press_consumed <= 1'b0;
      end else if (traction_stable_count == 3'd7 && !traction_press_consumed) begin
        traction_press_consumed <= 1'b1;
        if (hmi_go)
          hmi_traction_latched <= TRACTION_GO;
        else if (hmi_back)
          hmi_traction_latched <= TRACTION_BACK;
      end
    end
  end

  wire latched_go   = (hmi_traction_latched == TRACTION_GO);
  wire latched_back = (hmi_traction_latched == TRACTION_BACK);

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

  wire hmi_owns_traction = latched_go | latched_back | hmi_traction | hmi_neutral | hmi_traction_hold;
  wire hmi_owns_steering = hmi_steering | hmi_neutral | hmi_steering_hold;
  wire fspi_blocked = ~fspi_direction_armed;
  assign go_released    = movement_blocked | (hmi_owns_traction ? ~latched_go   : (fspi_blocked | fspi_released[0]));
  assign back_released  = movement_blocked | (hmi_owns_traction ? ~latched_back : (fspi_blocked | fspi_released[1]));
  assign left_released  = movement_blocked | (hmi_owns_steering ? (~hmi_left | hmi_left_denied) : (fspi_blocked | fspi_released[2]));
  assign right_released = movement_blocked | (hmi_owns_steering ? (~hmi_right | hmi_right_denied) : (fspi_blocked | fspi_released[3]));
endmodule

// reg5: bit6=protocol valid, bit7=heartbeat toggle, bits[1:0]=right/left deny.
// bit2=set red-light GO hold; bit3=clear hold on confirmed green. Set wins.
// A lost heartbeat releases only the original lane permissions. A previously
// observed red stays held until an explicit green (or FPGA power-on reset).
// Old software writes bits[3:2]=00 and cannot inadvertently release this hold.
// Stable-word CDC and one atomic byte prevent mixed left/right permissions.
module lane_permission_guard #(
    parameter TIMEOUT_CYCLES = 18750000
) (
    input wire clk, rst_n,
    input wire [7:0] async_permission,
    output wire left_denied, right_denied,
    output wire traffic_go_denied
);
  (* ASYNC_REG="TRUE" *) reg [7:0] meta, synced;
  reg [7:0] candidate;
  reg [3:0] stable;
  reg [24:0] age;
  reg heartbeat, alive;
  reg [1:0] deny;
  reg red_hold;
  assign left_denied=alive & deny[0];
  assign right_denied=alive & deny[1];
  assign traffic_go_denied=red_hold;
  always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
      meta<=0; synced<=0; candidate<=0; stable<=0;
      age<=0; heartbeat<=0; alive<=0; deny<=0; red_hold<=0;
    end else begin
      meta<=async_permission; synced<=meta;
      if(synced!=candidate) begin candidate<=synced; stable<=0; end
      else if(stable!=15) stable<=stable+1'b1;
      if(age<TIMEOUT_CYCLES) age<=age+1'b1;
      else begin alive<=0; deny<=0; end
      if(stable==8 && candidate[6] && (!alive || candidate[7]!=heartbeat)) begin
        heartbeat<=candidate[7]; deny<=candidate[1:0]; age<=0; alive<=1;
        if(candidate[2]) red_hold<=1;
        else if(candidate[3]) red_hold<=0;
      end
      if(stable==8 && !candidate[6]) begin alive<=0; deny<=0; end
    end
  end
endmodule

// Only the new persistent enable bit crosses into the existing STOP domain.
module auto_brake_enable_gate (
    input  wire clk,
    input  wire rst_n,
    input  wire async_enable,
    input  wire brake_request_in,
    output wire enable_sync,
    output wire brake_request_out
);
  (* ASYNC_REG = "TRUE" *) reg [1:0] enable_pipe;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      enable_pipe <= 2'b00;
    end else begin
      enable_pipe <= {enable_pipe[0], async_enable};
    end
  end
  // Disabling only prevents new requests. Existing STOP latch state remains
  // in remote_stop_controller and still requires the original REARM action.
  assign enable_sync = enable_pipe[1];
  assign brake_request_out = enable_sync && brake_request_in;
endmodule

// A new automatic brake request holds STOP low until a fresh manual E press.
// E may force-clear the latch while the request is still high. That same
// request is then consumed until it clears; a subsequent rise brakes again.
// Manual E itself holds STOP low, so STOP returns high when E is released.
module remote_stop_controller (
    input  wire clk,
    input  wire rst_n,
    input  wire brake_request,
    input  wire rearm_command,
    input  wire fspi_directions_idle,
    output wire stop_pressed,
    output wire movement_blocked,
    output wire fspi_direction_armed
);
  reg brake_latched;
  reg brake_event_armed;
  reg rearm_previous;
  reg fspi_armed;
  wire rearm_rising = rearm_command & ~rearm_previous;
  wire new_brake_event = brake_request & brake_event_armed;

  assign stop_pressed = rst_n && (brake_latched | rearm_command);
  assign movement_blocked = brake_latched | rearm_command;
  assign fspi_direction_armed = fspi_armed;

  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      brake_latched  <= 1'b0;
      brake_event_armed <= 1'b1;
      rearm_previous <= 1'b0;
      fspi_armed     <= 1'b1;
    end else begin
      rearm_previous <= rearm_command;

      // A held pre-brake direction must not resume after manual unlock.
      // Require all four FSPI registers to be idle before accepting a new one.
      if (brake_latched || new_brake_event || rearm_command)
        fspi_armed <= 1'b0;
      else if (fspi_directions_idle)
        fspi_armed <= 1'b1;

      if (!brake_request)
        brake_event_armed <= 1'b1;

      if (rearm_rising && brake_latched) begin
        // Explicit operator override. Consume a still-present alarm so the
        // following cycle cannot immediately undo this manual unlock.
        brake_latched <= 1'b0;
        if (brake_request)
          brake_event_armed <= 1'b0;
      end else if (new_brake_event) begin
        brake_latched <= 1'b1;
        brake_event_armed <= 1'b0;
      end
    end
  end
endmodule

// The UART decoder and the remote-button controller use different clocks.
// A held E code must survive eight 25 MHz samples before it can toggle STOP;
// this rejects the short intermediate values possible during a 4-bit CDC.
module hmi_control_synchronizer (
    input  wire       clk,
    input  wire       rst_n,
    input  wire [3:0] async_code,
    output reg  [3:0] code_sync,
    output wire       rearm_command
);
  reg [3:0] code_meta;
  reg [2:0] e_stable_count;
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      code_meta      <= 4'h0;
      code_sync      <= 4'h0;
      e_stable_count <= 3'd0;
    end else begin
      code_meta <= async_code;
      code_sync <= code_meta;
      if (code_sync == 4'he) begin
        if (e_stable_count != 3'd7)
          e_stable_count <= e_stable_count + 3'd1;
      end else begin
        e_stable_count <= 3'd0;
      end
    end
  end
  assign rearm_command = (code_sync == 4'he) && (e_stable_count == 3'd7);
endmodule
