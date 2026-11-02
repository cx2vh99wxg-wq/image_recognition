#include "preprocess.h"

/* Keep this board's public entry point and existing sampling policy. */
#define PERCEPTION_LETTERBOX_UNIT_SCALE_COPY 0
#include "../../../common/image/letterbox_rgb888_impl.inc"
#undef PERCEPTION_LETTERBOX_UNIT_SCALE_COPY
