import sys
from board_io import execute

side = sys.argv[1]
if side == 'M':
    remote = '''from pathlib import Path
import subprocess
p = Path('/home/linaro/M/pcie')
for file in p.glob('front*conf'):
 print(file.name, file.read_text().strip().replace(chr(10),' '))
stage=Path('/userdata/poster_frames_20261119')
print('stage', sorted(x.name for x in stage.iterdir()))
print('inputs', len(list((stage/'inputs').glob('*.bgr'))))
for line in subprocess.getoutput('ps -eo pid,comm,args').splitlines():
 if 'pcie_capture' in line and 'python' not in line:
  pid=line.split()[0]
  print('process', line[:200])
  e=Path('/proc',pid,'environ')
  if e.exists(): print([x.decode() for x in e.read_bytes().split(b'\\x00') if x.startswith((b'VISION_HDMI',b'LANE_CURVE'))])
'''
else:
    remote = '''import glob, os, shutil, subprocess
print('build', glob.glob('/home/linaro/S/x11/csrc/build/*'))
print('Xvfb', shutil.which('Xvfb'))
print('last_frames', glob.glob('/home/linaro/S/x11/csrc/*frame*'))
for line in subprocess.getoutput('ps -eo pid,comm,args').splitlines():
 if any(name in line for name in ('vehicle_control', 'eight_camera')) and 'python' not in line: print('process', line[:210])
environment=dict(os.environ, DISPLAY=':0', XAUTHORITY='/home/linaro/.Xauthority')
subprocess.run(['xwininfo','-root','-children'],env=environment, stdout=subprocess.PIPE,text=True,check=True).stdout.splitlines()
for line in subprocess.run(['xwininfo','-root','-children'],env=environment,stdout=subprocess.PIPE,text=True,check=True).stdout.splitlines():
 if 'Perception' in line: print(line.strip())
'''
print(execute(side, remote))
