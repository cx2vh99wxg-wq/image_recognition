#ifndef FPGA_REGMAP_H
#define FPGA_REGMAP_H
#include <stdint.h>
/* drive_6ch protocol A6: QUAD, mode 3, MSB first, 1 MHz bring-up.
 * Write: 00 addr data. Read: 80 addr 00(dummy), then RX one byte,
 * all under ONE chip select. Motor outputs are active low, stop is held.
 * Heartbeat loss >=500ms forces stop and clears all pending motion. */
#define FSPI_DEVICE "/dev/spidev4.0"
#define FSPI_SPEED_HZ 1000000u
#define FSPI_SPI_MODE 3
#define FSPI_BITS_PER_WORD 8
#define FSPI_CMD_WRITE 0x00u
#define FSPI_CMD_READ 0x80u
#define FSPI_REG_FORWARD 0u
#define FSPI_REG_BACKWARD 1u
#define FSPI_REG_LEFT 2u
#define FSPI_REG_RIGHT 3u
#define FSPI_REG_STOP 4u
#define FSPI_REG_COUNT 5u
#define FSPI_REG_HEARTBEAT 5u
#define FSPI_HEARTBEAT 0xA5u
#define FSPI_REG_VIEW 0x10u
#define FSPI_REG_CAMERA 0x11u
#define FSPI_REG_VERSION 0x7Fu
#define FSPI_VERSION 0xA6u
#define FSPI_VAL_ACTIVE 0u
#define FSPI_VAL_IDLE 1u
#define UART_LCD_PREAMBLE0 0x30u
#define UART_LCD_PREAMBLE1 0x90u
static inline uint8_t fspi_encode_enable(uint8_t e){return e?0:1;}
#endif
