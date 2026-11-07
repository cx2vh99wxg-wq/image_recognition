#ifndef PCIE_DMA_READBACK_GUARD_H
#define PCIE_DMA_READBACK_GUARD_H
#include <stdint.h>
#include <time.h>

namespace pcie_capture
{
// Tested with both boards' shipped DMA driver under live inference load.
// The following readback ioctl must not race the receive-buffer update.
inline void wait_dma_readback(uint64_t guard_ns = 10000ULL)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    const uint64_t until = (uint64_t)t.tv_sec * 1000000000ULL + t.tv_nsec + guard_ns;
    do
    {
        clock_gettime(CLOCK_MONOTONIC, &t);
    } while ((uint64_t)t.tv_sec * 1000000000ULL + t.tv_nsec < until);
}
}
#endif
