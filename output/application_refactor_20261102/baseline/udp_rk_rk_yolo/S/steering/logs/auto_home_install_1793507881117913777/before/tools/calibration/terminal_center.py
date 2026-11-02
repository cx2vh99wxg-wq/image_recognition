"""Bash startup opt-in: confirm a new M session once after normal S startup.

Opening the first terminal after M restart is the operator's declaration that
the unchanged sensor is stationary at physical center on a fixed vehicle.
This cannot discover mechanical center and never performs homing motion.
"""
import argparse
import ctypes
import json
import math
import os
from pathlib import Path
import select
import socket
import struct
import subprocess
import sys
import time
import zlib

ROOT=Path(__file__).resolve().parents[2]


def decode_packet(packet):
    if len(packet)<42 or packet[12:14]!=b'\x08\x00' or packet[14]>>4!=4 or packet[23]!=17:return None
    header=(packet[14]&15)*4
    if header<20 or packet[26:30]!=socket.inet_aton('192.168.100.10') or packet[30:34]!=socket.inet_aton('192.168.100.20'):return None
    udp=14+header
    if len(packet)<udp+8 or struct.unpack_from('!H',packet,udp+2)[0]!=54321:return None
    length=struct.unpack_from('!H',packet,udp+4)[0]
    if length!=56 or len(packet)<udp+length:return None
    wire=packet[udp+8:udp+length]
    if wire[:8]!=b'STF1\x00\x01\x00\x30' or wire[40:44]!=b'\0'*4:return None
    if zlib.crc32(wire[:44])!=struct.unpack_from('!I',wire,44)[0]:return None
    session,seq,flags=struct.unpack_from('!3I',wire,8)
    age=struct.unpack_from('!I',wire,28)[0]
    if not session or flags not in (1,3) or age>=100 or not struct.unpack_from('!Q',wire,32)[0]:return None
    return session


def reference_session(path):
    try:
        values=path.read_text().split()
        if len(values)!=4 or values[0]!='STEER_PHYSICAL_REFERENCE_V1' or values[3]!='1':return None
        session=int(values[1]);zero=float(values[2])
        return session if 0<session<=0xffffffff and math.isfinite(zero) else None
    except (OSError,ValueError):return None


def receive_socket():
    s=socket.socket(socket.AF_PACKET,socket.SOCK_RAW,socket.htons(0x0800))
    class Filter(ctypes.Structure):
        _fields_=[('code',ctypes.c_ushort),('jt',ctypes.c_ubyte),('jf',ctypes.c_ubyte),('k',ctypes.c_uint32)]
    class Program(ctypes.Structure):
        _fields_=[('len',ctypes.c_ushort),('filter',ctypes.POINTER(Filter))]
    operations=[(0x28,0,0,12),(0x15,0,8,0x0800),(0x30,0,0,23),(0x15,0,6,17),
                (0xb1,0,0,14),(0x48,0,0,16),(0x15,0,3,54321),
                (0x40,0,0,22),(0x15,0,1,0x53544631),(0x06,0,0,65535),(0x06,0,0,0)]
    filters=(Filter*len(operations))(*(Filter(*x) for x in operations))
    program=Program(len(operations),filters)
    lib=ctypes.CDLL(None,use_errno=True)
    lib.setsockopt.argtypes=[ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_void_p,ctypes.c_uint]
    if lib.setsockopt(s.fileno(),socket.SOL_SOCKET,26,ctypes.byref(program),ctypes.sizeof(program)):
        s.close();raise OSError(ctypes.get_errno(),'Cannot attach passive IMU packet filter')
    s.setblocking(False)
    return s


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--fixed-chassis',action='store_true');p.add_argument('--mechanical-center',action='store_true')
    p.add_argument('--same-mount',action='store_true');p.add_argument('--check',action='store_true')
    p.add_argument('--wait-seconds',type=float,default=45)
    a=p.parse_args()
    if not all([a.fixed_chassis,a.mechanical_center,a.same_mount]):p.error('Explicit physical-center declarations required')
    if not sys.platform.startswith('linux') or os.geteuid()!=0:p.error('Run with sudo on S Linux')
    if not 0<a.wait_seconds<=60:p.error('Wait must be 0..60 seconds')
    import fcntl
    lock=os.open('/tmp/steering-terminal-center.lock',os.O_CREAT|os.O_RDWR|os.O_CLOEXEC|os.O_NOFOLLOW,0o600)
    try:
        try:fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
        except BlockingIOError:
            print('AUTO_CENTER_SKIP another terminal is checking; no control change',flush=True);return 0
        config=json.loads((ROOT/'runtime.json').read_text())
        if config.get('schema')!='STEER_RUNTIME_LAUNCH_V1' or not config.get('enabled'):
            raise ValueError('Accepted steering runtime profile is required')
        reference=Path(config['environment']['STEERING_REFERENCE_FILE'])
        expected=reference_session(reference)
        if not expected:raise ValueError('Invalid physical reference; inspect configuration')
        last_pid=None;stable_since=0;next_pid_poll=0
        print('AUTO_CENTER_WAIT keep steering physically centered and stationary until confirmation completes',flush=True)
        with receive_socket() as s:
            deadline=time.monotonic()+a.wait_seconds
            while time.monotonic()<deadline:
                if not select.select([s],[],[],.25)[0]:continue
                current=decode_packet(s.recv(65535))
                if current is None:continue
                # Never re-zero an already bound session merely because another
                # terminal was opened while the mechanism was turned.
                # A manual confirmation may have completed in another shell
                # while this helper was waiting for normal system startup.
                latest=json.loads((ROOT/'runtime.json').read_text())
                reference=Path(latest['environment']['STEERING_REFERENCE_FILE'])
                if reference_session(reference)==current:
                    print('AUTO_CENTER_SKIP already confirmed M session=%u; no controller restart'%current,flush=True);return 0
                if a.check:
                    print('AUTO_CENTER_CHECK new_M_session=%u reference_session=%u would_confirm=1 no_actuation=1'%(current,expected),flush=True);return 0
                now=time.monotonic()
                if now<next_pid_poll:continue
                next_pid_poll=now+.25
                ids=subprocess.run(['pgrep','-x','vehicle_control'],capture_output=True,text=True).stdout.split()
                if len(ids)>1:raise ValueError('Multiple vehicle_control processes')
                pid=ids[0] if ids else None
                if pid!=last_pid:last_pid=pid;stable_since=now
                # make run checks its new FSPI PID after three seconds. Wait
                # until that check has finished before replacing this process.
                if pid and now-stable_since>=4:break
            else:
                print('AUTO_CENTER_DEFER telemetry/controller not ready; no zero changed. Open another terminal to retry.',flush=True);return 3
        command=[sys.executable,str(ROOT/'tools/calibration/confirm_center.py'),
                 '--fixed-chassis','--mechanical-center','--same-mount']
        result=subprocess.run(command,capture_output=True,text=True,timeout=20)
        if result.returncode:
            print(result.stdout+result.stderr,flush=True);return result.returncode
        config=json.loads((ROOT/'runtime.json').read_text())
        new=reference_session(Path(config['environment']['STEERING_REFERENCE_FILE']))
        print('AUTO_CENTER_CONFIRMED M session=%s normal control resumed; log follows'%new,flush=True)
        print(result.stdout,flush=True)
        return 0
    finally:os.close(lock)


if __name__=='__main__':
    try:raise SystemExit(main())
    except (OSError,ValueError,RuntimeError,subprocess.SubprocessError,KeyError) as e:
        print('AUTO_CENTER_FAILED',str(e),file=sys.stderr);raise SystemExit(1)
