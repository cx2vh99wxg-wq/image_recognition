"""Independent fixed-chassis goals through the production C controller.

Does not fit the resulting trials. Checks a second settled angle after each
native goal has released, so delayed movement cannot turn an early pass into
an unnoticed failure. A failed SPI/timing/session operation is never replayed.
"""
from pathlib import Path
import argparse,json,os,signal,subprocess,time
from steering_bench import Telemetry,baseline_of,status,fields

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--fixed-chassis',action='store_true',required=True)
    for key in ['reference','model','goal-tool','output']:p.add_argument('--'+key,required=True)
    p.add_argument('--repeats',type=int,default=3);p.add_argument('--turn-tolerance-deg',type=float,default=3);a=p.parse_args()
    if os.name=='nt' or not 1<=a.repeats<=5:p.error('Linux bench only; 1..5 repeats')
    if not .1<=a.turn_tolerance_deg<=3:p.error('turn tolerance must be 0.1..3deg')
    dest=Path(a.output);dest.mkdir(exist_ok=False,parents=True)
    magic,session,zero,declared=Path(a.reference).read_text().split()
    if magic!='STEER_PHYSICAL_REFERENCE_V1' or declared!='1':raise ValueError('operator physical reference required')
    session=int(session);zero=float(zero);records=[];index=0;child=None
    def save(state):
        report=dict(schema='STEER_NATIVE_GOAL_ACCEPTANCE_V1',state=state,session=session,zero_raw_deg=zero,
            target_tolerance_deg=a.turn_tolerance_deg,center_tolerance_deg=3,records=records,model_not_modified=True,
            all_passed=bool(records) and all(r.get('pass',False) for r in records))
        temp=dest/'progress.tmp';temp.write_text(json.dumps(report,indent=2));temp.replace(dest/'progress.json')
    def observe(prefix):
        monitor=Telemetry(prefix,session,zero,18)
        try:monitor.collect(1.,allow_initial=True);return baseline_of(monitor.samples[-60:],zero)
        finally:monitor.close()
    def interrupted(*_):raise KeyboardInterrupt('operator cancelled')
    signal.signal(signal.SIGTERM,interrupted);signal.signal(signal.SIGINT,interrupted)
    try:
        status();save('RUNNING')
        for repeat in range(a.repeats):
            for target in [12,0,-12,0,15,0,-15,0]:
                index+=1;prefix=dest/('%03d_goal_%s'%(index,str(target).replace('-','negative')))
                before=observe(Path(str(prefix)+'_before'));tolerance=3 if target==0 else a.turn_tolerance_deg
                env=dict(os.environ,STEERING_PRIORITY='10',STEERING_MAX_CORRECTION_PULSES='20',
                    STEERING_TARGET_TOLERANCE_DEG=str(a.turn_tolerance_deg),STEERING_CENTER_TOLERANCE_DEG='3')
                args=[a.goal_tool,'--backend','fspi','--fixed-chassis','--model',a.model,'--reference',a.reference,
                      '--target-deg',str(target),'--tolerance-deg',str(tolerance)]
                start=time.monotonic()
                with prefix.with_suffix('.console.log').open('x') as log:
                    child=subprocess.Popen(args,env=env,stdout=log,stderr=subprocess.STDOUT,start_new_session=True)
                    try:code=child.wait(timeout=28)
                    except BaseException:
                        child.send_signal(signal.SIGTERM);child.wait(timeout=4);raise
                    finally:child=None
                lines=prefix.with_suffix('.console.log').read_text(errors='replace').splitlines()
                result=[fields(s) for s in lines if s.startswith('GOAL_RESULT ')]
                if not result:raise RuntimeError('no native result; no automatic pulse replay')
                f=result[-1]
                if f['state_name'] not in ['SETTLED','RESOLUTION_LIMIT']:raise RuntimeError('native controller fault/cancel: '+str(f))
                after=observe(Path(str(prefix)+'_after'))
                r=dict(repeat=repeat,target_deg=target,tolerance_deg=tolerance,before_deg=before,after_deg=after,
                    error_deg=target-after,controller_status=f['state_name'],exit_code=code,
                    elapsed_s=time.monotonic()-start,pass_=code==0 and f['state_name']=='SETTLED' and abs(target-after)<=tolerance)
                r['pass']=r.pop('pass_');records.append(r);prefix.with_suffix('.json').write_text(json.dumps(r,indent=2))
                save('RUNNING');print('NATIVE_GOAL '+json.dumps(r),flush=True)
        save('COMPLETE');status();print('NATIVE_GOALS_COMPLETE',len(records),sum(r['pass'] for r in records),flush=True)
    except BaseException:
        if child and child.poll() is None:child.send_signal(signal.SIGTERM);child.wait(timeout=4)
        save('STOPPED');raise
if __name__=='__main__':main()
