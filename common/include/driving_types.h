/*
 * driving_types.h — 公共数据结构（唯一真源）
 *
 * 由【人员 A · 感知】起草并冻结。任何修改必须三人 review。
 * 本文件同时承载两条跨模块契约：
 *   - A → B : LaneResult   (感知层产出，写入 shm_lane)
 *   - B → C : ControlCommandMsg (决策层产出，写入 shm_cmd)
 * 所有结构体按 1 字节对齐，并附编译期尺寸断言，防止成员增删导致
 * 三端共享内存布局错配。
 */
#ifndef DRIVING_TYPES_H
#define DRIVING_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 契约版本：任何字段增删都须自增，便于三端兼容判断 */
#define LANERESULT_VERSION 1u
#define CMDMSG_VERSION     1u
#ifndef TL_VERSION
#define TL_VERSION         1u   /* 红绿灯识别结果（A→B 新增契约） */
#endif

/* ----------------------------------------------------------------------
 * A → B 契约：车道感知结果
 * 物理含义（与旧版差异，务必注意）：
 *   - confidence 为“真实计算值”，不再硬编码 90/75/60/40；
 *   - curve_offset 为“带符号像素偏移”，取代旧版恒为 25° 的伪角度；
 *     正值表示远处的车道线重心相对近处向右偏（右弯），负值左弯。
 *   - 单目相机无法可靠反算真实转角，故只给像素级偏移，由决策层/控制层
 *     自行映射为转向量。
 * 总尺寸：64 字节（共享内存一帧固定大小）。
 * -------------------------------------------------------------------- */
typedef enum {
    LANE_UNKNOWN  = 0,   /* 未检出可靠车道 */
    LANE_STRAIGHT = 1,   /* 直道 */
    LANE_LEFT     = 2,   /* 左弯 */
    LANE_RIGHT    = 3    /* 右弯 */
} LaneDirection;

typedef struct {
    uint32_t        version;        /* 契约版本，固定为 LANERESULT_VERSION */
    uint32_t        frame_id;       /* 单调递增帧号 */
    uint64_t        timestamp_us;   /* 采集时间戳（微秒） */
    LaneDirection   direction;      /* 弯道方向枚举 */
    int32_t         curve_offset;   /* 带符号像素偏移，正值=右弯 */
    uint32_t        confidence;     /* 真实置信度 0~100 */
    uint32_t        lane_pixel_cnt; /* 车道线概率高于阈值的像素总数 */
    uint32_t        reserved[8];    /* 预留扩展位（清零） */
} LaneResult;                      /* = 4+4+8+4+4+4+4+32 = 64 字节 */

/* ----------------------------------------------------------------------
 * B → C 契约：车辆控制命令
 * 收敛为单一枚举 + 优先级，避免旧版“既左转又刹车”的冲突态。
 * 总尺寸：48 字节。
 * -------------------------------------------------------------------- */
typedef enum {
    CMD_NONE   = 0,   /* 无动作 */
    CMD_GO     = 1,   /* 前进 */
    CMD_BACK   = 2,   /* 后退 */
    CMD_LEFT   = 3,   /* 左转 */
    CMD_RIGHT  = 4,   /* 右转 */
    CMD_BRAKE  = 5,   /* 刹车 */
    CMD_STOP   = 6    /* 急停 */
} ControlCommand;

typedef struct {
    uint32_t       version;     /* CMDMSG_VERSION */
    uint32_t       frame_id;    /* 决策帧号 */
    ControlCommand command;     /* 单一动作枚举 */
    uint8_t        enable;      /* 命令生效标志 */
    uint8_t        priority;    /* 优先级，急停设为最高 */
    uint8_t        confidence;  /* 决策置信度 0~100 */
    uint8_t        _pad;        /* 对齐填充 */
    uint32_t       reserved[8]; /* 预留扩展 */
} ControlCommandMsg;            /* = 4+4+4+1+1+1+1+32 = 48 字节 */

