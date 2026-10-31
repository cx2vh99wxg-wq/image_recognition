"""Explicitly confirm an already straight, fixed steering mechanism after M reboot.

No homing motion, IMU reset, model edits, or calibration pulses. The existing
controller is stopped, fresh real feedback is measured, then normal control
resumes with a reference bound to that one M session.
"""
import argparse
import json
import math
import os
from pathlib import Path
import selectors
import signal
import statistics
import subprocess
import sys
import time

from steering_bench import checked_sample, fields

ROOT=Path(__file__).resolve().parents[2]


def validate_center(samples):
    if len(samples)<60:
        raise ValueError('At least 60 fresh samples required')
    session=samples[0]['session']
    if not session or any(s['session']!=session for s in samples):
        raise ValueError('IMU session changed')
    for old,new in zip(samples,samples[1:]):
        gap=new['rx_local_ns']-old['rx_local_ns']
        if not 0<gap<100000000 or not 0<(new['sequence']-old['sequence'])%0x100000000<0x80000000:
            raise ValueError('Feedback is discontinuous or duplicated')
    if samples[-1]['rx_local_ns']-samples[0]['rx_local_ns']<500000000:
        raise ValueError('Stationary baseline too short')
    angles=[s['raw_relative_deg'] for s in samples]
    if any(not math.isfinite(s['raw_relative_deg']) or not math.isfinite(s['rate_dps']) or
           not 0<=s['source_age_ms']<100 for s in samples):
        raise ValueError('Invalid or expired sample')
    if max(angles)-min(angles)>.4 or any(abs(s['rate_dps'])>2 for s in samples):
        raise ValueError('Keep the sensor and steering completely stationary')
    zero=statistics.median(angles[-60:])
    if abs(zero)>2000:
        raise ValueError('Reference outside supported range')
    return session,zero


