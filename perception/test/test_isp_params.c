/*
 * test_isp_params.c — ISP 参数字典单元测试（【人员 A · 感知】）
 *
 * 覆盖：isp_param_lookup 按 id 返回“正确”字典项（不整体错位一格）、
 * isp_param_clamp 合法值保持、越界值裁剪到边界、非法 id 返回 NULL。
 * 这一项曾因 DESC_TABLE 1 基索引与引用减 1 的 off-by-one 而整体错位，
 * 导致所有参数的 min/max/def 串位、clamp 把合法值裁坏——本测试专门防回归。
 */
#include "isp_params.h"

#include <stdio.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("  [FAIL] %s\n", msg); g_fail++; } \
    else { printf("  [ok]   %s\n", msg); } \
} while (0)

/* 期望表：id -> (期望 name, min, max, def) */
static struct {
    isp_param_id_t id;
    const char *name;
    int min_val, max_val, def_val;
} EXPECT[] = {
    { ISP_PARAM_MODE,        "mode",          0,   3,   1   },
    { ISP_PARAM_BLC,         "blc",           0,   255, 16  },
    { ISP_PARAM_AWB,         "awb",           0,   255, 128 },
    { ISP_PARAM_CNN_LEVEL,   "cnn_level",     0,   4,   2   },
    { ISP_PARAM_SATURATION,  "saturation",    0,   255, 128 },
    { ISP_PARAM_BRIGHTNESS,  "brightness",    0,   255, 128 },
    { ISP_PARAM_AWB_EN,      "awb_en",        0,   1,   1   },
    { ISP_PARAM_BINARIZATION,"binarization",  0,   255, 128 },
    { ISP_PARAM_SOBEL,       "sobel",         0,   255, 64  },
    { ISP_PARAM_ISP_JUDGE,   "isp_judge",     0,   1,   0   },
    { ISP_PARAM_CB_MIN,      "cb_min",        0,   255, 16  },
    { ISP_PARAM_CB_MAX,      "cb_max",        0,   255, 240 },
    { ISP_PARAM_CR_MIN,      "cr_min",        0,   255, 16  },
    { ISP_PARAM_CR_MAX,      "cr_max",        0,   255, 240 },
};

int main(void)
{
    printf("== test_isp_params ==\n");

    /* 1) 每个 id 的 lookup 必须返回“完全匹配”的字典项 */
    for (size_t i = 0; i < sizeof(EXPECT) / sizeof(EXPECT[0]); i++) {
        const isp_param_desc_t *d = isp_param_lookup(EXPECT[i].id);
        char buf[128];
        snprintf(buf, sizeof(buf), "lookup %s 字段一致", EXPECT[i].name);
        CHECK(d != NULL, buf);
        if (d) {
            CHECK(d->id == EXPECT[i].id,                 "  id 匹配");
            CHECK(strcmp(d->name, EXPECT[i].name) == 0,  "  name 匹配");
            CHECK(d->min_val == EXPECT[i].min_val,       "  min 匹配");
            CHECK(d->max_val == EXPECT[i].max_val,       "  max 匹配");
            CHECK(d->def_val == EXPECT[i].def_val,       "  def 匹配");
        }
    }

    /* 2) 非法 id 必须返回 NULL */
    CHECK(isp_param_lookup((isp_param_id_t)0) == NULL,     "id=0x00 返回 NULL");
    CHECK(isp_param_lookup((isp_param_id_t)ISP_PARAM_COUNT) == NULL, "id=0x0F 返回 NULL");

    /* 3) clamp：合法值保持，越界值裁剪到边界，非法 id 返回 0 */
    int v;
    v = 200; CHECK(isp_param_clamp(ISP_PARAM_BLC, &v) == 1 && v == 200, "blc(200) 保持 200");
    v = -5;  CHECK(isp_param_clamp(ISP_PARAM_BLC, &v) == 0 && v == 0,   "blc(-5) 裁到 0");
    v = 999; CHECK(isp_param_clamp(ISP_PARAM_BLC, &v) == 0 && v == 255, "blc(999) 裁到 255");
    v = 1;   CHECK(isp_param_clamp(ISP_PARAM_AWB_EN, &v) == 1 && v == 1, "awb_en(1) 保持 1");
    v = 2;   CHECK(isp_param_clamp(ISP_PARAM_AWB_EN, &v) == 0 && v == 1, "awb_en(2) 裁到 1");
    v = 50;  CHECK(isp_param_clamp((isp_param_id_t)0, &v) == 0,          "非法 id clamp 返回 0");

    printf(g_fail == 0 ? "test_isp_params: PASS\n" : "test_isp_params: FAIL (%d)\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
