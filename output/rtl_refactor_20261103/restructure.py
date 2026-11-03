"""Reproduce the four edits from the saved PDS RTL baseline."""
from pathlib import Path
import re

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
SOURCE = ROOT / 'fpga/fpga_pcie_ov5640/source'
PACKAGE = HERE / 'replacement/fpga/fpga_pcie_ov5640/source'


def splice(text, first, last, body):
    start = text.index(first)
    stop = text.index(last, start)
    return text[:start] + body + text[stop:]


def spi(text):
    text = splice(text, '\t\t\tIDLE: begin', '\t\t\tdefault: state <= IDLE;', '''            IDLE: begin
                ram_wr_en <= 1'b0;
                byte_cnt <= 2'd0;
                // The first valid byte is the high address for either command.
                if (rx_pose) begin
                    case (cmd_reg)
                        2'b01: begin
                            addr_high <= wr_data_reg;
                            transaction_high <= wr_data_reg;
                            transaction_high_zero <= (wr_data_reg == 8'd0);
                            state <= RCV_ADDR_H;
                            byte_cnt <= 2'd1;
                        end
                        2'b10: begin
                            read_addr_high <= wr_data_reg;
                            transaction_high <= wr_data_reg;
                            transaction_high_zero <= (wr_data_reg == 8'd0);
                            state <= READ_ADDR_H;
                            byte_cnt <= 2'd1;
                        end
                        default: ;
                    endcase
                end
            end

            RCV_ADDR_H, READ_ADDR_H: begin
                // Keep the separate high-byte registers and debug state codes.
                if (rx_pose && byte_cnt == 2'd1) begin
                    ram_addr <= {transaction_high, wr_data_reg};
                    // Decode in parallel with address capture to keep the bank's
                    // subsequent write-enable path short at the FSPI RAM clock.
                    address_in_bank <= transaction_high_zero && wr_data_reg < 8'd6;
                    address_is_gear <= transaction_high_zero && wr_data_reg == 8'd6;
                    address_read_valid <= transaction_high_zero && wr_data_reg < 8'd7;
                    byte_cnt <= 2'd2;
                    state <= (state == RCV_ADDR_H) ? RCV_ADDR_L : READ_ADDR_L;
                end
            end

            RCV_ADDR_L: begin
                if (rx_pose && byte_cnt == 2'd2) begin
                    // The bank sees this enable on the following falling edge.
                    ram_wr_en <= 1'b1;
                    byte_cnt <= 2'd3;
                    state <= WRITE_DATA;
                end
            end
            WRITE_DATA: begin
                ram_wr_en <= 1'b0;
                state <= IDLE;
            end

            READ_ADDR_L: begin
                if (byte_cnt == 2'd2)
                    state <= READ_DATA;
            end
            READ_DATA: begin
                if (tx_nege)
                    state <= IDLE;
            end

''')
    text = text.replace('// assign。', '''// Address predicates are registered on the same edge as ram_addr.
reg [7:0] transaction_high;
reg transaction_high_zero;
reg address_in_bank, address_is_gear, address_read_valid;

// assign。''', 1)
    text = text.replace("\t\tread_addr_high <= 8'd0;", """\t\tread_addr_high <= 8'd0;
        transaction_high <= 8'd0;
        transaction_high_zero <= 1'b1;
        address_in_bank <= 1'b1;
        address_is_gear <= 1'b0;
        address_read_valid <= 1'b1;""", 1)
    text = text.replace("ram_wr_en && ram_addr == 16'd6", "ram_wr_en && address_is_gear")
    text = text.replace("ram_wr_en && ram_addr[15:0] < 16'd6", "ram_wr_en && address_in_bank")
    text = text.replace("ram_addr[15:0] < 16'd7", "address_read_valid")
    return text


def vehicle(text):
    text = splice(text, '  function [7:0] frame_byte', '  uart_tx #(', '''  localparam [135:0] TEXT_PREFIX = {"page0.tGear.txt=", 8'h22};
  localparam [183:0] SLIDER_PREFIX = "page0.vaGearActual.val=";

  function [7:0] frame_byte(input [5:0] n,input [7:0] gear);
    begin
      if (n < 17)
        frame_byte = TEXT_PREFIX[135 - 8*n -: 8];
      else if (n >= 24 && n <= 46)
        frame_byte = SLIDER_PREFIX[183 - 8*(n-24) -: 8];
      else case (n)
        17: frame_byte = gear;
        18, 19: frame_byte = 8'hb5;  // GB2312 档
        20: frame_byte = 8'h22;
        47: frame_byte = (gear == "?" ? "0" : gear);
        default: frame_byte = 8'hff; // Three terminator bytes per command.
      endcase
    end
  endfunction
''')
    start = text.index('    always @(posedge clk or negedge rst_n)', text.index('module remote_speed_controller'))
    end = text.index('endmodule', start)
    old = text[start:end]
    schedule = old[old.index('            case(auto_state)'):old.index('            if(sync_value==')]
    text = text[:start] + '''    wire calibrate = sync_value == 8'ha5 && previous_sync != 8'ha5 &&
                     ready && key_pipe[1] && !pressed && !drive_active;

    // CDC stages and event history are sampled independently of scheduling.
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

    // The estimator counts qualified contact edges, including automatic presses.
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

    // Later assignments win on the same edge: target, manual takeover, A5.
    // A5 cancels scheduling without clearing the old pulse counter.
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            pulse_count <= 0; auto_state <= IDLE; wanted <= 1;
            attempts_left <= 0; pending <= 0; phase_at_press <= 0;
        end else begin
''' + schedule + '''            if (calibrate) begin
                pending <= 0;
                auto_state <= IDLE;
            end
        end
    end
''' + text[end:]
    return text


