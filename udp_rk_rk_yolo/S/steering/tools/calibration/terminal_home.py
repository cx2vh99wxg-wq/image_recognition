"""Start real endpoint homing once per S boot / M telemetry session.

Passive packet capture does not steal the controller's UDP socket. The normal
startup sequence is retained, and an unconfirmed reference blocks automatic
steering while the helper waits for startup ownership to become available.
"""
import argparse
import json
import os
from pathlib import Path
import select
import subprocess
import sys
import time
from terminal_center import decode_packet,receive_socket,reference_session
from auto_home import atomic_json

ROOT=Path(__file__).resolve().parents[2]


def already_homed(receipt,boot,session,reference):
    try:
        return (receipt.get('schema')=='STEER_ENDPOINT_HOME_V1' and receipt.get('passed') is True
            and receipt.get('S_boot_id')==boot and receipt.get('session')==session
            and receipt.get('physical_straight_visually_confirmed') is True
            and Path(receipt['reference_file'])==reference and reference_session(reference)==session)
    except (KeyError,TypeError,ValueError):return False


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--fixed-chassis',action='store_true');p.add_argument('--endstops-verified',action='store_true')
    p.add_argument('--check',action='store_true');p.add_argument('--wait-seconds',type=float,default=45)
    a=p.parse_args()
    if not a.fixed_chassis or not a.endstops_verified:p.error('Stationary chassis and verified stops required')
    if not sys.platform.startswith('linux') or os.geteuid()!=0:p.error('Run as root on S')
    if not 0<a.wait_seconds<=60:p.error('Wait must be 0..60 seconds')
    import fcntl
    lock=os.open('/tmp/steering-terminal-center.lock',os.O_CREAT|os.O_RDWR|os.O_CLOEXEC|os.O_NOFOLLOW,0o600)
    try:
        try:fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
        except BlockingIOError:
            print('AUTO_HOME_SKIP another terminal helper is active',flush=True);return 0
        profile=json.loads((ROOT/'homing.json').read_text())
        if not all(profile.get(k) for k in ('enabled','qualified','mechanical_endstops_verified')):
            print('AUTO_HOME_DISABLED endpoint commissioning and straight verification required; no zero declared',flush=True);return 3
        boot=Path('/proc/sys/kernel/random/boot_id').read_text().strip()
        config_path=ROOT/'runtime.json';config=json.loads(config_path.read_text())
        if config.get('schema')!='STEER_RUNTIME_LAUNCH_V1' or not config.get('enabled'):
            raise ValueError('Accepted steering runtime profile is required')
        try:receipt=json.loads((ROOT/'auto_home.current.json').read_text())
        except (OSError,ValueError):receipt={}
        original_reference=Path(config['environment']['STEERING_REFERENCE_FILE'])
        # S 可能重启而 M 会话仍保留；前台 make run 可使能自动转向前，
        # 先使上一轮 S 启动的参考零点失效。
        blocked=False
        def block_reference(session):
            nonlocal blocked
            if a.check or blocked:return
            pending=ROOT/'logs'/('auto_home_startup_%s.reference'%boot)
            pending.write_text('STEER_PHYSICAL_REFERENCE_V1 %u 0 0\n'%session)
            latest=json.loads(config_path.read_text())
            latest['environment']['STEERING_REFERENCE_FILE']=str(pending)
            atomic_json(config_path,latest);blocked=True
        if receipt.get('S_boot_id')!=boot:block_reference(1)
        print('AUTO_HOME_WAIT keep chassis stationary and motion keys released; waiting for real IMU/startup',flush=True)
        last_pid=None;stable_since=0;next_pid_poll=0
        with receive_socket() as receiver:
            deadline=time.monotonic()+a.wait_seconds
            while time.monotonic()<deadline:
                if not select.select([receiver],[],[],.25)[0]:continue
                session=decode_packet(receiver.recv(65535))
                if session is None:continue
                latest=json.loads(config_path.read_text())
                reference=Path(latest['environment']['STEERING_REFERENCE_FILE'])
                if already_homed(receipt,boot,session,reference):
                    print('AUTO_HOME_SKIP verified current S boot and M session=%u; no pulses'%session,flush=True);return 0
                if a.check:
                    print('AUTO_HOME_CHECK new_home_required=1 M_session=%u no_actuation=1'%session,flush=True);return 0
                block_reference(session)
                now=time.monotonic()
                if now<next_pid_poll:continue
                next_pid_poll=now+.25
                ids=subprocess.run(['pgrep','-x','vehicle_control'],capture_output=True,text=True).stdout.split()
                if len(ids)>1:raise ValueError('Multiple normal FSPI owners')
                pid=ids[0] if ids else None
                if pid!=last_pid:last_pid=pid;stable_since=now
                # 保留前台 make run 三秒后的 PID 检查。
                if pid and now-stable_since>=4:break
            else:
                print('AUTO_HOME_DEFER no fresh IMU/controller; automatic steering remains unconfirmed',flush=True);return 3
        command=[sys.executable,str(ROOT/'tools/calibration/auto_home.py'),
                 '--session',str(session),'--fixed-chassis','--endstops-verified']
        child=subprocess.Popen(command)
        try:return child.wait(timeout=125)
        finally:
            if child.poll() is None:
                child.terminate();child.wait(timeout=8)
    finally:os.close(lock)


if __name__=='__main__':
    try:raise SystemExit(main())
    except (OSError,ValueError,RuntimeError,KeyError,subprocess.SubprocessError) as e:
        print('AUTO_HOME_STARTUP_FAILED',str(e),flush=True);raise SystemExit(1)
