from pathlib import Path
import sys, subprocess, re
w=Path(__file__).resolve().parent
root=w.parents[1]/'udp_rk_rk_yolo'
tag=sys.argv[1]
src=w/'baseline' if tag=='before' else root
for board in 'MS':
    source=(src/board/'csrc/shared_memory.c').read_text(encoding='utf-8')
    body=source[source.index('static int g_curve_shm_id'):source.index('    printf("弯道检测共享内存已清理')].rsplit('\n',1)[0]
    # Addresses depend on ASLR. Compare the call trace and segment state instead.
    body=re.sub(r'    printf\("弯道检测共享内存初始化成功[^\n]+\n','',body)
    include='#include "curve_shm_attach.h"\n' if tag!='before' else ''
    main='''
    puts("cleanup complete");
}
int main(void) {
    for (int writer=0; writer<=1; ++writer)
        for (scenario=0; scenario<5; ++scenario) {
            memset(&segment,0x55,sizeof segment);
            call=0;
            int rc=curve_detection_shm_init(writer);
            printf("result %d %d %d %u %u %u\\n",writer,scenario,rc,
                   segment.writer_active,segment.curve_type,segment.reserved[0]);
            curve_detection_shm_cleanup();
        }
}
'''
    code=w/f'curve_{board}_{tag}.c'
    code.write_text((w/'curve_stub.c').read_text(encoding='utf-8-sig')+include+body+main,encoding='utf-8')
    exe=code.with_suffix('.exe')
    subprocess.run(['gcc','-std=c11','-O2','-I'+str(w/'shim'),'-I'+str(root/'common/perception/include'),
                   str(code),'-o',str(exe)],check=True,cwd=w)
    data=subprocess.run([str(exe)],check=True,cwd=w,capture_output=True)
    for key in ('stdout','stderr'):
        f=w/f'curve_{board}_{tag}.{key}'
        f.write_bytes(getattr(data,key))
        if tag!='before':
            assert f.read_bytes()==(w/f'curve_{board}_before.{key}').read_bytes(),board
    print(board,'10 SysV cases',tag)
