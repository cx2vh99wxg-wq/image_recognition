"""Exercise original and revised stand-alone RTL with identical input streams."""
import argparse
import os
from pathlib import Path
import re
import subprocess
import tempfile

from build_pds import FPGA, HERE

TESTS = {
    'cnn/RGB888_YCbCr_cnn.v': ('RGB888_YCbCr_cnn', 'clk'),
    'cnn/conv2d.v': ('conv2d', 'clk'),
    'cnn/rgb_end.v': ('rgb_end', 'clk'),
    'isp/Classifier.v': ('Classifier', 'clk'),
    'isp/ISP/color_binary.v': ('color_binary', 'pixelclk'),
    'isp/ISP/color_dt.v': ('color_dt', 'video_pclk'),
    'isp/ISP/color_en_generate.v': ('color_en_generate', 'clk'),
    'isp/ISP/rgb2hsv.v': ('rgb2hsv', 'clk'),
    'isp/ISP/rgb2ycbcr.v': ('rgb2ycbcr', 'pixelclk'),
    'isp/shape/image_rgb2hsv.v': ('image_rgb2hsv', 'clk'),
    'isp/shape/image_rgb2ycbcr.v': ('image_rgb2ycbcr', 'clk'),
    'isp/shape/threshold_binary.v': ('threshold_binary', 'clk'),
    'isp/fps/fps_calculation.v': ('fps_calculation', ''),
    'isp/fps/fps_count.v': ('fps_count', 'pixelclk'),
    'isp/fps/fps_display.v': ('char_display', 'clk_pixel'),
    'isp/sort.v': ('sort', 'clk'),
    'martix/Line_Shift_RAM.v': ('Line_Shift_RAM', 'clock'),
    'uart_lcd/uart_rx.v': ('uart_rx', 'clk'),
    'uart_lcd/uart_tx.v': ('uart_tx', 'clk'),
}
SUPPORT = ('martix/RAMshift_taps.v',)
PIPELINE_TESTS = {
    'cnn/conv_top.v': ('conv_top', 'clk'),
    'isp/ISP/matrix_3x3_1bit.v': ('matrix_3x3_1bit', 'clk'),
    'isp/matrix_3x3_8bit.v': ('matrix_3x3_8bit', 'clk'),
    'isp/dialate.v': ('dialate', 'clk'),
    'isp/erode.v': ('erode', 'clk'),
    'isp/middle_filter.v': ('middle_filter', 'clk'),
    'martix/line_shift_ram_8bit_new.v': ('line_shift_ram_8bit_new', 'clock'),
}
PIPELINE_SUPPORT = (
    *tuple(f'cnn/conv{layer}.v' for layer in range(1, 8)),
    'cnn/conv_4channel_delay.v',
    'cnn/conv_example.v',
    'cnn/conv_example_1_1.v',
    'cnn/conv_example_rgb_delay.v',
    'cnn/conv_2channel.v',
    'cnn/conv_2channel_7.v',
    'isp/sort.v',
    'martix/Matrix_Generate_3X3_Buf.v',
    'martix/Line_Shift_RAM.v',
    *SUPPORT,
)
AXI_TESTS = {
    'axi_ctrl/axi4_ctrl.v': ('axi4_ctrl', 'axi_clk'),
    'axi_ctrl/axi4_ctrl_3ch.v': ('axi4_ctrl_3ch', 'axi_clk'),
}
BOARD_TESTS = {
    'image_pcie_capture.v': ('video_dma_line_formatter', 'pclk_div2'),
    'ov5640/rtl/ov5640_top.v': ('ov5640_clock_meter', 'clk_50M'),
}
CYCLES = 20000


def bracket_end(text, start):
    depth = 0
    for index in range(start, len(text)):
        if text[index] == '(':
            depth += 1
        elif text[index] == ')':
            depth -= 1
            if not depth:
                return index
    raise ValueError('Unbalanced port list')


