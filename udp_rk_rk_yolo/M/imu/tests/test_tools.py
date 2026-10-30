"""Synthetic fixtures test algorithms only; no fixture is a hardware measurement."""
import csv,importlib.util,json,math,os,signal,socket,struct,subprocess,tempfile,zlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('calibrate',ROOT/'tools/calibrate.py')
cal=importlib.util.module_from_spec(spec);spec.loader.exec_module(cal)
def rows(rate=.03,sim=False):
    return [dict(session='1',seq=str(i+1),sample_time_ns=str(10**9+i*10**7),flags=str(5 if sim else 1),
      ax_mps2='0',ay_mps2='0',az_mps2='9.80665',gx_dps='0.01',gy_dps='-0.02',
      gz_dps=str(rate),temp_c='25') for i in range(300)]
good=rows();result=cal.calibrate(good,[0,0,-2])
assert result['axis']==[0,0,-1] and abs(result['bias'][2]-.03)<1e-9
for bad in [rows(sim=True),rows(rate=11),good[:50]]:
    try:cal.calibrate(bad,[0,0,1])
    except ValueError:pass
    else:raise AssertionError('Invalid calibration accepted')
vibrating=rows()
for i,r in enumerate(vibrating):r['gz_dps']=str((-1)**i)
try:cal.calibrate(vibrating,[0,0,1])
except ValueError:pass
else:raise AssertionError('Vibrating calibration accepted')
sweep=rows(rate=30.03)
axis=cal.axis_from_left_sweep(sweep,[.01,-.02,.03]);assert abs(axis[2]-1)<1e-6
with tempfile.TemporaryDirectory() as tmp:
    tmp=Path(tmp);csv_path=tmp/'angle.csv'
    sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);sock.bind(('127.0.0.1',0));sock.settimeout(3)
    process=subprocess.Popen([str(ROOT/'build/angle_monitor'),'--samples','20','--interval-ms','10',
      '--mock-rate-dps','30','--csv',str(csv_path),'--send-to','127.0.0.1','--port',str(sock.getsockname()[1])],
      stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
    packets=[sock.recv(256) for _ in range(20)]
    stdout,stderr=process.communicate(timeout=4)
    assert process.returncode==0,(stdout,stderr)
    for packet in packets:
        assert len(packet)==48 and packet[:4]==b'STF1'
        assert zlib.crc32(packet[:44])==struct.unpack('!I',packet[44:])[0]
        assert struct.unpack('!I',packet[16:20])[0]==5 # VALID + SIMULATED, never physical zero
    times=[struct.unpack('!Q',p[32:40])[0] for p in packets]
    angles=[struct.unpack('!i',p[20:24])[0] for p in packets]
    assert all(b>a for a,b in zip(times,times[1:]))
    assert abs(angles[-1]/1000-30*(times[-1]-times[0])/1e9)<.002
    with csv_path.open() as f:logged=list(csv.DictReader(f))
    assert len(logged)==20 and all(r['zero_valid']=='0' for r in logged)
    rejected=subprocess.run([str(ROOT/'build/angle_monitor'),'--zero-at-center','--samples','1'],capture_output=True)
    assert rejected.returncode==2
    refused=subprocess.run([str(ROOT/'build/imu_dump'),'--samples','1','--csv',str(csv_path)],capture_output=True)
    assert refused.returncode==1
if os.name!='nt':
    with socket.socket(socket.AF_INET,socket.SOCK_DGRAM) as sock:
        sock.bind(('127.0.0.1',0));sock.settimeout(3)
        p=subprocess.Popen([str(ROOT/'build/angle_monitor'),'--samples','0','--interval-ms','10',
          '--print-every','1000','--mock-rate-dps','30','--send-to','127.0.0.1',
          '--port',str(sock.getsockname()[1])],stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
        def packet():
            b=sock.recv(256)
            assert len(b)==48 and zlib.crc32(b[:44])==struct.unpack('!I',b[44:])[0]
            return dict(session=struct.unpack('!I',b[8:12])[0],seq=struct.unpack('!I',b[12:16])[0],
              flags=struct.unpack('!I',b[16:20])[0],angle=struct.unpack('!i',b[20:24])[0],
              t=struct.unpack('!Q',b[32:40])[0])
        try:
            for _ in range(60):before=packet()
            assert before['angle']>15000
            for _ in range(2):
                p.send_signal(signal.SIGUSR2)
                for attempt in range(100):
                    after=packet()
                    if after['session']!=before['session']:break
                else:raise AssertionError('No relative-zero acknowledgement')
                assert p.poll() is None and after['angle']==0 and after['flags']==5
                assert after['seq']>before['seq'] and after['t']>before['t']
                for _ in range(15):later=packet()
                assert later['session']==after['session'] and later['angle']>3000 and later['flags']==5
                before=later
        finally:
            if p.poll() is None:p.terminate()
            out,err=p.communicate(timeout=3)
        assert p.returncode==130 and out.count('RELATIVE ZERO set:')==2,(out,err)
print('PASS tools: calibration, UDP, mock isolation, zero arming, CSV protection; runtime relative zero preserves sampling/session semantics and never grants mechanical zero')
