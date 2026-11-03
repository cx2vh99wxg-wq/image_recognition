#include "postprocess.h"
#include <cassert>
#include <cstdio>
#include <random>
#include <vector>

int main(int argc,char **argv)
{
    assert(argc==2);
    FILE *out=fopen(argv[1],"wb");
    assert(out);
    FILE *file=fopen("labels_fixture","wb");
    assert(file);
    fputs("person\r\ncar\nvery long label with clipped tail\n",file);
    for(int i=3;i<93;++i) fprintf(file,"category %d\n",i);
    fclose(file);
    std::mt19937 rng(29);
    for(int i=0;i<320;++i) {
        if(i%5==0) deinitPostProcess();
        if(i%5==1) assert(initPostProcess("labels_fixture")==0);
        if(i%5==2) assert(initPostProcess("missing_file")==-1);
        int height=(i%8+1)*8,width=(i%11+1)*8;
        std::vector<int8_t> maps[3];
        for(int level=0;level<3;++level) {
            int stride=8<<level;
            int plane=(width/stride)*(height/stride);
            maps[level].resize(plane*3*PROP_BOX_SIZE);
            for(int a=0;a<3;++a) for(int cell=0;cell<plane;++cell) {
                auto write=[&](int channel,int value) {
                    maps[level][(PROP_BOX_SIZE*a+channel)*plane+cell]=(int8_t)value;
                };
                for(int channel=0;channel<5;++channel) write(channel,int(rng()%256)-128);
                for(int channel=5;channel<PROP_BOX_SIZE;++channel) write(channel,-128);
                write(5,int(rng()%256)-128);
                write(6,int(rng()%256)-128);
                write(7,i%2 ? -65 : -90);
            }
        }
        const float thresholds[]={0.0f,0.15f,0.25f,0.45f,0.95f,1.0f};
        float conf=thresholds[i%6];
        BOX_RECT pads={i%19,i%10,i%17,i%12};
        detect_result_group_t group={};
        int rc=post_process_simple(maps[0].data(),maps[1].data(),maps[2].data(),
                                   height,width,conf,0.45f,pads,(i%11+1)*0.17f,(i%7+1)*0.18f,&group);
        fwrite(&rc,sizeof rc,1,out);
        fwrite(&group,sizeof group,1,out);
    }
    assert(initPostProcess(nullptr)==-1);
    fclose(out);
}
