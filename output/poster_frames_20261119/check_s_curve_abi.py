from board_io import execute

print(execute('S', '''import subprocess
from pathlib import Path
for file in ('/home/linaro/S/csrc/shared_memory.c','/home/linaro/S/include/shared_memory.h'):
 p=Path(file); lines=p.read_text().splitlines()
 for i,line in enumerate(lines):
  if 'int curve_detection_update(' in line or 'int curve_detection_update_until(' in line:
   print(p.name,i, '\\n'.join(lines[i:i+18]))
 print('layout', [(i,l.strip()) for i,l in enumerate(lines) if 'deadline_format' in l or 'deadline_lo' in l][:8])
for file in ('/home/linaro/S/x11/csrc/build/shared_memory.o','/home/linaro/S/udp/build/video_receiver'):
 result=subprocess.run(['nm',file],capture_output=True,text=True)
 print(file, [l.strip() for l in result.stdout.splitlines() if 'curve_detection_update' in l])
''', 8))
