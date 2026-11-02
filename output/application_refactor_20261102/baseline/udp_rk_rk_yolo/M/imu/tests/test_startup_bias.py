"""Synthetic fixtures exercise startup validation; they are not measurements."""
from pathlib import Path
import importlib.util,math,tempfile,json
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

good=rows();report,holdout=boot.validate_recording(good,[0,0,1],[-1,-2,.7])
assert report['old_residual_norm_dps']>.1 and len(holdout)==600
assert report['axis']==[0,0,1] and not report['physical_center_established'] and not report['axis_changed']
assert max(abs(a-b) for a,b in zip(report['bias'],[-.88,-1.86,.62]))<.001
for reason in ['simulated','session','gap','gyro','tilt','bias_drift','bad_prior']:
    bad=rows();old=[-1,-2,.7]
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
    if reason=='bad_prior':old=[0,0,0]
    try:boot.validate_recording(bad,[0,0,1],old)
    except ValueError:pass
    else:raise AssertionError('unsafe startup accepted: '+reason)
with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
    p=Path(d)/'current.cal';boot.atomic_write(p,b'first');boot.atomic_write(p,b'second');assert p.read_bytes()==b'second'
unit=(ROOT/'feedback/m-steering-imu.service').read_text()
assert 'tools/start_angle_monitor.py' in unit and '/tmp/' not in unit
print('PASS startup bias: cold residual recovery, independent holdout split, session/gap/simulation/motion/drift rejection, unchanged axis/physical zero, atomic persistence and persistent service entry')
