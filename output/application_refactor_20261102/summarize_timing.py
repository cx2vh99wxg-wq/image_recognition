"""Summarize compiler timing against the previously delivered clock pairs."""
import json
from pathlib import Path
import re

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
projects=json.loads((OUT/'firmware_candidates.json').read_text(encoding='utf-8'))
results={}
for candidate in projects:
    board=candidate['board']
    base_file=ROOT/('tmp/speed_right_clone_20261029/timing_verified.json' if board=='S' else 'tmp/imu_reliability_20261101/timing_check.json')
    base=json.loads(base_file.read_text(encoding='utf-8'))
    report=Path(candidate['directory'])/'report_timing'/(candidate['top_module']+'.rtr')
    text=report.read_text(errors='replace')
    result={}
    for name,label in [('Setup_Slow','Setup Summary(Slow Corner):'),('Hold_Slow','Hold Summary(Slow Corner):'),('Setup_Fast','Setup Summary(Fast Corner):'),('Hold_Fast','Hold Summary(Fast Corner):')]:
        block=text.split(label)[1].split('===')[0]
        rows=[[a,b,float(w)] for a,b,w,tns,n,total in re.findall(r'(\S+)\s+(\S+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+(\d+)\s+(\d+)',block)]
        assert len(rows)>=24,(board,name,len(rows))
        failed=[x for x in rows if x[2]<0]
        allowed={tuple(x[:2]) for x in base[name].get('fail_pairs',base[name].get('failed_pairs',[]))}
        newly_failed=[x for x in failed if tuple(x[:2]) not in allowed]
        result[name]=dict(clock_pair_rows=len(rows),failed_pairs=failed,new_failed_pairs=newly_failed)
    result['no_new_failing_clock_pair']=all(not item['new_failed_pairs'] for key,item in result.items() if isinstance(item,dict))
    result['all_hold_pairs_clean']=not result['Hold_Slow']['failed_pairs'] and not result['Hold_Fast']['failed_pairs']
    result['full_design_timing_clean']=all(not item['failed_pairs'] for key,item in result.items() if isinstance(item,dict))
    result['note']='Existing asynchronous camera/DDR setup failing pairs remain; generation success is not an all-timing-pass claim.'
    results[board]=result
(OUT/'compiler_timing_summary.json').write_text(json.dumps(results,indent=2),encoding='utf-8')
for board,item in results.items():print('COMPILER_TIMING',board,'new_pairs=',not item['no_new_failing_clock_pair'],'hold_clean=',item['all_hold_pairs_clean'],'full_clean=',item['full_design_timing_clean'])
