"""Mock sender must remain unconnected so a later static-IP change is usable."""
from pathlib import Path
import os,socket,subprocess,time,zlib

program=Path(__file__).resolve().parents[1]/'build/angle_monitor'
with socket.socket(socket.AF_INET,socket.SOCK_DGRAM) as receiver:
    receiver.bind(('127.0.0.1',0));receiver.settimeout(3)
    p=subprocess.Popen([str(program),'--backend','mock','--samples','0',
        '--interval-ms','10','--print-every','1000','--send-to','127.0.0.1',
        '--port',str(receiver.getsockname()[1])],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    try:
        packet,peer=receiver.recvfrom(100)
        assert peer[0]=='127.0.0.1' and len(packet)==48 and packet[:4]==b'STF1'
        assert zlib.crc32(packet[:-4])==int.from_bytes(packet[-4:],'big')
        # Socket must have no pinned peer or local interface address. Inspection
        # of the actual CLI catches accidentally reintroducing connect().
        inodes=set()
        for fd in Path('/proc/%d/fd'%p.pid).iterdir():
            try:
                target=os.readlink(fd)
                if target.startswith('socket:['):inodes.add(target[8:-1])
            except FileNotFoundError:pass
        rows=[r.split() for r in Path('/proc/net/udp').read_text().splitlines()[1:]]
        actual=[r for r in rows if r[9] in inodes]
        assert len(actual)==1 and actual[0][1].startswith('00000000:') and actual[0][2]=='00000000:0000',actual
        previous=int.from_bytes(packet[12:16],'big')
        packet,_=receiver.recvfrom(100)
        assert int.from_bytes(packet[12:16],'big')>previous
    finally:
        p.terminate();out,err=p.communicate(timeout=3)
    assert p.returncode==130,(out,err)
    assert b'IMU UDP summary sent=' in out and b'errors=0' in out,(out,err)
print('PASS real CLI UDP: valid STF1, advancing samples, wildcard source/unconnected socket, clean shutdown')
