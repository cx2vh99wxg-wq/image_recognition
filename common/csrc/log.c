/*
 * log.c — 统一日志宏的实现（A 起草，B 于 2026-10-06 修复级别过滤生效）
 */
#include "log.h"

static LogLevel g_level = LOG_LEVEL_INFO;

const char* log_level_name(LogLevel l) {
    switch (l) {
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_WARN:  return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        default:              return "?";
    }
}

void log_set_level(LogLevel l) { g_level = l; }

LogLevel log_get_level(void) { return g_level; }

int log_enabled(LogLevel l) { return (int)l >= (int)g_level; }
