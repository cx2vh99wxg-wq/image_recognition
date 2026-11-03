import subprocess,sys,base64,gzip,json,hashlib,pathlib,os,shutil,tempfile,time
b=pathlib.Path('/home/linaro')
side='@SIDE@'
assert (b/side).is_dir()
chunks=[]
print('SYNC_READY',flush=True)
for line in sys.stdin:
 try:
  i,d=line.strip().split(' ',1)
  if i=='END':
   raw=base64.b64decode(''.join(chunks),validate=True)
   assert hashlib.sha256(raw).hexdigest()==d
   rows=json.loads(gzip.decompress(raw))
   plan=[]
   for rel,data,digest in rows:
    r=pathlib.PurePosixPath(rel)
    active=r.parts[0]=='@active'
    if active:
     p=b/'.bashrc'
     assert hashlib.sha256(p.read_bytes()).hexdigest()==r.parts[1]
     r=pathlib.PurePosixPath('active_home.bashrc')
    else:
     assert r.parts[0] in (side,'common') and '..' not in r.parts
     p=b.joinpath(*r.parts)
    assert p.resolve()==p
    v=base64.b64decode(data,validate=True)
    assert hashlib.sha256(v).hexdigest()==digest
    if p.name=='.bashrc':subprocess.run(['/bin/bash','-n'],input=v,check=True)
    plan.append((r,p,v,digest))
   backup=b/('source_sync_backup_'+side+'_'+str(time.time_ns()))
   backup.mkdir()
   changed=0
   for r,p,v,digest in plan:
    if p.exists() and p.read_bytes()==v: continue
    p.parent.mkdir(parents=True,exist_ok=True)
    mode=0o755 if p.suffix in ('.sh','.bash') else 0o644
    uid=gid=0
    if p.exists():
     st=p.stat();mode=st.st_mode&0o777;uid=st.st_uid;gid=st.st_gid
     old=backup.joinpath(*r.parts);old.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,old)
    fd,tmp=tempfile.mkstemp(dir=str(p.parent),prefix='.source_sync_')
    with os.fdopen(fd,'wb') as f:f.write(v)
    os.chmod(tmp,mode);os.chown(tmp,uid,gid);os.replace(tmp,p)
    changed+=1
   for r,p,v,digest in plan:assert hashlib.sha256(p.read_bytes()).hexdigest()==digest
   print('SYNC_DONE %s %d %d %s %s'%(side,len(plan),changed,backup,d),flush=True)
   break
  n=int(i)
  assert n==len(chunks) or (n==len(chunks)-1 and chunks[n]==d)
  if n==len(chunks):chunks.append(d)
  print('SYNC_ACK '+i+' '+hashlib.sha256(d.encode()).hexdigest()[:16],flush=True)
 except Exception as e:print('SYNC_ERR '+repr(e),flush=True)
