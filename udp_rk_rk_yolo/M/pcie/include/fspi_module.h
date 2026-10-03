#ifndef FSPI_MODULE_H
#define FSPI_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FSPI模块初始化
 * @return 0表示成功，-1表示失败
 */
int fspi_init(void);

/**
 * @brief FSPI写数据
 * @param addr 写入地址 (16位)
 * @param data 写入数据 (8位)
 * @return 0表示成功，-1表示失败
 */
int fspi_write_data(uint16_t addr, uint8_t data);

/**
 * @brief FSPI读数据
 * @param addr 读取地址 (16位)
 * @param data 读取数据指针 (8位)
 * @return 0表示成功，-1表示失败
 */
int fspi_read_data(uint16_t addr, uint8_t *data);

// 外部全局变量声明
extern int current_video_stream;

/**
 * @brief 切换视频流
 * @param stream_id 视频流ID (1-6)
 * @return 0表示成功，-1表示失败
 */
int switch_video_stream(int stream_id);

/**
 * @brief FSPI清理资源
 */
void fspi_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // FSPI_MODULE_H