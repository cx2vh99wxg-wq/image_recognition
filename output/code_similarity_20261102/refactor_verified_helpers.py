"""Consolidate duplicated pixel swapping and font initialization without changing APIs."""
from collections import OrderedDict
import hashlib
import json
from pathlib import Path
import re

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
audit=json.loads((OUT/'audit_before.json').read_text(encoding='utf-8'))
records={item['path']:item for item in audit['files']}
names=['udp_rk_rk_yolo/S/pcie/include/camera_orientation.h',
       'udp_rk_rk_yolo/M/pcie/csrc/yolop_opencv_integration.cpp']
texts={}
for name in names:
    path=ROOT/name
    raw=path.read_bytes()
    assert hashlib.sha256(raw).hexdigest()==records[name]['sha256'],name
    backup=OUT/'baseline'/name
    backup.parent.mkdir(parents=True,exist_ok=True)
    backup.write_bytes(raw)
    texts[name]=(raw.decode('utf-8').replace('\r\n','\n'),'\r\n' if b'\r\n' in raw else '\n')

camera,camera_newline=texts[names[0]]
marker='// Rotate each camera tile in place, keeping the tiles in their original slots.'
helper='''// Exchange mirrored spans; shared by paired tile rows and an odd middle row.
static inline void camera_swap_mirrored_span(uint16_t* forward, uint16_t* backward,
                                             int pixel_count) {
    for (int remaining = pixel_count; remaining > 0; --remaining) {
        const uint16_t saved = *forward;
        *forward++ = *backward;
        *backward-- = saved;
    }
}

'''
assert camera.count(marker)==1
camera=camera.replace(marker,helper+marker)
paired='''                for (int x = 0; x < tile_width; ++x) {
                    uint16_t tmp = top[x];
                    top[x] = bottom[tile_width - 1 - x];
                    bottom[tile_width - 1 - x] = tmp;
                }'''
middle='''                for (int x = 0; x < tile_width / 2; ++x) {
                    uint16_t tmp = middle[x];
                    middle[x] = middle[tile_width - 1 - x];
                    middle[tile_width - 1 - x] = tmp;
                }'''
assert camera.count(paired)==camera.count(middle)==1
camera=camera.replace(paired,'                camera_swap_mirrored_span(top, bottom + tile_width - 1, tile_width);')
camera=camera.replace(middle,'                camera_swap_mirrored_span(middle, middle + tile_width - 1, tile_width / 2);')

source,newline=texts[names[1]]
start=source.index('void init_font() {')
end=source.index('// 简单文字绘制函数',start)
body=source[start:end]
glyphs=OrderedDict()
for code,row,value in re.findall(r'font_5x7\[(\d+)\]\[(\d+)\]\s*=\s*(0x[0-9A-Fa-f]+)',body):
    values=glyphs.setdefault(int(code),{})
    assert int(row) not in values
    values[int(row)]=value
assert len(glyphs)==25 and all(set(rows)==set(range(7)) for rows in glyphs.values())
replacement='''void init_font() {
    if (font_initialized) return;

    // One record per supported glyph; unspecified ASCII entries remain blank.
    struct Glyph5x7 {
        unsigned int code;
        uint8_t rows[7];
    };
    static const Glyph5x7 glyphs[] = {
'''
replacement+=''.join('        {%u, {%s}},\n'%(code,', '.join(rows[row] for row in range(7)))
                     for code,rows in glyphs.items())
replacement+='''    };
    memset(font_5x7, 0, sizeof(font_5x7));
    for (const Glyph5x7& glyph : glyphs) {
        memcpy(font_5x7[glyph.code], glyph.rows, sizeof(glyph.rows));
    }
    font_initialized = true;
}

'''
updated=source[:start]+replacement+source[end:]
assert updated[:start]==source[:start] and updated.endswith(source[end:])
(ROOT/names[0]).write_bytes(camera.replace('\n',camera_newline).encode('utf-8'))
(ROOT/names[1]).write_bytes(updated.replace('\n',newline).encode('utf-8'))
print('REFACTORED_TWO_LOCAL_HELPERS glyphs=%u; APIs, pixel order and all model/control code preserved'%len(glyphs))
