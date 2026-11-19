from board_io import execute

print(execute('M', '''from pathlib import Path
import subprocess,shutil
for path in sorted(Path('/userdata/actor_video_1119').glob('*.mp4')):
 result=subprocess.getoutput('ffprobe -v error -select_streams v:0 -show_entries stream=width,height,duration -of csv=p=0 '+str(path))
 print(path.name,path.stat().st_size,result)
p=Path('/home/linaro/M/pcie')
print('objs',[(x.name,x.stat().st_size) for x in (p/'build').glob('*.o')])
q=p/'build/pcie_capture'
print('current path',q.stat().st_size)
print('unshare',subprocess.run(['unshare','--ipc','--mount','true']).returncode)
''',10))
