#ifndef FSPI_MODULE_H
#define FSPI_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 打开固定 SPI 设备并写入总线模式、字宽和时钟配置；成功返回 0。 */
int fspi_init(void);

/* 向 16 位地址写入一字节，失败时最多尝试三次；成功为 0，失败为 -1。 */
int fspi_write_data(uint16_t addr, uint8_t data);

/* 先发送地址，再连续采样五次；从成功读值中选出现次数最多的字节，
 * 次数相同时选数值较小者。成功为 0，全部失败为 -1。 */
int fspi_read_data(uint16_t addr, uint8_t *data);

/* 保存已选择的视频流编号，由 switch_video_stream 在写入成功后更新。 */
extern int current_video_stream;

/* 仅接受视频流编号 1..6，写往地址 0 后尝试读回。
 * 读回失败或读值不同只记录日志；写入成功时仍更新 current_video_stream 并返回 0。 */
int switch_video_stream(int stream_id);

/* 关闭已打开的设备句柄并重置内部状态；重复调用不会再次关闭。 */
void fspi_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* FSPI_MODULE_H */
