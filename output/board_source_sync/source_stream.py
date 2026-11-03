import base64,gzip,hashlib,json,os,sys
from pathlib import Path
root=Path('udp_rk_rk_yolo')
items=[]
for side in (sys.argv[1],'common'):
 for p in sorted((root/side).rglob('*')):
  if not p.is_file() or p.is_symlink() or set(p.parts)&{'build','logs','log','.vscode','.claude','__pycache__','model','calibration','evidence'}: 
   if 'calibration' not in p.parts or 'tools' not in p.parts: continue
   if not p.is_file() or p.is_symlink() or p.suffix!='.py': continue
  if p.suffix.lower() not in {'.c','.cpp','.h','.hpp','.inc','.py','.sh','.bash','.ps1'} and p.name!='Makefile':continue
  data=p.read_bytes()
  if p.suffix in (".sh",".bash",".py") or p.name=="Makefile": data=data.replace(b"\r\n",b"\n")
  items.append((p.relative_to(root).as_posix(),base64.b64encode(data).decode(),hashlib.sha256(data).hexdigest()))
if len(sys.argv)>2 and int(sys.argv[2])>=0: items=items[int(sys.argv[2])::6]
blob=gzip.compress(json.dumps(items,separators=(',',':')).encode())
print(json.dumps(dict(count=len(items),hash=hashlib.sha256(blob).hexdigest(),data=base64.b64encode(blob).decode())))


