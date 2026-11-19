"""Read-only service/sample heartbeat. Never opens SPI, sends controls or resets zero."""
import argparse,json,re,subprocess,time
from pathlib import Path

def monitor_pid():
    pid=int(subprocess.check_output(['systemctl','show','m-steering-imu','-p','MainPID','--value'],text=True))
    if pid<=0:raise RuntimeError('IMU service not running')
    expected=Path(__file__).resolve().parents[1]/'build/angle_monitor'
    for process in Path('/proc').iterdir():
        if not process.name.isdecimal():
            continue
        try:
            if (process/'exe').readlink()!=expected:
                continue
            status=(process/'status').read_text()
            if int(process.name)==pid or re.search(r'^PPid:\s*'+str(pid)+r'$',status,re.MULTILINE):
                return int(process.name)
        except (FileNotFoundError, PermissionError, ProcessLookupError):
            continue
    raise RuntimeError('IMU service is waiting for S/R or the angle monitor stopped')

def snapshot():
    pid=monitor_pid()
    text=subprocess.check_output(['journalctl','_PID='+str(pid),'-n','8','--no-pager','-o','cat'],text=True)
    lines=[s for s in text.splitlines() if s.startswith('REAL seq=')]
    if not lines:raise RuntimeError('No real sensor heartbeat in service log')
    fields=dict(re.findall(r'(seq|relative_deg|static_hold|hold_changes)=([-\d.]+)',lines[-1]))
    return dict(pid=pid,sequence=int(fields['seq']),relative_deg=float(fields['relative_deg']),
        static_hold=int(fields['static_hold']),hold_changes=int(fields.get('hold_changes',0)))
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--seconds',type=float,default=3)
    p.add_argument('--pid',action='store_true',help='Print the current angle monitor PID')
    a=p.parse_args()
    if a.pid:
        print(monitor_pid())
        return 0
    if not .5<=a.seconds<=10:p.error('Use0.5..10seconds')
    before=snapshot();start=time.monotonic();time.sleep(a.seconds);after=snapshot()
    delta=(after['sequence']-before['sequence'])&0xffffffff
    good=after['pid']==before['pid'] and 0<delta<0x80000000
    print(json.dumps(dict(status='SERVICE_SAMPLES_ADVANCING' if good else 'SERVICE_OR_CONTINUITY_CHANGED',
        before=before,after=after,observation_seconds=time.monotonic()-start,
        physical_zero_confirmed=False,controller_feedback_freshness_not_measured=True),indent=2))
    return 0 if good else 1
if __name__=='__main__':
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        raise SystemExit(str(error))
