"""Compile in an isolated directory; never installs, starts, stops or tests apps."""
import argparse
import hashlib
import json
import os
from pathlib import Path,PurePosixPath
import platform
import subprocess
import sys
import tarfile
import time

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--board',choices=['M','S'],required=True)
args=parser.parse_args()
assert platform.machine()=='aarch64','Build on the intended ARM board only'
root=Path('/home/linaro/refactor_letterbox_candidate_20261102')
assert root.parent==Path('/home/linaro')
if root.exists():
    owner=json.loads((root/'candidate.owner.json').read_text())
    assert owner==dict(schema='APPLICATION_REFACTOR_CANDIDATE_V1',board=args.board)
else:
    root.mkdir()
    (root/'candidate.owner.json').write_text(json.dumps(dict(schema='APPLICATION_REFACTOR_CANDIDATE_V1',board=args.board)))
archive=Path('/tmp/letterbox_refactor_sources_20261102.tgz')
with tarfile.open(archive,'r:gz') as tar:
    for member in tar.getmembers():
        name=PurePosixPath(member.name)
        assert member.isfile() and not name.is_absolute() and '..' not in name.parts
        if name.parts[0] not in {args.board,'common'}:continue
        target=root.joinpath(*name.parts)
        assert target.resolve().is_relative_to(root.resolve())
        target.parent.mkdir(parents=True,exist_ok=True)
        with tar.extractfile(member) as source:target.write_bytes(source.read())
        target.chmod(member.mode&0o777)
board=root/args.board
for relative in ['pcie/lib','lib']:
    original=Path('/home/linaro')/args.board/relative
    link=board/relative
    if original.is_dir() and not link.exists():
        link.parent.mkdir(parents=True,exist_ok=True)
        link.symlink_to(original,target_is_directory=True)

commands=[['make','-j2','all']]
if args.board=='M':commands.append([sys.executable,'imu/tools/build.py'])
else:commands.append([sys.executable,'steering/tools/build.py','--hardware'])
results=[]
for command in commands:
    print('COMPILE_ONLY',args.board,' '.join(command),flush=True)
    process=subprocess.run(command,cwd=board)
    results.append(dict(command=command,returncode=process.returncode))
    if process.returncode:break
outputs=['pcie/build/pcie_capture','udp/build/udp_sender','imu/build/angle_monitor'] if args.board=='M' else [
    'pcie/build/app','udp/build/video_receiver','x11/csrc/build/eight_camera_renderer','fspi/build/vehicle_control',
    'steering/actuator/build/steering_step_fspi','steering/actuator/build/steering_goal_fspi']
binaries={}
for name in outputs:
    path=board/name
    if path.is_file():binaries[name]=dict(size=path.stat().st_size,sha256=hashlib.sha256(path.read_bytes()).hexdigest())
result=dict(board=args.board,root=str(root),compiled=all(x['returncode']==0 for x in results) and len(results)==len(commands),
            runtime_tests_run=False,production_files_replaced=False,processes_started_or_stopped=False,
            archive_sha256=hashlib.sha256(archive.read_bytes()).hexdigest(),commands=results,binaries=binaries)
# Read the ELF64 .text sections as data; never execute either object.
import struct
def section_digest(path, wanted):
    data=path.read_bytes()
    assert data[:6]==b'\x7fELF\x02\x01'
    section_offset=struct.unpack_from('<Q',data,40)[0]
    entry_size,entry_count,names_index=struct.unpack_from('<HHH',data,58)
    def header(index):return struct.unpack_from('<IIQQQQIIQQ',data,section_offset+index*entry_size)
    names=header(names_index);strings=data[names[4]:names[4]+names[5]]
    for index in range(entry_count):
        item=header(index);end=strings.find(b'\0',item[0])
        if strings[item[0]:end].decode()==wanted:
            assert item[5]>0, 'The compared machine-code section must not be empty'
            return hashlib.sha256(data[item[4]:item[4]+item[5]]).hexdigest()
    raise ValueError('ELF section not found: '+wanted)
if result['compiled']:
    old=Path('/home/linaro/refactor_candidate_20261102')/args.board/'pcie/build/preprocess.o'
    new=board/'pcie/build/preprocess.o'
    before_text=section_digest(old,'.text');after_text=section_digest(new,'.text')
    result['preprocess_object_text_before_sha256']=before_text
    result['preprocess_object_text_after_sha256']=after_text
    result['preprocess_object_text_identical']=before_text==after_text
    if before_text!=after_text:result['needs_investigation']=True

(root/'compile_result.json').write_text(json.dumps(result,indent=2))
print('CANDIDATE_COMPILE_RESULT '+json.dumps(result),flush=True)
raise SystemExit(0 if result['compiled'] and result.get('preprocess_object_text_identical') else 1)
