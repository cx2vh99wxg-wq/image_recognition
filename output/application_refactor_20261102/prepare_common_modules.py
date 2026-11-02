"""Snapshot production inputs and consolidate identical cross-board headers."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
APP=ROOT/'udp_rk_rk_yolo'
if (OUT/'baseline_manifest.json').exists():
    raise SystemExit('Baseline already recorded; do not overwrite it or repeat migration')
EXCLUDED={'.git','build','__pycache__','log','logbackup'}
EXTS={'.c','.cpp','.cc','.h','.hpp','.py','.sh','.bash','.ps1','.tcl','.v','.vh','.sv','.fdc','.pcf','.pds','.json','.cal','.table','.rknn','.HMI','.tft','.sbit','.sfc'}
saved=[]
for directory in ['udp_rk_rk_yolo','HMI','fpga/fpga_pcie_ov5640/source']:
    for folder,dirs,names in os.walk(ROOT/directory):
        dirs[:]=[name for name in dirs if name not in EXCLUDED]
        for name in names:
            path=Path(folder)/name
            if path.suffix not in EXTS and name!='Makefile':continue
            relative=path.relative_to(ROOT)
            raw=path.read_bytes()
            saved.append(dict(path=relative.as_posix(),sha256=hashlib.sha256(raw).hexdigest(),size=len(raw)))
            if path.suffix not in {'.rknn','.HMI','.tft'}:
                target=OUT/'baseline'/relative
                target.parent.mkdir(parents=True,exist_ok=True)
                target.write_bytes(raw)
for project in ['project/project.pds','project/project.fdc',
                'project_m_imu/project_m_imu.pds','project_m_imu/mpu9250_m.fdc',
                'project_m_imu_nack_20261101/project_m_imu.pds','project_m_imu_nack_20261101/mpu9250_m.fdc']:
    path=ROOT/'fpga/fpga_pcie_ov5640'/project
    relative=path.relative_to(ROOT)
    target=OUT/'baseline'/relative;target.parent.mkdir(parents=True,exist_ok=True)
    shutil.copy2(path,target)
    saved.append(dict(path=relative.as_posix(),sha256=hashlib.sha256(path.read_bytes()).hexdigest(),size=path.stat().st_size))
(OUT/'baseline_manifest.json').write_text(json.dumps(dict(branch=subprocess.check_output(['git','branch','--show-current'],cwd=ROOT,text=True).strip(),files=saved),ensure_ascii=False,indent=2),encoding='utf-8')

names=['line_style.h','line_style_wire.h','line_style_ipc.h',
       'traffic_light.h','traffic_light_wire.h','traffic_light_ipc.h']
migrations=[]
for name in names:
    m=APP/'M/include'/name;s=APP/'S/include'/name
    assert m.read_bytes()==s.read_bytes(),name
    destination=APP/'common/perception/include'/name
    destination.parent.mkdir(parents=True,exist_ok=True)
    shutil.copy2(m,destination)
    for board,path in [('M',m),('S',s)]:
        guard='FPGA_'+board+'_'+name.upper().replace('.','_')+'_FORWARD'
        path.write_text('''/* Compatibility include path; the shared implementation is maintained once. */
#ifndef {guard}
#define {guard}
#include "../../common/perception/include/{name}"
#endif
'''.format(guard=guard,name=name),encoding='utf-8')
    migrations.append(dict(shared=destination.relative_to(ROOT).as_posix(),original_paths=[m.relative_to(ROOT).as_posix(),s.relative_to(ROOT).as_posix()],original_sha256=hashlib.sha256(destination.read_bytes()).hexdigest()))
name='video_transport_v2.h'
m=APP/'M/udp/include'/name;s=APP/'S/udp/include'/name
assert m.read_bytes()==s.read_bytes()
destination=APP/'common/transport/include'/name
destination.parent.mkdir(parents=True,exist_ok=True)
shutil.copy2(m,destination)
for board,path in [('M',m),('S',s)]:
    guard='FPGA_'+board+'_VIDEO_TRANSPORT_FORWARD_H'
    path.write_text('''/* Compatibility include path; packet layout and assembler are shared. */
#ifndef {guard}
#define {guard}
#include "../../../common/transport/include/video_transport_v2.h"
#endif
'''.format(guard=guard),encoding='utf-8')
migrations.append(dict(shared=destination.relative_to(ROOT).as_posix(),original_paths=[m.relative_to(ROOT).as_posix(),s.relative_to(ROOT).as_posix()],original_sha256=hashlib.sha256(destination.read_bytes()).hexdigest()))
(OUT/'common_migration.json').write_text(json.dumps(migrations,ensure_ascii=False,indent=2),encoding='utf-8')
print('COMMON_MODULES_CREATED seven identical implementations; fourteen compatibility paths; original code bytes preserved')
