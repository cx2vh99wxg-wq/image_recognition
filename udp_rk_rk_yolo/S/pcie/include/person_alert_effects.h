#ifndef _PERSON_ALERT_EFFECTS_H_
#define _PERSON_ALERT_EFFECTS_H_

#include <stdint.h>
#include <time.h>
#include "postprocess.h"

#ifdef __cplusplus
extern "C" {
#endif

// 特效配置参数
#define ALERT_BORDER_WIDTH 12        // 红色边框宽度
#define ALERT_FADE_LAYERS 8          // 渐变层数
#define ALERT_BLINK_INTERVAL 400     // 闪烁间隔 (毫秒)
#define ALERT_MAX_FADE_DISTANCE 60   // 最大淡化距离(像素)

// 颜色定义
typedef struct {
    uint8_t r, g, b;
} Color;

// 特效状态结构
typedef struct {
    uint64_t last_blink_time;       // 上次闪烁时间(毫秒)
    int blink_state;                // 闪烁状态 (0=暗, 1=亮)
    int is_initialized;             // 是否已初始化
} AlertEffectState;

// 渐变计算参数
typedef struct {
    float distance_ratio;           // 距离比例 (0.0-1.0)
    float alpha;                   // 透明度 (0.0-1.0)
    Color color;                   // 当前颜色
} FadeInfo;

// 全局特效状态
extern AlertEffectState g_alert_state;

// 初始化特效系统
void alert_effects_init(void);

// 获取当前时间戳 (毫秒)
uint64_t get_current_time_ms(void);

// 更新闪烁状态
void update_blink_state(AlertEffectState* state);

// 计算渐变信息
void calculate_fade_info(int pixel_x, int pixel_y, const BOX_RECT* box, FadeInfo* fade_info);

// 混合颜色 (基于alpha值混合原色和特效色)
Color blend_colors(Color original, Color effect, float alpha);

// 绘制单个人员的警示特效
void draw_person_alert_effect(uint8_t* rgb_data, int width, int height, 
                             const BOX_RECT* person_box, AlertEffectState* state);

// 应用人员警示特效到所有检测到的人员
void apply_person_alert_effects(uint8_t* rgb_data, int width, int height, 
                               const detect_result_group_t* results);

#ifdef __cplusplus
}
#endif

#endif // _PERSON_ALERT_EFFECTS_H_