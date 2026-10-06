/*
 * driving_config.h — 全局配置（【人员 B · 决策与系统工程师】唯一维护，A/C 只读）
 *
 * 本文件是整个项目的"唯一魔法数字真源"：尺寸、端口、IP、key、阈值一律在此，
 * 代码内禁止出现裸数字。A 与 C 对本文件只读不改；想改 → 群里说 → B 改 →
 * 三人 git pull → 重新编译。
 *
 * 【B 重构记录 2026-10-06】
 *  1. 修正共享内存 key 语义：0x1234567B = shm_cmd（控制命令 B→C），
 *     0x1234567C = shm_display（显示控制 B→LCD）。与分工方案 7.1 一致，
 *     纠正 A 初版写反的问题。
 *  2. 冻结 10-05 新增三感知 key（0x1234567F/80/81），供 S 端经 UDP 转发后消费。
 *  3. 新增 UDP 网络配置段（IP/端口/心跳/看门狗），数值沿用旧版实测值。
 *  4. 新增 DisplayMode / PersonState 两个 B 侧契约定义（shm_display / shm_person）。
 *     注：按分工约定 driving_types.h 由 A 起草冻结，这两个 B 侧结构暂存本文件，
 *     待三人 review 后并入 driving_types.h。
 */
#ifndef DRIVING_CONFIG_H
#define DRIVING_CONFIG_H

/* ======================================================================
 * 一、图像规格
 * ==================================================================== */
#define IMG_WIDTH   640
#define IMG_HEIGHT  480
#define IMG_CH      3
#define IMG_BPP_565 2                 /* RGB565 每像素字节数 */

/* ======================================================================
 * 二、PCIe 采集（A 使用，配置集中于此）
 * ==================================================================== */
#define PCIE_DEVICE        "/dev/pango_pci_driver"
#define PCIE_LEAD_PIXELS   120        /* 每行前导像素，必须剥离 */
#define PCIE_LINE_PIXELS   (IMG_WIDTH + PCIE_LEAD_PIXELS)   /* 原始行宽（含前导） */
#define PCIE_FRAME_BYTES   ((size_t)PCIE_LINE_PIXELS * IMG_HEIGHT * IMG_BPP_565)
#define IMG_FRAME_BYTES    ((size_t)IMG_WIDTH * IMG_HEIGHT * IMG_BPP_565)

/* ======================================================================
 * 三、共享内存 key（沿原编号 base 0x12345678+偏移，语义重定义）
 *
 * key 语义务必以分工方案 7.1 为准，尤其注意：
 *   0x1234567B = shm_cmd    （控制命令，B 写 / C 读）
 *   0x1234567C = shm_display（显示控制，B 写 / LCD 读）
 * 与早期误标相反，A 初版曾写反，B 于 2026-10-06 修正冻结。
 * ==================================================================== */
#define SHM_KEY_PCIE_IMG  0x12345679  /* A 写(M板)：640x480 RGB565 原始图 */
#define SHM_KEY_UDP_IMG   0x1234567A  /* B 写(S板)：640x480 RGB565 UDP 收图 */
#define SHM_KEY_CMD       0x1234567B  /* B 写 / C 读：ControlCommandMsg 控制命令 */
#define SHM_KEY_DISPLAY   0x1234567C  /* B 写 / LCD 读：DisplayMode 显示模式 */
#define SHM_KEY_PERSON    0x1234567D  /* B 写 / B、C 读：PersonState 行人状态 */
#define SHM_KEY_LANE      0x1234567E  /* A 写(M板) / B 转发写(S板) / B、C 读：LaneResult */

/* 10-05 新增感知结果（A 写 M 板共享内存；S 端消费需经 UDP 转发，B 定案采纳） */
#define SHM_KEY_TRAFFIC_LIGHT 0x1234567F  /* A 写 / B 读：TrafficLightResult（红绿灯灯色） */
#define SHM_KEY_ZEBRA         0x12345680  /* A 写 / B 读：ZebraResult（斑马线） */
#define SHM_KEY_LANE_MARK     0x12345681  /* A 写 / B 读：LaneMarkResult（虚实线/压线/变道） */

/* ======================================================================
 * 四、共享内存尺寸（取自结构体，保证契约一致，禁止手填数字）
 * ==================================================================== */
#include "driving_types.h"
#define SHM_LANE_SIZE      sizeof(LaneResult)
#define SHM_CMD_SIZE       sizeof(ControlCommandMsg)
#define SHM_IMG_SIZE       IMG_FRAME_BYTES
#define SHM_TL_SIZE        sizeof(TrafficLightResult)
#define SHM_ZEBRA_SIZE     sizeof(ZebraResult)
#define SHM_LANE_MARK_SIZE sizeof(LaneMarkResult)
#define SHM_PERSON_SIZE    sizeof(PersonState)
#define SHM_DISPLAY_SIZE   sizeof(DisplayMode)

/* ======================================================================
 * 五、车道线判定阈值（A 算法用，B 集中管理）
 * ==================================================================== */
