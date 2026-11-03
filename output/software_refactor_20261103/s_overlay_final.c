#include "yolo_integration.h"
#include "person_alert_effects.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
static void draw_char(uint8_t *rgb_data, int width, int height, int x, int y, char c, uint8_t r,
                      uint8_t g, uint8_t b)
{
    // 简化的5x7字体点阵 - 主要字符
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
        // . (point)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C},
        // %
        {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03},

        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

        {0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11},

        {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},

        {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E},

        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},

        {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},

        {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},

        {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E},

        {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},

        {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11},

        {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},

        {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},

        {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},

        {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C},

        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},

        {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E},

        {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},

        {0x0F, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C},

        {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},

        {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11},

        {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},

        {0x11, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x04},

        {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11},

        {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},

        {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04},

        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},
    };

    // The key order matches the bitmap table above.
    static const char glyph_keys[] = "0123456789.% ABCEPRSONTLIDFGHJKMUVWXYZ";
    const char key = (c >= 'a' && c <= 'z') ? (char)(c - 'a' + 'A') : c;
    const char *entry = key != '\0' ? strchr(glyph_keys, key) : NULL;
    const uint8_t *char_data = font_5x7[entry ? entry - glyph_keys : 12];

    // X11 只翻转 S 端画面的 Y 轴；预先倒置字形行，使屏幕上的文字正向且字符顺序不变。
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

// 把字符串逐字绘入图像。
static void draw_text(uint8_t *rgb_data, int width, int height, int x, int y, const char *text,
                      uint8_t r, uint8_t g, uint8_t b)
{
    int char_width = 6; // 5像素宽度 + 1像素间距
    int pos_x = x;

    for (int i = 0; text[i] != '\0' && pos_x < width - 5; i++)
    {
        if (pos_x >= 0 && y >= 0 && y + 7 < height)
        {
            draw_char(rgb_data, width, height, pos_x, y, text[i], r, g, b);
        }
        pos_x += char_width;
    }
}

// 绘制填充的矩形背景
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

// 求出两个边界框的重叠率(IoU)。
static float calculate_iou(const BOX_RECT *box1, const BOX_RECT *box2)
{
    int x1 = (box1->left > box2->left) ? box1->left : box2->left;
    int y1 = (box1->top > box2->top) ? box1->top : box2->top;
    int x2 = (box1->right < box2->right) ? box1->right : box2->right;
    int y2 = (box1->bottom < box2->bottom) ? box1->bottom : box2->bottom;

    if (x2 <= x1 || y2 <= y1)
        return 0.0f; // 无重叠

    int intersection = (x2 - x1) * (y2 - y1);
    int area1 = (box1->right - box1->left) * (box1->bottom - box1->top);
    int area2 = (box2->right - box2->left) * (box2->bottom - box2->top);
    int union_area = area1 + area2 - intersection;

    return (union_area > 0) ? (float)intersection / union_area : 0.0f;
}

// 从候选中排除重复目标。
static int deduplicate_detections(detect_result_group_t *results, float iou_threshold)
{
    if (!results || results->count <= 1)
        return 0;

    int removed = 0;

    for (int i = 0; i < results->count; i++)
    {
        if (results->results[i].prop <= 0)
            continue; // 该对象此前已标记为删除。

        for (int j = i + 1; j < results->count; j++)
        {
            if (results->results[j].prop <= 0)
                continue; // 该对象此前已标记为删除。

            // 核对是否是相同类别。
            if (strcmp(results->results[i].name, results->results[j].name) == 0)
            {
                float iou = calculate_iou(&results->results[i].box, &results->results[j].box);

                if (iou > iou_threshold)
                {
                    // 重叠度高，保留置信度更高的，删除置信度低的
                    if (results->results[i].prop >= results->results[j].prop)
                    {
                        // 按本轮结果推进i的置信度为最高值。
                        if (results->results[j].prop > results->results[i].prop)
                        {
                            results->results[i].prop = results->results[j].prop;
                        }
                        results->results[j].prop = -1; // 将候选 j 加入删除集合。
                        removed++;
                    }
                    else
                    {
                        // 按本轮结果推进j的置信度为最高值。
                        results->results[j].prop = results->results[j].prop;
                        results->results[i].prop = -1; // 将候选 i 加入删除集合。
                        removed++;
                        break; // i已被删除，跳出内层循环
                    }
                }
            }
        }
    }

    // 压缩数组，移除被标记删除的元素
    int write_idx = 0;
    for (int read_idx = 0; read_idx < results->count; read_idx++)
    {
        if (results->results[read_idx].prop > 0)
        {
            if (write_idx != read_idx)
            {
                results->results[write_idx] = results->results[read_idx];
            }
            write_idx++;
        }
    }
    results->count = write_idx;

    return removed;
}

