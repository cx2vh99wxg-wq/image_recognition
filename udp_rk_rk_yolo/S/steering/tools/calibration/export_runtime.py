"""Validated V1 JSON -> compact portable C table. No fitting/extrapolation."""
import argparse,hashlib,json,math
from pathlib import Path
from response_model import SCHEMA

def export(model):
    if model.get('schema')=='STEER_PARTITION_RESPONSE_V2':return export_partition(model)
    if model.get('schema')!=SCHEMA or model.get('simulation_only') or model.get('invalidated'):raise ValueError('Unknown, simulated or invalidated response schema')
    cells=model.get('cells',[])
    if not 1<=len(cells)<=128:raise ValueError('Runtime requires 1..128 measured cells')
    lines=['STEER_RESPONSE_TABLE_V1 '+str(len(cells))];seen=set()
    for c in cells:
        d={'left':1,'right':2}.get(c['direction']);n=c['n'];us=c['duration_us'];priority=c['priority']
        key=(d,us,priority);values=c.get('responses_deg',[])
        numbers=[c[k] for k in ['min_deg','max_deg','mean_deg','start_min_deg','start_max_deg']]
        if d is None or key in seen or any(type(v)!=int for v in [n,us,priority,c['moved']]) or not 1<=us<=60000 or not 0<=priority<=30 or not 1<=n<=64 or not 0<=c['moved']<=n:
            raise ValueError('Invalid cell identity/count/request')
        if len(values)!=n or not all(math.isfinite(v) for v in numbers+values):raise ValueError('Missing/nonfinite measured responses')
        if min(values)<-.4 or max(values)>40 or abs(min(values)-numbers[0])>.002 or abs(max(values)-numbers[1])>.002 or abs(sum(values)/n-numbers[2])>.002:
            raise ValueError('Cell statistics differ from empirical observations')
        if not -20<=numbers[3]<=numbers[4]<=20:raise ValueError('Start angle outside mechanical envelope')
        seen.add(key)
        lines.append(' '.join(map(str,[d,us,priority,n,c['moved'],*numbers,len(values),*values])))
    return '\n'.join(lines)+'\n'

def export_partition(model):
    from partition_response import fit
    if model.get('invalidated') or model.get('simulation_only'):raise ValueError('Invalidated/simulated partition model')
    sources=model.get('sources',[])
    if not sources:raise ValueError('No real source observations')
    for s in sources:
        if hashlib.sha256(Path(s['file']).read_bytes()).hexdigest()!=s['sha256']:raise ValueError('Changed source observation')
    reproduced=fit([s['file'] for s in sources],model['partition_width_deg'])
    if reproduced['cells']!=model['cells'] or reproduced['reference']!=tuple(model['reference']):
        raise ValueError('Partition statistics do not reproduce from real trials')
    cells=[c for c in model['cells'] if not c['brake_us']]
    if not 1<=len(cells)<=128:raise ValueError('Runtime supports 1..128 single-pulse partitions')
    lines=['STEER_RESPONSE_TABLE_V2 '+str(len(cells))]
    for c in cells:
        d=c['direction'];values=[v*(1 if d==1 else -1) for v in c['responses_deg']];n=len(values)
        if not 1<=n<=64 or min(values)<-.4 or max(values)>36:raise ValueError('Invalid signed response')
        moved=sum(v>.15 for v in values)
        fields=[d,c['first_us'],c['priority'],n,moved,min(values),max(values),sum(values)/n,
                c['start_min_deg'],c['start_max_deg'],n,c['zone_low_deg'],c['zone_high_deg'],
                min(0,c['peak_low_deg']),max(0,c['peak_high_deg']),*values]
        lines.append(' '.join(map(str,fields)))
    return '\n'.join(lines)+'\n'

def export_burst(model,profile):
    """Opt-in bench profile. Export does not mark it accepted for road use."""
    from burst_response import observation
    if model.get('schema')!='STEER_PARTITION_RESPONSE_V2' or profile.get('schema')!='STEER_BURST_PROFILE_V1':raise ValueError('V2 fine model and explicit burst profile required')
    if list(profile['reference'])!=list(model['reference']):raise ValueError('different physical references')
    maximum=profile['maximum_us'];s,zero=profile['reference']
    if type(maximum)!=int or not 60000<maximum<=250000 or maximum%1000:raise ValueError('invalid burst deadline')
    measured=[]
    for source in profile['sources']:
        if hashlib.sha256(Path(source['file']).read_bytes()).hexdigest()!=source['sha256']:raise ValueError('changed burst observation')
        r=observation(source['file'])
        if [r['session'],r['zero_raw_deg']]!=[s,zero]:raise ValueError('changed burst reference')
        measured.append(r)
    for d in [1,2]:
        ds=[r for r in measured if r['direction']==d]
        if len(ds)<3 or not any(r['max_phase_ms']*1000>=maximum for r in ds):raise ValueError('insufficient direction/deadline evidence')
        delay=profile['directions'][str(d)]
        ms,guard,cap,peak=[delay[k] for k in ['coast_ms','guard_ms','coast_cap_deg','guard_cap_deg']]
        if type(ms)!=int or type(guard)!=int or not 0<=ms<=200 or not ms<=guard<=500 or not 0<cap<=peak<=18:raise ValueError('invalid predictive parameters')
        tails=[r['tail'] for r in ds if r['tail']]
        if len(tails)<3 or peak<max(r['peak_coast_deg'] for r in tails)*1.2+.5:raise ValueError('guard smaller than measured peak + margin')
        if not any(r.get('target_set') and r.get('stop_coast_ms')==ms and r.get('stop_guard_ms')==guard and
            r.get('stop_coast_cap_deg')==cap and r.get('stop_guard_cap_deg')==peak for r in ds):raise ValueError('profile not physically exercised')
    text=export_partition(model);lines=text.splitlines();lines[0]=lines[0].replace('_V2','_V3')
    left,right=profile['directions']['1'],profile['directions']['2']
    lines.insert(1,' '.join(map(str,['BURST',maximum,s,zero,left['coast_ms'],left['guard_ms'],right['coast_ms'],right['guard_ms'],
        left['coast_cap_deg'],left['guard_cap_deg'],right['coast_cap_deg'],right['guard_cap_deg']])))
    return '\n'.join(lines)+'\n'

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--model',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--burst-profile',type=Path);a=p.parse_args()
    data=a.model.read_bytes();model=json.loads(data);text=export_burst(model,json.loads(a.burst_profile.read_text())) if a.burst_profile else export(model)
    with a.output.open('x',encoding='ascii',newline='\n') as f:f.write(text)
    a.output.with_suffix(a.output.suffix+'.json').write_text(json.dumps({'schema':'STEER_RUNTIME_EXPORT_V1','source_schema':model['schema'],
        'source_json_sha256':hashlib.sha256(data).hexdigest(),'runtime_table_sha256':hashlib.sha256(text.encode()).hexdigest()},indent=2))
if __name__=='__main__':main()
