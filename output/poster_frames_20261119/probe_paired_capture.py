import sys
from board_io import execute

side=sys.argv[1]
base='''from pathlib import Path
import glob, hashlib, os, shutil, subprocess
def cmd(*args):
 return subprocess.run(args,capture_output=True,text=True).stdout.strip()
print('utc',cmd('date','-Ins'))
for f in Path('/userdata').iterdir():
 if any(k in f.name for k in ('front','poster','actor','hdmi','video')): print('dir', f.name, sorted(x.name for x in f.iterdir())[:45] if f.is_dir() else '')
for line in cmd('ps','-eo','pid,comm,args').splitlines():
 if 'python' not in line and any(k in line for k in ('pcie_capture','eight_camera_re','vehicle_control','udp_sender','video_receiver')):
  fields=line.split()
  exe=Path('/proc', fields[0], 'exe')
  print(line[:150], os.path.realpath(exe), hashlib.sha256(exe.read_bytes()).hexdigest()[:20] if exe.is_file() else '')
'''
extra_m='''
for f in glob.glob('/dev/v4l/by-id/*'): print('v4l',f,cmd('v4l2-ctl','-d',f,'--get-fmt-video','--get-input'))
print('net',cmd('ip','-br','addr'))
print('isolated',bool(shutil.which('unshare')))
'''
extra_s='''
os.environ.update(DISPLAY=':0',XAUTHORITY='/home/linaro/.Xauthority')
for line in cmd('xwininfo','-root','-tree').splitlines():
 if 'Perception' in line: print('window',line.strip())
print('net',cmd('ip','-br','addr'))
print('x11tools',{k:shutil.which(k) for k in ('xwd','xwininfo','xmessage','Xephyr','Xvfb')})
'''
print(execute(side,base+(extra_m if side=='M' else extra_s),10))
