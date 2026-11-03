#ifndef FSPI_REGISTER_IO_H
#define FSPI_REGISTER_IO_H

#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef enum
{
    SPI_MODE_SINGLE = 1,
    SPI_MODE_DUAL,
    SPI_MODE_QUAD
} SpiTransferMode;

typedef struct
{
    char device[MAX_DEVICE_PATH];
    uint32_t speed_hz;
    uint32_t transfer_size;
    uint32_t cycle_count;
    uint32_t spi_mode;
    uint8_t bits_per_word;
    uint16_t delay_us;
    SpiTransferMode protocol_mode;
} SpiTestConfig;

static inline int configure_spi_interface(int fd, SpiTestConfig *config)
{
    const struct {
        unsigned long request;
        void *value;
        const char *error;
    } settings[] = {
        {SPI_IOC_WR_MODE32, &config->spi_mode, "Failed to set SPI mode"},
        {SPI_IOC_WR_BITS_PER_WORD, &config->bits_per_word, "Failed to set bits per word"},
        {SPI_IOC_WR_MAX_SPEED_HZ, &config->speed_hz, "Failed to set SPI speed"},
    };
    for (size_t i = 0; i < sizeof(settings) / sizeof(settings[0]); ++i)
    {
        if (ioctl(fd, settings[i].request, settings[i].value) == -1)
        {
            perror(settings[i].error);
            return -1;
        }
    }
    return 0;
}

static inline int fspi_transfer(int fd, uint8_t *tx, uint8_t *rx,
                                const SpiTestConfig *config, int idle_direction_has_width)
{
    const uint8_t lines = config->protocol_mode == SPI_MODE_QUAD ? 4 :
                          config->protocol_mode == SPI_MODE_DUAL ? 2 : 1;
    struct spi_ioc_transfer tr = {
        .tx_buf = (uintptr_t)tx,
        .rx_buf = (uintptr_t)rx,
        .len = config->transfer_size,
        .delay_usecs = config->delay_us,
        .speed_hz = config->speed_hz,
        .bits_per_word = config->bits_per_word,
        .tx_nbits = tx || idle_direction_has_width ? lines : 0,
        .rx_nbits = rx || idle_direction_has_width ? lines : 0,
    };
    return ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}

// Addressed reads are two transactions. Keep the mode of the last attempted
// transaction while restoring the caller's transfer length on every exit.
static inline int fspi_read_addressed(int fd, uint16_t address, uint8_t *data, SpiTestConfig *config,
                                      int idle_direction_has_width, int verbose)
{
    uint8_t address_bytes[] = {(uint8_t)(address >> 8), (uint8_t)address};
    if (verbose)
        printf("TX Address: [0x%02X] [0x%02X]\n", address_bytes[0], address_bytes[1]);

    config->spi_mode = (config->spi_mode | SPI_TX_QUAD) & ~SPI_RX_QUAD;
    if (configure_spi_interface(fd, config) != 0)
        return -1;

    const uint32_t previous_size = config->transfer_size;
    config->transfer_size = sizeof(address_bytes);
    if (fspi_transfer(fd, address_bytes, NULL, config, idle_direction_has_width) < 0)
    {
        perror("Send address failed");
        config->transfer_size = previous_size;
        return -1;
    }

    uint8_t response = 0;
    config->spi_mode = (config->spi_mode & ~SPI_TX_QUAD) | SPI_RX_QUAD;
    config->transfer_size = sizeof(response);
    int result = configure_spi_interface(fd, config);
    if (result == 0 && fspi_transfer(fd, NULL, &response, config, idle_direction_has_width) < 0)
    {
        perror("Read data failed");
        result = -1;
    }

    config->transfer_size = previous_size;
    if (result != 0)
        return -1;

    *data = response;
    if (verbose)
        printf("RX Data: [0x%02X]\n", *data);
    return 0;
}

typedef int (*FspiWriteAttempt)(int, uint16_t, uint8_t);
typedef int (*FspiReadAttempt)(int, uint16_t, uint8_t *);

static inline int fspi_retry_write(FspiWriteAttempt attempt, int tries, int fd,
                                  uint16_t address, uint8_t data)
{
    for (int i = 0; i < tries; ++i)
    {
        if (attempt(fd, address, data) == 0)
            return 0;
        if (i + 1 < tries)
            usleep(1000);
    }
    return -1;
}

static inline int fspi_vote_read(FspiReadAttempt attempt, int tries, int fd,
                                uint16_t address, uint8_t *data)
{
    int counts[256] = {0};
    uint8_t sample = 0;
    for (int i = 0; i < tries; ++i)
    {
        if (attempt(fd, address, &sample) == 0)
            ++counts[sample];
        usleep(500);
    }

    int highest = 0;
    int winner = 0;
    for (int value = 0; value < 256; ++value)
    {
        if (counts[value] > highest)
        {
            highest = counts[value];
            winner = value;
        }
    }
    if (highest == 0)
        return -1;
    *data = (uint8_t)winner;
    return 0;
}

#endif