/* ----------------------------------------------------------------------
 * A → B 契约（新增）：红绿灯识别结果
 * 红绿灯是前向路口感知，按 10-05 复核结论由【人员 A · 感知】负责识别，
 * B 仅用本结果做「红停绿行」决策。
 *   state     : 当前点亮灯色（红/黄/绿/未知）
 *   detected  : 1=画面中检出点亮的信号灯
 *   box_*     : 点亮灯包围盒（全图像素坐标，-1=无）
 *   red_/yellow_/green_area : 三色各自最大连通块面积（供 B 调试/多灯判定）
 *   confidence: 真实计算（点亮面积 / 参考面积，封顶 100），禁硬编码
 * 总尺寸：64 字节。
 * -------------------------------------------------------------------- */
typedef enum {
    TL_UNKNOWN = 0,   /* 未检出点亮灯 */
    TL_RED     = 1,   /* 红灯 */
    TL_YELLOW  = 2,   /* 黄灯 */
    TL_GREEN   = 3    /* 绿灯 */
} TrafficLightState;

typedef struct {
    uint32_t          version;        /* TL_VERSION */
    uint32_t          frame_id;       /* 与当帧 LaneResult.frame_id 一致 */
    uint64_t          timestamp_us;   /* 与当帧 LaneResult.timestamp_us 一致 */
    TrafficLightState state;          /* 当前点亮灯色 */
    uint32_t          detected;       /* 1=检出点亮信号灯 */
    int32_t           box_x;          /* 灯包围盒左上 x（-1=无） */
    int32_t           box_y;          /* 灯包围盒左上 y */
    int32_t           box_w;          /* 灯包围盒宽 */
    int32_t           box_h;          /* 灯包围盒高 */
    uint32_t          red_area;       /* 红色候选最大连通块面积 */
    uint32_t          yellow_area;    /* 黄色候选最大连通块面积 */
    uint32_t          green_area;     /* 绿色候选最大连通块面积 */
    uint32_t          confidence;     /* 0~100，真实计算 */
    uint32_t          reserved[2];    /* 预留扩展（清零） */
} TrafficLightResult;                 /* = 4+4+8+4+4+4+4+4+4+4+4+4+8 = 64 字节 */

/* ----------------------------------------------------------------------
 * A → B 契约（新增）：车道线形态与斑马线感知结果
 * 与 TrafficLightResult 同属 10-05 新增感知，由【人员 A】识别，B 消费：
 *   - LaneMarkResult：左右车道线实/虚定性 + 压线 + 跨越实线报警信号
 *   - ZebraResult    ：斑马线（人行横道）检出，供行人过街场景决策
 * 两者总尺寸均固定 64 字节，与 LaneResult/TrafficLightResult 同规格。
 * -------------------------------------------------------------------- */
#ifndef LANEMARK_VERSION
#define LANEMARK_VERSION 1u   /* 车道线形态结果版本 */
#endif
#ifndef ZEBRA_VERSION
#define ZEBRA_VERSION    1u   /* 斑马线识别结果版本 */
#endif

typedef enum {
    LANE_MARK_NONE    = 0,   /* 该侧未检出车道线 */
    LANE_MARK_SOLID   = 1,   /* 实线 */
    LANE_MARK_DASHED  = 2,   /* 虚线 */
    LANE_MARK_UNKNOWN = 3    /* 检出但断空率落于模糊区，无法定性 */
} LaneMarkType;

