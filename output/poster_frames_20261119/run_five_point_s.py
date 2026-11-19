from pathlib import Path
import sys
import tarfile
import time

from board_io import execute, get, put_archive

root = Path(__file__).resolve().parent
stage = '/userdata/poster_frames_20261119'
target = stage + '/five_point_s'
names = ['112507_01', '113343_03']
names += [f'mountain_{x:03}' for x in (1, 2, 3, 4, 5, 7, 9, 10, 12, 13, 14, 16,
                                        17, 18, 19, 20, 22, 25, 26, 27, 28, 29)]
mode = sys.argv[1]
if mode == 'start':
    archive = root / 'five_point_s_inputs.tar.gz'
    with tarfile.open(archive, 'w:gz') as output:
        for stem in names:
            output.add(root / 'five_point_m' / f'{stem}.png', arcname=f'input-{stem}.png')
    put_archive('S', archive, target + '/inputs.tar.gz')
    print(execute('S', f'''import subprocess
from pathlib import Path
stage=Path({stage!r})
with (stage/'five_point_s/render.log').open('w') as log:
 p=subprocess.Popen(['python3','-u',stage/'render_five_point_on_s.py'],stdin=subprocess.DEVNULL,stdout=log,stderr=log,start_new_session=True)
 print('S render process',p.pid)
''', 7))
elif mode == 'status':
    print(execute('S', f'''from pathlib import Path
p=Path({target!r})
log=p/'render.log'
if log.exists(): print(log.read_text()[-2500:])
print('rendered',len([f for f in p.glob('*.png') if not f.name.startswith('input-')]))
if (p/'screens.tar.gz').exists(): print('archive bytes',(p/'screens.tar.gz').stat().st_size)
''', 7))
elif mode == 'get':
    for attempt in range(12):
        status = execute('S', f'''from pathlib import Path
p=Path({target!r},'render.log')
if p.exists(): print(p.read_text()[-2500:])
''', 7)
        if 'S_FRONT_RENDERED' in status:
            print(status)
            break
        if 'Traceback' in status:
            raise RuntimeError(status)
        time.sleep(3)
    else:
        raise TimeoutError('S board did not finish the screenshot archive')
    get('S', target + '/screens.tar.gz', root / 'five_point_s_screens.tar.gz')