// 将常见的多词类别压缩为屏幕标签。
static const char *display_class_name(const char *name)
{
    static const struct
    {
        const char *class_name;
        const char *label;
    } aliases[] = {
        {"traffic light", "light"},
        {"fire hydrant", "hydrant"},
        {"stop sign", "stop"},
        {"parking meter", "meter"},
        {"sports ball", "ball"},
        {"baseball bat", "bat"},
        {"baseball glove", "glove"},
        {"wine glass", "wine"},
        {"hot dog", "hotdog"},
        {"potted plant", "plant"},
        {"dining table", "table"},
        {"cell phone", "phone"},
        {"hair drier", "drier"},
        {"teddy bear", "teddy"},
    };

    for (size_t i = 0; i < sizeof(aliases) / sizeof(aliases[0]); ++i)
    {
        if (strcmp(name, aliases[i].class_name) == 0)
            return aliases[i].label;
    }
    return name;
}

void draw_detection_results(uint8_t *rgb_data, int width, int height,
                            const detect_result_group_t *results)
{
    if (!rgb_data || !results)
        return;

    // 创建可修改的副本进行去重
    detect_result_group_t filtered_results = *results;

#if ENABLE_DETECTION_DEDUP
    // 去重处理：合并重叠的同类别检测框
    deduplicate_detections(&filtered_results, DEDUP_IOU_THRESHOLD);
#endif

    // 先应用人员警示特效 (在绘制边框以前)
    apply_person_alert_effects(rgb_data, width, height, &filtered_results);

    for (int i = 0; i < filtered_results.count; i++)
    {
        const detect_result_t *result = &filtered_results.results[i];

        // 先把坐标限制在画布范围内。
        int left =
            result->box.left < 0 ? 0 : (result->box.left >= width ? width - 1 : result->box.left);
        int right = result->box.right < 0
                        ? 0
                        : (result->box.right >= width ? width - 1 : result->box.right);
        int top =
            result->box.top < 0 ? 0 : (result->box.top >= height ? height - 1 : result->box.top);
        int bottom = result->box.bottom < 0
                         ? 0
                         : (result->box.bottom >= height ? height - 1 : result->box.bottom);

        // 选择不同颜色的边界框 - 人员使用白色边框以区分红色警示特效
        uint8_t box_r = 0, box_g = 255, box_b = 0; // 未另行指定时轮廓使用绿色。

        if (strcmp(result->name, "person") == 0)
        {
            // 人员使用白色边框，与红色警示特效形成对比
            box_r = 255;
            box_g = 255;
            box_b = 255;
        }
        else
        {
            // 其他目标使用彩色边框
            switch (i % 3)
            {
            case 0:
                box_r = 255;
                box_g = 0;
                box_b = 0;
                break;
            case 1:
                box_r = 0;
                box_g = 255;
                box_b = 0;
                break;
            case 2:
                box_r = 0;
                box_g = 0;
                box_b = 255;
                break;
            }
        }

        // 每条边从其端点向内画三像素。极窄框也按画布裁剪。
        draw_filled_rect(rgb_data, width, height, left, top, right, top + 2,
                         box_r, box_g, box_b);
        draw_filled_rect(rgb_data, width, height, left, bottom - 2, right, bottom,
                         box_r, box_g, box_b);
        draw_filled_rect(rgb_data, width, height, left, top, left + 2, bottom,
                         box_r, box_g, box_b);
        draw_filled_rect(rgb_data, width, height, right - 2, top, right, bottom,
                         box_r, box_g, box_b);

        char label_text[64];
        char short_name[sizeof(result->name)];
        strcpy(short_name, display_class_name(result->name));
        snprintf(label_text, sizeof(label_text), "%s %.0f%%", short_name, result->prop * 100.0f);

        // 求出标签背景大小。
        int text_width = strlen(label_text) * 6; // 每个字符6像素宽
        int text_height = 8;                     // 7像素高度 + 1像素边距

        // 保证标签不会超出图像边界
        int box_width = right - left;

        // 对于常见的短单词，设置保护，避免截断
        int min_protect_len = 3; // 默认最小保护长度
        if (strcmp(short_name, "bottle") == 0 || strcmp(short_name, "person") == 0 ||
            strcmp(short_name, "chair") == 0 || strcmp(short_name, "table") == 0)
        {
            min_protect_len = strlen(short_name); // 完全保护这些单词
        }

        // 重新计算，保证不过度截断
        if (text_width > box_width)
        {
            // 求出能显示多少个字符。
            int available_chars = box_width / 6;

            if (available_chars >= 8)
            {
                // 有足够空间显示 name + %
                // 预留5个字符给"00%"和空格，剩余给name
                int max_name_chars = available_chars - 5;
                if (max_name_chars >= min_protect_len && max_name_chars < strlen(short_name))
                {
                    short_name[max_name_chars] = '\0';
                    snprintf(label_text, sizeof(label_text), "%s %.0f%%", short_name,
                             result->prop * 100.0f);
                    text_width = strlen(label_text) * 6;
                }
            }
            else if (available_chars >= min_protect_len)
            {
                // 只显示名称，不显示百分比
                int max_name_chars = available_chars;
                if (max_name_chars < strlen(short_name) && max_name_chars >= min_protect_len)
                {
                    short_name[max_name_chars] = '\0';
                }
                snprintf(label_text, sizeof(label_text), "%s", short_name);
                text_width = strlen(label_text) * 6;
            }
            // 若空间不够显示受保护的单词，就保持原样，让它超出边界也没关系
        }

        // 标签位置（智能选择最佳位置）
        int label_x = left;
        int label_y = top - text_height - 2;

        // 若上方空间不够，尝试其他位置
        if (label_y < 5)
        {
            // 尝试放在边界框内部上方
            label_y = top + 3;
            if (label_y + text_height > bottom - 3)
            {
                // 若框太小，放在框下方
                label_y = bottom + 2;
                if (label_y + text_height >= height)
                {
                    // 若下方也没空间，强制放在图像顶部
                    label_y = 2;
                }
            }
        }

        // 保证标签不会超出图像右边界
        if (label_x + text_width >= width)
        {
            label_x = width - text_width - 2;
            if (label_x < 0)
                label_x = 0;
        }

        // 对于极小的检测框，仍然显示标签但调整策略
        int min_box_size = 20; // 用作布局参照的框大小。
        bool is_small_box = ((right - left) < min_box_size || (bottom - top) < min_box_size);

        if (is_small_box)
        {
            // 对于小框，简化标签文本
            if (strlen(short_name) > 4)
            {
                short_name[4] = '\0'; // 该显示字段最多容纳 4 个字符。
            }
            snprintf(label_text, sizeof(label_text), "%s", short_name); // 只显示名称，不显示百分比
            text_width = strlen(label_text) * 6;
        }

        // 扩大背景区域以提高可读性
        int bg_padding = 2;
        draw_filled_rect(rgb_data, width, height, label_x - bg_padding, label_y - bg_padding,
                         label_x + text_width + bg_padding, label_y + text_height + bg_padding, 0,
                         0, 0); // 标签底色取黑色。

        // Draw the inclusive outline with the same clipping as the background.
        const int x0 = label_x - bg_padding;
        const int y0 = label_y - bg_padding;
        const int x1 = label_x + text_width + bg_padding;
        const int y1 = label_y + text_height + bg_padding;
        draw_filled_rect(rgb_data, width, height, x0, y0, x1, y0, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x0, y1, x1, y1, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x0, y0, x0, y1, 255, 255, 255);
        draw_filled_rect(rgb_data, width, height, x1, y0, x1, y1, 255, 255, 255);

        // 绘制标签文字（黄色，更显眼）
        draw_text(rgb_data, width, height, label_x, label_y + 1, label_text, 255, 255,
                  0); // 显示文字采用黄色。
    }
}

