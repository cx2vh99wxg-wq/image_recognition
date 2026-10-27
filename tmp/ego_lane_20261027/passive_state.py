"""Bounded read-only observation of line metadata and HMI warning segments."""
import ctypes,struct,time,json,pathlib,sys,os,hashlib
role=sys.argv[1];assert role in ('M','S')
work=pathlib.Path('/home/linaro')/role/'ego_lane_20261027'
lib=ctypes.CDLL(None);lib.shmget.argtypes=[ctypes.c_int,ctypes.c_size_t,ctypes.c_int];lib.shmget.restype=ctypes.c_int
lib.shmat.argtypes=[ctypes.c_int,ctypes.c_void_p,ctypes.c_int];lib.shmat.restype=ctypes.c_void_p;lib.shmdt.argtypes=[ctypes.c_void_p]
mapped={}
def read(key):
    if key not in mapped:
        sid=lib.shmget(key,64,0)
        if sid<0:return None
        p=lib.shmat(sid,None,0o10000)
        if p==ctypes.c_void_p(-1).value:return None
        mapped[key]=p
    p=mapped[key]
    for _ in range(4):
        a=struct.unpack('<16I',ctypes.string_at(p,64))
        if not a[0]&1 and a[1]==0x4c534931 and a[0]==ctypes.c_uint32.from_address(p).value:
            return dict(session=a[2],source=a[3],left=a[4],right=a[5],Q=[a[6],a[7]],diagnostic=a[14],blocked=a[15],ttl_ms=(((a[13]<<32)|a[12])-time.monotonic_ns())//1000000)
records=[]
try:
    for _ in range(6):
        record=dict(t_ns=time.monotonic_ns(),detected=read(0x12345681))
        if role=='S':record['warning']=read(0x12345682)
        records.append(record);print(json.dumps(record),flush=True);time.sleep(1)
finally:
    for p in mapped.values():lib.shmdt(p)
(work/'passive_state.json').write_text(json.dumps(records,indent=2))
assert all(r['detected'] and r['detected']['ttl_ms']>0 for r in records)
assert len({r['detected']['source'] for r in records})>=4
print('PASSIVE_METADATA_FRESH',role,flush=True)
