from pathlib import Path
import hashlib, subprocess, sys
work=Path(__file__).resolve().parent
root=work.parents[1]/'udp_rk_rk_yolo'
tag=sys.argv[1]
src=work/'baseline' if tag=='before' else root
for board in ('M','S'):
    cpp=board=='M'
    inc=src/board/'pcie/include'
    name='yolop_opencv_integration.cpp' if cpp else 'yolo_integration.c'
    exe=work/(board+'_'+tag+'.exe')
    source=src/board/'pcie/csrc'/name
    if not cpp:
        full=source.read_text(encoding='utf-8')
        source=work/('s_overlay_'+tag+'.c')
        source.write_text('#include "yolo_integration.h"\n#include "person_alert_effects.h"\n#include <stdio.h>\n#include <string.h>\n#include <stdbool.h>\n'+
                          full[full.index('static void draw_char('):full.index('void rgb565_to_rgb888_yolo(')],encoding='utf-8')
    args=['g++' if cpp else 'gcc','-std=c++11' if cpp else '-std=gnu11','-O2',
          '-ffunction-sections','-fdata-sections','-I'+str(inc),'-I'+str(root/board/'pcie/include'),
          str(source),str(work/('m_test.cpp' if cpp else 's_test.c')),
          '-Wl,--gc-sections','-o',str(exe)]
    subprocess.run(args,check=True,cwd=work)
    out=work/(board+'_'+tag+'.bin')
    with (work/(board+'_'+tag+'.log')).open('wb') as f:
        subprocess.run([str(exe),str(out)],cwd=work,stdout=f,stderr=subprocess.STDOUT,check=True)
    data=out.read_bytes()
    print(board,len(data),hashlib.sha256(data).hexdigest())
    if tag!='before':
        assert data==(work/(board+'_before.bin')).read_bytes(), board
        assert (work/(board+'_'+tag+'.log')).read_bytes()==(work/(board+'_before.log')).read_bytes(), board
