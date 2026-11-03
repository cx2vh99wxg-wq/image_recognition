import base64,gzip,hashlib,json,subprocess,sys
from pathlib import Path
w=Path(__file__).resolve().parent
for board in 'SM':
 p=json.loads(subprocess.check_output([sys.executable,str(w/'source_stream.py'),board]))
 rows=[]
 for rel,encoded,old in json.loads(gzip.decompress(base64.b64decode(p['data']))):
  if not (rel.endswith(('.sh','.bash','.py')) or rel.endswith('/Makefile')):continue
  raw=base64.b64decode(encoded)
  if b'\r\n' not in raw:continue
  rows.append((rel,old,hashlib.sha256(raw.replace(b'\r\n',b'\n')).hexdigest()))
 z=base64.b64encode(gzip.compress(json.dumps(rows).encode())).decode()
 src="""import pathlib,base64,gzip,json,hashlib,os,stat,tempfile
root=pathlib.Path('/home/linaro')
rows=json.loads(gzip.decompress(base64.b64decode('PAYLOAD')))
def sha(b):return hashlib.sha256(b).hexdigest()
for rel,old,new in rows:
 p=root/rel
 assert p.resolve()==p
 raw=p.read_bytes()
 assert sha(raw) in (old,new),rel
 if sha(raw)==new:continue
 v=raw.replace(b'\\r\\n',b'\\n')
 assert sha(v)==new
 st=p.stat()
 fd,tmp=tempfile.mkstemp(dir=str(p.parent),prefix='.source_sync_')
 with os.fdopen(fd,'wb') as f:f.write(v)
 os.chown(tmp,st.st_uid,st.st_gid)
 os.chmod(tmp,stat.S_IMODE(st.st_mode))
 os.replace(tmp,p)
 assert sha(p.read_bytes())==new
print('LF_VERIFIED',len(rows))
""".replace('PAYLOAD',z)
 command="python3 -u -c 'import base64;exec(base64.b64decode(\""+base64.b64encode(src.encode()).decode()+"\"))'\n"
 (w/(board+'_lf.sh')).write_text(command)
 print(board,len(rows),len(command))
