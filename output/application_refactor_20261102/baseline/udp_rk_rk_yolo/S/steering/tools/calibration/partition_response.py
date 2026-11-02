"""Fit current-angle conditioned actions; no hardware access or extrapolation.

V2 is deliberately separate from the V1 single-pulse table. Opposite braking
has a transient trajectory and must never be reduced to its final net angle.
"""
from pathlib import Path
import argparse,hashlib,json,math,statistics

SCHEMA='STEER_PARTITION_RESPONSE_V2'

def checked(path):
    path=Path(path);r=json.loads(path.read_text())
    if not isinstance(r,dict) or r.get('schema')!='STEER_ACTION_TRIAL_V2' or r.get('result')!='RECORDED':
        raise ValueError('unrecorded or wrong action schema')
    numeric=['zero_raw_deg','before_deg','after_deg','delta_deg','peak_low_deg','peak_high_deg']
    if any(not isinstance(r.get(k),(int,float)) or not math.isfinite(r[k]) for k in numeric):
        raise ValueError('non-finite observation')
    if not r['session'] or r['priority'] not in range(31) or not r['memory_locked']:
        raise ValueError('unknown reference or scheduling')
    if r['max_age_ms']>=100 or r['samples']<100 or abs(r['after_deg']-r['before_deg']-r['delta_deg'])>.002:
        raise ValueError('stale or incomplete samples')
    timings=r['timings'];n=2 if r['brake_us'] else 1
    if len(timings)!=n or r['direction'] not in (1,2):raise ValueError('missing physical phase')
    for i,t in enumerate(timings):
        duration=r['brake_us'] if i else r['first_us']
        direction=3-r['direction'] if i else r['direction']
        if not 0<duration<=60000 or t['duration_us']!=duration or t['direction']!=direction:
            raise ValueError('phase identity mismatch')
        b,e,rb,re=(t[k] for k in ['enable_begin_ns','enable_end_ns','release_begin_ns','release_end_ns'])
        if not 0<b<=e<=rb<=re:raise ValueError('invalid physical timestamps')
        actual=re-e
        if actual<duration*1000-50000 or actual>duration*1000+2000000:
            raise ValueError('pulse timing outside -50us/+2ms')
        if i and b-timings[0]['release_end_ns']<r['gap_us']*1000:
            raise ValueError('overlapping or short release gap')
    if r['peak_low_deg']>min(0,r['delta_deg'])+.01 or r['peak_high_deg']<max(0,r['delta_deg'])-.01:
        raise ValueError('missing transient extrema')
    if r['before_deg']+r['peak_low_deg']< -18 or r['before_deg']+r['peak_high_deg']>18:
        raise ValueError('outside characterized envelope')
    r['file']=str(path);r['sha256']=hashlib.sha256(path.read_bytes()).hexdigest();return r

def fit(paths,width=3.):
    if not .5<=width<=6:raise ValueError('invalid partition width')
    groups={};accepted=[];rejected=[];reference=None
    for p in paths:
        try:
            r=checked(p);ref=(r['session'],r['zero_raw_deg'])
            if reference is not None and ref!=reference:raise ValueError('different physical reference/session')
            reference=ref
            zone=math.floor((r['before_deg']+width/2)/width)
            key=(zone,r['direction'],r['first_us'],r['brake_us'],r['gap_us'],r['priority'])
            groups.setdefault(key,[]).append(r);accepted.append(dict(file=r['file'],sha256=r['sha256']))
        except (ValueError,KeyError,TypeError) as e:rejected.append(dict(file=str(p),reason=str(e)))
    cells=[]
    for (zone,d,first,brake,gap,priority),rs in sorted(groups.items()):
        deltas=[r['delta_deg'] for r in rs]
        cells.append(dict(zone=zone,zone_low_deg=(zone-.5)*width,zone_high_deg=(zone+.5)*width,
            direction=d,first_us=first,brake_us=brake,gap_us=gap,priority=priority,n=len(rs),
            start_min_deg=min(r['before_deg'] for r in rs),start_max_deg=max(r['before_deg'] for r in rs),
            mean_deg=statistics.mean(deltas),stddev_deg=statistics.stdev(deltas) if len(rs)>1 else None,
            responses_deg=deltas,peak_low_deg=min(r['peak_low_deg'] for r in rs),peak_high_deg=max(r['peak_high_deg'] for r in rs)))
    return dict(schema=SCHEMA,reference=reference,partition_width_deg=width,cells=cells,sources=accepted,rejected=rejected,
                qualified_for_road_control=False)

