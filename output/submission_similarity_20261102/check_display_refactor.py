"""Compare old and new drawing results without starting the vehicle application."""
from pathlib import Path
import subprocess

from refactor_display_tables import SOFTWARE

WORK = Path(__file__).resolve().parent
before = (WORK / 'display_refactor_before' / SOFTWARE.name).read_text(encoding='utf-8')
after = SOFTWARE.read_text(encoding='utf-8')


def between(text, start, stop):
    begin = text.index(start)
    return text[begin:text.index(stop, begin)]


def character(text, name):
    body = between(text, 'static void draw_char(', '// 把字符串逐字绘入图像。')
    return body.replace('draw_char(', name+'(', 1)


def border(text, name, start):
    body = between(text, start, '        // 图像处理：绘制标签文字')
    return f'''static void {name}(uint8_t *rgb_data, int width, int height,
                        int label_x, int label_y, int text_width, int text_height, int bg_padding)
{{
{body}
}}
'''


source = '\n'.join([
    '#include <stdint.h>\n#include <string.h>\n#include <assert.h>\n#include <stdio.h>',
    between(after, 'static void draw_filled_rect(', '// 求出两个边界框'),
    character(before, 'old_char'), character(after, 'new_char'),
    border(before, 'old_border', '        // 图像处理：添加白色边框'),
    border(after, 'new_border', '        // Draw the inclusive outline'),
    '''
int main(void)
{
    enum { width = 12, height = 10 };
    uint8_t a[width * height * 3], b[sizeof(a)];
    unsigned chars = 0, borders = 0;
    for (int c = 0; c < 256; ++c)
        for (int x = -6; x <= width + 1; ++x)
            for (int y = -8; y <= height + 1; ++y) {
                memset(a, 42, sizeof(a)); memset(b, 42, sizeof(b));
                old_char(a, width, height, x, y, (char)c, 7, 180, 211);
                new_char(b, width, height, x, y, (char)c, 7, 180, 211);
                assert(memcmp(a, b, sizeof(a)) == 0);
                ++chars;
            }
    for (int x = -4; x <= width + 1; ++x)
        for (int y = -4; y <= height + 1; ++y)
            for (int w = 0; w <= width + 4; ++w)
                for (int h = 0; h <= height + 4; ++h) {
                    memset(a, 42, sizeof(a)); memset(b, 42, sizeof(b));
                    old_border(a, width, height, x, y, w, h, 2);
                    new_border(b, width, height, x, y, w, h, 2);
                    assert(memcmp(a, b, sizeof(a)) == 0);
                    ++borders;
                }
    printf("Identical pixels: %u glyph cases, %u outline cases\\n", chars, borders);
}
'''])
work = WORK / 'display_refactor_checks'
work.mkdir(exist_ok=True)
code = work / 'compare_pixels.c'
code.write_text(source, encoding='utf-8')
exe = work / 'compare_pixels.exe'
subprocess.run(['gcc', '-std=c11', '-O2', str(code), '-o', str(exe)], check=True)
subprocess.run([str(exe)], check=True)