def ports(raw, module):
    text = re.sub(rb'//[^\n]*|/\*[\s\S]*?\*/', b'', raw).decode('ascii', errors='replace')
    header = re.search(rf'\bmodule\s+{module}\s*', text)
    start = header.end()
    if text[start] == '#':
        start = bracket_end(text, text.index('(', start)) + 1
    start = text.index('(', start)
    direction, width = None, ''
    for item in text[start + 1:bracket_end(text, start)].split(','):
        parsed = re.fullmatch(
            r'\s*(input|output)\s+(?:(?:wire|reg)\b\s*)?(\[[^\]]+\]\s*)?(\w+)\s*',
            item,
        )
        if parsed:
            direction, width, name = parsed.groups()
        elif direction and re.fullmatch(r'\w+', item.strip()):
            name = item.strip()
        else:
            raise ValueError(f'{module}: unsupported test port {item!r}')
        widths = {'DATA_WIDTH': 8, 'C_DATA_LEN': 128, 'C_STRB_LEN': 16, 'C_ID_LEN': 4,
                  'C_R_WIDTH': 128, 'C_W_WIDTH': 16}
        for symbol, value in widths.items():
            width = (width or '').replace(symbol, str(value))
        yield direction, width, name


def stimulus(name, module):
    if name in {'rst_n', 'reset_n', 'rst'}:
        return 'cycle >= 3 && cycle % 2048 > 1'
    if name == 'axi_reset':
        return 'cycle < 3 || cycle % 2048 < 2'
    if name == 'wframe_vsync':
        return 'cycle % 720 < 480 ? 3\'b111 : 3\'b000'
    if name == 'wframe_data_en':
        return 'cycle % 720 < 480 ? 3\'b111 : 3\'b000'
    if name == 'rframe_vsync':
        return 'cycle % 720 < 480'
    if name in {'ddr_init_done', 'locked', 'lane_init_done'}:
        return 'cycle >= 3'
    if name == 'one_second_pulse':
        return 'cycle % 128 == 0'
    if name == 'measure_enable':
        return 'cycle >= 3 && cycle % 512 < 256'
    if name == 'o_dma_write_addr':
        return 'cycle % 105'
    if name == 'o_dma_write_data_req':
        return 'cycle % 105 < 95'
    if name == 'axi_dma_write_data':
        return '{$urandom, $urandom, $urandom, $urandom}'
    if name == 'char_en' and module == 'char_display':
        return '(cycle / 128) % 43'
    if name in {'vs', 'vs_i', 'i_vsync', 'video_vs', 'vsync', 'i_vs', 'per_img_vsync'}:
        return 'cycle % 128 < 4'
    if name == 'uart_rxd':
        return 'cycle % 640 >= 64 && (cycle % 640 >= 576 || ((cycle / 64) % 2))'
    if name == 'uart_tx_en':
        return 'cycle % 768 == 32'
    if name in {'h_cnt', 'i_x', 'video_x', 'x'}:
        return 'cycle % 640'
    if name in {'v_cnt', 'i_y', 'video_y', 'y'}:
        return '(cycle / 640) % 480' if module == 'char_display' else '$urandom % 480'
    if name == 'fps':
        return 'cycle % 512'
    if module in {pair[0] for pair in PIPELINE_TESTS.values()} and name in {'per_img_href', 'valid_in', 'in_de'}:
        return 'cycle % 12 < 8'
    if name == 'col_left':
        return '80'
    if name == 'row_low':
        return '22'
    return '$urandom'


