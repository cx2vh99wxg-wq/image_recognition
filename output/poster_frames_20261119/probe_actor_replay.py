from board_io import execute

print(execute('M', '''from pathlib import Path
p=Path('/userdata/poster_frames_20261119/paired_current')
for name in ('city','mountain'):
 f=p/f'run_paired_m-{name}.log'
 if not f.is_file():continue
 print(name)
 lines=f.read_text(errors='replace').splitlines()
 for l in [x for x in lines if ('TRAFFIC PERF' in x or 'budget_ms=' in x or 'optional detector unavailable' in x or 'failed' in x.lower())][-17:]: print(l)
 print('tail', lines[-5:])
''',12))
