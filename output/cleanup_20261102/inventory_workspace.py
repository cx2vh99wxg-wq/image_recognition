"""Read-only workspace sizing. Does not follow links or change project files."""
from collections import defaultdict
import json
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = Path(__file__).resolve().parent
files = []
links = []
errors = []
stack = [ROOT]
while stack:
    folder = stack.pop()
    try:
        with os.scandir(folder) as entries:
            for entry in entries:
                path = Path(entry.path)
                relative = path.relative_to(ROOT).as_posix()
                if entry.is_symlink() or getattr(entry.stat(follow_symlinks=False), 'st_file_attributes', 0) & 0x400:
                    links.append(relative)
                elif entry.is_dir(follow_symlinks=False):
                    stack.append(path)
                elif entry.is_file(follow_symlinks=False):
                    stat = entry.stat(follow_symlinks=False)
                    files.append(dict(path=relative, size=stat.st_size, mtime_ns=stat.st_mtime_ns))
    except OSError as error:
        errors.append(dict(path=str(folder), error=str(error)))
sizes = defaultdict(int)
counts = defaultdict(int)
for item in files:
    parts = item['path'].split('/')
    for count in range(1, len(parts)):
        directory = '/'.join(parts[:count])
        sizes[directory] += item['size']
        counts[directory] += 1
result = dict(root=str(ROOT), file_count=len(files), total_bytes=sum(x['size'] for x in files),
              files=files, links=links, errors=errors,
              directories=[dict(path=k, size=v, files=counts[k]) for k, v in sorted(sizes.items(), key=lambda p: -p[1])])
(OUT / 'inventory_before.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print('TOTAL_GiB %.3f FILES %u LINKS %u ERRORS %u' % (result['total_bytes']/2**30, len(files), len(links), len(errors)))
print('LARGEST_DIRECTORIES_UP_TO_DEPTH_3')
for item in result['directories']:
    if item['path'].count('/') < 3 and item['size'] > 1024**2:
        print('%.2f MiB %u files %s' % (item['size']/2**20, item['files'], item['path']))
print('LARGEST_35_FILES_EXCLUDING_GIT')
for item in sorted((x for x in files if not x['path'].startswith('.git/')), key=lambda x:-x['size'])[:35]:
    print('%.2f MiB %s' % (item['size']/2**20, item['path']))
