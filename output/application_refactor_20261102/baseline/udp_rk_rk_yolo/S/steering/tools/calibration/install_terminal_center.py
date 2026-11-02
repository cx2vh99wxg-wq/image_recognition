"""Install the guarded, interactive-terminal center hook on the S board."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time

ROOT=Path(__file__).resolve().parents[2]
BEGIN='# BEGIN STEERING_TERMINAL_CENTER'
END='# END STEERING_TERMINAL_CENTER'
ANCHOR='# Auto-start S-End FPGA System on login'


def insert_hook(original,block):
    if original.count(BEGIN)>1 or original.count(END)>1 or original.count(BEGIN)!=original.count(END):
        raise ValueError('Ambiguous existing terminal hook')
    if BEGIN in original:
        start=original.index(BEGIN);end=original.index(END,start)+len(END)
        if end<len(original) and original[end]=='\n':end+=1
        return original[:start]+block.rstrip()+'\n'+original[end:]
    if original.count(ANCHOR)!=1:
        raise ValueError('Existing S startup block not found uniquely; do not append after foreground make run')
    at=original.index(ANCHOR)
    return original[:at]+block.rstrip()+'\n\n'+original[at:]


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--home',default='/home/linaro');a=parser.parse_args()
    if not sys.platform.startswith('linux') or os.geteuid()!=0:parser.error('Run with sudo on S Linux')
    target=Path(a.home).resolve()/'.bashrc'
    if target.parent!=Path('/home/linaro') or target.is_symlink():
        raise ValueError('This hook targets the regular S desktop user /home/linaro only')
    original=target.read_text(encoding='utf-8')
    block=(ROOT/'startup/terminal_center.bash').read_text(encoding='utf-8')
    result=insert_hook(original,block)
    if result==original:
        print('TERMINAL_CENTER_HOOK_ALREADY_INSTALLED');return 0
    out=ROOT/'logs'/('terminal_center_install_'+str(time.time_ns()));out.mkdir()
    shutil.copy2(target,out/'bashrc.before')
    temp=target.with_name('.bashrc.steering-new');temp.write_text(result,encoding='utf-8')
    subprocess.run(['bash','-n',str(temp)],check=True)
    attributes=target.stat();os.chmod(temp,attributes.st_mode&0o777);os.chown(temp,attributes.st_uid,attributes.st_gid)
    temp.replace(target)
    shutil.copy2(target,out/'bashrc.after')
    record=dict(schema='STEER_TERMINAL_CENTER_INSTALL_V1',target=str(target),backup=str(out/'bashrc.before'),
        new_sha256=hashlib.sha256(target.read_bytes()).hexdigest(),
        installed_before_foreground_make_run=True,noninteractive_shells_unchanged=True,
        normal_S_startup_preserved=True,confirmation_executed_by_installer=False)
    (out/'installation.json').write_text(json.dumps(record,indent=2))
    print('TERMINAL_CENTER_HOOK_INSTALLED '+json.dumps(record),flush=True)
    return 0


if __name__=='__main__':raise SystemExit(main())
