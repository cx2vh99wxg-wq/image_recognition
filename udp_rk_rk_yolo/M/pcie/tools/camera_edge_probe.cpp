// Read-only published-video regression. Never opens PCIe, FSPI or vehicle control.
#include "shared_memory.h"
#include "../../../common/image/frame_pixels.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <unistd.h>

int main(int argc,char **argv)
{
    const int seconds=argc>1?atoi(argv[1]):10;
    if(seconds<1 || seconds>120)return 2;
    SharedMemoryManager manager={};
    if(shared_memory_init(&manager,SHM_TYPE_PCIE,0))return 3;
    std::vector<uint16_t> image(640*480);
    uint32_t width=0,height=0,id=0,first=0,last=0;
    unsigned frames=0,errors=0,tail_bad_frames=0,checker_frames=0;
    using Clock=std::chrono::steady_clock;
    const auto start=Clock::now();
    while(std::chrono::duration<double>(Clock::now()-start).count()<seconds)
    {
        const int result=pcie_shared_memory_read_frame(&manager,(uint8_t*)image.data(),&width,&height,&id);
        if(result){errors+=result<0;usleep(1000);continue;}
        if(width!=640 || height!=480){++errors;continue;}
        if(!frames)first=id;
        last=id;++frames;
        bool tail_bad=false,checker=false;
        // Local M top-left and network lower-right are corrected previews;
        // top-right and lower-left remain raw views with exact row extension.
        for(int tile=0;tile<4;++tile)
        {
            const int x0=(tile%2)*320,y0=(tile/2)*240;
            for(int y=238;y<240;++y)
            {
                int black=0,white=0,edges=0;
                for(int x=160;x<320;++x)
                {
                    const uint16_t p=image[(y0+y)*640+x0+x];
                    black+=p==0;white+=p==65535;
                    if(x>160)
                    {
                        const uint16_t prev=image[(y0+y)*640+x0+x-1];
                        edges+=(p==0 || p==65535) && (prev==0 || prev==65535) && p!=prev;
                    }
                    if((tile==1 || tile==2) && p!=image[(y0+237)*640+x0+x])tail_bad=true;
                }
                if(black>=40 && white>=40 && edges>=40)checker=true;
            }
        }
        tail_bad_frames+=tail_bad;checker_frames+=checker;
        if(frames==1 && argc>2)
        {
            std::vector<uint8_t> bgr(image.size()*3);
            frame_unpack_rgb565(image.data(),bgr.data(),image.size());
            for(size_t i=0;i<bgr.size();i+=3){const uint8_t t=bgr[i];bgr[i]=bgr[i+2];bgr[i+2]=t;}
            FILE *f=fopen(argv[2],"wb");
            if(f){fprintf(f,"P6\n640 480\n255\n");fwrite(bgr.data(),1,bgr.size(),f);fclose(f);}
        }
        usleep(1000);
    }
    const double elapsed=std::chrono::duration<double>(Clock::now()-start).count();
    printf("EDGE_PROBE seconds=%.3f sampled=%u sample_fps=%.3f publish_fps=%.3f first=%u last=%u read_errors=%u tail_bad_frames=%u checker_frames=%u\n",
           elapsed,frames,frames/elapsed,frames>1?(last-first)/elapsed:0.,first,last,errors,tail_bad_frames,checker_frames);
    shared_memory_cleanup(&manager);
    return !frames || errors || tail_bad_frames || checker_frames?1:0;
}
