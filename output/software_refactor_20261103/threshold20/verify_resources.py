from pathlib import Path
import re, subprocess
w=Path(__file__).resolve().parent
root=w.parents[2]/'udp_rk_rk_yolo'
common=root/'common/perception/include'

prefix=r'''
#include "yolo_integration.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int allocation, fail_at, query_calls, fail_query, fail_init, fail_read, fail_label;
static int live;
static void *test_malloc(size_t count) {
    printf("allocate %zu\n",count);
    if (++allocation==fail_at) return NULL;
    void *p=malloc(count);
    assert(p);
    ++live;
    return p;
}
static void test_free(void *p) {
    if(p) { --live; puts("free"); }
    free(p);
}
static size_t test_fread(void *buffer,size_t size,size_t count,FILE *fp) {
    if(fail_read) return 0;
    return fread(buffer,size,count,fp);
}
int rknn_init(rknn_context *ctx,void *model,uint32_t size,uint32_t flags,rknn_init_extend *ext) {
    printf("init %u\n",size);
    assert(size==6 && !memcmp(model,"fixture",6) && flags==0 && !ext);
    if(fail_init) return -7;
    *ctx=13;
    return 0;
}
int rknn_destroy(rknn_context ctx) {
    assert(ctx==13);
    puts("destroy");
    return 0;
}
int rknn_query(rknn_context ctx,rknn_query_cmd cmd,void *p,uint32_t size) {
    assert(ctx==13);
    printf("query %d\n",cmd);
    if(++query_calls==fail_query) return -9;
    if(cmd==RKNN_QUERY_IN_OUT_NUM) {
        assert(size==sizeof(rknn_input_output_num));
        rknn_input_output_num *n=p;
        n->n_input=2; n->n_output=3;
    } else {
        assert(size==sizeof(rknn_tensor_attr));
        rknn_tensor_attr *a=p;
        a->dims[1]=640;
        a->dims[2]=640;
        snprintf(a->name,sizeof a->name,"attr_%u",a->index);
        printf("tensor %u\n",a->index);
    }
    return 0;
}
#ifdef TEST_S
int initPostProcess(const char *path) {
    assert(path && !strcmp(path,"labels"));
    puts("labels");
    return fail_label ? -1 : 0;
}
void deinitPostProcess(void) { puts("labels cleanup"); }
#endif
#define malloc test_malloc
#define free test_free
#define fread test_fread
'''
main=r'''
int main(void) {
    FILE *f=fopen("model_fixture","wb");
    assert(f && fwrite("fixture",1,6,f)==6);
    fclose(f);
    for(int test=0;test<20;++test) {
        YOLOEngine engine={0};
        allocation=query_calls=0;
        fail_at=test>=1 && test<=8 ? test : 0;
        fail_query=test>=9 && test<=14 ? test-8 : 0;
        fail_init=test==15;
        fail_read=test==16;
        fail_label=test==17;
        const char *model=test==18 ? "missing_model" : "model_fixture";
        const char *label=test==19 ? NULL : "labels";
        int rc=yolo_engine_init(&engine,model,label);
        printf("state %d %d %d %d %d\n",test,rc,engine.is_initialized,engine.model_width,engine.model_height);
        if(rc==0) {
            assert(engine.ctx==13 && engine.inputs && engine.outputs);
            yolo_engine_cleanup(&engine);
            yolo_engine_cleanup(&engine);
        }
        assert(live==0);
    }
    assert(yolo_engine_init(NULL,"model_fixture","labels")==-1);
    yolo_engine_cleanup(NULL);
}
'''
for b in 'MS':
    results=[]
    for tag,source in [('before',w/f'{b}_engine.c'),('after',root/b/'pcie/csrc/yolo_integration.c')]:
        src=source.read_text(encoding='utf-8')
        body='\n'.join(re.search(rf'^\w+ {name}\([^;]*?\)\n\{{.*?^\}}',src,re.M|re.S)[0]
                       for name in ['yolo_engine_init','yolo_engine_cleanup'])
        data=prefix+('#include "yolo_engine_resources.h"\n' if tag=='after' else '')+body+main
        test=w/f'{b}_resources_{tag}.c'
        test.write_text(data,encoding='utf-8')
        exe=test.with_suffix('.exe')
        args=['gcc','-std=gnu11','-O3','-I'+str(root/b/'pcie/include'),'-I'+str(common)]
        if b=='S': args.append('-DTEST_S')
        subprocess.run(args+[str(test),'-o',str(exe)],check=True)
        result=subprocess.run([str(exe)],cwd=w,capture_output=True,check=True)
        results.append((result.stdout,result.stderr))
    assert results[0]==results[1],b
    print(b,'20 resource lifecycle cases: calls, states, cleanup and diagnostics identical')
