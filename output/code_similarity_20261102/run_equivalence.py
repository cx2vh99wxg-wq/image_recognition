"""Run local baseline/current equivalence and whole-TU syntax checks."""
import json
import os
from pathlib import Path
import subprocess
import sys

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
BUILD=OUT/'test_build'
BUILD.mkdir(exist_ok=True)
CXX=Path('D:/zheliVScode/mingw64/bin/g++.exe')
environment=dict(os.environ,TMP=str(BUILD),TEMP=str(BUILD),TMPDIR=str(BUILD))
results=[]

def run(label,command):
    process=subprocess.run([str(x) for x in command],cwd=ROOT,env=environment,
                           capture_output=True,text=True,encoding='utf-8',errors='replace')
    item=dict(test=label,command=[str(x) for x in command],returncode=process.returncode,
              stdout=process.stdout,stderr=process.stderr)
    results.append(item)
    print(label,process.returncode,process.stdout.strip(),flush=True)
    if process.returncode:
        (OUT/'tests.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
        print(process.stderr)
        raise SystemExit(process.returncode)
    return process.stdout.strip()

camera_test=ROOT/'udp_rk_rk_yolo/S/pcie/tests/test_camera_orientation.cpp'
for mode in ['baseline','current']:
    executable=BUILD/('camera_'+mode+'.exe')
    command=[CXX,'-std=c++11','-O2','-Wall','-Wextra','-Werror']
    if mode=='baseline':
        header=OUT/'baseline/udp_rk_rk_yolo/S/pcie/include/camera_orientation.h'
        command.append('-DCAMERA_ORIENTATION_SOURCE="'+header.as_posix()+'"')
    command += [camera_test,'-o',executable]
    run('compile_camera_'+mode,command)
    run('test_camera_'+mode,[executable])

font_test=ROOT/'udp_rk_rk_yolo/M/pcie/tests/test_compatibility_font.py'
baseline=OUT/'baseline/udp_rk_rk_yolo/M/pcie/csrc/yolop_opencv_integration.cpp'
run('test_font_baseline',[sys.executable,font_test,'--compiler',CXX,'--source',baseline,'--work-dir',BUILD])
run('test_font_current',[sys.executable,font_test,'--compiler',CXX,'--work-dir',BUILD])
current=ROOT/'udp_rk_rk_yolo/M/pcie/csrc/yolop_opencv_integration.cpp'
for mode,path in [('baseline',baseline),('current',current)]:
    run('whole_M_translation_unit_syntax_'+mode,[CXX,'-std=c++11','-fsyntax-only',
        '-I'+str(ROOT/'udp_rk_rk_yolo/M/pcie/include'),
        '-I'+str(ROOT/'udp_rk_rk_yolo/M/include'),path])
(OUT/'tests.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
print('ALL_OFFLINE_EQUIVALENCE_CHECKS_PASSED',flush=True)
