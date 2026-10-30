"""Summarize a real recording; expected final angle is an external reference."""
import argparse,csv,json,math,statistics
from pathlib import Path

def analyze(rows,expected_final=None):
    if len(rows)<2:raise ValueError("At least two samples required")
    t=[int(r['sample_time_ns'])/1e9 for r in rows]
    angle=[float(r['relative_deg']) for r in rows]
    if any(not math.isfinite(x) for x in angle):raise ValueError("Use angle_monitor CSV")
    duration=t[-1]-t[0]
    if duration<=0:raise ValueError("Invalid timestamps")
    result={'samples':len(rows),'duration_s':duration,'effective_hz':(len(rows)-1)/duration,
      'sequence_gaps':sum(int(r.get('seq_gap',0)) for r in rows),
      'max_age_ms':max(int(r.get('source_age_ms',0)) for r in rows),
      'start_deg':angle[0],'final_deg':angle[-1],
      'net_drift_deg':angle[-1]-angle[0],
      'net_drift_deg_per_minute':(angle[-1]-angle[0])*60/duration,
      'min_deg':min(angle),'max_deg':max(angle),
      'simulated':any(int(r['flags'])&4 for r in rows),
      'physical_accuracy_verified':False}
    if expected_final is not None:
        result.update(expected_final_deg=expected_final,final_error_deg=angle[-1]-expected_final,
          reference_note='Expected angle supplied by operator; independent physical reference still required')
    return result

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--input',type=Path,required=True)
    p.add_argument('--expected-final-deg',type=float);p.add_argument('--output',type=Path)
    a=p.parse_args()
    with a.input.open(newline='',encoding='utf-8-sig') as f:r=analyze(list(csv.DictReader(f)),a.expected_final_deg)
    text=json.dumps(r,indent=2);print(text)
    if a.output:
        with a.output.open('x',encoding='utf-8') as f:f.write(text+'\n')
