"""Empirical pulse-response table. Retain no-op samples; do not force a line fit."""
import argparse
from collections import defaultdict
import hashlib
import json
import math
from pathlib import Path
import re
import statistics
from steering_bench import timing_valid

SCHEMA = 'STEER_BENCH_RESPONSE_V1'


def reference_matches(model,session,zero_raw):
    binding=model.get('reference_binding')
    if binding is None:
        return model.get('observed_sessions')==[session]
    known=set(binding.get('verified_same_physical_zero_sessions',[]))
    return (binding.get('schema')=='STEER_BENCH_REFERENCE_V1' and
        binding.get('user_same_physical_zero_confirmed') is True and
        binding.get('active_session')==session and session in known and
        set(model.get('observed_sessions',[])).issubset(known) and
        math.isfinite(zero_raw) and abs(binding.get('zero_raw_deg',math.inf)-zero_raw)<.000001)


def fit(paths):
    groups = defaultdict(list)
    excluded = []
    records = []
    for path in paths:
        data = path.read_bytes()
        r = json.loads(data)
        if 'trial' not in r or 'requested_us' not in r:
            continue
        pulses = r.get('pulse_records', [])
        if r.get('result') != 'RECORDED' or len(pulses) != 1 or pulses[0]['result'] != 'COMMAND_COMPLETED':
            excluded.append(path.name)
            continue
        if not timing_valid(r['requested_us'],float(pulses[0]['software_elapsed_ms'])):
            excluded.append(dict(file=path.name,reason='software timing outlier'))
            continue
        sign = 1 if r['direction']=='left' else -1
        response = r['delta_deg']*sign
        if not math.isfinite(response) or response < -.4:
            excluded.append(path.name)
            continue
        record = dict(trial=r['trial'],direction=r['direction'],duration_us=r['requested_us'],
                      priority=r.get('actuator_realtime_priority',0),response_deg=response,
                      before_deg=r['before_deg'],after_deg=r['after_deg'],
                      actual_ms=float(pulses[0]['software_elapsed_ms']),
                      session=r['reference_session'],launch_ns=r['launch_local_ns'],
                      sha256=hashlib.sha256(data).hexdigest())
        groups[(record['direction'],record['duration_us'],record['priority'])].append(record)
        records.append(record)
    cells=[]
    for (direction,duration,priority), rows in sorted(groups.items()):
        values=[r['response_deg'] for r in rows]
        cells.append(dict(direction=direction,duration_us=duration,priority=priority,n=len(rows),
            moved=sum(v>.15 for v in values),
            mean_deg=statistics.mean(values),median_deg=statistics.median(values),
            min_deg=min(values),max_deg=max(values),
            responses_deg=values,
            start_min_deg=min(r['before_deg'] for r in rows),start_max_deg=max(r['before_deg'] for r in rows),
            actual_min_ms=min(r['actual_ms'] for r in rows),actual_max_ms=max(r['actual_ms'] for r in rows)))
    return dict(schema=SCHEMA,cells=cells,records=sorted(records,key=lambda r:r['launch_ns']),
                excluded=excluded,observed_sessions=sorted({r['session'] for r in records}),
                scope='Fixed chassis; user marked relative zero; no road control or mechanical zero granted',
                exact_single_valued_time_angle_function_established=False)


def choose(model,current,target,tolerance=1,min_samples=2,priority=0,
           angle_limit=10,target_limit=8,request_limit_us=20000):
    if model.get('schema') != SCHEMA:
        raise ValueError('Unknown model schema')
    if not all(math.isfinite(v) for v in [current,target,tolerance,angle_limit,target_limit]) or not 0<target_limit<=angle_limit<=18 or not 1<=request_limit_us<=60000 or abs(current)>angle_limit or abs(target)>target_limit or not .1<=tolerance<=3:
        raise ValueError('Outside bench target/tolerance envelope')
    error=target-current
    if abs(error)<=tolerance:
        return dict(status='REACHED',error_deg=error)
    sign=1 if error>0 else -1
    direction='left' if sign>0 else 'right'
    ranked=[]
    enough=False
    for c in model['cells']:
        if c['direction']!=direction or c['priority']!=priority or c['n']<min_samples or c['duration_us']>request_limit_us:
            continue
        if not c['start_min_deg']-2<=current<=c['start_max_deg']+2:
            continue
        enough=True
        if c['moved']==0 or c['min_deg']<-.15 or c['max_deg']>2*angle_limit:
            continue
        # Zero response is allowed but remains a no-progress event. The worst
        # observed nonzero response must not increase the residual materially.
        worst=max(abs(error-sign*v) for v in [max(0,c['min_deg']),c['max_deg']])
        values=c.get('responses_deg') or [r['response_deg'] for r in model.get('records',[])
            if (r['direction'],r['duration_us'],r['priority'])==(direction,c['duration_us'],priority)]
        if not values:values=[c['min_deg'],c['max_deg']]
        # E[|remaining error|], not |error - E[motion]|: a 0/6-degree
        # actuator cannot be mistaken for a precise 3-degree actuator.
        expected=statistics.mean(abs(error-sign*v) for v in values)
        guard=min(2*angle_limit,max(1,c['max_deg']*1.2+.3))
        if abs(current+sign*guard)>angle_limit or worst>abs(error)+.15 or expected>=abs(error)-.15:
            continue
        ranked.append((worst,expected,c['duration_us'],c,guard))
    if not ranked:
        return dict(status='RESOLUTION_LIMIT' if enough else 'INSUFFICIENT_MODEL',error_deg=error)
    _,_,_,cell,guard=min(ranked,key=lambda item:item[:3])
    return dict(status='PULSE',direction=direction,duration_us=cell['duration_us'],
                guard_step_deg=guard,error_deg=error,predicted_mean_deg=cell['mean_deg'],
                observed_range_deg=[cell['min_deg'],cell['max_deg']],samples=cell['n'])


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--logs',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--reference-binding',type=Path,
                   help='Explicit user-confirmed common physical zero; never inferred across sessions')
    args=p.parse_args()
    # A trials fit the model; T goal trials stay out as independent validation.
    paths=[v for v in args.logs.glob('*.json')
           if re.match(r'^A\d+_',v.name) and not v.name.endswith('.reservation.json')]
    model=fit(paths)
    if args.reference_binding:
        model['reference_binding']=json.loads(args.reference_binding.read_text())
        binding=model['reference_binding']
        if not reference_matches(model,binding.get('active_session'),binding.get('zero_raw_deg',math.inf)):
            raise ValueError('Reference binding does not cover the measured sessions')
    with args.output.open('x') as f:
        json.dump(model,f,indent=2)
    print(json.dumps(dict(records=len(model['records']),cells=model['cells'])),flush=True)


if __name__=='__main__':
    main()
