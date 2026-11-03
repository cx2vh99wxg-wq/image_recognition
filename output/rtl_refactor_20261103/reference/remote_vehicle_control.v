`timescale 1ns / 1ps
// 车控模块集中于此文件，接口端口和模块体逻辑均延续原实现。
// FPGA 到 HMI 只回传状态，与接收命令及所有车辆输出相互分离。
// 屏幕按 GB2312 解码，B5 B5 表示“档”；周期刷新可在
// 切页或仅屏幕重启后恢复显示，档位不靠 HMI 本地计数。
module hmi_gear_feedback_reference #(
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

// SPEED 与普通低有效瞬时触点一致；滑块目标采用
// 有界脉冲，但人工持续按住绝不能被截短成固定时长。
module remote_speed_controller_reference #(
    parameter integer DEBOUNCE_CYCLES=250000,
    parameter integer PULSE_CYCLES=12500000, // 滑块触发时长：25 MHz 时钟下为 500 ms。
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
    // 人工触点按下无需先等反馈输入到达高电平；
    // RIGHT 同样没有这一前置条件。只有排队的绝对档位
    // 请求需要有效基准，人工按下和释放始终独立生效。
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
                // 未感知到触点边沿时停止请求，不盲目反复发送脉冲。
                if(phase_gray==phase_at_press)pending<=0;
              end else pulse_count<=pulse_count+1'b1;
              GAP:if(pulse_count>=GAP_CYCLES-1)begin auto_state<=IDLE;pulse_count<=0;end
                  else pulse_count<=pulse_count+1'b1;
              default:auto_state<=IDLE;
            endcase
            if(target_event&&ready&&target_sync>=1&&target_sync<=3)begin
                wanted<=target_sync;pending<=1;attempts_left<=2;
            end
            // 人工输入优先于排队的滑块操作：按住期间
            // 持续输出低电平，收到 00 即刻释放。
            if(hmi_pipe[1]||(sensed_press&&auto_state!=PRESS))begin
                pending<=0;auto_state<=IDLE;pulse_count<=0;
            end
            if(sync_value==8'ha5&&previous_sync!=8'ha5&&ready&&key_pipe[1]&&!pressed&&!drive_active)begin
                phase_gray<=0;pending<=0;auto_state<=IDLE;
            end
        end
    end
endmodule


// 前后轴与转向轴分别仲裁，HMI GO/BACK 不得遮蔽
// 自动 LEFT/RIGHT。各轴保留原过期命令防护：
// 人工释放后先等待该轴 FSPI 位回到空闲。
// HMI 前进/后退意图锁存，直到 STOP 或反方向的新按下事件。
// 左右转向继续遵循原来的按住有效、松开释放规则。
module remote_direction_arbiter_reference (
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

  // STOP（包括自动 STOP）清除锁存意图；阻止期间仍跟踪
  // 接收码，避免 STOP 前按住的按键在 REARM 后
  // 自动恢复驱动，操作者须先松开，再重新按 GO/BACK。
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

      // 采用现有经过滤波的 STOP 信号，多位 CDC 中短暂出现的 E
      // 中间值不能清除合法的前后行驶锁存。
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

// reg5 协议：位 6 表有效，位 7 为翻转心跳，位 [1:0] 分别禁止右/左。
// 位 2 置红灯 GO 保持，位 3 在确认绿灯后解除；同时出现时置位优先。
// 心跳丢失仅撤销原车道权限；曾确认的红灯
// 继续保持，直到显式绿灯或 FPGA 上电复位。
// 旧软件写位 [3:2]=00，不能误解除该保持。
// 稳定字 CDC 配合单个原子字节，避免左右权限混用。
module lane_permission_guard_reference #(
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

// 只有新增的持续使能位跨入现有 STOP 时钟域。
module auto_brake_enable_gate_reference (
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
  // 关闭 AUTO 仅阻止新请求，既有 STOP 锁存
  // 仍由 remote_stop_controller 保持，并依旧要求原 REARM 动作。
  assign enable_sync = enable_pipe[1];
  assign brake_request_out = enable_sync && brake_request_in;
endmodule

// 新自动刹停请求把 STOP 保持为低，直到人工重新按下 E。
// 即使请求仍高，E 也可强制清除锁存；本次
// 请求随即被消费，待其先清零再上升才再次刹停。
// 人工 E 本身仍使 STOP 为低，只有 E 松开后 STOP 才恢复高。
module remote_stop_controller_reference (
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

      // 刹停前已保持的方向命令不能在人工解锁后自动恢复。
      // 四个 FSPI 运动寄存器须全部空闲，才接收下一条命令。
      if (brake_latched || new_brake_event || rearm_command)
        fspi_armed <= 1'b0;
      else if (fspi_directions_idle)
        fspi_armed <= 1'b1;

      if (!brake_request)
        brake_event_armed <= 1'b1;

      if (rearm_rising && brake_latched) begin
        // 明确的人工覆盖会消费仍存在的告警，防止
        // 下一周期立刻再次锁住，抵消本次人工解锁。
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

// UART 解码与遥控按键控制处于不同的时钟域。
// E 码须连续经过八次 25 MHz 采样才允许切换 STOP，
// 以过滤四位 CDC 过程中可能出现的短暂中间码。
module hmi_control_synchronizer_reference (
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
