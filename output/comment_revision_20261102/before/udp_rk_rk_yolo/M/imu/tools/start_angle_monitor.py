"""Start the relative IMU service after a checked stationary bias calibration.

Keep the chassis and sensor stationary for the first ~12 seconds. This
calibrates angular-rate bias, never the mounting axis or mechanical center.
No valid UDP angle is published until the independent holdout replay passes.
"""
from pathlib import Path
import csv,hashlib,json,math,os,re,signal,statistics,subprocess,sys,time
from calibrate import calibrate

SAMPLES=1200

def read_calibration(path):
    p=Path(path).read_text().split()
    if len(p)!=9 or p[0]!='IMUCAL1' or p[1]!='axis' or p[5]!='bias':raise ValueError('invalid base calibration')
    axis=list(map(float,p[2:5]));bias=list(map(float,p[6:9]))
    if not all(math.isfinite(v) for v in axis+bias):raise ValueError('nonfinite calibration')
    return axis,bias

def atomic_write(path,data):
    path=Path(path);path.parent.mkdir(parents=True,exist_ok=True)
    tmp=path.with_name(path.name+'.new.'+str(os.getpid()))
    try:
        with tmp.open('xb') as f:f.write(data);f.flush();os.fsync(f.fileno())
        tmp.replace(path)
        if os.name!='nt':
            fd=os.open(path.parent,os.O_RDONLY|os.O_DIRECTORY)
            try:os.fsync(fd)
            finally:os.close(fd)
    finally:
        if tmp.exists():tmp.unlink()

def validate_recording(rows,axis,old_bias):
    if len(rows)!=SAMPLES:raise ValueError('incomplete startup capture')
    # Validate continuity/session of the entire capture, including warmup.
    calibrate(rows,axis)
    fit,holdout=rows[200:600],rows[600:]
    report=calibrate(fit,axis);check=calibrate(holdout,axis)
    accel_std=[statistics.stdev(float(r['a'+k+'_mps2']) for r in fit+holdout) for k in 'xyz']
    if max(accel_std)>.05 or max(report['gyro_std']+check['gyro_std'])>.12:raise ValueError('sensor moving/vibrating; keep stationary and retry')
    separation=math.sqrt(sum((a-b)**2 for a,b in zip(report['bias'],check['bias'])))
    old_residual=math.sqrt(sum((a-b)**2 for a,b in zip(report['bias'],old_bias)))
    if separation>.08:raise ValueError('bias not stable between fit and independent holdout')
    if old_residual>.5:raise ValueError('bias differs too far from installed calibration; inspect mounting/device')
    if abs(report['temperature_c']-check['temperature_c'])>2:raise ValueError('sensor still warming too quickly')
    report.update(schema='IMU_STARTUP_BIAS_V1',old_bias=old_bias,old_residual_norm_dps=old_residual,
        holdout_bias_separation_dps=separation,accel_axis_std=accel_std,
        fit_samples=len(fit),holdout_samples=len(holdout),total_samples=len(rows),
        startup_stationary_required=True,physical_center_established=False,axis_changed=False)
    return report,holdout

def prepare(raw_path,base_path,directory,replay):
    directory=Path(directory);axis,old=read_calibration(base_path)
    with Path(raw_path).open(newline='') as f:rows=list(csv.DictReader(f))
    report,holdout=validate_recording(rows,axis,old)
    candidate=directory/'candidate.cal';holdfile=directory/'holdout.csv'
    candidate.write_text('IMUCAL1\naxis '+' '.join('%.9g'%v for v in report['axis'])+
        '\nbias '+' '.join('%.9g'%v for v in report['bias'])+'\n')
    with holdfile.open('w',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=list(rows[0]));writer.writeheader();writer.writerows(holdout)
    output=subprocess.check_output([str(replay),str(holdfile),str(candidate)],text=True,timeout=5)
    metrics={k:float(v) for k,v in re.findall(r'(\w+)=([-+\d.]+)',output)}
    if metrics.get('samples')!=600 or metrics.get('post5_hold_fraction',0)<.95 or metrics.get('post5_range_deg',1)>.05:
        raise ValueError('independent stationary replay failed: '+output)
    report.update(holdout_replay=metrics,raw_sha256=hashlib.sha256(Path(raw_path).read_bytes()).hexdigest(),
        base_calibration_sha256=hashlib.sha256(Path(base_path).read_bytes()).hexdigest(),
        calibration_sha256=hashlib.sha256(candidate.read_bytes()).hexdigest(),status='VALIDATED_NO_MECHANICAL_ZERO')
    (directory/'report.json').write_text(json.dumps(report,indent=2))
    return candidate,report

def main(argv=None):
    argv=list(sys.argv[1:] if argv is None else argv)
    if argv==['--help']:print(__doc__);return 0
    if os.name=='nt':raise ValueError('service startup runs on Linux; validation functions are portable')
    if '--stationary-hold' not in argv or '--backend' not in argv or argv[argv.index('--backend')+1]!='fpga':raise ValueError('real FPGA stationary-hold service required')
    if '--zero-at-center' in argv:raise ValueError('rate calibration cannot assert a mechanical center')
    if argv.count('--calibration')!=1:raise ValueError('one explicit base calibration required')
    index=argv.index('--calibration')+1;base=Path(argv[index])
    root=Path(__file__).resolve().parents[1]
    directory=root/'calibration/startup_runs'/('%d_%d'%(time.time_ns(),os.getpid()));directory.mkdir(parents=True)
    raw=directory/'raw.csv';child=None
    def stopped(*_):
        if child and child.poll() is None:
            child.send_signal(signal.SIGTERM);child.wait(timeout=5)
        raise SystemExit(130)
    signal.signal(signal.SIGTERM,stopped);signal.signal(signal.SIGINT,stopped)
    print('IMU_STARTUP_CALIBRATING stationary_required=1 seconds=12 udp_valid=0 mechanical_zero=0',flush=True)
    with (directory/'capture.log').open('x') as log:
        child=subprocess.Popen([str(root/'build/imu_dump'),'--backend','fpga','--samples',str(SAMPLES),
            '--interval-ms','1','--print-every','100','--wait-device','--csv',str(raw)],stdout=log,stderr=subprocess.STDOUT)
        try:code=child.wait(timeout=30)
        except BaseException:
            if child.poll() is None:child.send_signal(signal.SIGTERM);child.wait(timeout=5)
            raise
        finally:child=None
    if code:raise ValueError('startup raw capture failed; telemetry withheld')
    candidate,report=prepare(raw,base,directory,root/'build/replay_stationary')
    current=root/'calibration/startup_bias.current.cal'
    atomic_write(current,candidate.read_bytes());atomic_write(current.with_suffix('.cal.json'),json.dumps(report,indent=2).encode())
    argv[index]=str(current)
    print('IMU_STARTUP_READY bias='+str(report['bias'])+' validated_hold='+str(report['holdout_replay']['post5_hold_fraction'])+
        ' report='+str(directory/'report.json')+' mechanical_zero=0',flush=True)
    os.execv(str(root/'build/angle_monitor'),[str(root/'build/angle_monitor'),*argv])

if __name__=='__main__':
    try:raise SystemExit(main())
    except (OSError,ValueError,IndexError,subprocess.SubprocessError) as error:
        print('IMU_STARTUP_REJECTED '+str(error)+'; no uncalibrated valid UDP angle published',file=sys.stderr,flush=True)
        raise SystemExit(1)
