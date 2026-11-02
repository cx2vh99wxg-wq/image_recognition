#ifndef _PERSON_ALERT_EFFECTS_H_
#define _PERSON_ALERT_EFFECTS_H_

#include <stdint.h>
#include <time.h>
#include "postprocess.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 闪烁间隔以毫秒计，距离常量以像素计；边框和层数常量保留原接口。 */
#define ALERT_BORDER_WIDTH 12
#define ALERT_FADE_LAYERS 8
#define ALERT_BLINK_INTERVAL 400
#define ALERT_MAX_FADE_DISTANCE 60

/* 混色计算使用 RGB 通道顺序，图像缓冲区写回时采用 BGR 字节顺序。 */
typedef struct {
    uint8_t r, g, b;
} Color;

/* 每个状态记录上次翻转的毫秒时间、当前亮暗状态及初始化标志。 */
typedef struct {
    uint64_t last_blink_time;
    int blink_state;
    int is_initialized;
} AlertEffectState;

/* 兼容旧渐变接口的输出：相对距离、透明度和目标颜色。 */
typedef struct {
    float distance_ratio;
    float alpha;
    Color color;
} FadeInfo;

/* 多个人员框复用的全局闪烁状态。 */
extern AlertEffectState g_alert_state;

/* 重置全局状态并从亮相位开始计时。 */
void alert_effects_init(void);

/* 读取基于 gettimeofday 的当前毫秒时间。 */
uint64_t get_current_time_ms(void);

/* 达到闪烁间隔时翻转状态；未初始化的状态保持原值。 */
void update_blink_state(AlertEffectState* state);

/* 计算给定像素相对于检测框的兼容渐变信息。 */
void calculate_fade_info(int pixel_x, int pixel_y, const BOX_RECT* box, FadeInfo* fade_info);

/* 将 alpha 限制在 0..1 后逐通道混合两种颜色。 */
Color blend_colors(Color original, Color effect, float alpha);

/* 对一个人员框及最近角边缘叠加红色渐变；rgb_data 实际为 BGR 字节布局。 */
void draw_person_alert_effect(uint8_t* rgb_data, int width, int height,
                             const BOX_RECT* person_box, AlertEffectState* state);

/* 遍历检测结果，只对类别名为 person 的框绘制警示效果。 */
void apply_person_alert_effects(uint8_t* rgb_data, int width, int height,
                               const detect_result_group_t* results);

#ifdef __cplusplus
}
#endif

#endif /* _PERSON_ALERT_EFFECTS_H_ */
