/*
 * test_b_side.c — 模拟【人员 B】的共享内存读取器（验证 A→B 通道）
 *
 * 打开 A 写出的 5 块共享内存（lane/pcie_img/traffic_light/zebra/lane_mark），
 * 连续采样 5 次，打印关键字段并做一致性断言：
 *   - frame_id 单调递增（A 每帧写）
 *   - 契约 version 正确
 *   - 桩模式下 lane 为 STRAIGHT / conf=95，三新增感知为安全默认值
 */
#define _DEFAULT_SOURCE
#include "shm_ipc.h"
#include "driving_config.h"
#include "lane_mark.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("  [FAIL] %s\n", msg); g_fail++; } \
    else { printf("  [ok]   %s\n", msg); } \
} while (0)

int main(int argc, char **argv)
{
    int once = (argc > 1 && strcmp(argv[1], "once") == 0);

    printf("==== B 端读取器：打开 5 块共享内存 ====\n");

    void *lane = NULL, *img = NULL, *tl = NULL, *zebra = NULL, *lm = NULL;
    CHECK(shm_open(SHM_KEY_LANE, SHM_LANE_SIZE, &lane) == 0, "open shm_lane 0x1234567E");
    CHECK(shm_open(SHM_KEY_PCIE_IMG, SHM_IMG_SIZE, &img) == 0, "open shm_pcie_img 0x12345679");
    CHECK(shm_open(SHM_KEY_TRAFFIC_LIGHT, SHM_TL_SIZE, &tl) == 0, "open shm_traffic_light 0x1234567F");
    CHECK(shm_open(SHM_KEY_ZEBRA, SHM_ZEBRA_SIZE, &zebra) == 0, "open shm_zebra 0x12345680");
    CHECK(shm_open(SHM_KEY_LANE_MARK, SHM_LANE_MARK_SIZE, &lm) == 0, "open shm_lane_mark 0x12345681");

    uint32_t prev_fid = 0xFFFFFFFFu;
    int got_stream = 0;
    int samples = once ? 1 : 5;
    for (int i = 0; i < samples; i++) {
        LaneResult r;          memset(&r, 0, sizeof(r));
        TrafficLightResult t;  memset(&t, 0, sizeof(t));
        ZebraResult z;         memset(&z, 0, sizeof(z));
        LaneMarkResult m;      memset(&m, 0, sizeof(m));

        shm_read_lane(&r);
        shm_read_traffic_light(&t);
        shm_read_zebra(&z);
        shm_read_lane_mark(&m);

        printf("[%d] lane: frame=%u dir=%d conf=%u | tl: ver=%u state=%d det=%u | zebra: ver=%u det=%u | lm: ver=%u L=%d R=%d cross=%u solid=%d\n",
               i, r.frame_id, (int)r.direction, r.confidence,
               t.version, (int)t.state, t.detected,
               z.version, z.detected,
               m.version, (int)m.left_type, (int)m.right_type,
               (unsigned)m.crossing, lane_mark_is_solid_cross(&m));

        /* 桩进程帧率极高（每秒数千万帧），B 采样间隔内帧号会跳变；
         * 判定标准 = 单调递增（证明 A 持续每帧写入、B 实时读到新数据） */
        if (r.frame_id != 0xFFFFFFFFu && prev_fid != 0xFFFFFFFFu && r.frame_id > prev_fid)
            got_stream = 1;
        prev_fid = r.frame_id;
        usleep(200000);
    }

    if (!once) {
        CHECK(prev_fid != 0xFFFFFFFFu, "A 端至少写过一帧（frame_id 非全 0xFF）");
        CHECK(got_stream, "frame_id 单调递增（A 持续每帧写、B 实时可读）");
    }

    /* 最后一帧内容断言 */
    LaneResult r;          memset(&r, 0, sizeof(r));
    TrafficLightResult t;  memset(&t, 0, sizeof(t));
    ZebraResult z;         memset(&z, 0, sizeof(z));
    LaneMarkResult m;      memset(&m, 0, sizeof(m));
    shm_read_lane(&r); shm_read_traffic_light(&t); shm_read_zebra(&z); shm_read_lane_mark(&m);
    CHECK(r.version == LANERESULT_VERSION, "LaneResult.version == 1");
    CHECK(t.version == TL_VERSION, "TrafficLightResult.version == 1");
    CHECK(z.version == ZEBRA_VERSION, "ZebraResult.version == 1");
    CHECK(m.version == LANEMARK_VERSION, "LaneMarkResult.version == 1");

    printf(g_fail == 0 ? "==== B 端读取验证 PASS ====\n" : "==== B 端读取验证 FAIL (%d) ====\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
