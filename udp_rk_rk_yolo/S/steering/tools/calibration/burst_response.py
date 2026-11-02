"""Real single-action time/angle observations and measured release-tail model.

Long actions are kept separate from the original <=60ms V2 calibration. Delay
is identified on S receive timestamps, so no M/S clock synchronization is
assumed. Guarded early releases retain their actual measured GPIO duration.
"""
from pathlib import Path
import argparse,csv,hashlib,json,math,statistics

SCHEMA='STEER_SINGLE_BURST_RESPONSE_V1'

def observation(path):
    p=Path(path);r=json.loads(p.read_text())
    if not isinstance(r,dict) or r.get('schema')!='STEER_SINGLE_BURST_TRIAL_V1' or r.get('result')!='RECORDED':raise ValueError('not a recorded single burst')
    if not r['memory_locked'] or r['priority']!=10 or r['max_age_ms']>=100 or r['brake_us'] or r['direction'] not in (1,2):raise ValueError('unqualified timing/feedback/mode')
    if not 60<r['max_phase_ms']<=250 or not r['session']:raise ValueError('invalid burst cap/reference')
    if len(r['timings'])!=1:raise ValueError('no physical action')
    t=r['timings'][0];elapsed=t['release_end_ns']-t['enable_end_ns']
    if not 0<elapsed<=r['max_phase_ms']*1000000+2000000:raise ValueError('invalid actual pulse duration')
    if not r['stop_reason'] and not -.05<=(elapsed/1e6-r['first_us']/1000)<=2:raise ValueError('deadline mismatch')
    for k in ['before_deg','after_deg','delta_deg','peak_low_deg','peak_high_deg','zero_raw_deg']:
        if not isinstance(r[k],(int,float)) or not math.isfinite(r[k]):raise ValueError('nonfinite angle')
    if abs(r['after_deg']-r['before_deg']-r['delta_deg'])>.002:raise ValueError('angle difference mismatch')
    if r['before_deg']+r['peak_low_deg']< -18 or r['before_deg']+r['peak_high_deg']>18:raise ValueError('physical envelope exceeded')
    rows=list(csv.DictReader(p.with_suffix('.csv').open()))
    rows=[{k:float(v) for k,v in row.items()} for row in rows]
    before=[z for z in rows if z['host_ns']<=t['release_end_ns']]
    tail=[z for z in rows if z['host_ns']>=t['release_end_ns']]
    delay=None
    if before and tail:
        last=before[-1];gap=(t['release_end_ns']-last['host_ns'])/1e9;sign=1 if r['direction']==1 else -1
        if 0<=gap<.03 and last['rate_dps']*sign>8:
            at_release=last['angle_deg']+last['rate_dps']*gap
            rate=last['rate_dps']*sign;coast=(r['after_deg']-at_release)*sign
            peak=max(0,max((z['angle_deg']-at_release)*sign for z in tail))
            if coast>=0:delay=dict(rate_dps=rate,coast_deg=coast,peak_coast_deg=peak,estimate_ms=coast/rate*1000,guard_basis_ms=peak/rate*1000)
    r.update(file=str(p),sha256=hashlib.sha256(p.read_bytes()).hexdigest(),actual_us=elapsed/1000,tail=delay)
    return r

def fit(paths,width=8):
    observations=[];rejected=[];reference=None
    for p in paths:
        try:
            r=observation(p);ref=[r['session'],r['zero_raw_deg']]
            if reference is not None and ref!=reference:raise ValueError('different physical reference/session')
            reference=ref;observations.append(r)
        except (ValueError,KeyError,TypeError,AttributeError) as e:rejected.append(dict(file=str(p),reason=str(e)))
    groups={}
    for r in observations:
        if r['stop_reason']:continue # 固定时长查表只使用完整标称脉冲。
        zone=math.floor((r['before_deg']+width/2)/width)
        groups.setdefault((zone,r['direction'],r['first_us'],r['priority']),[]).append(r)
    cells=[]
    for (zone,d,us,priority),rs in sorted(groups.items()):
        values=[r['delta_deg'] for r in rs]
        cells.append(dict(zone=zone,zone_low_deg=max(-18,(zone-.5)*width),zone_high_deg=min(18.001,(zone+.5)*width),
            direction=d,first_us=us,priority=priority,n=len(rs),responses_deg=values,mean_deg=statistics.mean(values),
            start_min_deg=min(r['before_deg'] for r in rs),start_max_deg=max(r['before_deg'] for r in rs),
            peak_low_deg=min(r['peak_low_deg'] for r in rs),peak_high_deg=max(r['peak_high_deg'] for r in rs)))
    delay={}
    for d in [1,2]:
        values=[r['tail'] for r in observations if r['direction']==d and r['tail']]
        estimate=statistics.median(z['estimate_ms'] for z in values) if values else None
        guard=math.ceil(max(z['guard_basis_ms'] for z in values)+30) if values else None
        delay[str(d)]=dict(n=len(values),estimate_ms=estimate,guard_ms=guard,observations=values,
                          qualified=len(values)>=3 and 0<=estimate<=200 and estimate<=guard<=500 if values else False)
    return dict(schema=SCHEMA,reference=reference,partition_width_deg=width,cells=cells,release_delay=delay,
        sources=[dict(file=r['file'],sha256=r['sha256']) for r in observations],rejected=rejected,approved_for_runtime=False)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('trials',nargs='+');p.add_argument('--output',required=True);a=p.parse_args()
    paths=sorted({q for x in a.trials for q in (Path(x).glob('B*.json') if Path(x).is_dir() else [Path(x)])})
    model=fit(paths);Path(a.output).write_text(json.dumps(model,indent=2));print(json.dumps(dict(observations=len(model['sources']),cells=len(model['cells']),delay=model['release_delay'])))
if __name__=='__main__':main()