def camera(text):
    text = splice(text, 'always @(posedge cam_pclk or negedge rst_n)', '// 摄像头时序：采输入场同步信号的上升沿\nassign', '''// Coordinates count delivered pixels across lines and frames.
wire last_column = active_x == H_PIXEL-1;
always @(posedge cam_pclk or negedge rst_n) begin
    if (!rst_n) begin
        active_x <= 0;
        active_y <= 0;
    end else if (cmos_frame_valid) begin
        if (last_column) begin
            active_x <= 0;
            if (active_y == V_PIXEL-1)
                active_y <= 0;
            else
                active_y <= active_y + 1;
        end else active_x <= active_x + 1;
    end
end

''')
    text = splice(text, '// 每帧完成后推进帧计数。', '// 摄像头时序：8位数据转16位RGB565数据', '''// Open on the edge after WAIT_FRAME counted VSYNC rises. Stay open until reset.
always @(posedge cam_pclk or negedge rst_n) begin
    if (!rst_n) begin
        cmos_ps_cnt <= 4'd0;
        frame_val_flag <= 1'b0;
    end else if (pos_vsync) begin
        if (cmos_ps_cnt < WAIT_FRAME) cmos_ps_cnt <= cmos_ps_cnt + 4'd1;
        if (cmos_ps_cnt == WAIT_FRAME) frame_val_flag <= 1'b1;
    end
end

''')
    text = text[:text.index('// 摄像头时序：8位数据转16位RGB565数据')] + '''// The exported valid pulse follows the previous byte phase, including the
// first blanking cycle after an odd-byte line. Keep the last assembled word.
always @(posedge cam_pclk or negedge rst_n) begin
    if (!rst_n) begin
        cmos_data_t <= 16'd0;
        cam_data_d0 <= 8'd0;
        byte_flag <= 1'b0;
        byte_flag_d0 <= 1'b0;
    end else begin
        byte_flag_d0 <= byte_flag;
        if (cam_href) begin
            byte_flag <= ~byte_flag;
            cam_data_d0 <= cam_data;
            if (byte_flag) cmos_data_t <= {cam_data_d0,cam_data};
        end else begin
            byte_flag <= 1'b0;
            cam_data_d0 <= 8'd0;
        end
    end
end

endmodule
'''
    return text


def gamma(text):
    begin = text.index('// 判断。')
    stop = text.index(' Curve_Gamma_18 Curve_Gamma_18(', begin)
    thresholds = [(int(limit), signal) for limit, signal in
                  re.findall(r'per_data_1<=(\d+)\)\s+post_img_Y<=(\w+);', text[begin:stop])]
    assert len(thresholds) == 58 and thresholds[0] == (570, 'per_img_Y')
    assert thresholds[-1] == (1012, 'gamma_19')
    assert all(a[0] < b[0] for a, b in zip(thresholds, thresholds[1:]))

    def tree(rows, depth):
        indent = '    '*depth
        if len(rows) == 1:
            return [f'{indent}post_img_Y <= {rows[0][1]};']
        split = len(rows)//2
        return ([f"{indent}if (per_data_1 <= 10'd{rows[split-1][0]}) begin"]
                + tree(rows[:split], depth+1) + [indent+'end else begin']
                + tree(rows[split:], depth+1) + [indent+'end'])

    selection = [
        '// The score is delayed. The bypass takes the current luminance input.',
        '// Curve outputs already carry their own one-clock latency.',
        'always @(posedge clk or negedge rst_n) begin',
        "    if (!rst_n) post_img_Y <= 8'd0;",
        '    else if (post_img_href_1) begin',
        "        if (per_data_1 <= 10'd570) post_img_Y <= per_img_Y;",
        "        else if (per_data_1 <= 10'd1012) begin",
        *tree(thresholds[1:], 3),
        "        end else if (per_data_1 > 10'd1012) post_img_Y <= gamma_18;",
        '    end',
        'end',
        ''
    ]
    return text[:begin] + '\n'.join(selection) + '\n' + text[stop:]


if __name__ == '__main__':
    changes = {'fspi/spi_ram_crtl.v': spi, 'remote_vehicle_control.v': vehicle,
               'ov5640/rtl/cmos_capture_data.v': camera, 'cnn/gamma_judge.v': gamma}
    staged = {}
    for name, transform in changes.items():
        raw = (HERE/'original'/name).read_bytes()
        original = raw.decode('utf-8')
        text = transform(original.replace('\r\n', '\n'))
        text = '// 2026-11-03: cycle-preserving structure refactor.\n' + text
        staged[name] = text.replace('\n', '\r\n' if b'\r\n' in raw else '\n').encode('utf-8')
    for name, raw in staged.items():
        current = (SOURCE/name).read_bytes()
        if current not in (raw, (HERE/'original'/name).read_bytes()):
            raise RuntimeError(f'Source has changed since this migration: {name}')
    for name, raw in staged.items():
        (SOURCE/name).write_bytes(raw)
        target = PACKAGE/name
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(raw)
