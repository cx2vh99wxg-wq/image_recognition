from pathlib import Path
import sys
from board_io import execute, put, get, put_archive

local = Path(__file__).resolve().parent
side, action, *args = sys.argv[1:]
root = '/userdata/poster_frames_20261119/paired_current'
if action == 'upload':
    for name in args:
        file = local / name
        if file.stat().st_size > 40000:
            put_archive(side, file, f'{root}/{file.name}')
        else:
            put(side, file, f'{root}/{file.name}')
elif action == 'start':
    script, *params = args
    logfile = script.removesuffix('.py') + '-' + '-'.join(params) + '.log'
    print(execute(side, f'''from pathlib import Path
import subprocess
p=Path({root!r})
with (p/{logfile!r}).open('w') as log:
 proc=subprocess.Popen(['python3','-u',p/{script!r},*{params!r}],cwd=p,stdin=subprocess.DEVNULL,stdout=log,stderr=log,start_new_session=True)
 print('START',proc.pid,{logfile!r})
''', 8))
elif action == 'show':
    print(execute(side, f'''from pathlib import Path
p=Path({root!r})
for name in {args!r}:
 f=p/name
 if f.is_file():
  lines=f.read_text(errors='replace').splitlines()
  if name.startswith('run_paired_m'):
   selected=[l for l in lines if l.startswith(('SHOT','M_','MISSING'))]
   print(name,len(lines),len(selected))
   print('\\n'.join(selected))
   if 'M_REPLAY_COMPLETE' not in lines[-1]: print('TAIL','\\n'.join(lines[-6:]))
  else: print('\\n'.join(lines[-25:]))
''', 10))
elif action == 'get':
    for name in args:
        get(side, f'{root}/{name}', local / name)
