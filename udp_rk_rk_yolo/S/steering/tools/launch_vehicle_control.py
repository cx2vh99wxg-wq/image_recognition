"""Optional accepted steering profile for the existing FSPI launch commands.

Without runtime.json the original environment/behavior is retained. execve
keeps the PID used by S/Makefile. Reference/session validity remains enforced
by the C runtime; an expired reference never falls back to long turn pulses.
"""
from pathlib import Path
import argparse,json,os,sys

ALLOWED={'STEERING_CONTROL_MODE','STEERING_RESPONSE_TABLE','STEERING_REFERENCE_FILE',
 'STEERING_PRIORITY','STEERING_TARGET_TOLERANCE_DEG','STEERING_CENTER_TOLERANCE_DEG',
 'STEERING_LANE_TARGET_LIMIT_DEG','STEERING_CURVE_GAIN','STEERING_MAX_CORRECTION_PULSES',
 'STEERING_FEEDBACK_PORT'}

def environment(config,original):
    result=dict(original)
    if not config.exists():return result
    profile=json.loads(config.read_text())
    if profile.get('schema')!='STEER_RUNTIME_LAUNCH_V1' or type(profile.get('enabled'))!=bool:raise ValueError('invalid steering launch profile')
    if not profile['enabled']:return result
    values=profile.get('environment',{})
    if not isinstance(values,dict) or not set(values)<=ALLOWED or not all(isinstance(v,str) for v in values.values()):raise ValueError('unexpected launch key/value')
    if values.get('STEERING_CONTROL_MODE')!='closed-loop' or not all(values.get(k) for k in ['STEERING_RESPONSE_TABLE','STEERING_REFERENCE_FILE']):raise ValueError('explicit closed-loop table/reference required')
    for key in ['STEERING_RESPONSE_TABLE','STEERING_REFERENCE_FILE']:
        if not Path(values[key]).is_absolute() or not Path(values[key]).is_file():raise ValueError('missing absolute '+key)
    result.update(values);return result

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');a=p.parse_args()
    root=Path(__file__).resolve().parents[2];config=root/'steering/runtime.json'
    try:env=environment(config,os.environ)
    except (OSError,ValueError,KeyError) as e:print('STEER_LAUNCH_REJECTED',str(e),file=sys.stderr);return 2
    if a.check:
        print(json.dumps(dict(configuration_present=config.exists(),mode=env.get('STEERING_CONTROL_MODE','legacy'),no_actuation=True)));return 0
    binary=root/'fspi/build/vehicle_control'
    os.chdir(binary.parent.parent);os.execve(str(binary),[str(binary)],env)

if __name__=='__main__':raise SystemExit(main())
