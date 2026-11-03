#ifndef TEST_SPI_H
#define TEST_SPI_H
#include <stdint.h>
#define SPI_CPOL 2
#define SPI_CPHA 1
#define SPI_TX_QUAD 0x200
#define SPI_RX_QUAD 0x800
#define SPI_IOC_WR_MODE32 1
#define SPI_IOC_WR_BITS_PER_WORD 2
#define SPI_IOC_WR_MAX_SPEED_HZ 3
#define SPI_IOC_MESSAGE(n) (4+(n))
struct spi_ioc_transfer {
    uint64_t tx_buf, rx_buf;
    uint32_t len, speed_hz;
    uint16_t delay_usecs;
    uint8_t bits_per_word, cs_change, tx_nbits, rx_nbits;
};
#endif
