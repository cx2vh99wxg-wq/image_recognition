#!/usr/bin/env python3
"""Bounded diagnostic: retain matched NN RGB/mask pairs without publishing."""
import json, pathlib, shutil, struct, time, zlib
root=pathlib.Path('/tmp/line_left_samples_20261027');root.mkdir(exist_ok=True)
request=pathlib.Path('/tmp/m_lane_snapshot.request')
index=[]
for n in range(12):
    request.touch();deadline=time.monotonic()+5
    while request.exists() and time.monotonic()<deadline:time.sleep(.05)
    if request.exists():print('SNAPSHOT_TIMEOUT',n,flush=True);break
    prefix='sample_%02d'%n
    text=pathlib.Path('/tmp/m_lane_snapshot.txt').read_text()
    source=int(text.split('source=')[1].split()[0])
    for original,suffix in [('m_lane_snapshot.ppm','.ppm'),('m_lane_snapshot_mask.pgm','_mask.pgm'),('m_lane_snapshot.txt','.txt')]:
        shutil.copyfile('/tmp/'+original,str(root/(prefix+suffix)))
    index.append(dict(name=prefix,source=source,metadata=text))
    (root/'index.json').write_text(json.dumps(index,indent=2))
    if n==0:
        pixels=(root/(prefix+'.ppm')).read_bytes().split(b'\n',3)[3]
        chunk=lambda t,d:struct.pack('!I',len(d))+t+d+struct.pack('!I',zlib.crc32(t+d)&0xffffffff)
        raw=b''.join(b'\0'+pixels[y*640*3:(y+1)*640*3] for y in range(480))
        (root/'first.png').write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('!2I5B',640,480,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(raw,3))+chunk(b'IEND',b''))
    print('SAMPLE',n,source,text.replace('\n',' | '),flush=True)
    time.sleep(1)
print('COLLECTION_DONE',len(index),flush=True)
