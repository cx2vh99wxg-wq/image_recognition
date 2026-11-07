#ifndef PCIE_DMA_FRAME_ALIGNMENT_H
#define PCIE_DMA_FRAME_ALIGNMENT_H
#include "pango_dma_abi.h"
#include "dma_readback_guard.h"
#include "../image/camera_dma_alignment.h"
#include <sys/ioctl.h>
#include <cstdio>

namespace pcie_capture
{
struct FrameAlignment
{
    bool checked = false;
    unsigned attempts = 0;
};

// Called on a complete, unannotated raw frame. Return 1 after draining a
// partial frame so the caller recaptures before publishing any pixels.
inline int align_dma_frame(int fd, DMA_OPERATION *operation, const uint16_t *raw,
                           int width, int height, FrameAlignment *state,
                           uint64_t guard_ns = 10000ULL)
{
    if(state->checked)return 0;
    const unsigned attempt=state->attempts++;
    if(attempt>=8 && attempt%30)return 0;
    int rows=0;
    if(!camera_dma_alignment(raw,width,height,&rows))return 0;
    state->checked=true;
    std::printf("[DMA ALIGN] recovered frame phase; discard_rows=%d\n",rows);
    for(int row=0;row<rows;++row)
    {
        operation->offset_addr=0;
        if(ioctl(fd,PCI_DMA_WRITE_CMD,operation)<0)return -1;
        wait_dma_readback(guard_ns);
        if(ioctl(fd,PCI_READ_FROM_KERNEL_CMD,operation)<0)return -1;
    }
    return rows?1:0;
}
}
#endif
