import copy,importlib.util,json,tempfile
from pathlib import Path
p=Path(__file__).resolve().parents[2]/'tools/calibration/partition_response.py'
spec=importlib.util.spec_from_file_location('partition_response',p);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
base=dict(schema='STEER_ACTION_TRIAL_V2',session=123,zero_raw_deg=.018,priority=10,memory_locked=True,
          result='RECORDED',direction=1,first_us=30000,brake_us=0,gap_us=2000,before_deg=0.,after_deg=2.,delta_deg=2.,
          peak_low_deg=0.,peak_high_deg=2.1,samples=220,max_age_ms=3,
          timings=[dict(direction=1,duration_us=30000,enable_begin_ns=1000000,enable_end_ns=1100000,
                        release_begin_ns=31100000,release_end_ns=31200000)])
scratch=Path(__file__).resolve().parents[2]/'build';scratch.mkdir(exist_ok=True)
with tempfile.TemporaryDirectory(dir=scratch) as t:
    root=Path(t);paths=[]
    for i,delta in enumerate([1.8,2.,2.2]):
        r=copy.deepcopy(base);r['after_deg']=r['delta_deg']=delta;r['peak_high_deg']=delta+.1
        q=root/('%d.json'%i);q.write_text(json.dumps(r));paths.append(q)
    model=m.fit(paths);assert len(model['cells'])==1 and len(model['sources'])==3
    assert m.choose(model,0,2)['first_us']==30000
    for current,target in [(3,5),(0,.5),(0,1),(0,-2),(17,18)]:
        try:r=m.choose(model,current,target)
        except ValueError:continue
        assert r is None and abs(target-current)<=1
    try:m.choose(model,0,2,priority=0)
    except ValueError:pass
    else:raise AssertionError('priority mixed')
    # The final net +2 is unsafe at +14 if an opposite action first peaks +5.
    transient=copy.deepcopy(model);c=transient['cells'][0]
    c.update(zone_low_deg=13.5,zone_high_deg=16.5,start_min_deg=14,start_max_deg=14,peak_high_deg=5)
    try:m.choose(transient,14,16)
    except ValueError:pass
    else:raise AssertionError('transient was reduced to final net angle')
    for field,value in [('session',124),('max_age_ms',100),('memory_locked',False)]:
        r=copy.deepcopy(base);r[field]=value;q=root/'bad.json';q.write_text(json.dumps(r))
        result=m.fit(paths+[q]);assert len(result['rejected'])==1
    r=copy.deepcopy(base);r['timings'][0]['release_end_ns']=33100001
    q=root/'bad.json';q.write_text(json.dumps(r));assert len(m.fit([q])['rejected'])==1
    inward=dict(zone_low_deg=-15,zone_high_deg=-9,start_min_deg=-12.5,start_max_deg=-11.5,
        direction=1,first_us=19000,brake_us=0,gap_us=2000,priority=10,n=3,responses_deg=[2,2,2],peak_low_deg=0,peak_high_deg=2.1)
    outward=dict(inward,direction=2,first_us=25000,start_min_deg=-11,start_max_deg=-9.1,responses_deg=[-4,-4,-4],peak_low_deg=-5.5,peak_high_deg=0)
    plan=dict(schema=m.SCHEMA,cells=[inward,outward])
    try:m.choose(plan,-12,-15,2)
    except ValueError:pass
    else:raise AssertionError('unsafe direct outward step')
    assert m.choose_reposition(plan,-12,-15,2)['_reposition']
    assert m.choose(plan,-10,-15,2)['direction']==2
    short=dict(inward,direction=1,first_us=19000,zone_low_deg=-3,zone_high_deg=3,start_min_deg=-1,start_max_deg=1,
               responses_deg=[3,3,3],peak_low_deg=0,peak_high_deg=3.2)
    misses=dict(short,zone_low_deg=3,zone_high_deg=9,start_min_deg=4,start_max_deg=6,n=9,responses_deg=[0]*9,peak_high_deg=0)
    reliable=dict(short,first_us=30000,responses_deg=[1.5,2,3])
    assert m.choose(dict(schema=m.SCHEMA,cells=[short,misses,reliable]),0,10)['first_us']==30000
print('PASS partition model: session/scheduling/timing isolation, actual start support, transient envelope and resolution refusal')
