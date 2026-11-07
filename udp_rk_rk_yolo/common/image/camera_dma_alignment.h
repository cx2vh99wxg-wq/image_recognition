#ifndef CAMERA_DMA_ALIGNMENT_H
#define CAMERA_DMA_ALIGNMENT_H
#include <stdint.h>
#include <stddef.h>
#include <cstdlib>

// The DMA offset addresses host memory, not a row in the FPGA video FIFO.
// Recover the stream's row phase once after open (including an interrupted
// previous reader). BBWW transport tails identify a tile's penultimate row;
// the duplicated camera diagonal distinguishes the two 240-row halves.
static inline bool camera_dma_alignment(const uint16_t *raw, int width, int height,
                                        int *discard_rows, unsigned *normal_score=nullptr,
                                        unsigned *swapped_score=nullptr)
{
    if (!raw || !discard_rows || width < 64 || width % 4 || height < 8 || height % 2)
        return false;
    const int w=width/2, h=height/2, count=w/2;
    int marker=-1;
    for(int y=0;y<h;++y)
    {
        bool match=true;
        for(int half=0;half<2 && match;++half)
            for(int tile=0;tile<2 && match;++tile)
            {
                const uint16_t *p=raw+(size_t)(y+half*h)*width+tile*w+w/2;
                int black=0,white=0,edges=0;
                for(int x=0;x<count;++x)
                {
                    black+=p[x]==0;white+=p[x]==65535;
                    if(x && (p[x]==0 || p[x]==65535) && (p[x-1]==0 || p[x-1]==65535))
                        edges+=p[x]!=p[x-1];
                }
                match=black+white>=count*9/10 && black>=count/4 && white>=count/4 && edges>=count/3;
            }
        if(match)
        {
            if(marker>=0)return false; // Ambiguous, never guess a phase.
            marker=y;
        }
    }
    if(marker<0)return false;
    int phase=(marker+2)%h;
    unsigned normal=0,swapped=0,samples=0;
    for(int y=h/8;y<h*7/8;y+=12)
        for(int x=w/8;x<w*7/8;x+=12)
        {
            const uint16_t *a=raw+(size_t)((y+phase)%height)*width+x;
            const uint16_t *b=raw+(size_t)((y+h+phase)%height)*width+x;
            const auto difference=[](uint16_t p,uint16_t q) {
                return std::abs((p&31)-(q&31))+std::abs(((p>>5)&63)-((q>>5)&63))+
                       std::abs((p>>11)-(q>>11));
            };
            normal+=difference(a[0],b[w]);swapped+=difference(a[w],b[0]);++samples;
        }
    const unsigned best=normal<swapped?normal:swapped, other=normal<swapped?swapped:normal;
    if(normal_score)*normal_score=normal;
    if(swapped_score)*swapped_score=swapped;
    if(!samples || best>7*samples || other<best+5*samples || other<2*best)return false;
    if(swapped<normal)phase+=h;
    *discard_rows=phase;
    return true;
}
#endif
