/* Verify the host side of the SMDT auto-opcode transport without hardware. */
#define ioctl transport_ioctl
#define perror transport_perror
#define usleep transport_sleep
#include "csrc/fspi_module.c"
#undef ioctl
#undef perror
#undef usleep
#include <assert.h>
#include <stdarg.h>

static unsigned writes, reads;
static int short_write, short_read;
static uint8_t model_registers[5], hmi_reply;

void transport_perror(const char *message) { (void)message; }
int transport_sleep(useconds_t duration) { (void)duration; return 0; }

int transport_ioctl(int fd, unsigned long request, ...) {
    assert(fd == 42);
    va_list args;
    va_start(args, request);
    void *argument = va_arg(args, void *);
    va_end(args);
    if (request == SPI_IOC_WR_MODE32) {
        assert((*(uint32_t *)argument & 3u) == SPI_MODE_3);
        return 0;
    }
    if (request == SPI_IOC_WR_BITS_PER_WORD) {
        assert(*(uint8_t *)argument == 8); return 0;
    }
    if (request == SPI_IOC_WR_MAX_SPEED_HZ) {
        assert(*(uint32_t *)argument == 24000000); return 0;
    }
    // HMI reads must not issue a manual TX opcode or a MESSAGE(2).
    assert(request == SPI_IOC_MESSAGE(1));
    struct spi_ioc_transfer *tr = argument;
    assert(tr->speed_hz == 24000000 && tr->bits_per_word == 8);
    if (tr->tx_buf) {
        ++writes;
        assert(tr->len == 3 && tr->tx_nbits == 4 && !tr->rx_buf);
        assert((g_config.spi_mode & (SPI_TX_QUAD | SPI_RX_QUAD)) == SPI_TX_QUAD);
        const uint8_t *host = (const uint8_t *)(uintptr_t)tr->tx_buf;
        const uint8_t wire[4] = {0x00, host[0], host[1], host[2]};
        const unsigned address = ((unsigned)wire[1] << 8) | wire[2];
        assert(address < 5);
        if (short_write) return 2;
        model_registers[address] = wire[3];
        return 3;
    }
    ++reads;
    assert(tr->len == 1 && tr->rx_nbits == 4 && tr->rx_buf);
    assert((g_config.spi_mode & (SPI_TX_QUAD | SPI_RX_QUAD)) == SPI_RX_QUAD);
    if (short_read) return 0;
    *(uint8_t *)(uintptr_t)tr->rx_buf = hmi_reply;
    return 1;
}

int main(void) {
    g_spi_fd = 42;
    g_config.speed_hz = 24000000;
    g_config.bits_per_word = 8;
    g_config.spi_mode = SPI_MODE_FLAGS;
    g_config.transfer_size = SPI_TRANSFER_SIZE;
    g_config.protocol_mode = SPI_MODE_QUAD;
    assert(SPI_TRANSFER_SIZE == 3);
    memset(model_registers, 1, sizeof(model_registers));
    for (unsigned address = 0; address < 5; ++address) {
        assert(fspi_write_data(address, 0) == 0);
        for (unsigned other = 0; other < 5; ++other)
            assert(model_registers[other] == (other == address ? 0 : 1));
        assert(fspi_write_data(address, 1) == 0);
    }
    unsigned previous = writes;
    short_write = 1;
    assert(fspi_write_data(2, 0) == -1 && errno == EIO);
    assert(writes == previous + WRITE_RETRY_COUNT && model_registers[2] == 1);
    short_write = 0;
    previous = writes;
    for (unsigned code = 0; code <= 0xee; code += 0x11) {
        hmi_reply = code;
        uint8_t value = 0x5a;
        assert(fspi_read_data(0, &value) == 0 && value == hmi_reply);
    }
    assert(writes == previous); // Reading HMI can never issue a write packet.
    unsigned previous_reads = reads;
    short_read = 1;
    uint8_t value = 0x5a;
    assert(fspi_read_data(0, &value) == -1 && value == 0x5a && errno == EIO);
    assert(reads == previous_reads + READ_RETRY_COUNT);
    assert(g_config.transfer_size == 3);
    puts("PASS SMDT transport: controller-generated opcodes, isolated registers, RX-only HMI and short-transfer rejection");
    return 0;
}
