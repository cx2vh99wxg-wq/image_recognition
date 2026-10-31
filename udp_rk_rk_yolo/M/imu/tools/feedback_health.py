"""Read-only service/sample heartbeat. Never opens SPI, sends controls or resets zero."""
import argparse,json,re,subprocess,time
def snapshot():
    pid=int(subprocess.check_output(['systemctl','show','m-steering-imu','-p','MainPID','--value'],text=True))
    if pid<=0:raise RuntimeError('IMU service not running')
    text=subprocess.check_output(['journalctl','_PID='+str(pid),'-n','8','--no-pager','-o','cat'],text=True)
    lines=[s for s in text.splitlines() if s.startswith('REAL seq=')]
    if not lines:raise RuntimeError('No real sensor heartbeat in service log')
    fields=dict(re.findall(r'(seq|relative_deg|static_hold|hold_changes)=([-\d.]+)',lines[-1]))
    return dict(pid=pid,sequence=int(fields['seq']),relative_deg=float(fields['relative_deg']),
        static_hold=int(fields['static_hold']),hold_changes=int(fields.get('hold_changes',0)))
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--seconds',type=float,default=3);a=p.parse_args()
    if not .5<=a.seconds<=10:p.error('Use0.5..10seconds')
    before=snapshot();start=time.monotonic();time.sleep(a.seconds);after=snapshot()
    delta=(after['sequence']-before['sequence'])&0xffffffff
    good=after['pid']==before['pid'] and 0<delta<0x80000000
    print(json.dumps(dict(status='SERVICE_SAMPLES_ADVANCING' if good else 'SERVICE_OR_CONTINUITY_CHANGED',
        before=before,after=after,observation_seconds=time.monotonic()-start,
        physical_zero_confirmed=False,controller_feedback_freshness_not_measured=True),indent=2))
    return 0 if good else 1
if __name__=='__main__':raise SystemExit(main())
