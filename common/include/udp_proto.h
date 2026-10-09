/*
 * udp_proto.h — UDP 传输协议定义（【人员 B】实现，双端唯一真源）
 *
 * 设计目标（相对旧版 M/udp 的改进，见分工方案附录对照表）：
 *  1. 帧头魔数由 0xFF00FF00 改为 ASCII 魔数 'LANE'（可读、避免与图像数据混淆）；
 *  2. 直发 RGB565（640x480x2=614400B），删掉旧版"RGB888→565 往返转换"；
 *  3. 图像数据按块打包（每块 ≤1400B，块头 16B），取代旧版"每行一个包
 *     （482 包/帧）"的低效实现——本协议为 1 帧头 + ~439 块/帧；
 *  4. 弯道结果以语义字段（direction/curve_offset/confidence）随帧头携带，
 *     取代旧版恒 25° 的伪角度 curve_angle；
 *  5. 新增心跳包（200ms）供看门狗使用；命令包（S→M）为调试预留；
 *  6. 所有数值均收进 driving_config.h，本头文件不出现裸数字。
 *
 * 字节序：本协议字段按"双端主机序"传输（M/S 均为 RK3568 aarch64 小端）。
 * 若未来迁移其他架构，需在 pack/unpack 处补字节序转换。图像数据原样透传。
 *
 * 包类型总览：
 *   [M→S] udp_frame_hdr_t 帧头（76B packed，含车道结果语义字段）
 *   [M→S] udp_data_hdr_t  数据块（16B packed + payload）
 *   [双向] udp_heartbeat_t 心跳（16B packed）
 *   [S→M] udp_cmd_t        命令（20B packed，调试/反向控制预留）
 */
#ifndef UDP_PROTO_H
#define UDP_PROTO_H

#include <stdint.h>
#include <stddef.h>
#include "driving_types.h"   /* LaneResult / ControlCommandMsg 字段 */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 协议常量（数值真源在 driving_config.h；魔数定义于此） ---- */
#define UDP_PROTO_VERSION   1u

/* ASCII 魔数（大端可读字符串） */
#define UDP_MAGIC_FRAME   0x4C414E45u   /* 'L''A''N''E' 图像帧头 */
#define UDP_MAGIC_DATA    0x44415441u   /* 'D''A''T''A' 图像数据块 */
#define UDP_MAGIC_HB      0x48424541u   /* 'H''B''E''A' 心跳 */
#define UDP_MAGIC_CMD     0x434D4458u   /* 'C''M''D''X' 命令（预留） */

/* 帧头 flags 位定义（v1 置 0，供后续扩展：TL/LaneMark/Zebra 随帧转发） */
#define UDP_FLAG_LANE_VALID  0x00000001u  /* 本帧携带有效车道结果 */
#define UDP_FLAG_TL_VALID    0x00000002u  /* 本帧携带红绿灯结果（见下方摘要布局） */
#define UDP_FLAG_LM_VALID    0x00000004u  /* 本帧携带虚实线结果 */
#define UDP_FLAG_ZEBRA_VALID 0x00000008u  /* 本帧携带斑马线结果 */

/* ---- 「三感知摘要」：复用帧头 reserved[4]（16B）承载（B 集成，2026-10-09） ----
 *
 * 背景：A 端在 M 板产出红绿灯/虚实线/斑马线三项感知（各为 64B 结构体），
 * 但帧头 76B 布局装不下；改结构体尺寸又会牵动整套重组/单测。故**复用帧头
 * 本就存在的 reserved[4]（16B，原本恒 0）** 承载「决策与显示所需的紧凑摘要」：
 * 决策只关心「状态 + 置信度 + 少量标志位」，不需要包围盒/面积等调试字段。
 *
 * 与 flags 配合：未携带的项其 *_VALID 位为 0，接收端**不得使用**对应字段。
 *
 *   reserved[0]  bit 0..7   tl_state       TrafficLightState（0未知/1红/2黄/3绿）
 *                bit 8..15  tl_confidence  0..100
 *                bit 16     tl_detected    1=检出点亮信号灯
 *   reserved[1]  bit 0..7   lm_left_type   LaneMarkType（0无/1实线/2虚线/3未知）
 *                bit 8..15  lm_right_type  LaneMarkType
 *                bit 16     lm_crossing        1=压线
 *                bit 17     lm_crossing_left
 *                bit 18     lm_crossing_right
 *                bit 19     lm_lane_change     1=正在变道（跨越车道线）
 *                bit 20..27 lm_confidence  0..100
 *   reserved[2]  bit 0      zebra_detected
 *                bit 1..8   zebra_confidence 0..100
 *   reserved[3]  int32      ego_offset_px  车辆中心相对车道中心偏移（正=偏右）
 *
 * 说明：本扩展**不改变任何结构体尺寸/版本号**，旧固件收到后 reserved 全 0、
 * flags 无对应位，行为与之前完全一致（向后兼容）。
 */
#define UDP_AUX_HAS_TL      0x1u   /* udp_hdr_get_aux 返回位：含红绿灯 */
#define UDP_AUX_HAS_LM      0x2u   /* 含虚实线 */
#define UDP_AUX_HAS_ZEBRA   0x4u   /* 含斑马线 */

/* ---- 包类型识别结果 ---- */
typedef enum {
    UDP_PKT_UNKNOWN   = 0,
    UDP_PKT_FRAME_HDR = 1,   /* 帧头 */
    UDP_PKT_DATA      = 2,   /* 图像数据块 */
    UDP_PKT_HEARTBEAT = 3,   /* 心跳 */
    UDP_PKT_CMD       = 4    /* 命令 */
} udp_pkt_type_t;