def choose(model,current,target,tolerance=1.,limit=18.,priority=10,minimum_samples=3):
    if model['schema']!=SCHEMA or not all(map(math.isfinite,[current,target,tolerance,limit])) or not 0<limit<=18:
        raise ValueError('invalid model/angle limit')
    if abs(current)>limit or abs(target)>limit:raise ValueError('target outside envelope')
    error=target-current
    if abs(error)<=tolerance:return None
    candidates=[]
    for c in model['cells']:
        if c['priority']!=priority or c['n']<minimum_samples:continue
        # Both the named partition and actual observed starts must support use.
        if not c['zone_low_deg']<=current<c['zone_high_deg']:continue
        if not c['start_min_deg']-.75<=current<=c['start_max_deg']+.75:continue
        low=min(0,c['peak_low_deg'])*1.2-.3;high=max(0,c['peak_high_deg'])*1.2+.3
        if current+low< -limit or current+high>limit:continue
        outcomes=c['responses_deg'];worst=max(abs(error-v) for v in outcomes)
        expected=statistics.mean(abs(error-v) for v in outcomes)
        # No interpolation through dead zones, hidden removal of zero moves or
        # accepting a fine correction that can worsen the remaining error.
        if worst>abs(error)+.15 or expected>=abs(error)-.15:continue
        family=[f for f in model['cells'] if f['direction']==c['direction'] and f['first_us']==c['first_us'] and
                f['brake_us']==c['brake_us'] and f['gap_us']==c['gap_us'] and f['priority']==priority]
        family_values=[v*(1 if c['direction']==1 else -1) for f in family for v in f['responses_deg']]
        miss_rate=(sum(v<=.15 for v in family_values)+1)/(len(family_values)+2)
        score=expected+miss_rate*min(abs(error),6)
        candidates.append((score,worst,c['first_us']+c['brake_us'],c))
    if not candidates:raise ValueError('RESOLUTION_OR_COVERAGE_LIMIT')
    return min(candidates,key=lambda c:c[:3])[3]

def choose_reposition(model,current,target,tolerance=1.,limit=18.,priority=10,minimum_samples=3):
    """One measured inward step may open a safe outward approach window.

    This is a two-step distribution search, not an unbounded oscillation.
    Every possible first response retains the full transient envelope and is
    either followed by a locally supported choice or assigned its full error.
    """
    if model['schema']!=SCHEMA or not all(map(math.isfinite,[current,target,tolerance,limit])) or not 0<limit<=18 or abs(current)>limit or abs(target)>limit:
        raise ValueError('invalid model/angle limit')
    error=target-current
    if not tolerance<abs(error)<=4:raise ValueError('NO_BOUNDED_REPOSITION')
    candidates=[]
    for c in model['cells']:
        if c['priority']!=priority or c['n']<minimum_samples or c['brake_us']:continue
        if c['direction']!=(2 if error>0 else 1):continue
        if not c['zone_low_deg']<=current<c['zone_high_deg']:continue
        if not c['start_min_deg']-.75<=current<=c['start_max_deg']+.75:continue
        sign=1 if c['direction']==1 else -1
        if min(v*sign for v in c['responses_deg'])<-.15 or max(v*sign for v in c['responses_deg'])>5:continue
        lo=min(0,c['peak_low_deg'])*1.2-.3;hi=max(0,c['peak_high_deg'])*1.2+.3
        if current+lo< -limit or current+hi>limit:continue
        future=[];planned=0
        for v in c['responses_deg']:
            after=current+v
            try:
                second=choose(model,after,target,tolerance,limit,priority,minimum_samples)
                cost=0. if second is None else statistics.mean(abs(target-after-x) for x in second['responses_deg'])
                planned+=1
            except ValueError:cost=abs(target-after)
            future.append(cost)
        expected=statistics.mean(future)
        if planned*2<len(future) or expected>=abs(error)-.25:continue
        candidates.append((expected,c['first_us'],c))
    if not candidates:raise ValueError('NO_BOUNDED_REPOSITION')
    return dict(min(candidates,key=lambda c:c[:2])[2],_reposition=True)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('trials',nargs='+');p.add_argument('--output',required=True)
    p.add_argument('--width-deg',type=float,default=3.);args=p.parse_args()
    paths=sorted({q for item in args.trials for q in (Path(item).rglob('*.json') if Path(item).is_dir() else [Path(item)])})
    model=fit(paths,args.width_deg);out=Path(args.output);out.parent.mkdir(parents=True,exist_ok=True)
    out.write_text(json.dumps(model,indent=2),encoding='utf-8')
    print('PARTITION_MODEL observations=%d rejected=%d cells=%d road_control_qualified=0'%(len(model['sources']),len(model['rejected']),len(model['cells'])))
if __name__=='__main__':main()
