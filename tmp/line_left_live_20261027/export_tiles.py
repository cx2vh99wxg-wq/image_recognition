import array,pathlib,struct,zlib
root=pathlib.Path('/tmp/line_left_samples_20261027')
def png(path,data,w=320,h=240,channels=1):
    def chunk(kind,payload):return struct.pack('!I',len(payload))+kind+payload+struct.pack('!I',zlib.crc32(kind+payload)&0xffffffff)
    raw=b''.join(b'\0'+data[y*w*channels:(y+1)*w*channels] for y in range(h))
    path.write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('!2I5B',w,h,8,2 if channels==3 else 0,0,0,0))+chunk(b'IDAT',zlib.compress(raw,3))+chunk(b'IEND',b''))
for n in range(12):
    prefix=root/('sample_%02d'%n)
    for suffix,channels in [('.ppm',3),('_mask.pgm',1)]:
        data=prefix.with_name(prefix.name+suffix).read_bytes().split(b'\n',3)[3]
        cropped=b''.join(data[y*640*channels:y*640*channels+320*channels] for y in range(240))
        png(prefix.with_name(prefix.name+('_tile.png' if channels==3 else '_lane.png')),cropped,channels=channels)
    drivable=prefix.with_name(prefix.name+'_probe_drivable.f32')
    if drivable.exists():
        values=array.array('f');values.frombytes(drivable.read_bytes());assert len(values)==2*640*480
        mask=bytes(255 if values[640*480+y*640+x]>values[y*640+x] else 0 for y in range(240) for x in range(320))
        png(prefix.with_name(prefix.name+'_green.png'),mask)
print('EXPORT_OK',flush=True)
