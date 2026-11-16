"""Launch the S-side controller with steering gated by the HMI.

execve keeps the PID used by S/Makefile. The C runtime keeps automatic steering
off until it has a valid mechanical reference and a fresh HMI approval.
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
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--check',action='store_true')
    modes=p.add_mutually_exclusive_group()
    modes.add_argument('--hmi-autopilot',dest='hmi_mode',action='store_const',const='--hmi-autopilot')
    modes.add_argument('--hmi-switch-test',dest='hmi_mode',action='store_const',
                       const='--hmi-switch-test',help='Test the HMI handshake without steering targets')
    p.set_defaults(hmi_mode='--hmi-autopilot')
    a=p.parse_args()
    root=Path(__file__).resolve().parents[2];config=root/'steering/runtime.json'
    try:env=environment(config,os.environ)
    except (OSError,ValueError,KeyError) as e:
        print('STEER_LAUNCH_REJECTED',str(e),file=sys.stderr)
        if a.check:return 2
        # Keep the brake and display monitors running without an approved reference.
        env={key:value for key,value in os.environ.items() if key not in ALLOWED}
    if a.check:
        print(json.dumps(dict(configuration_present=config.exists(),mode=env.get('STEERING_CONTROL_MODE','hmi-gated-monitor'),no_actuation=True)));return 0
    binary=root/'fspi/build/vehicle_control'
    args=[str(binary),a.hmi_mode]
    os.chdir(binary.parent.parent);os.execve(str(binary),args,env)

if __name__=='__main__':raise SystemExit(main())