typedef struct {
    uint32_t     version;         /* LANEMARK_VERSION */
    uint32_t     frame_id;        /* 与当帧 LaneResult.frame_id 一致 */
    uint64_t     timestamp_us;    /* 与当帧 LaneResult.timestamp_us 一致 */
    LaneMarkType left_type;       /* 左侧线形态 */
    LaneMarkType right_type;      /* 右侧线形态 */
    int32_t      left_x;          /* 左侧线近场 x，-1=未检出 */
    int32_t      right_x;         /* 右侧线近场 x，-1=未检出 */
    int32_t      ego_offset_px;   /* 车辆中心相对车道中心偏移，正=偏右 */
    uint32_t     left_gap_pm;     /* 左断空率千分比 0~1000（越大越像虚线） */
    uint32_t     right_gap_pm;    /* 右断空率千分比 0~1000 */
    uint32_t     confidence;      /* 结果可信度 0~100 */
    uint8_t      crossing;        /* 1=压线 */
    uint8_t      crossing_left;   /* 1=压左侧线 */
    uint8_t      crossing_right;  /* 1=压右侧线 */
    uint8_t      lane_change;     /* 1=正在变道（跨越车道线） */
    uint32_t     reserved[3];     /* 预留扩展（清零） */
} LaneMarkResult;                 /* = 4+4+8+4+4+4+4+4+4+4+4+4+12 = 64 字节 */

typedef struct {
    uint32_t version;        /* ZEBRA_VERSION */
    uint32_t frame_id;       /* 与当帧 LaneResult.frame_id 一致 */
    uint64_t timestamp_us;   /* 与当帧 LaneResult.timestamp_us 一致 */
    uint32_t detected;       /* 1=检出斑马线 */
    uint32_t stripe_count;   /* 横向亮条纹段数 */
    uint32_t contrast;       /* 明暗对比 0~255 */
    int32_t  center_y;       /* 条纹区域中心行，-1=无 */
    uint32_t confidence;     /* 0~100 */
    uint32_t reserved[7];    /* 预留扩展（清零） */
} ZebraResult;               /* = 4+4+8+4+4+4+4+4+28 = 64 字节 */

/* ----------------------------------------------------------------------
 * 编译期尺寸断言：C 用 _Static_assert，C++ 用 static_assert
 * -------------------------------------------------------------------- */
#ifdef __cplusplus
    static_assert(sizeof(LaneResult) == 64, "LaneResult must be 64 bytes");
    static_assert(sizeof(ControlCommandMsg) == 48, "ControlCommandMsg must be 48 bytes");
    static_assert(sizeof(TrafficLightResult) == 64, "TrafficLightResult must be 64 bytes");
    static_assert(sizeof(LaneMarkResult) == 64, "LaneMarkResult must be 64 bytes");
    static_assert(sizeof(ZebraResult) == 64, "ZebraResult must be 64 bytes");
    static_assert(sizeof(LaneDirection) == 4, "LaneDirection must be 4 bytes");
    static_assert(sizeof(ControlCommand) == 4, "ControlCommand must be 4 bytes");
    static_assert(sizeof(TrafficLightState) == 4, "TrafficLightState must be 4 bytes");
    static_assert(sizeof(LaneMarkType) == 4, "LaneMarkType must be 4 bytes");
#else
    _Static_assert(sizeof(LaneResult) == 64, "LaneResult must be 64 bytes");
    _Static_assert(sizeof(ControlCommandMsg) == 48, "ControlCommandMsg must be 48 bytes");
    _Static_assert(sizeof(TrafficLightResult) == 64, "TrafficLightResult must be 64 bytes");
    _Static_assert(sizeof(LaneMarkResult) == 64, "LaneMarkResult must be 64 bytes");
    _Static_assert(sizeof(ZebraResult) == 64, "ZebraResult must be 64 bytes");
    _Static_assert(sizeof(LaneDirection) == 4, "LaneDirection must be 4 bytes");
    _Static_assert(sizeof(ControlCommand) == 4, "ControlCommand must be 4 bytes");
    _Static_assert(sizeof(TrafficLightState) == 4, "TrafficLightState must be 4 bytes");
    _Static_assert(sizeof(LaneMarkType) == 4, "LaneMarkType must be 4 bytes");
#endif

#ifdef __cplusplus
}
#endif

#endif /* DRIVING_TYPES_H */
