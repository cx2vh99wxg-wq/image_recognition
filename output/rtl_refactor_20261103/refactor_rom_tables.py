"""Consolidate the Gamma lookup pipeline and pack the SCCB register table."""
from pathlib import Path
import re

from build_pds import FPGA, HERE
from refactor_gamma_curves import GROUP

SOURCE = FPGA / 'source'
SNAPSHOT = HERE / 'deep_before'


def hex_rows(values, bits, columns):
    digits = bits // 4
    return ',\n'.join(
        '    ' + ', '.join(f"{bits}'h{value:0{digits}X}" for value in values[i:i + columns])
        for i in range(0, len(values), columns)
    )


def original(path):
    snapshot = SNAPSHOT / path
    return (snapshot if snapshot.exists() else SOURCE / path).read_bytes()


def gamma_rom(path):
    source = original(path)
    rows = {}
    for labels, value in GROUP.findall(source):
        for key in re.findall(rb"8'h([\da-f]{2})", labels, re.I):
            address = int(key, 16)
            if address in rows:
                raise ValueError(f'{path}: duplicate address {address}')
            rows[address] = int(value, 16)
    if set(rows) != set(range(256)):
        raise ValueError(f'{path}: incomplete curve')
    table = hex_rows([rows[i] for i in reversed(range(256))], 8, 16)
    unpacked = [int(value, 16) for value in re.findall(r"8'h([0-9A-F]{2})", table)][::-1]
    assert unpacked == [rows[i] for i in range(256)]
    prefix = source.decode('utf-8').split('always@', 1)[0]
    prefix = re.sub(r'output\s+reg\s+\[7:0\]', 'output wire [7:0]', prefix)
    rtl = prefix + f"""// 高地址在高位，输入亮度直接索引对应的字节。
localparam [2047:0] CURVE_BYTES = {{
{table}
}};

gamma_curve_lookup #(.CURVE_BYTES(CURVE_BYTES)) u_lookup (
    .clk(clk), .pixel_in(Pre_Data), .pixel_out(Post_Data)
);
endmodule
"""
    if path.stem == 'Curve_Gamma_18':
        rtl += """
module gamma_curve_lookup #(parameter [2047:0] CURVE_BYTES = 0) (
    input wire clk,
    input wire [7:0] pixel_in,
    output reg [7:0] pixel_out
);
    // 拆成 16 个等长页，低四位只在选中的页内索引。
    wire [127:0] curve_page = CURVE_BYTES[{pixel_in[7:4], 7'b0} +: 128];
    wire [7:0] curve_pixel = curve_page[{pixel_in[3:0], 3'b0} +: 8];

    // 未知输入对应旧表无匹配项，沿用上一次输出。
    always @(posedge clk) begin
        case (^pixel_in)
            1'b0, 1'b1: pixel_out <= curve_pixel;
            default: ;
        endcase
    end
endmodule
"""
    return rtl.encode()


def gamma_judge(path):
    rtl = original(path).decode('utf-8').replace('\r\n', '\n')
    declarations = re.findall(r'(?m)^wire \[7:0\] gamma_(\d+);\n', rtl)
    assert list(map(int, declarations)) == list(range(18, 91))
    rtl = re.sub(r'(?m)^wire \[7:0\] gamma_\d+;\n', '', rtl)
    start = rtl.index('    reg', rtl.index(');'))
    rtl = rtl[:start] + 'wire [7:0] curve_luminance [18:90];\n\n' + rtl[start:]
    rtl = re.sub(r'\bgamma_(\d+)\b', r'curve_luminance[\1]', rtl)
    names = {'per_data_1': 'score_stage1', 'post_img_Y_dalay_1': 'original_y_stage1',
             'post_img_href_1': 'href_stage1', 'post_img_vsync_1': 'vsync_stage1'}
    rtl = re.sub(r'\b(?:' + '|'.join(names) + r')\b', lambda match: names[match[0]], rtl)
    return rtl.encode()


def camera_rom(path):
    rtl = original(path).decode('utf-8').replace('\r\n', '\n')
    start = rtl.index('always @(reg_index) begin')
    end = rtl.index('endmodule', start)
    rows = re.findall(r"(?m)^\s*(\d+): reg_data <= \{16'h([\da-f]+),8'h([\da-f]+)\};\s*$",
                      rtl[start:end], re.I)
    assert [int(address) for address, *_ in rows] == list(range(251))
    words = [int(register + value, 16) for _, register, value in rows] + [0xffffff] * 5
    table = hex_rows(words[::-1], 24, 8)
    assert [int(word, 16) for word in re.findall(r"24'h([0-9A-F]{6})", table)][::-1] == words
    return (rtl[:start] + f"""// 每项按寄存器地址、数据字节排列，低位对应最早发送的配置字。
localparam [6143:0] INIT_WORDS = {{
{table}
}};

// 保留原索引触发和非阻塞更新。越界或未知索引返回全一。
always @(reg_index) begin
    case (^reg_index)
        1'b0, 1'b1: begin
            if (reg_index[8])
                reg_data <= 24'hffffff;
            else
                reg_data <= INIT_WORDS[reg_index[7:0] * 24 +: 24];
        end
        default: reg_data <= 24'hffffff;
    endcase
end

""" + rtl[end:]).encode()


def main():
    revisions = {Path(f'cnn/Curve_Gamma_{i}.v'): gamma_rom for i in range(18, 91)}
    revisions.update({Path('cnn/gamma_judge.v'): gamma_judge, Path('ov5640/rtl/reg_config.v'): camera_rom})
    pending = []
    for path, transform in revisions.items():
        before = original(path)
        after = transform(path)
        if (SOURCE / path).read_bytes() not in (before, after):
            raise ValueError(f'{path}: unexpected local edit')
        pending.append((path, before, after))
    for path, before, after in pending:
        snapshot = SNAPSHOT / path
        snapshot.parent.mkdir(parents=True, exist_ok=True)
        if not snapshot.exists():
            snapshot.write_bytes(before)
        target = SOURCE / path
        if target.read_bytes() != after:
            temp = target.with_suffix('.rom-refactor')
            temp.write_bytes(after)
            temp.replace(target)
    print('Verified 18,688 Gamma bytes and 251 OV5640 register words')


if __name__ == '__main__':
    main()
