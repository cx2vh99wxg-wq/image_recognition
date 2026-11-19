from board_io import execute

print(execute('M', '''from pathlib import Path
import subprocess
p=Path('/userdata/poster_frames_20261119')
for f in sorted(p.glob('five*')): print(f.name,f.stat().st_size)
for line in subprocess.getoutput('ps -eo pid,etime,state,comm,args').splitlines():
 if any(word in line for word in ('five_point','ffmpeg','cc1plus')) and 'python3 -u -c' not in line: print(line[:250])
for f in [p/'five_point_m/inference.log',p/'five_point_m/centers.txt']:
 if f.exists(): print(f.name,f.stat().st_size,f.read_text(errors='replace')[-1800:])
''', 8))