def bench(raw, module, clock, cycles=CYCLES):
    ports_list = list(ports(raw, module))
    clocks = {clock} if clock else set()
    if module == 'char_display':
        clocks.add('sys_clk')
    elif module == 'fps_count':
        clocks.add('clk')
    elif module.startswith('axi4_ctrl'):
        clocks.update({'wframe_pclk', 'rframe_pclk'})
    elif module == 'ov5640_clock_meter':
        clocks.add('lane_pclk')
    lines = [f'module test_{module}(output reg finished = 0);', 'integer cycle;']
    for direction, width, name in ports_list:
        if direction == 'input':
            lines.append(f'reg {width} {name} = 0;')
            if name in clocks:
                lines.append(f'always #{5 if name == clock else 7} {name} = ~{name};')
        else:
            lines.append(f'wire {width} old_{name}, new_{name};')
    params = ''
    if module in {'uart_rx', 'uart_tx'}:
        params = '#(.CLK_FREQ(64000000), .UART_BPS(1000000))'
    elif module == 'conv_top':
        params = '#(.IMG_HDISP(8), .IMG_VDISP(5))'
    elif module.startswith('matrix_3x3_'):
        params = '#(.COL(8), .ROW(5))'
    elif module in {'dialate', 'erode', 'middle_filter'}:
        params = '#(.U_COL(8), .U_ROW(5))'
    for prefix, suffix in [('old', '_original'), ('new', '')]:
        connections = ', '.join(
            f'.{name}({name if direction == "input" else prefix+"_"+name})'
            for direction, _, name in ports_list
        )
        lines.append(f'{module}{suffix} {params} dut_{prefix} ({connections});')
    coverage = {
        'axi4_ctrl_3ch': 'new_axi_wvalid && axi_wready',
        'video_dma_line_formatter': 'new_ch0_rframe_req',
        'ov5640_clock_meter': 'new_lane_pclk_count_reg != 0',
    }.get(module)
    if coverage:
        lines.append('integer coverage_events = 0;')
    lines += ['initial begin', f'  for (cycle = 0; cycle < {cycles}; cycle = cycle+1) begin']
    if clock:
        lines.append(f'    @(negedge {clock});')
    for direction, _, name in ports_list:
        if direction == 'input' and name not in clocks:
            lines.append(f'    {name} = {stimulus(name, module)};')
    if clock:
        lines.append(f'    @(posedge {clock});')
    lines.append('    #1;')
    for direction, _, name in ports_list:
        if direction == 'output':
            lines += [f'    if (old_{name} !== new_{name})',
                      f'      $fatal(1, "{module}.{name} cycle %0d old=%h new=%h", cycle, old_{name}, new_{name});']
    if coverage:
        lines.append(f'    if ({coverage}) coverage_events = coverage_events + 1;')
    lines.append('  end')
    if coverage:
        lines.append(f'  if (coverage_events == 0) $fatal(1, "{module}: no coverage event observed");')
    lines += ['  finished = 1;', 'end', 'endmodule']
    return '\n'.join(lines)


def fifo_models():
    """Use the same clocked FIFO model in both paths for ISP wrapper comparison."""
    blocks = []
    for width in (1, 8):
        blocks.append(f'''
module FIFO_SC_{width}bit_Top (
    input clk, rst, wr_en, rd_en,
    input [{width - 1}:0] wr_data,
    output reg [{width - 1}:0] rd_data,
    output rd_empty, wr_full, almost_full, almost_empty
);
    reg [{width - 1}:0] memory [0:1023];
    reg [9:0] read_index, write_index;
    assign rd_empty = read_index == write_index;
    assign wr_full = read_index == write_index + 10'd1;
    assign almost_full = wr_full;
    assign almost_empty = rd_empty;
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            read_index <= 0;
            write_index <= 0;
            rd_data <= 0;
        end else begin
            if (wr_en && !wr_full) begin
                memory[write_index] <= wr_data;
                write_index <= write_index + 1'b1;
            end
            if (rd_en && !rd_empty) begin
                rd_data <= memory[read_index];
                read_index <= read_index + 1'b1;
            end
        end
    end
endmodule
''')
    blocks.append('''
module blk_mem_gen_0 (
    input [7:0] wr_data,
    input [9:0] wr_addr, rd_addr,
    input wr_clk, rd_clk, wr_en, rst,
    output reg [7:0] rd_data
);
    reg [7:0] memory [0:1023];
    always @(posedge wr_clk)
        if (wr_en)
            memory[wr_addr] <= wr_data;
    always @(posedge rd_clk)
        rd_data <= memory[rd_addr];
endmodule
''')
    return '\n'.join(blocks)