/* ---- 帧头包（76 字节 packed：19×u32） ---- */
typedef struct __attribute__((packed)) {
    uint32_t magic;            /* UDP_MAGIC_FRAME */
    uint32_t version;          /* UDP_PROTO_VERSION */
    uint32_t frame_id;         /* 单调递增帧号（与 LaneResult.frame_id 一致） */
    uint32_t width;            /* 图像宽 */
    uint32_t height;           /* 图像高 */
    uint32_t data_size;        /* 图像总字节数 = width*height*2 */
    uint32_t block_size;       /* 每数据块 payload 字节数（≤ UDP_BLOCK_SIZE） */
    uint32_t block_count;      /* 本帧数据块总数 = ceil(data_size/block_size) */
    uint32_t timestamp_s;      /* 采集时间戳秒 */
    uint32_t timestamp_us_lo;  /* 采集时间戳微秒低 32 位 */
    uint32_t direction;        /* LaneDirection：直道/左/右/未知 */
    int32_t  curve_offset;     /* 弯道像素偏移（带符号，正=右弯） */
    uint32_t confidence;       /* 车道置信度 0~100 */
    uint32_t lane_pixel_cnt;   /* 车道线像素总数 */
    uint32_t flags;            /* UDP_FLAG_* */
    uint32_t reserved[4];      /* 预留扩展（0） */
} udp_frame_hdr_t;             /* = 19×4 = 76 字节 */

/* ---- 数据块包（16 字节 packed + payload） ---- */
typedef struct __attribute__((packed)) {
    uint32_t magic;            /* UDP_MAGIC_DATA */
    uint32_t frame_id;         /* 所属帧号 */
    uint32_t block_idx;        /* 块序号 0..block_count-1 */
    uint32_t checksum;         /* payload 累加和 */
    uint8_t  payload[];        /* 变长：实际数据 block_size 字节 */
} udp_data_hdr_t;              /* = 16 字节 + payload */

/* ---- 心跳包（16 字节 packed） ---- */
typedef struct __attribute__((packed)) {
    uint32_t magic;            /* UDP_MAGIC_HB */
    uint32_t version;          /* UDP_PROTO_VERSION */
    uint32_t seq;              /* 单调递增序号 */
    uint32_t status;           /* 0=正常（预留：错误码） */
} udp_heartbeat_t;             /* = 16 字节 */

/* ---- 命令包（20 字节 packed，S→M 预留） ---- */
typedef struct __attribute__((packed)) {
    uint32_t magic;            /* UDP_MAGIC_CMD */
    uint32_t version;          /* UDP_PROTO_VERSION */
    uint32_t frame_id;         /* 决策帧号 */
    uint32_t command;          /* ControlCommand 枚举值 */
    uint8_t  enable;           /* 命令生效标志 */
    uint8_t  priority;         /* 优先级 */
    uint8_t  confidence;       /* 0~100 */
    uint8_t  reserved;         /* 对齐（0） */
} udp_cmd_t;                   /* = 20 字节 */

/* ---- 三感知摘要 接口（布局见上文 reserved[4] 注释） ---- */

/* 把三项感知打包进帧头 reserved[4] + flags。任一入参为 NULL 表示本帧不携带该项
 * （其 *_VALID 位保持 0）。必须在 udp_pack_frame_hdr() 之后调用（后者会清零）。 */
void udp_hdr_set_aux(udp_frame_hdr_t *hdr,
                     const TrafficLightResult *tl,
                     const LaneMarkResult     *lm,
                     const ZebraResult        *zebra);

/* 从帧头取出三感知摘要并还原为完整结构（帧号/时间戳与帧头一致，便于按帧对齐）。
 * 任一输出参数可为 NULL（不关心该项）。未携带的项：不做写入，且不计入返回值。
 * 返回实际携带的有效项位掩码（UDP_AUX_HAS_*）。 */
uint32_t udp_hdr_get_aux(const udp_frame_hdr_t *hdr,
                         TrafficLightResult *tl,
                         LaneMarkResult     *lm,
                         ZebraResult        *zebra);

/* ---- 接口 ---- */

/* 识别一个 UDP 数据报的类型（校验 magic 与最小长度） */
udp_pkt_type_t udp_identify(const uint8_t *buf, size_t len);

/* 从 LaneResult 填充帧头（block_size 取 0 时用 UDP_BLOCK_SIZE 默认值） */
int udp_pack_frame_hdr(udp_frame_hdr_t *hdr, const LaneResult *lane,
                       uint32_t width, uint32_t height, uint32_t block_size);

/* 从帧头恢复 LaneResult（S 端 UDP 转发写 shm_lane 用）。成功 0，失败 -1 */
int udp_hdr_to_lane(const udp_frame_hdr_t *hdr, LaneResult *lane);

/* 校验并解帧头：buf 指向完整帧头包，成功 0，失败 -1 */
int udp_unpack_frame_hdr(const uint8_t *buf, size_t len, udp_frame_hdr_t *out);

/* 计算数据块 payload 累加和（简单校验，抗偶发损坏） */
uint32_t udp_block_checksum(const uint8_t *data, size_t n);

/* 打包/解包心跳 */
int udp_pack_heartbeat(udp_heartbeat_t *hb, uint32_t seq);
int udp_unpack_heartbeat(const uint8_t *buf, size_t len, udp_heartbeat_t *out);

/* 打包/解包命令（S→M 预留） */
int udp_pack_cmd(udp_cmd_t *cmd, const ControlCommandMsg *m);
int udp_unpack_cmd(const uint8_t *buf, size_t len, udp_cmd_t *out);

/* 工具：包类型名（调试打印） */
const char *udp_type_name(udp_pkt_type_t t);

#ifdef __cplusplus
}
#endif

#endif /* UDP_PROTO_H */