#define LANE_ROI_TOP_FRAC  0.45f   /* ROI 自顶向下的起始比例 */
#define LANE_STRAIGHT_TH   24      /* 偏移绝对值小于该值判定为直道（像素） */
#define LANE_OFFSET_MAX    200     /* curve_offset 饱和上限（像素） */
#define LANE_PIXEL_REF     4000    /* 置信度覆盖率的参考像素数 */
#define LANE_STD_MAX       90      /* 车道线横向标准差上限（像素） */

/* ======================================================================
 * 六、UDP 网络配置（B 使用；数值沿用旧版实测：M=100.10 → S=100.20 :8888）
 *
 * 网线直连拓扑（无路由器）：两端板卡手动配置同子网静态 IP。
 *   M 端：192.168.100.10 / 255.255.255.0
 *   S 端：192.168.100.20 / 255.255.255.0
 * 注意：IP 必须与 setnetwork.sh 里板卡实际配置一致；改 IP 属于配置变更，
 * 只需改本文件 + 板卡网络配置，代码无需动。
 * ==================================================================== */
#define UDP_PORT            8888
#define UDP_IP_M            "192.168.100.10"   /* M 端板卡 IP（发送方源地址） */
#define UDP_IP_S            "192.168.100.20"   /* S 端板卡 IP（接收方目标地址） */
#define UDP_BLOCK_SIZE      1400               /* 每数据块 payload 上限（≤MTU 安全） */
#define UDP_SEND_BUF_BYTES  (2 * 1024 * 1024)  /* socket 发送缓冲（2MB） */
#define UDP_RECV_BUF_BYTES  (4 * 1024 * 1024)  /* socket 接收缓冲（4MB） */
#define UDP_HEARTBEAT_MS    200                /* 心跳周期：>200ms 无心跳看门狗自停 */
#define UDP_WATCHDOG_TIMEOUT_MS 1000           /* 看门狗超时（C 端落地 FPGA 兜底） */
#define UDP_FRAME_TIMEOUT_MS   200             /* 接收端整帧重组超时（丢弃残帧） */

/* ======================================================================
 * 七、B 侧契约结构（暂存本文件，待三人 review 后并入 driving_types.h）
 * ==================================================================== */

/* ---- 显示模式（B 写 shm_display，render_lcd 读） ---- */
typedef enum {
    DISPLAY_MODE_SPLIT       = 0,  /* 双路拼接 1280x480：左本地(含行人框) 右远端(车道图) */
    DISPLAY_MODE_LOCAL_ONLY  = 1,  /* 仅本地 PCIe 图 */
    DISPLAY_MODE_REMOTE_ONLY = 2,  /* 仅远端 UDP 图 */
    DISPLAY_MODE_BLANK       = 3   /* 黑屏 */
} DisplayMode;

/* ---- 行人状态（B 写 shm_person，C 可读用于安全兜底） ---- */
typedef enum {
    PERSON_NONE    = 0,   /* 无行人 */
    PERSON_FAR     = 1,   /* 远处行人（小框，低优先） */
    PERSON_NEAR    = 2,   /* 近处行人（大框，高优先） */
    PERSON_UNKNOWN = 3
} PersonPresence;

typedef struct {
    uint32_t    version;        /* PERSON_VERSION */
    uint32_t    frame_id;       /* 与当帧对齐 */
    uint64_t    timestamp_us;
    uint32_t    detected;       /* 1=检出行人 */
    int32_t     box_x;          /* 最近行人框（全图坐标，-1=无） */
    int32_t     box_y;
    int32_t     box_w;
    int32_t     box_h;
    uint32_t    person_count;   /* 画面中行人总数 */
    uint32_t    confidence;     /* 0~100 */
    uint32_t    reserved[6];    /* 预留扩展（清零） */
} PersonState;                  /* = 4+4+8+4+4+4+4+4+4+4+24 = 64 字节 */
#define PERSON_VERSION 1u

/* ======================================================================
 * 八、决策（planning）阈值（B 使用）
 * ==================================================================== */
#define DEC_GO_CONF_MIN        50u    /* CMD_GO 所需最小车道置信度（0~100） */
#define DEC_BRAKE_CONF_MIN     60u    /* 行人/红灯触发停车的最小置信度 */
#define DEC_LANE_MAX_AGE_MS    500u   /* 车道结果最大年龄，超龄降级为 CMD_NONE */
#define DEC_TL_MAX_AGE_MS      500u   /* 红绿灯结果最大年龄 */
#define DEC_PERSON_MAX_AGE_MS  500u   /* 行人结果最大年龄 */
#define DEC_LOOP_MS            100u   /* 决策主循环周期（与 C 控制循环解耦） */

/* ControlCommandMsg.priority 取值约定（B→C 契约语义值） */
#define CMD_PRI_NORMAL    0u   /* 常规动作 */
#define CMD_PRI_STOP      1u   /* 红灯停车 */
#define CMD_PRI_EMERGENCY 2u   /* 行人/急停，C 端最高优先 */

/* ======================================================================
 * 九、共享内存读写保护（B 使用）
 * ==================================================================== */
#define SHM_READ_MAX_TRIES  4u   /* 双读校验最大重试次数（防 64B 撕裂） */

/* ======================================================================
 * 十、编译规范
 * ==================================================================== */
#ifndef CROSS_PREFIX
#define CROSS_PREFIX "aarch64-linux-gnu-"
#endif

#endif /* DRIVING_CONFIG_H */
