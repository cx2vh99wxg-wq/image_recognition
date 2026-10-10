/*
 * lane_detect.c — YOLOPv2 车道线/可行驶区域分割（【人员 A · 感知】）
 *
 * 与旧实现的节点语义保持一致：模型输出节点 0 = 可行驶区域 (1,2,H,W)，
 * 节点 1 = 车道线 (1,1,H,W)；节点 2~4 为检测头，经解码/NMS写入 detections。
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

/* Validated five-output YOLOPv2 adapter, including detection heads. */
int lane_model_init(lane_model_t *m,const char *path)
{
    if (!m) return -1;
    memset(m,0,sizeof(*m));
    if (rknn_vision_init(&m->engine,path,1)<0) return -1;
    m->m_w=m->engine.width; m->m_h=m->engine.height; m->ready=1;
    return 0;
}
int lane_model_run(lane_model_t *m,const uint8_t *rgb,int w,int h,lane_seg_t *out)
{
    if (!m || !m->ready || !out || w<=0 || h<=0) return -1;
    if (out->w!=w || out->h!=h) {
        lane_seg_free(out);
        if (lane_seg_alloc(out,w,h)<0) return -1;
    }
    return rknn_vision_run(&m->engine,rgb,w,h,-1,&m->detections,out->lane_prob,out->drivable);
}
void lane_model_release(lane_model_t *m)
{
    if (!m) return;
    rknn_vision_close(&m->engine); m->ready=0;
}
