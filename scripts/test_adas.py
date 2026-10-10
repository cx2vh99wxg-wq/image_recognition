#!/usr/bin/env python3
"""Portable safety/postprocess tests. Optional real AArch64 Linux compilation.
python scripts/test_adas.py --cc gcc [--zig /path/to/zig]
"""
import argparse, os, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
os.chdir(ROOT)
p=argparse.ArgumentParser();p.add_argument('--cc',default=os.getenv('CC','gcc'));p.add_argument('--zig');a=p.parse_args()
out=ROOT/'tmp/adas_tests';out.mkdir(parents=True,exist_ok=True)
flags=['-std=gnu11','-D_DEFAULT_SOURCE','-Wall','-Wextra','-Werror','-O2']
inc=['-Icommon/include','-Iplanning/include','-Iperception/include','-Icontrol/include']
groups={
 'closed_loop':['planning/test/test_closed_loop.c','planning/csrc/drive_policy.c','control/csrc/actuator.c','common/csrc/vision_frame.c','planning/csrc/frame_compose.c','planning/csrc/stub_pattern.c','planning/csrc/udp_receiver.c','common/csrc/udp_proto.c'],
 'yolo_profile':['perception/test/test_yolo_profile.c','perception/csrc/rknn_vision.c','perception/csrc/yolo_decode.c'],
}
log=[]
def run(args):
    r=subprocess.run(list(map(str,args)),stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,encoding='utf-8',errors='replace')
    log.append(r.stdout);print(r.stdout,end='')
    (out/'results.log').write_text(''.join(log),encoding='utf-8')
    if r.returncode:raise SystemExit(r.returncode)
for name,src in groups.items():
    exe=out/(name+('.exe' if os.name=='nt' else ''))
    run([a.cc,*flags,*inc,*src,'-lm','-o',exe]);run([exe])
if a.zig:
    # Linux libc/pthread/socket/SPI declarations come from Zig's real sysroot.
    # X11 is only a declaration stub here: this does NOT verify X11 linking.
    sources=set(sum(groups.values(),[]))
    sources={s for s in sources if '/test/' not in s}
    sources.update(str(s).replace('\\','/') for folder in ('planning/csrc','perception/csrc','control/csrc','common/csrc') for s in Path(folder).glob('*.c'))
    env=os.environ.copy();env['ZIG_GLOBAL_CACHE_DIR']=str(out/'zig-cache');os.environ.update(env)
    for src in sorted(sources):
        obj=out/(src.replace('/','_')+'.o')
        run([a.zig,'cc','-target','aarch64-linux-gnu',*flags,*inc,'-Iscripts/x11stub','-c',src,'-o',obj])
    message=f'AArch64 Linux object compilation PASS: {len(sources)} production source files (X11 declarations stubbed).\n'
    log.append(message);print(message,end='')
    control=['control/csrc/main_control.c','control/csrc/actuator.c','control/csrc/fspi_driver.c','common/csrc/shm_ipc.c','common/csrc/log.c']
    run([a.zig,'cc','-target','aarch64-linux-gnu',*flags,*inc,*control,'-lrt','-o',out/'control_main.aarch64'])
    message='AArch64 Linux control executable link PASS (not run on hardware).\n'
    log.append(message);print(message,end='')
log.append('ADAS tests PASS; results: tmp/adas_tests/results.log\n')
(out/'results.log').write_text(''.join(log),encoding='utf-8')
print(log[-1],end='')
