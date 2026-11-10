#ifndef YOLO_QUANTIZED_DECODE_H
#define YOLO_QUANTIZED_DECODE_H

#include <stdio.h>
#include <string.h>

namespace yolo_quantized
{
class Labels
{
  public:
    int load(const char *path, const char *line_endings)
    {
        if (!path)
        {
            printf("标签文件路径无效\n");
            return -1;
        }
        FILE *file = fopen(path, "r");
        if (!file)
        {
            printf("无法打开标签文件: %s\n", path);
            return -1;
        }

        char line[256];
        int count = 0;
        // Keep the original read order and truncation for long lines.
        while (fgets(line, sizeof(line), file) && count < OBJ_CLASS_NUM)
        {
            const size_t end = strcspn(line, line_endings);
            const size_t length = end < OBJ_NAME_MAX_SIZE ? end : OBJ_NAME_MAX_SIZE - 1;
            memcpy(names[count], line, length);
            names[count++][length] = '\0';
        }
        fclose(file);
        if (count < OBJ_CLASS_NUM)
            printf("警告: 只加载了 %d 个标签，期望 %d 个\n", count, OBJ_CLASS_NUM);

        loaded = true;
        printf("成功加载 %d 个标签\n", count);
        return 0;
    }

    void reset() { loaded = false; }

    void name(int id, char (&output)[OBJ_NAME_MAX_SIZE]) const
    {
        if (loaded && id < OBJ_CLASS_NUM)
        {
            strncpy(output, names[id], OBJ_NAME_MAX_SIZE - 1);
            output[OBJ_NAME_MAX_SIZE - 1] = '\0';
        }
        else
        {
            snprintf(output, OBJ_NAME_MAX_SIZE, "class_%d", id);
        }
    }

  private:
    char names[OBJ_CLASS_NUM][OBJ_NAME_MAX_SIZE] = {};
    bool loaded = false;
};

inline int8_t quantize_threshold(float threshold, int32_t zero_point, float scale)
{
    const float quantized = (threshold / scale) + zero_point;
    return (int8_t)(quantized <= -128 ? -128 : (quantized >= 127 ? 127 : quantized));
}

inline float dequantize(int8_t value, int32_t zero_point, float scale)
{
    return ((float)value - (float)zero_point) * scale;
}

inline int image_edge(float edge, float scale)
{
    const float coordinate = edge / scale;
    return (int)(coordinate < 0 ? 0 : coordinate);
}

template <typename Emit>
void scan(int8_t *const (&outputs)[3], int height, int width, float display_threshold,
          float scan_threshold, bool inclusive_class_scan, BOX_RECT pads,
          float scale_w, float scale_h, const Labels &labels, Emit emit,
          const unsigned char *allowed_classes = nullptr)
{
    static const int anchors[3][6] = {{10,13,16,30,33,23},
                                      {30,61,62,45,59,119},
                                      {116,90,156,198,373,326}};
    const int32_t zero_point = -128;
    const float quant_scale = 0.003921569;
    const int8_t display_cutoff = quantize_threshold(display_threshold, zero_point, quant_scale);
    const int8_t scan_cutoff = quantize_threshold(scan_threshold, zero_point, quant_scale);
    const auto read = [&](int8_t value) { return dequantize(value, zero_point, quant_scale); };

    for (int level = 0; level < 3; ++level)
    {
        const int stride = 8 << level;
        const int grid_w = width / stride;
        const int grid_h = height / stride;
        if (grid_w <= 0 || grid_h <= 0)
            continue;
        const int plane = grid_w * grid_h;
        for (int anchor = 0; anchor < 3; ++anchor)
        {
            const int8_t *anchor_data = outputs[level] + PROP_BOX_SIZE * anchor * plane;
            for (int cell = 0; cell < plane; ++cell)
            {
                const int8_t *values = anchor_data + cell;
                const int8_t object_score = values[4 * plane];
                if (object_score < scan_cutoff)
                    continue;

                int class_id = 0;
                int8_t class_score = values[5 * plane];
                for (int id = 1; id < OBJ_CLASS_NUM; ++id)
                {
                    const int8_t score = values[(5 + id) * plane];
                    if (score > class_score)
                    {
                        class_id = id;
                        class_score = score;
                    }
                }
                if (class_score < scan_cutoff || (!inclusive_class_scan && class_score == scan_cutoff))
                    continue;
                if (allowed_classes && !allowed_classes[class_id])
                    continue;

                float x = read(values[0]) * 2.0 - 0.5;
                float y = read(values[plane]) * 2.0 - 0.5;
                float w = read(values[2 * plane]) * 2.0;
                float h = read(values[3 * plane]) * 2.0;
                x = (x + cell % grid_w) * (float)stride;
                y = (y + cell / grid_w) * (float)stride;
                w = w * w * (float)anchors[level][anchor * 2];
                h = h * h * (float)anchors[level][anchor * 2 + 1];
                x -= w / 2.0;
                y -= h / 2.0;

                const float left = x - pads.left;
                const float top = y - pads.top;
                detect_result_t result = {};
                result.box.left = image_edge(left, scale_w);
                result.box.top = image_edge(top, scale_h);
                result.box.right = image_edge(left + w, scale_w);
                result.box.bottom = image_edge(top + h, scale_h);
                result.prop = read(class_score) * read(object_score);
                labels.name(class_id, result.name);
                if (!emit(result, object_score >= display_cutoff && class_score > display_cutoff))
                    return;
            }
        }
    }
}
}

#endif
