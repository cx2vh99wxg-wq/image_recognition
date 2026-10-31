"""Explicit operator physical-center declaration bound to fresh M telemetry."""
import argparse,json,os,statistics
from pathlib import Path
from steering_bench import Telemetry,baseline_of
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--expected-session',type=int,required=True)
    p.add_argument('--confirm-physical-center',action='store_true',required=True);p.add_argument('--fixed-chassis',action='store_true',required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    if os.name=='nt':p.error('Fresh physical telemetry capture runs on S/Linux')
    if not 0<a.expected_session<=0xffffffff:p.error('Invalid session')
    a.output.parent.mkdir(parents=True,exist_ok=True)
    if a.output.exists() or a.output.with_suffix(a.output.suffix+'.json').exists():p.error('Use a new reference filename')
    monitor=Telemetry(a.output,a.expected_session,0,2000)
    try:
        monitor.collect(1,allow_initial=True);raw=baseline_of(monitor.samples[-60:],0)
        with a.output.open('x',encoding='ascii') as f:f.write('STEER_PHYSICAL_REFERENCE_V1 %d %.9g 1\n'%(a.expected_session,raw))
        a.output.with_suffix(a.output.suffix+'.json').write_text(json.dumps(dict(schema='STEER_PHYSICAL_REFERENCE_V1',session=a.expected_session,zero_raw_deg=raw,
            user_physical_center_confirmed=True,user_fixed_chassis_confirmed=True,samples=len(monitor.samples),mechanical_ZERO_VALID_granted=False),indent=2))
        print('PHYSICAL_REFERENCE_CREATED session=%d zero_raw_deg=%.6f'%(a.expected_session,raw),flush=True)
    finally:monitor.close()
if __name__=='__main__':main()