def collect_center(folder):
    command=[str(ROOT/'feedback/build/steering_feedback_monitor'),
             '--bind','192.168.100.20','--source','192.168.100.10',
             '--port','54321','--duration-ms','1800','--print-every','1']
    samples=[];session=None;pending=b''
    process=subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    selector=selectors.DefaultSelector();selector.register(process.stdout,selectors.EVENT_READ)
    end=time.monotonic()+5
    try:
        with (folder/'feedback.log').open('x',buffering=1) as log:
            while time.monotonic()<end:
                ready=selector.select(.02)
                if not ready:
                    if process.poll() is not None:break
                    continue
                data=os.read(process.stdout.fileno(),65536)
                if not data:break
                pending+=data
                while b'\n' in pending:
                    raw,pending=pending.split(b'\n',1);line=raw.decode('utf-8',errors='replace')
                    log.write(line+'\n')
                    if line.startswith('SESSION_CHANGE '):raise ValueError('IMU session changed')
                    if line.startswith('STATUS ') and samples and line.split()[1] not in ('REAL_RELATIVE','REAL_ZERO_VALID'):
                        raise ValueError('Feedback became invalid')
                    if line.startswith('RX '):
                        if session is None:session=int(fields(line)['session'])
                        sample=checked_sample(line,session,time.monotonic_ns())
                        if sample:samples.append(sample)
        process.wait(timeout=1)
        if process.returncode:raise RuntimeError('Real feedback monitor failed')
    finally:
        if process.poll() is None:process.send_signal(signal.SIGTERM);process.wait(timeout=2)
        selector.close();process.stdout.close()
    return samples


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixed-chassis',action='store_true')
    parser.add_argument('--mechanical-center',action='store_true')
    parser.add_argument('--same-mount',action='store_true')
    args=parser.parse_args()
    if not all([args.fixed_chassis,args.mechanical_center,args.same_mount]):
        parser.error('Physically center and fix the mechanism; all three declarations are required')
    if not sys.platform.startswith('linux') or os.geteuid()!=0:
        parser.error('Run with sudo on S Linux')
    def interrupted(signum,frame):
        raise KeyboardInterrupt('Center confirmation interrupted')
    signal.signal(signal.SIGINT,interrupted)
    signal.signal(signal.SIGTERM,interrupted)
    import fcntl
    config_path=ROOT/'runtime.json';config=json.loads(config_path.read_text())
    assert config['schema']=='STEER_RUNTIME_LAUNCH_V1' and config['enabled']
    model=Path(config['environment']['STEERING_RESPONSE_TABLE'])
    if model.read_text().splitlines()[0].split()[0] not in ('STEER_RESPONSE_TABLE_V1','STEER_RESPONSE_TABLE_V2'):
        raise ValueError('Session-bound V3 models require a separately reviewed rebind')
    binary=ROOT.parent/'fspi/build/vehicle_control'
    ids=subprocess.run(['pgrep','-x','vehicle_control'],capture_output=True,text=True).stdout.split()
    if len(ids)>1:raise ValueError('Multiple controllers; stop and inspect')
    environment=dict(os.environ)
    if ids:
        proc=Path('/proc')/ids[0]
        if (proc/'exe').resolve()!=binary.resolve():raise ValueError('Unexpected controller executable')
        environment=dict(x.split('=',1) for x in (proc/'environ').read_text().split(chr(0)) if '=' in x)
    folder=ROOT/'logs'/('center_confirm_'+str(time.time_ns()));folder.mkdir()
    marker=Path('/tmp/steering-bench.active');token='confirm_center pid='+str(os.getpid())+'\n'
    marker_created=False;lock=None;resume=False
    try:
        with marker.open('x') as f:f.write(token)
        marker_created=True
        if ids:
            os.kill(int(ids[0]),signal.SIGTERM)
            end=time.monotonic()+4
            while (Path('/proc')/ids[0]).exists() and time.monotonic()<end:time.sleep(.02)
        if subprocess.run(['pgrep','-x','vehicle_control'],stdout=subprocess.DEVNULL).returncode!=1:
            raise RuntimeError('Controller did not stop; no reference changed')
        lock=os.open('/tmp/steering-actuator.lock',os.O_CREAT|os.O_RDWR|os.O_NOFOLLOW,0o600)
        fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
        samples=collect_center(folder);session,zero=validate_center(samples)
        reference=folder/'center.reference'
        reference.write_text('STEER_PHYSICAL_REFERENCE_V1 %u %.6f 1\n'%(session,zero))
        audit=dict(schema='STEER_MANUAL_CENTER_CONFIRM_V1',session=session,zero_raw_deg=zero,
            sample_count=len(samples),same_mount_declared=True,physical_center_declared=True,
            fixed_chassis_declared=True,automatic_homing=False,IMU_reset=False,
            previous_reference=config['environment']['STEERING_REFERENCE_FILE'])
        (folder/'reference.json').write_text(json.dumps(audit,indent=2))
        (folder/'runtime.before.json').write_bytes(config_path.read_bytes())
        config['environment']['STEERING_REFERENCE_FILE']=str(reference)
        temp=config_path.with_name('runtime.json.center-new')
        with temp.open('w') as f:json.dump(config,f,indent=2);f.write('\n');f.flush();os.fsync(f.fileno())
        temp.replace(config_path);resume=True
    finally:
        if lock is not None:os.close(lock)
        if marker_created and marker.read_text()==token:marker.unlink()
    if resume:
        log_path=folder/'production.log'
        with log_path.open('x') as log:
            process=subprocess.Popen(['stdbuf','-oL','python3',str(ROOT/'tools/launch_vehicle_control.py')],
                cwd=ROOT.parent/'fspi',env=environment,stdin=subprocess.DEVNULL,
                stdout=log,stderr=subprocess.STDOUT,start_new_session=True)
        (ROOT.parent/'fspi/fspi.pid').write_text(str(process.pid)+'\n')
        end=time.monotonic()+5
        while '[STEER_REFERENCE]' not in log_path.read_text(errors='replace'):
            if process.poll() is not None:raise RuntimeError('Controller failed; inspect '+str(log_path))
            if time.monotonic()>=end:raise RuntimeError('Reference not accepted; inspect '+str(log_path))
            time.sleep(.05)
        audit.update(controller_pid=process.pid,production_log=str(log_path),normal_control_resumed=True)
        (folder/'reference.json').write_text(json.dumps(audit,indent=2))
        print('STEER_CENTER_CONFIRMED '+json.dumps(audit),flush=True)
    return 0


if __name__=='__main__':
    try:raise SystemExit(main())
    except KeyboardInterrupt:
        print('STEER_CENTER_CONFIRM_CANCELLED; inspect controller state before continuing',file=sys.stderr)
        raise SystemExit(130)
    except (OSError,ValueError,RuntimeError,AssertionError,subprocess.SubprocessError) as e:
        print('STEER_CENTER_CONFIRM_FAILED',str(e),file=sys.stderr);raise SystemExit(1)