def board_reference(rel, module, original):
    """Extract the old stage from its original top without replacing any stage logic."""
    revised = (FPGA / 'source' / rel).read_bytes()
    start = revised.index(f'module {module} ('.encode())
    end = revised.index(b');', start) + 2
    header = revised[start:end] + b'\n'
    if module == 'video_dma_line_formatter':
        declarations = re.search(
            rb'(?m)^  reg[^\n]*\bconcatenated_data;[^\n]*\n(?:^  reg[^\n]*\n){6}',
            original,
        ).group()
        begin = original.index(b'  reg [11:0] o_dma_write_addr_dly1;')
        body = declarations + original[begin:original.rindex(b'endmodule')]
    else:
        begin = original.index(b'  reg [2:0] cmos2_enable_sync;')
        end = original.index(b'  reg [2:0] cmos5_enable_sync;', begin)
        body = original[begin:end].replace(b'cmos2_', b'lane_')
        body = re.sub(rb'(?m)^  reg \[31:0\] lane_pclk_count_reg;\r?\n', b'', body)
    return header + body + b'\nendmodule\n'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--suite', choices=['modules', 'pipeline', 'axi', 'board'], default='modules')
    args = parser.parse_args()
    pipeline = args.suite == 'pipeline'
    tests = {'modules': TESTS, 'pipeline': PIPELINE_TESTS, 'axi': AXI_TESTS, 'board': BOARD_TESTS}[args.suite]
    files = [*tests, *{'modules': SUPPORT, 'pipeline': PIPELINE_SUPPORT, 'axi': (), 'board': ()}[args.suite]]
    cycles = 60000 if args.suite == 'board' else CYCLES
    original = {rel: (HERE / 'original' / rel).read_bytes() for rel in files}
    if args.suite == 'board':
        original = {rel: board_reference(rel, tests[rel][0], raw) for rel, raw in original.items()}
    names = set(re.findall(rb'\bmodule\s+(\w+)', b'\n'.join(original.values())))
    module_names = re.compile(rb'\b(' + b'|'.join(sorted(names)) + rb')\b')
    benches = [bench(original[rel], *pair, cycles=cycles) for rel, pair in tests.items()]
    benches.append(f'module compare_expanded;\nwire [{len(tests)-1}:0] finished;')
    for index, (module, _) in enumerate(tests.values()):
        benches.append(f'test_{module} u_{index}(finished[{index}]);')
    benches.append(f'initial begin wait (&finished); $display("PASS {len(tests)} modules x {cycles} observations"); $finish; end\nendmodule')
    results = Path(os.environ.get('SIM_RESULTS_DIR', HERE / 'results_expanded'))
    results.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='pineapple-expanded-') as work:
        work = Path(work)
        old = work / 'original.v'
        old.write_bytes(b'\n'.join(module_names.sub(rb'\1_original', data) for data in original.values()))
        tb = work / 'compare_expanded.sv'
        models = fifo_models() if pipeline else ''
        if args.suite == 'axi':
            models = (HERE / 'tests' / 'axi_fifo_models.v').read_text(encoding='utf-8')
        tb.write_text('`timescale 1ns/1ps\n' + '\n\n'.join(benches) + models,
                      encoding='utf-8')
        with (results / f'expanded_{args.suite}.build.log').open('wb') as log:
            subprocess.run(['verilator', '--binary', '--timing', '-j', '3', '-Wno-fatal',
                            '--top-module', 'compare_expanded', '--Mdir', str(work / 'obj'),
                            str(tb), str(old), *(str(FPGA / 'source' / f) for f in files)],
                           stdout=log, stderr=subprocess.STDOUT, check=True)
        result = subprocess.run([str(work / 'obj/Vcompare_expanded')], capture_output=True, timeout=180)
        (results / f'expanded_{args.suite}.run.log').write_bytes(result.stdout + result.stderr)
        print(result.stdout.decode(errors='replace'), end='')
        result.check_returncode()


if __name__ == '__main__':
    main()
