/*
 * lane_detect.c — YOLOPv2 车道线/可行驶区域分割（【人员 A · 感知】）
 *
 * 与旧实现的节点语义保持一致：模型输出节点 0 = 可行驶区域 (1,2,H,W)，
 * 节点 1 = 车道线 (1,1,H,W)；节点 2~4 为检测头，本系统不使用。
 * 真值推理依赖 RKNN 运行时（板端），lane_model_extract 不依赖 RKNN，
 * 可被单元测试直接覆盖。
 */
#include "lane_detect.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- 分割容器 ---- */

int lane_seg_alloc(lane_seg_t *s, int w, int h)
{
    if (!s || w <= 0 || h <= 0) return -1;
    s->w = w;
    s->h = h;
    /* calloc 清零：分割概率图语义上"未检出=0"，malloc 残留会污染重心/计数 */
    s->lane_prob = (float *)calloc((size_t)w * h, sizeof(float));
    s->drivable  = (float *)calloc((size_t)w * h * 2u, sizeof(float));
    if (!s->lane_prob || !s->drivable) {
        lane_seg_free(s);
        return -1;
    }
    return 0;
}

void lane_seg_free(lane_seg_t *s)
{
    if (!s) return;
    free(s->lane_prob);
    free(s->drivable);
    s->lane_prob = NULL;
    s->drivable = NULL;
    s->w = s->h = 0;
}

/* ---- 节点语义映射（不依赖 RKNN，可测） ---- */

int lane_model_extract(const float *drivable_raw, const float *lane_raw,
                       int w, int h, lane_seg_t *out)
{
    if (!drivable_raw || !lane_raw || !out || w <= 0 || h <= 0) return -1;
    if (out->w != w || out->h != h) {
        lane_seg_free(out);
        if (lane_seg_alloc(out, w, h) != 0) return -1;
    }
    /* 可行驶区域：节点 0 为两分类 (1,2,H,W)，整段拷贝（使用方按 channel 取类） */
    memcpy(out->drivable, drivable_raw, (size_t)w * h * 2u * sizeof(float));
    /* 车道线：节点 1 为单通道概率 */
    memcpy(out->lane_prob, lane_raw, (size_t)w * h * sizeof(float));
    return 0;
}

/* ---- RKNN 引擎（板端） ---- */

int lane_model_init(lane_model_t *m, const char *model_path)
{
    if (!m || !model_path) return -1;
    memset(m, 0, sizeof(*m));

    FILE *fp = fopen(model_path, "rb");
    if (!fp) { fprintf(stderr, "open model failed: %s\n", model_path); return -1; }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fsize <= 0) { fclose(fp); return -1; }

    void *blob = malloc((size_t)fsize);
    if (!blob) { fclose(fp); return -1; }
    if (fread(blob, 1, (size_t)fsize, fp) != (size_t)fsize) {
        free(blob); fclose(fp); return -1;
    }
    fclose(fp);

    int ret = rknn_init(&m->ctx, blob, (uint32_t)fsize, 0, NULL);
    free(blob);
    if (ret < 0) { fprintf(stderr, "rknn_init failed %d\n", ret); return -1; }

    ret = rknn_query(m->ctx, RKNN_QUERY_IN_OUT_NUM, &m->io_num, sizeof(m->io_num));
    if (ret < 0) { rknn_destroy(m->ctx); return -1; }

    m->in_attr  = (rknn_tensor_attr *)malloc((size_t)m->io_num.n_input  * sizeof(rknn_tensor_attr));
    m->out_attr = (rknn_tensor_attr *)malloc((size_t)m->io_num.n_output * sizeof(rknn_tensor_attr));
    m->inputs   = (rknn_input *)malloc((size_t)m->io_num.n_input  * sizeof(rknn_input));
    m->outputs  = (rknn_output *)malloc((size_t)m->io_num.n_output * sizeof(rknn_output));
    if (!m->in_attr || !m->out_attr || !m->inputs || !m->outputs) {
        lane_model_release(m);
        return -1;
    }
    memset(m->in_attr, 0, (size_t)m->io_num.n_input  * sizeof(rknn_tensor_attr));
    memset(m->out_attr, 0, (size_t)m->io_num.n_output * sizeof(rknn_tensor_attr));
    memset(m->inputs, 0, (size_t)m->io_num.n_input  * sizeof(rknn_input));
    memset(m->outputs, 0, (size_t)m->io_num.n_output * sizeof(rknn_output));

    for (uint32_t i = 0; i < m->io_num.n_input; i++) {
        m->in_attr[i].index = i;
        rknn_query(m->ctx, RKNN_QUERY_INPUT_ATTR, &m->in_attr[i], sizeof(rknn_tensor_attr));
    }
    for (uint32_t i = 0; i < m->io_num.n_output; i++) {
        m->out_attr[i].index = i;
        rknn_query(m->ctx, RKNN_QUERY_OUTPUT_ATTR, &m->out_attr[i], sizeof(rknn_tensor_attr));
    }

    m->m_w = LANE_MODEL_W;
    m->m_h = LANE_MODEL_H;
    m->in_buf  = (uint8_t *)malloc((size_t)m->m_w * m->m_h * 3u);
    m->lane_buf  = (float *)malloc((size_t)m->m_w * m->m_h * sizeof(float));
    m->drive_buf = (float *)malloc((size_t)m->m_w * m->m_h * 2u * sizeof(float));
    if (!m->in_buf || !m->lane_buf || !m->drive_buf) {
        lane_model_release(m);
        return -1;
    }
    /* 清零：防止模型输出尺寸小于预期时，未写满的尾部残留随机值被拷进 lane_seg */
    memset(m->in_buf,   0, (size_t)m->m_w * m->m_h * 3u);
    memset(m->lane_buf, 0, (size_t)m->m_w * m->m_h * sizeof(float));
    memset(m->drive_buf,0, (size_t)m->m_w * m->m_h * 2u * sizeof(float));
    m->ready = 1;
    return 0;
}

