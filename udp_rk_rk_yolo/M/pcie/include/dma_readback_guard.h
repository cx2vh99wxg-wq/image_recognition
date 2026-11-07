#ifndef M_DMA_READBACK_GUARD_H
#define M_DMA_READBACK_GUARD_H
#include "../../../common/pcie/dma_readback_guard.h"

namespace m_capture
{
// With the shipped PCIe driver, the DMA submission ioctl can return before
// the first receive blocks are ready for the following copy ioctl. Board
// A/B tests still found occasional corrupt row prefixes at 5us; 10us cleared
// them under the active RKNN/display load. This is a per-row 10us guard,
// not a frame delay or an image repair/filter operation.
inline void wait_dma_readback()
{
    pcie_capture::wait_dma_readback();
}
}
#endif
