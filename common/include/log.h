/*
 * log.h — 统一日志宏（【人员 A · 感知】起草，【人员 B】于 2026-10-06 修复）
 *
 * 各模块统一打印前缀，便于联调时区分来源。
 * 用法：LOGI("frame %u\n", id);  LOGE("open fail: %s\n", strerror(errno));
 * 通过 log_set_level 可在运行时过滤日志级别（B 修复：此前 log_set_level 为
 * 死代码，宏未查级别，现由 log_enabled() 统一判级）。
 *
 * 模块前缀：默认 "APP"，各模块可在包含本头文件前自行定义，例如
 *   #define LOG_TAG "PLANNING"
 *   #include "log.h"
 * 即可让本模块日志前缀显示为 [PLANNING]。
 */
#ifndef PERCEPTION_LOG_H
#define PERCEPTION_LOG_H

#include <stdio.h>

/* 日志级别（与 common/csrc/log.c 的实现一一对应） */
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO  = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_ERROR = 3
} LogLevel;

/* 模块标签：允许各模块在 include 前覆盖 */
#ifndef LOG_TAG
#define LOG_TAG "APP"
#endif

/* 运行时日志级别控制（实现见 common/csrc/log.c） */
void log_set_level(LogLevel l);
LogLevel log_get_level(void);
const char *log_level_name(LogLevel l);
int  log_enabled(LogLevel l);   /* B 新增：级别过滤判据 */

#define LOGI(fmt, ...) \
    do { if (log_enabled(LOG_LEVEL_INFO))  fprintf(stdout, "[" LOG_TAG "][I] " fmt, ##__VA_ARGS__); } while (0)

#define LOGW(fmt, ...) \
    do { if (log_enabled(LOG_LEVEL_WARN))  fprintf(stdout, "[" LOG_TAG "][W] " fmt, ##__VA_ARGS__); } while (0)

#define LOGE(fmt, ...) \
    do { if (log_enabled(LOG_LEVEL_ERROR)) fprintf(stderr, "[" LOG_TAG "][E] " fmt, ##__VA_ARGS__); } while (0)

#ifndef PERCEPTION_NO_DEBUG
#define LOGD(fmt, ...) \
    do { if (log_enabled(LOG_LEVEL_DEBUG)) fprintf(stdout, "[" LOG_TAG "][D] " fmt, ##__VA_ARGS__); } while (0)
#else
#define LOGD(fmt, ...) ((void)0)
#endif

#endif /* PERCEPTION_LOG_H */
