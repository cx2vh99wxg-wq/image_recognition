"""Synthetic fixtures exercise startup validation; they are not measurements."""
from pathlib import Path
import importlib.util,math,tempfile,json
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1]
import sys
sys.path.insert(0,str(ROOT/'tools'))
import start_angle_monitor as boot

def rows():
    values=[]
    for i in range(1200):
        v=dict(session='7',seq=str(i+1),sample_time_ns=str(10**9+i*10**7),flags='1',temp_c='28')
        for n,k in enumerate('xyz'):
            v['g'+k+'_dps']=str([-1,-2,.7][n]+[.12,.14,-.08][n]+.02*math.sin(i*.31+n))
            v['a'+k+'_mps2']=str((9.80665 if k=='z' else 0)+.01*math.sin(i*.2+n))
        values.append(v)
    return values

good=rows();report,holdout=boot.validate_recording(good,[0,0,1])
assert len(holdout)==600
assert report['axis']==[0,0,1] and not report['physical_center_established'] and not report['axis_changed']
assert max(abs(a-b) for a,b in zip(report['bias'],[-.88,-1.86,.62]))<.001
shifted=rows()
for sample in shifted:sample['gx_dps']=str(float(sample['gx_dps'])+1.0)
assert abs(boot.validate_recording(shifted,[0,0,1])[0]['bias'][0]-.12)<.001
for reason in ['simulated','session','gap','gyro','tilt','bias_drift']:
    bad=rows()
    if reason=='simulated':bad[600]['flags']='5'
    if reason=='session':bad[800]['session']='8'
    if reason=='gap':
        for v in bad[800:]:v['sample_time_ns']=str(int(v['sample_time_ns'])+100000000)
    if reason=='gyro':
        for i,v in enumerate(bad):v['gz_dps']=str(.7+(.5 if i%2 else -.5))
    if reason=='tilt':
        for i,v in enumerate(bad):
            a=.08*math.sin(i*.01);v['ax_mps2']=str(9.80665*math.sin(a));v['az_mps2']=str(9.80665*math.cos(a))
    if reason=='bias_drift':
        for v in bad[600:]:v['gy_dps']=str(float(v['gy_dps'])+.12)
    try:boot.validate_recording(bad,[0,0,1])
    except ValueError:pass
    else:raise AssertionError('unsafe startup accepted: '+reason)
with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
    p=Path(d)/'current.cal';boot.atomic_write(p,b'first');boot.atomic_write(p,b'second');assert p.read_bytes()==b'second'

    class FakeChild:
        def __init__(self, returncode=None):
            self.returncode = returncode
        def poll(self):
            return self.returncode
        def terminate(self):
            self.returncode = -15
        def wait(self, timeout):
            return self.returncode

    peer = ("192.168.100.20", 54321)
    request = boot.REQUEST.pack(b"SCZ1", 47)
    retry = boot.REQUEST.pack(b"SCZ1", 48)
    class Events:
        def __init__(self):
            self.items = iter([(request, (peer[0], 1234)), (request, peer), (request, peer),
                               (request, peer), (retry, peer), (retry, peer), (retry, peer)])
        def recvfrom(self, _):
            event = next(self.items)
            if started and len(started) == 1 and received:
                started[0][1].returncode = 0
            received.append(event)
            return event

    baseline = Path(d)/"base.cal"
    boot.atomic_write(baseline,b"IMUCAL1\naxis 0 0 1\nbias 999 999 999\n")
    assert boot.read_axis(baseline) == [0, 0, 1]
    started, received = [], []
    def launch(args, **options):
        child = FakeChild()
        started.append((args, child, options))
        return child
    def prepare(raw, base, directory, replay):
        assert base == str(baseline)
        candidate = directory/"candidate.cal"
        candidate.write_text("IMUCAL1\naxis 0 0 1\nbias 0.2 0 0\n")
        return candidate, {"bias": [.2, 0, 0]}
    with patch.object(boot.subprocess, "Popen", side_effect=launch), \
         patch.object(boot, "prepare", side_effect=prepare), \
         patch.object(boot.time, "monotonic", side_effect=[0, .1, .2, .25, .3, .4, .5, 1.6, 1.7]):
        try:
            boot.serve(Events(), peer[0], Path(d), [str(baseline)], 0, peer[1])
        except StopIteration:
            pass
    assert [Path(s[0][0]).name for s in started] == ["imu_dump", "angle_monitor", "imu_dump"]
    assert started[1][2]["env"]["STEERING_IMU_SESSION"] == "47"
    assert started[1][1].returncode == -15 and started[2][1].returncode == -15
unit=(ROOT/'feedback/m-steering-imu.service').read_text()
assert 'tools/start_angle_monitor.py' in unit and '/tmp/' not in unit
print('PASS startup bias: independent holdout, stable new biases, packet source, session binding, lease expiry and duplicate rejection')
