from pathlib import Path
import sys,json,base64,gzip,hashlib
w=Path(__file__).resolve().parent
root=w.parents[1]/'udp_rk_rk_yolo'
side=sys.argv[1]
items=[]
def add(rel,raw):
 items.append((rel,base64.b64encode(raw).decode(),hashlib.sha256(raw).hexdigest()))
for rel in json.loads((w/'manifest.json').read_text()):
 if rel.split('/')[0] not in (side,'common'):continue
 raw=(root/rel).read_bytes()
 if rel.endswith(('Makefile','.bashrc')): raw=raw.replace(b'\r\n',b'\n')
 add(rel,raw)
old=hashlib.sha256((w/f'active_{side}.before').read_bytes()).hexdigest()
add('@active/'+old,(w/f'active_{side}.after').read_bytes())
blob=gzip.compress(json.dumps(items,separators=(',',':')).encode())
print(json.dumps(dict(count=len(items),hash=hashlib.sha256(blob).hexdigest(),data=base64.b64encode(blob).decode())))
