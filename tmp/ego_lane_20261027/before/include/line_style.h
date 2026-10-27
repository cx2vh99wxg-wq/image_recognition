#ifndef LINE_STYLE_H
#define LINE_STYLE_H
#include <stdint.h>
/* Quality is an engineering score, not a calibrated probability. */
enum { LINE_UNKNOWN=0, LINE_SOLID=1, LINE_DASHED=2 };
enum { LINE_OK=0, LINE_NO_MASK=1, LINE_NO_TRACK=2, LINE_SHORT_TRACK=3,
       LINE_BAD_FIT=4, LINE_NO_PAINT=5, LINE_AMBIGUOUS=6, LINE_CONFIRMING=7,
       LINE_DUPLICATE=8, LINE_STALE=9 };
typedef struct {
    uint32_t type, quality, duty, spans, gaps, coverage;
    float x_bottom, x_top;
    uint32_t reason, raw_type;
    float fit_a, fit_b, fit_c;
    uint32_t first_y, last_y;
} YOLOLineBoundary;
typedef struct {
    YOLOLineBoundary left, right;
} YOLOLineStyleResult;
static inline const char* line_style_name(uint32_t style) {
    return style==LINE_SOLID?"SOLID":style==LINE_DASHED?"DASHED":"UNKNOWN";
}
static inline const char* line_reason_name(uint32_t reason) {
    static const char* names[]={"OK","NO MASK","NO TRACK","SHORT","BAD FIT","NO PAINT","AMBIG","WAIT 3","DUPLICATE","STALE"};
    return reason<10?names[reason]:"UNKNOWN";
}
#endif
