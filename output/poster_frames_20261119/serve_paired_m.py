from hashlib import sha256
from http.server import BaseHTTPRequestHandler, HTTPServer
import json
from pathlib import Path
import sys
import tarfile
import time

root = Path('/userdata/poster_frames_20261119/paired_current')
samples = root / 'evidence'
stage = sys.argv[1]
manifest = root / (stage + '_requested.json')
selection = json.loads(manifest.read_text())
metadata = {}
for path in samples.glob('*_clips.json'):
    metadata.update(json.loads(path.read_text()))
observations = {}
for path in root.glob('run_paired_m-*.log'):
    for line in path.read_text(errors='replace').splitlines():
        if line.startswith('SHOT '):
            words = line.split()
            observations[words[1]] = words[2:]
for item in selection:
    item.update(metadata[item['shot']])
    item['observations'] = observations[item['shot']]
selected = samples / (stage + '_selection.json')
selected.write_text(json.dumps(selection))
shots = samples / (stage + '_shots.txt')
shots.write_text(''.join(f'{x["shot"]} {x["view"]}\n' for x in selection))
name = stage + '_selection.tar.gz'
archive = root / name
with tarfile.open(archive, 'w:gz', compresslevel=3) as out:
    added = set()
    for item in selection:
        assert item['view'] in ('raw','ipm')
        for ext in ['meta',item['view']]:
            src = samples / (item['shot'] + '.' + ext)
            if src not in added:
                out.add(src, arcname=src.name)
                added.add(src)
    for src in (shots,selected):
        out.add(src,arcname=src.name)
data = archive.read_bytes()
digest = sha256(data).hexdigest()

class Transfer(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.client_address[0] != '192.168.100.20' or self.path != '/' + name:
            self.send_error(404)
            return
        self.send_response(200)
        self.send_header('Content-Length',str(len(data)))
        self.send_header('X-SHA256',digest)
        self.end_headers()
        self.wfile.write(data)
        self.server.done = True

with HTTPServer(('192.168.100.10',43879),Transfer) as server:
    server.timeout = 3
    server.done = False
    print('M_SELECTION_READY', len(selection), len(data), digest, flush=True)
    stop = time.monotonic() + 75
    while time.monotonic() < stop and not server.done:
        server.handle_request()
    if not server.done:
        raise TimeoutError('S board did not fetch selected frames')
    print('M_SELECTION_SENT', flush=True)
