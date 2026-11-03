#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
static void draw_filled_rect(uint8_t *rgb_data, int width, int height, int x1, int y1, int x2,
                             int y2, uint8_t r, uint8_t g, uint8_t b)
{
    for (int y = y1; y <= y2 && y < height; y++)
    {
        for (int x = x1; x <= x2 && x < width; x++)
        {
            if (x >= 0 && y >= 0)
            {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = r;
                rgb_data[idx + 1] = g;
                rgb_data[idx + 2] = b;
            }
        }
    }
}


static void old_char(uint8_t *rgb_data, int width, int height, int x, int y, char c, uint8_t r,
                      uint8_t g, uint8_t b)
{
    // 图像处理：简化的5x7字体点阵 - 主要字符
    static const uint8_t font_5x7[][7] = {
        // 参数记录为 0。
        {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
        // 参数记录为 1。
        {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
        // 参数记录为 2。
        {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},
        // 参数记录为 3。
        {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},
        // 参数记录为 4。
        {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
        // 参数记录为 5。
        {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
        // 参数记录为 6。
        {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},
        // 参数记录为 7。
        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
        // 参数记录为 8。
        {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
        // 参数记录为 9。
        {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
        // 图像处理：. (point)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C},
        // 图像处理：%
        {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03},
        // space：本节对应的标识或作用域。
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // A：本节对应的标识或作用域。
        {0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11},
        // B：蓝色分量。
        {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},
        // C：本节对应的标识或作用域。
        {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E},
        // E：本节对应的标识或作用域。
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},
        // P：本节对应的标识或作用域。
        {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},
        // R：红色分量。
        {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},
        // S：本节对应的标识或作用域。
        {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E},
        // O：本节对应的标识或作用域。
        {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
        // N：本节对应的标识或作用域。
        {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11},
        // T：本节对应的标识或作用域。
        {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
        // L：本节对应的标识或作用域。
        {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},
        // I：本节对应的标识或作用域。
        {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},
        // D：本节对应的标识或作用域。
        {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C},
        // F：本节对应的标识或作用域。
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},
        // G：绿色分量。
        {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E},
        // H：本节对应的标识或作用域。
        {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},
        // J：本节对应的标识或作用域。
        {0x0F, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C},
        // K：本节对应的标识或作用域。
        {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},
        // M：本节对应的标识或作用域。
        {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11},
        // U：本节对应的标识或作用域。
        {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
        // V：本节对应的标识或作用域。
        {0x11, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x04},
        // W：本节对应的标识或作用域。
        {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11},
        // X：本节对应的标识或作用域。
        {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},
        // Y：本节对应的标识或作用域。
        {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04},
        // Z：本节对应的标识或作用域。
        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},
    };

    int char_index = -1;

    // 图像处理：映射字符到字体索引
    if (c >= '0' && c <= '9')
    {
        char_index = c - '0';
    }
    else if (c == '.')
    {
        char_index = 10;
    }
    else if (c == '%')
    {
        char_index = 11;
    }
    else if (c == ' ')
    {
        char_index = 12;
    }
    else if (c == 'A' || c == 'a')
    {
        char_index = 13;
    }
    else if (c == 'B' || c == 'b')
    {
        char_index = 14;
    }
    else if (c == 'C' || c == 'c')
    {
        char_index = 15;
    }
    else if (c == 'E' || c == 'e')
    {
        char_index = 16;
    }
    else if (c == 'P' || c == 'p')
    {
        char_index = 17;
    }
    else if (c == 'R' || c == 'r')
    {
        char_index = 18;
    }
    else if (c == 'S' || c == 's')
    {
        char_index = 19;
    }
    else if (c == 'O' || c == 'o')
    {
        char_index = 20;
    }
    else if (c == 'N' || c == 'n')
    {
        char_index = 21;
    }
    else if (c == 'T' || c == 't')
    {
        char_index = 22;
    }
    else if (c == 'L' || c == 'l')
    {
        char_index = 23;
    }
    else if (c == 'I' || c == 'i')
    {
        char_index = 24;
    }
    else if (c == 'D' || c == 'd')
    {
        char_index = 25;
    }
    else if (c == 'F' || c == 'f')
    {
        char_index = 26;
    }
    else if (c == 'G' || c == 'g')
    {
        char_index = 27;
    }
    else if (c == 'H' || c == 'h')
    {
        char_index = 28;
    }
    else if (c == 'J' || c == 'j')
    {
        char_index = 29;
    }
    else if (c == 'K' || c == 'k')
    {
        char_index = 30;
    }
    else if (c == 'M' || c == 'm')
    {
        char_index = 31;
    }
    else if (c == 'U' || c == 'u')
    {
        char_index = 32;
    }
    else if (c == 'V' || c == 'v')
    {
        char_index = 33;
    }
    else if (c == 'W' || c == 'w')
    {
        char_index = 34;
    }
    else if (c == 'X' || c == 'x')
    {
        char_index = 35;
    }
    else if (c == 'Y' || c == 'y')
    {
        char_index = 36;
    }
    else if (c == 'Z' || c == 'z')
    {
        char_index = 37;
    }
    else if (c == '-')
    {
        char_index = 12; // 图像处理：用空格代替短横线
    }
    else if (c == '_')
    {
        char_index = 12; // 图像处理：用空格代替下划线
    }
    else
    {
        // 图像处理：对于不支持的字符，不打印警告（避免日志过多），直接用空格代替
        char_index = 12; // 没有字形时默认显示空白。
    }

    if (char_index < 0 || char_index >= 38)
    {
        return;
    }

    const uint8_t *char_data = font_5x7[char_index];

    // 图像处理：X11 只翻转 S 端画面的 Y 轴；预先倒置字形行，使屏幕上的文字正向且字符顺序不变。
    for (int row = 0; row < 7; row++)
    {
        const uint8_t glyph_row = char_data[6 - row];
        for (int col = 0; col < 5; col++)
        {
            if (glyph_row & (1 << (4 - col)))
            {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < width && py >= 0 && py < height)
                {
                    int idx = (py * width + px) * 3;
                    rgb_data[idx] = r;
                    rgb_data[idx + 1] = g;
                    rgb_data[idx + 2] = b;
                }
            }
        }
    }
}


static void new_char(uint8_t *rgb_data, int width, int height, int x, int y, char c, uint8_t r,
                      uint8_t g, uint8_t b)
{
    // 图像处理：简化的5x7字体点阵 - 主要字符
    static const uint8_t font_5x7[][7] = {
        // 参数记录为 0。
        {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
        // 参数记录为 1。
        {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
        // 参数记录为 2。
        {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},
        // 参数记录为 3。
        {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},
        // 参数记录为 4。
        {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
        // 参数记录为 5。
        {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
        // 参数记录为 6。
        {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},
        // 参数记录为 7。
        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
        // 参数记录为 8。
        {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
        // 参数记录为 9。
        {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
        // 图像处理：. (point)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C},
        // 图像处理：%
        {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03},
        // space：本节对应的标识或作用域。
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // A：本节对应的标识或作用域。
        {0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11},
        // B：蓝色分量。
        {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},
        // C：本节对应的标识或作用域。
        {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E},
        // E：本节对应的标识或作用域。
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},
        // P：本节对应的标识或作用域。
        {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},
        // R：红色分量。
        {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},
        // S：本节对应的标识或作用域。
        {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E},
        // O：本节对应的标识或作用域。
        {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
        // N：本节对应的标识或作用域。
        {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11},
        // T：本节对应的标识或作用域。
        {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
        // L：本节对应的标识或作用域。
        {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},
        // I：本节对应的标识或作用域。
        {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},
        // D：本节对应的标识或作用域。
        {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C},
        // F：本节对应的标识或作用域。
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},
        // G：绿色分量。
        {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E},
        // H：本节对应的标识或作用域。
        {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},
        // J：本节对应的标识或作用域。
        {0x0F, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C},
        // K：本节对应的标识或作用域。
        {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},
        // M：本节对应的标识或作用域。
        {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11},
        // U：本节对应的标识或作用域。
        {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
        // V：本节对应的标识或作用域。
        {0x11, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x04},
        // W：本节对应的标识或作用域。
        {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11},
        // X：本节对应的标识或作用域。
        {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},
        // Y：本节对应的标识或作用域。
        {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04},
        // Z：本节对应的标识或作用域。
        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},
    };

    // The key order matches the bitmap table above.
    static const char glyph_keys[] = "0123456789.% ABCEPRSONTLIDFGHJKMUVWXYZ";
    const char key = (c >= 'a' && c <= 'z') ? (char)(c - 'a' + 'A') : c;
    const char *entry = key != '\0' ? strchr(glyph_keys, key) : NULL;
    const uint8_t *char_data = font_5x7[entry ? entry - glyph_keys : 12];

    // 图像处理：X11 只翻转 S 端画面的 Y 轴；预先倒置字形行，使屏幕上的文字正向且字符顺序不变。
    for (int row = 0; row < 7; row++)
    {
        const uint8_t glyph_row = char_data[6 - row];
        for (int col = 0; col < 5; col++)
        {
            if (glyph_row & (1 << (4 - col)))
            {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < width && py >= 0 && py < height)
                {
                    int idx = (py * width + px) * 3;
                    rgb_data[idx] = r;
                    rgb_data[idx + 1] = g;
                    rgb_data[idx + 2] = b;
                }
            }
        }
    }
}


static void old_border(uint8_t *rgb_data, int width, int height,
                        int label_x, int label_y, int text_width, int text_height, int bg_padding)
{
        // 图像处理：添加白色边框以增强对比度
        for (int border = 0; border < 1; border++)
        {
            // 矩形轮廓的顶部边。
            for (int x = label_x - bg_padding; x <= label_x + text_width + bg_padding; x++)
            {
                if (x >= 0 && x < width && label_y - bg_padding + border >= 0 &&
                    label_y - bg_padding + border < height)
                {
                    int idx = ((label_y - bg_padding + border) * width + x) * 3;
                    rgb_data[idx] = 255;
                    rgb_data[idx + 1] = 255;
                    rgb_data[idx + 2] = 255;
                }
                if (x >= 0 && x < width && label_y + text_height + bg_padding - border >= 0 &&
                    label_y + text_height + bg_padding - border < height)
                {
                    int idx = ((label_y + text_height + bg_padding - border) * width + x) * 3;
                    rgb_data[idx] = 255;
                    rgb_data[idx + 1] = 255;
                    rgb_data[idx + 2] = 255;
                }
            }
            // 矩形轮廓的两条侧边。
            for (int y = label_y - bg_padding; y <= label_y + text_height + bg_padding; y++)
            {
                if (y >= 0 && y < height && label_x - bg_padding + border >= 0 &&
                    label_x - bg_padding + border < width)
                {
                    int idx = (y * width + (label_x - bg_padding + border)) * 3;
                    rgb_data[idx] = 255;
                    rgb_data[idx + 1] = 255;
                    rgb_data[idx + 2] = 255;
                }
                if (y >= 0 && y < height && label_x + text_width + bg_padding - border >= 0 &&
                    label_x + text_width + bg_padding - border < width)
                {
                    int idx = (y * width + (label_x + text_width + bg_padding - border)) * 3;
                    rgb_data[idx] = 255;
                    rgb_data[idx + 1] = 255;
                    rgb_data[idx + 2] = 255;
                }
            }
        }


}

static void new_border(uint8_t *rgb_data, int width, int height,
                        int label_x, int label_y, int text_width, int text_height, int bg_padding)
{
        // Draw the inclusive outline with the same clipping as the background.
        const int x0 = label_x - bg_padding;
        const int y0 = label_y - bg_padding;
        const int x1 = label_x + text_width + bg_padding;
        const int y1 = label_y + text_height + bg_padding;
        draw_filled_rect(rgb_data, width, height, x0, y0, x1, y0, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x0, y1, x1, y1, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x0, y0, x0, y1, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x1, y0, x1, y1, 255, 255, 255);


}


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
    printf("Identical pixels: %u glyph cases, %u outline cases\n", chars, borders);
}
