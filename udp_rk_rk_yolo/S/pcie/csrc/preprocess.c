#include "preprocess.h"

/* 维持本板公开预处理入口，并选择其已有的采样策略。 */
#define PERCEPTION_LETTERBOX_UNIT_SCALE_COPY 1
#include "../../../common/image/letterbox_rgb888_impl.inc"
#undef PERCEPTION_LETTERBOX_UNIT_SCALE_COPY
