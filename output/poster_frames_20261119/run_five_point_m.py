from pathlib import Path

from board_io import execute, put, get

local = Path(__file__).resolve().parent
remote = '/userdata/poster_frames_20261119'
put('M', local / 'five_point_front.cpp', remote + '/five_point_front.cpp')
print(execute('M', '''from pathlib import Path
import shutil, subprocess, tarfile
p=Path('/userdata/poster_frames_20261119')
repo=Path('/home/linaro/M/pcie')
out=p/'five_point_m'
out.mkdir(exist_ok=True)
flags=['g++','-std=c++11','-O2','-ffunction-sections','-fdata-sections','-I'+str(repo/'include'),'-I/home/linaro/M/include']
build=subprocess.run(flags+[p/'five_point_front.cpp',p/'yolo_integration.o',p/'preprocess.o','-Wl,--gc-sections','-L'+str(repo/'lib'),'-Wl,-rpath,'+str(repo/'lib'),'-lrknnrt','-lm','-ldl','-pthread','-o',p/'five_point_front'],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
if build.returncode: raise RuntimeError(build.stdout[-4500:])
source=list(sorted((p/'inputs').glob('*.bgr')))
manifest=p/'five_point_input.txt'
manifest.write_text(''.join(f'{f} {out/(f.stem+".ppm")}\\n' for f in source))
log=out/'inference.log'
with log.open('w') as handle:
 result=subprocess.run([p/'five_point_front',repo/'model/yolopv2_front_320x240_mixed_error_guided_rk3568.rknn',manifest,repo/'front_hdmi_ipm.conf'],cwd=repo,stdout=handle,stderr=handle)
lines=log.read_text(errors='replace').splitlines()
if result.returncode: raise RuntimeError(str(lines[-20:]))
records=[line for line in lines if line.startswith('FIVE ')]
assert records
(out/'centers.txt').write_text('\\n'.join(records)+'\\n')
with tarfile.open(p/'five_point_m.tar.gz','w:gz') as archive:
 for record in records:
  frame=Path(record.split()[1])
  png=frame.with_suffix('.png')
  subprocess.run(['ffmpeg','-loglevel','error','-nostdin','-y','-i',frame,'-frames:v','1',png],check=True)
  archive.add(png,arcname=png.name)
 archive.add(out/'centers.txt',arcname='centers.txt')
print('CANDIDATES',len(records), (p/'five_point_m.tar.gz').stat().st_size)
for line in records: print(line.split('/')[-1])
''', 80))
get('M', remote + '/five_point_m.tar.gz', local / 'five_point_m.tar.gz')
