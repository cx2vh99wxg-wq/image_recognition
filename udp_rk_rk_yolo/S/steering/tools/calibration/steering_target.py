"""Bounded stage-3 BENCH controller: measured pulse -> settle -> replan.

An exploratory table is not an exact inverse function. Report resolution/no-
progress limits rather than claiming success, extrapolating or oscillating.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import signal
import subprocess
import sys
from steering_bench import ROOT, Telemetry, baseline_of, status
from response_model import choose, reference_matches


def main():
    def interrupted(signum,frame):
        raise KeyboardInterrupt('Target interrupted')
    signal.signal(signal.SIGTERM,interrupted)
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--trial',required=True)
    p.add_argument('--target-deg',type=float,required=True)
    p.add_argument('--tolerance-deg',type=float,default=1)
    p.add_argument('--model',type=Path,required=True)
    p.add_argument('--reference-session',type=int,required=True)
    p.add_argument('--zero-raw-deg',type=float,required=True)
    p.add_argument('--max-pulses',type=int,default=4)
    p.add_argument('--angle-limit-deg',type=float,default=10)
    p.add_argument('--target-limit-deg',type=float,default=8)
    p.add_argument('--request-limit-us',type=int,default=20000)
    args=p.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9_-]{1,60}',args.trial) or not 1<=args.max_pulses<=6:
        p.error('Invalid unique target ID or pulse bound')
    model_bytes=args.model.read_bytes();model=json.loads(model_bytes)
    envelope=dict(angle_limit=args.angle_limit_deg,target_limit=args.target_limit_deg,
                  request_limit_us=args.request_limit_us)
    choose(model,0,args.target_deg,args.tolerance_deg,**envelope)
    if not reference_matches(model,args.reference_session,args.zero_raw_deg):
        p.error('Model/session mismatch; re-establish the measured reference')
    prefix=ROOT/'logs'/args.trial
    report=dict(trial=args.trial,target_deg=args.target_deg,tolerance_deg=args.tolerance_deg,
                reference_session=args.reference_session,zero_raw_deg=args.zero_raw_deg,
                model_sha256=hashlib.sha256(model_bytes).hexdigest(),pulses=[],status='RESERVED',
                angle_limit_deg=args.angle_limit_deg,target_limit_deg=args.target_limit_deg,
                request_limit_us=args.request_limit_us,
                scope='stage3 fixed-chassis bench only; formal vehicle API not enabled')
    with open(str(prefix)+'.reservation.json','x') as f:json.dump(report,f,indent=2)
    monitor=None;child=None
    try:
        status()
        monitor=Telemetry(prefix,args.reference_session,args.zero_raw_deg,args.angle_limit_deg)
        monitor.collect(1,allow_initial=True)
        current=baseline_of(monitor.samples[-60:],args.zero_raw_deg)
        monitor.close();monitor=None
        report['initial_deg']=current
        no_progress=0;reversals=0;previous_direction=None
        for index in range(args.max_pulses+1):
            decision=choose(model,current,args.target_deg,args.tolerance_deg,**envelope)
            report['final_deg']=current;report['error_deg']=args.target_deg-current
            if decision['status']!='PULSE':
                report['status']=decision['status'];break
            if index==args.max_pulses:
                report['status']='PULSE_LIMIT';break
            if previous_direction and decision['direction']!=previous_direction:
                reversals+=1
            if reversals>1:
                report['status']='REVERSAL_LIMIT';break
            pulse_id=args.trial+'_p%02d'%(index+1)
            command=[sys.executable,str(Path(__file__).with_name('steering_bench.py')),
                '--trial',pulse_id,'--direction',decision['direction'],'--duration-us',str(decision['duration_us']),
                '--reference-session',str(args.reference_session),'--zero-raw-deg',str(args.zero_raw_deg),
                '--guard-step-deg',str(decision['guard_step_deg']),
                '--angle-limit-deg',str(args.angle_limit_deg),
                '--request-limit-us',str(args.request_limit_us),
                '--expected-start-deg',str(current)]
            with open(str(ROOT/'logs'/pulse_id)+'.console.log','x') as output:
                child=subprocess.Popen(command,stdout=output,stderr=subprocess.STDOUT)
                code=child.wait()
            pulse=json.loads((ROOT/'logs'/(pulse_id+'.json')).read_text())
            report['pulses'].append(dict(id=pulse_id,decision=decision,result=pulse))
            if code or pulse['result']!='RECORDED':
                report['status']='FEEDBACK_OR_ACTUATOR_FAULT'
                report['last_feedback_deg']=pulse.get('last_observed_deg')
                report['final_deg']=None;report['error_deg']=None
                break
            current=pulse['after_deg'];previous_direction=decision['direction']
            progress=abs(pulse['before_deg']-args.target_deg)-abs(current-args.target_deg)
            no_progress=no_progress+1 if progress<.15 else 0
            report['final_deg']=current;report['error_deg']=args.target_deg-current
            if abs(report['error_deg'])<=args.tolerance_deg:
                report['status']='REACHED';break
            if no_progress>=2:
                report['status']='NO_PROGRESS';break
        status()
    except (Exception,KeyboardInterrupt) as exc:
        report['status']='STOPPED';report['error']=str(exc)
    finally:
        if child and child.poll() is None:
            child.send_signal(signal.SIGTERM)
            try:child.wait(timeout=5)
            except subprocess.TimeoutExpired:report['release_state_unknown']=True
        if monitor:
            try:monitor.close()
            except Exception as exc:report['monitor_cleanup_error']=str(exc)
        with open(str(prefix)+'.json','x') as f:json.dump(report,f,indent=2)
        print('TARGET_SUMMARY='+json.dumps({k:report.get(k) for k in
            ['trial','target_deg','initial_deg','final_deg','error_deg','status','error']}),flush=True)
    return 0 if report['status']=='REACHED' else 1


if __name__=='__main__':
    raise SystemExit(main())
