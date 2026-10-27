#!/usr/bin/env python3
"""Read-only HMI/line observer; captures the real X11 screen on a blocked RIGHT."""
import ctypes, json, os, pathlib, struct, subprocess, time, zlib
root=pathlib.Path('/tmp/line_right_check_20261027');root.mkdir(exist_ok=True)
libc=ctypes.CDLL(None)
libc.shmget.argtypes=[ctypes.c_int,ctypes.c_size_t,ctypes.c_int];libc.shmget.restype=ctypes.c_int
libc.shmat.argtypes=[ctypes.c_int,ctypes.c_void_p,ctypes.c_int];libc.shmat.restype=ctypes.c_void_p
libc.shmdt.argtypes=[ctypes.c_void_p]
mapped={}
def read(key):
    if key not in mapped:
        sid=libc.shmget(key,64,0)
        if sid<0:return None
        p=libc.shmat(sid,None,0o10000)
        if p==ctypes.c_void_p(-1).value:return None
        mapped[key]=p
    p=mapped[key]
    for retry in range(4):
        data=struct.unpack('<16I',ctypes.string_at(p,64))
        seq=ctypes.c_uint32.from_address(p).value
        if data[0]==seq and not seq&1 and data[1]==0x4c534931:
            ttl=((data[13]<<32)|data[12])-time.monotonic_ns()
            return dict(source=data[3],left=data[4],right=data[5],Q=[data[6],data[7]],
                        detail=data[14],blocked=data[15],ttl_ms=ttl//1000000)
    return None
def capture():
    env=dict(os.environ,DISPLAY=':0',XAUTHORITY='/home/linaro/.Xauthority')
    path=root/'right_block.xwd'
    subprocess.run(['xwd','-root','-silent','-out',str(path)],env=env,check=True,timeout=8)
    data=path.read_bytes();a=struct.unpack('!25I',data[:100])
    header,version,fmt,depth,w,h,offset,byte_order,unit,bit_order,pad,bpp,stride,visual,rm,gm,bm,bits,entries,ncolors,*rest=a
    assert version==7 and fmt==2 and bpp==32 and byte_order in (0,1)
    start=header+ncolors*12
    x0,x1=int(w*.745),int(w*.99);y0,y1=int(h*.17),int(h*.92)
    def channel(value,mask):
        shift=(mask&-mask).bit_length()-1
        return ((value&mask)>>shift)*255//(mask>>shift)
    rows=[]
    for y in range(y0,y1):
        row=data[start+y*stride+x0*4:start+y*stride+x1*4]
        rgb=bytearray()
        for (pixel,) in struct.iter_unpack(('<' if byte_order==0 else '>')+'I',row):
            rgb.extend((channel(pixel,rm),channel(pixel,gm),channel(pixel,bm)))
        rows.append(b'\0'+rgb)
    def chunk(kind,payload):
        return struct.pack('!I',len(payload))+kind+payload+struct.pack('!I',zlib.crc32(kind+payload)&0xffffffff)
    image=b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('!2I5B',x1-x0,y1-y0,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(b''.join(rows),3))+chunk(b'IEND',b'')
    (root/'right_block_car.png').write_bytes(image)
    print('REAL_LCD_CAPTURED',root/'right_block_car.png',flush=True)
previous=None;captured=False
duration=max(10,min(300,int(os.environ.get('LINE_CHECK_SECONDS','180'))))
end=time.monotonic()+duration
with (root/'events.jsonl').open('w',buffering=1) as log:
    try:
        print('WATCH_READY read_only=1 seconds='+str(duration),flush=True)
        while time.monotonic()<end:
            detected=read(0x12345681);warning=read(0x12345682)
            signature=None if warning is None else (warning['left'],warning['right'],warning['detail'],warning['blocked'])
            if signature!=previous:
                record=dict(t_ns=time.monotonic_ns(),detected=detected,warning=warning)
                log.write(json.dumps(record)+'\n');print('STATE',json.dumps(record),flush=True);previous=signature
            if not captured and detected and warning and detected['ttl_ms']>0 and warning['ttl_ms']>0 and detected['right']==1 and warning['detail'] in (0x0d,0xdd) and warning['blocked']&2:
                captured=True
                (root/'trigger.json').write_text(json.dumps(dict(detected=detected,warning=warning),indent=2))
                time.sleep(.18)
                try:capture()
                except Exception as error:print('CAPTURE_ERROR',repr(error),flush=True)
            time.sleep(.02)
    finally:
        for p in mapped.values():libc.shmdt(p)
print('WATCH_DONE captured='+str(captured),flush=True)
