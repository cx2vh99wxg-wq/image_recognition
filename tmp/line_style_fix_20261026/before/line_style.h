#ifndef LINE_STYLE_H
#define LINE_STYLE_H
#include <stdint.h>
/* Quality is an engineering score, not a calibrated probability. */
enum { LINE_UNKNOWN=0, LINE_SOLID=1, LINE_DASHED=2 };
typedef struct {
    uint32_t type, quality, duty, spans, gaps, coverage;
    float x_bottom, x_top;
} YOLOLineBoundary;
typedef struct {
    YOLOLineBoundary left, right;
} YOLOLineStyleResult;
static inline const char* line_style_name(uint32_t style) {
    return style==LINE_SOLID?"SOLID":style==LINE_DASHED?"DASHED":"UNKNOWN";
}
#endif
