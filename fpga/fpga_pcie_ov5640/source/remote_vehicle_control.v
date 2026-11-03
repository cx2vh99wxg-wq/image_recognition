// 2026-11-03 注释校订。车控输入同步、挡位估计与按键调度。独立模块负责 STOP、REARM 和 HMI 回显。
`timescale 1ns / 1ps

// 档位文本带 GB2312 后缀，连续三个 FF 结束命令。滑块就绪后追加数值回显。
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
  localparam [135:0] TEXT_PREFIX = {"page0.tGear.txt=", 8'h22};
  localparam [183:0] SLIDER_PREFIX = "page0.vaGearActual.val=";

  function [7:0] frame_byte(input [5:0] n,input [7:0] gear);
    begin
      if (n < 17)
        frame_byte = TEXT_PREFIX[135 - 8*n -: 8];
      else if (n >= 24 && n <= 46)
        frame_byte = SLIDER_PREFIX[183 - 8*(n-24) -: 8];
      else case (n)
        17: frame_byte = gear;
        18, 19: frame_byte = 8'hb5;
        20: frame_byte = 8'h22;
        47: frame_byte = (gear == "?" ? "0" : gear);
        default: frame_byte = 8'hff;
      endcase
    end
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

// 自动脉冲按状态调度。同拍命令按目标、人工接管、空闲校准的既有顺序覆盖。
module remote_speed_controller #(
    parameter integer DEBOUNCE_CYCLES=250000,
    parameter integer PULSE_CYCLES=12500000,
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
    wire calibrate = sync_value == 8'ha5 && previous_sync != 8'ha5 &&
                     ready && key_pipe[1] && !pressed && !drive_active;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            key_pipe <= 3; hmi_pipe <= 0; target_pipe <= 0;
            target_meta <= 1; target_sync <= 1;
            sync_meta <= 0; sync_value <= 0; previous_sync <= 0; target_previous <= 0;
        end else begin
            key_pipe <= {key_pipe[0],key_n};
            hmi_pipe <= {hmi_pipe[0],hmi_level_async};
            target_pipe <= {target_pipe[1:0],target_toggle_async};
            target_previous <= target_pipe[2];
            target_meta <= target_gear_async;
            target_sync <= target_meta;
            sync_meta <= sync_command_async;
            sync_value <= sync_meta;
            previous_sync <= sync_value;
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            phase_gray <= 0; ready <= 0; pressed <= 0;
            debounce_count <= 0; release_count <= 0;
        end else begin
            if (key_pipe[1]) begin
                if (release_count < DEBOUNCE_CYCLES) release_count <= release_count + 1'b1;
                if (release_count >= DEBOUNCE_CYCLES-1) ready <= 1;
            end else release_count <= 0;

            if (key_pipe[1] == !pressed) debounce_count <= 0;
            else if (debounce_count >= DEBOUNCE_CYCLES-1) begin
                debounce_count <= 0;
                pressed <= !key_pipe[1];
                if (!key_pipe[1] && ready) phase_gray <= next_phase(phase_gray);
            end else debounce_count <= debounce_count + 1'b1;
            if (calibrate) phase_gray <= 0;
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            pulse_count <= 0; auto_state <= IDLE; wanted <= 1;
            attempts_left <= 0; pending <= 0; phase_at_press <= 0;
        end else begin
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

                if(phase_gray==phase_at_press)pending<=0;
              end else pulse_count<=pulse_count+1'b1;
              GAP:if(pulse_count>=GAP_CYCLES-1)begin auto_state<=IDLE;pulse_count<=0;end
                  else pulse_count<=pulse_count+1'b1;
              default:auto_state<=IDLE;
            endcase
            if(target_event&&ready&&target_sync>=1&&target_sync<=3)begin
                wanted<=target_sync;pending<=1;attempts_left<=2;
            end

            if(hmi_pipe[1]||(sensed_press&&auto_state!=PRESS))begin
                pending<=0;auto_state<=IDLE;pulse_count<=0;
            end
            if (calibrate) begin
                pending <= 0;
                auto_state <= IDLE;
            end
        end
    end
endmodule

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

  assign enable_sync = enable_pipe[1];
  assign brake_request_out = enable_sync && brake_request_in;
endmodule

// 制动锁存与档位调度独立。重新使能须遵守原报警及方向空闲条件。
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

      if (brake_latched || new_brake_event || rearm_command)
        fspi_armed <= 1'b0;
      else if (fspi_directions_idle)
        fspi_armed <= 1'b1;

      if (!brake_request)
        brake_event_armed <= 1'b1;

      if (rearm_rising && brake_latched) begin

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