/* 最近邻缩放到模型输入分辨率（RGB888 → NHWC UINT8，无归一化） */
static void lane_preprocess(const uint8_t *rgb, int sw, int sh,
                            uint8_t *dst, int dw, int dh)
{
    float sx = (float)sw / (float)dw;
    float sy = (float)sh / (float)dh;
    for (int y = 0; y < dh; y++) {
        int syi = (int)((float)y * sy);
        if (syi >= sh) syi = sh - 1;
        for (int x = 0; x < dw; x++) {
            int sxi = (int)((float)x * sx);
            if (sxi >= sw) sxi = sw - 1;
            const uint8_t *p = rgb + ((size_t)syi * sw + sxi) * 3;
            uint8_t *q = dst + ((size_t)y * dw + x) * 3;
            q[0] = p[0]; q[1] = p[1]; q[2] = p[2];
        }
    }
}

int lane_model_run(lane_model_t *m, const uint8_t *rgb888, int w, int h, lane_seg_t *out)
{
    if (!m || !m->ready || !rgb888 || !out) return -1;

    lane_preprocess(rgb888, w, h, m->in_buf, m->m_w, m->m_h);

    m->inputs[0].index = 0;
    m->inputs[0].type  = RKNN_TENSOR_UINT8;
    m->inputs[0].size  = (uint32_t)m->m_w * m->m_h * 3u;
    m->inputs[0].fmt   = RKNN_TENSOR_NHWC;
    m->inputs[0].buf   = m->in_buf;

    for (uint32_t i = 0; i < m->io_num.n_output; i++)
        m->outputs[i].want_float = 1;

    if (rknn_inputs_set(m->ctx, m->io_num.n_input, m->inputs) < 0) return -1;
    if (rknn_run(m->ctx, NULL) < 0) return -1;
    if (rknn_outputs_get(m->ctx, m->io_num.n_output, m->outputs, NULL) < 0) return -1;
    if (m->io_num.n_output < 2) { rknn_outputs_release(m->ctx, m->io_num.n_output, m->outputs); return -1; }

    /* 仅取需要的节点 0(可行驶) / 1(车道线)，按实际大小安全拷贝 */
    size_t drive_need = (size_t)m->m_w * m->m_h * 2u * sizeof(float);
    size_t drive_got  = m->outputs[0].size;
    size_t lane_need  = (size_t)m->m_w * m->m_h * sizeof(float);
    size_t lane_got   = m->outputs[1].size;

    if (drive_got > drive_need) drive_got = drive_need;
    if (lane_got  > lane_need)  lane_got  = lane_need;

    memcpy(m->drive_buf, m->outputs[0].buf, drive_got);
    memcpy(m->lane_buf,  m->outputs[1].buf, lane_got);
    rknn_outputs_release(m->ctx, m->io_num.n_output, m->outputs);

    return lane_model_extract(m->drive_buf, m->lane_buf, m->m_w, m->m_h, out);
}

void lane_model_release(lane_model_t *m)
{
    if (!m) return;
    if (m->lane_buf)  { free(m->lane_buf);  m->lane_buf = NULL; }
    if (m->drive_buf) { free(m->drive_buf); m->drive_buf = NULL; }
    if (m->in_buf)    { free(m->in_buf);    m->in_buf = NULL; }
    if (m->in_attr)   { free(m->in_attr);   m->in_attr = NULL; }
    if (m->out_attr)  { free(m->out_attr);  m->out_attr = NULL; }
    if (m->inputs)    { free(m->inputs);    m->inputs = NULL; }
    if (m->outputs)   { free(m->outputs);   m->outputs = NULL; }
    if (m->ctx > 0)   { rknn_destroy(m->ctx); m->ctx = 0; }
    m->ready = 0;
}
