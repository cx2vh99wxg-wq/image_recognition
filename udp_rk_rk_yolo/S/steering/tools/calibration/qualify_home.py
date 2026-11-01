"""One-time visual acceptance of a completed endpoint home; no steering motion.

This does not declare the current pose to be zero. The zero remains the
measured endpoint midpoint, and fresh feedback must still be within 3 degrees.
"""
import argparse,json,os,signal,subprocess,time
from pathlib import Path
from auto_home import ROOT,atomic_json,start_normal
from confirm_center import collect_center,validate_center


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--result',type=Path,required=True);p.add_argument('--straight-verified',action='store_true')
    a=p.parse_args()
    if not a.straight_verified or os.name=='nt' or os.geteuid()!=0:p.error('S root and one-time visual straight verification required')
    source=a.result.resolve();source.relative_to((ROOT/'logs').resolve())
    record=json.loads(source.read_text())
    if (record.get('schema')!='STEER_ENDPOINT_HOME_V1' or record.get('passed') is not True
        or not record.get('pending_visual_check') or record.get('S_boot_id')!=Path('/proc/sys/kernel/random/boot_id').read_text().strip()):
        raise ValueError('A completed commissioning result from this S boot is required')
    profile_path=ROOT/'homing.json';profile=json.loads(profile_path.read_text())
    if not profile['min_span_deg']<=record['span_deg']<=profile['max_span_deg']:raise ValueError('Invalid commissioning stroke')
    config_path=ROOT/'runtime.json';config=json.loads(config_path.read_text())
    model=Path(config['environment']['STEERING_RESPONSE_TABLE'])
    if model.read_text().splitlines()[0].split()[0]!='STEER_RESPONSE_TABLE_V2':raise ValueError('V2 fine model required')
    import fcntl
    lock=os.open('/tmp/steering-auto-home.lock',os.O_CREAT|os.O_RDWR|os.O_NOFOLLOW|os.O_CLOEXEC,0o600)
    fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
    ids=subprocess.run(['pgrep','-x','vehicle_control'],capture_output=True,text=True).stdout.split()
    if len(ids)>1:raise ValueError('Multiple normal controllers')
    env=dict(os.environ)
    if ids:
        proc=Path('/proc')/ids[0]
        if (proc/'exe').resolve()!=(ROOT.parent/'fspi/build/vehicle_control').resolve():raise ValueError('Unexpected executable')
        env=dict(x.split('=',1) for x in (proc/'environ').read_text().split(chr(0)) if '=' in x)
    out=ROOT/'logs'/('auto_home_qualification_'+str(time.time_ns()));out.mkdir()
    (out/'runtime.before.json').write_bytes(config_path.read_bytes())
    (out/'homing.before.json').write_bytes(profile_path.read_bytes())
    marker=Path('/tmp/steering-bench.active');token='qualify_home pid='+str(os.getpid())+'\n'
    owned=False;paused=False;spi_lock=None
    def stopped(*_):raise KeyboardInterrupt('Qualification interrupted')
    signal.signal(signal.SIGTERM,stopped);signal.signal(signal.SIGINT,stopped)
    try:
        with marker.open('x') as f:f.write(token)
        owned=True
        if ids:
            os.kill(int(ids[0]),signal.SIGTERM);end=time.monotonic()+4
            while (Path('/proc')/ids[0]).exists() and time.monotonic()<end:time.sleep(.02)
        if subprocess.run(['pgrep','-x','vehicle_control'],stdout=subprocess.DEVNULL).returncode!=1:raise ValueError('Controller did not stop')
        paused=True
        spi_lock=os.open('/tmp/steering-actuator.lock',os.O_CREAT|os.O_RDWR|os.O_NOFOLLOW|os.O_CLOEXEC,0o600)
        fcntl.flock(spi_lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
        samples=collect_center(out);session,current=validate_center(samples)
        if session!=record['session'] or abs(current-record['midpoint_raw_deg'])>3:
            raise ValueError('Session changed or steering moved away from the measured midpoint; home again')
        reference=out/'center.reference'
        reference.write_text('STEER_PHYSICAL_REFERENCE_V1 %u %.6f 1\n'%(session,record['midpoint_raw_deg']))
        record.update(pending_visual_check=False,physical_straight_visually_confirmed=True,
            reference_file=str(reference),qualification_sample_count=len(samples),
            qualification_raw_deg=current,commissioning_result=str(source))
        config['environment']['STEERING_REFERENCE_FILE']=str(reference);atomic_json(config_path,config)
        profile.update(enabled=True,qualified=True,commissioned_span_deg=record['span_deg'],
            min_span_deg=max(20,record['span_deg']-3),max_span_deg=min(44,record['span_deg']+3),
            commissioning_result=str(source),qualified_at_ns=time.time_ns(),
            note='Operator confirmed both stops and approximate straight midpoint. Repeat homes must match commissioned full stroke within 3 degrees.')
        atomic_json(profile_path,profile);atomic_json(ROOT/'auto_home.current.json',record)
        atomic_json(out/'qualification.json',record)
    finally:
        if spi_lock is not None:os.close(spi_lock)
        if owned and marker.exists() and marker.read_text()==token:marker.unlink()
        if paused:start_normal(out,env)
        os.close(lock)
    print('AUTO_HOME_QUALIFIED '+json.dumps(record),flush=True)
    return 0


if __name__=='__main__':raise SystemExit(main())
