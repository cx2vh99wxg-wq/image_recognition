"""One-off source migration with exhaustive comparison of the two ROM tables."""
from collections import defaultdict
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]
FPGA = ROOT / 'fpga/fpga_pcie_ov5640/source/isp/fps/fps_display.v'
SOFTWARE = ROOT / 'udp_rk_rk_yolo/S/pcie/csrc/yolo_integration.c'


def rom_body(name, width, rows, new_line):
    addresses = defaultdict(list)
    for index, value in enumerate(rows):
        addresses[value].append(index)
    lookup = name + '_row'
    lines = [f'// Decode identical bitmap rows with one case value.',
             f'function [{width-1}:0] {lookup};', '    input integer address;',
             '    begin', '        case (address)']
    for value, indices in addresses.items():
        for offset in range(0, len(indices), 8):
            chunk = indices[offset:offset+8]
            keys = ', '.join(str(index) for index in chunk)
            suffix = ':' if offset+8 >= len(indices) else ','
            assignment = f" {lookup} = {width}'h{value:0{width//4}X};" if suffix == ':' else ''
            lines.append(f'            {keys}{suffix}{assignment}')
    lines += [f"            default: {lookup} = {{{width}{{1'bx}}}};",
              '        endcase', '    end', 'endfunction', '',
              f'always @(*) begin : load_{name}',
              '    integer address;',
              f'    for (address = 0; address < {len(rows)}; address = address + 1)',
              f'        {name}[address] = {lookup}(address);', 'end']
    return new_line.join(lines)


def update_fpga(data):
    text = data.decode('gb18030')
    newline = '\r\n' if '\r\n' in text else '\n'
    for name, width, size in [('result_char', 80, 624), ('font', 16, 230)]:
        pattern = re.compile(
            rf'(reg\s+\[{width-1}:0\]\s*{name}\[0:{size-1}\];\r?\n)'
            r'always@\(\*\) begin\r?\n(.*?)^end\b', re.S | re.M)
        old = pattern.search(text)
        assert old is not None, name
        entries = re.findall(rf"{name}\[(\d+)\]\s*=\s*{width}'h([0-9a-fA-F]+);", old[2])
        assert [int(index) for index, _ in entries] == list(range(size)), name
        remaining = re.sub(r'//[^\r\n]*', '', old[2])
        remaining = re.sub(rf"{name}\[\d+\]\s*=\s*{width}'h[0-9a-fA-F]+;", '', remaining)
        assert not remaining.strip(), name
        rows = [int(value, 16) for _, value in entries]
        replacement = rom_body(name, width, rows, newline)
        decoded = {}
        for keys, value in re.findall(rf"([\d,\s]+):\s*{name}_row = {width}'h([0-9A-F]+);", replacement):
            for key in re.findall(r'\d+', keys):
                assert int(key) not in decoded
                decoded[int(key)] = int(value, 16)
        assert decoded == dict(enumerate(rows)), name
        text = text[:old.start()] + old[1] + replacement + text[old.end():]
    return text.encode('gb18030')


def update_software(data):
    text = data.decode('utf-8')
    newline = '\r\n' if '\r\n' in text else '\n'
    start = text.index('    int char_index = -1;', text.index('static void draw_char('))
    end = text.index('    // 图像处理：X11', start)
    replacement = """    // The key order matches the bitmap table above.
    static const char glyph_keys[] = "0123456789.% ABCEPRSONTLIDFGHJKMUVWXYZ";
    const char key = (c >= 'a' && c <= 'z') ? (char)(c - 'a' + 'A') : c;
    const char *entry = key != '\\0' ? strchr(glyph_keys, key) : NULL;
    const uint8_t *char_data = font_5x7[entry ? entry - glyph_keys : 12];

"""
    text = text[:start] + replacement.replace('\n', newline) + text[end:]
    start = text.index('        // 图像处理：添加白色边框')
    end = text.index('        // 图像处理：绘制标签文字', start)
    replacement = """        // Draw the inclusive outline with the same clipping as the background.
        const int x0 = label_x - bg_padding;
        const int y0 = label_y - bg_padding;
        const int x1 = label_x + text_width + bg_padding;
        const int y1 = label_y + text_height + bg_padding;
        draw_filled_rect(rgb_data, width, height, x0, y0, x1, y0, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x0, y1, x1, y1, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x0, y0, x0, y1, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x1, y0, x1, y1, 255, 255, 255);

"""
    return (text[:start] + replacement.replace('\n', newline) + text[end:]).encode('utf-8')


if __name__ == '__main__':
    snapshots = Path(__file__).with_name('display_refactor_before')
    assert not snapshots.exists()
    changes = [(p, p.read_bytes(), update) for p, update in [(FPGA, update_fpga), (SOFTWARE, update_software)]]
    outputs = [(p, old, update(old)) for p, old, update in changes]
    snapshots.mkdir()
    for path, original, updated in outputs:
        (snapshots / path.name).write_bytes(original)
        path.write_bytes(updated)
