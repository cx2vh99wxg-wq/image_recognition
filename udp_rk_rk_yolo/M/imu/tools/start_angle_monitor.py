"""Wait for an S/R calibration request, then validate a fresh stationary capture."""
from pathlib import Path
import csv,hashlib,json,math,os,re,signal,socket,statistics,struct,subprocess,sys,time
from calibrate import calibrate

SAMPLES=1200
CALIBRATION_PORT = 54322
REQUEST_TIMEOUT_S = 1.0
CAPTURE_TIMEOUT_S = 22.0
REQUEST = struct.Struct("!4sI")

def read_axis(path):
    p=Path(path).read_text().split()
    if len(p)!=9 or p[0]!='IMUCAL1' or p[1]!='axis' or p[5]!='bias':raise ValueError('invalid base calibration')
    axis=list(map(float,p[2:5]))
    if not all(math.isfinite(v) for v in axis) or math.hypot(*axis)<1e-9:
        raise ValueError('invalid installation axis')
    return axis

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

def validate_recording(rows,axis):
    if len(rows)!=SAMPLES:raise ValueError('incomplete startup capture')
    # 连续性和会话检查覆盖整段采集数据，预热阶段也包括在内。
    calibrate(rows,axis)
    fit,holdout=rows[200:600],rows[600:]
    report=calibrate(fit,axis);check=calibrate(holdout,axis)
    accel_std=[statistics.stdev(float(r['a'+k+'_mps2']) for r in fit+holdout) for k in 'xyz']
    if max(accel_std)>.05 or max(report['gyro_std']+check['gyro_std'])>.12:raise ValueError('sensor moving/vibrating; keep stationary and retry')
    separation=math.sqrt(sum((a-b)**2 for a,b in zip(report['bias'],check['bias'])))
    if separation>.08:raise ValueError('bias not stable between fit and independent holdout')
    if abs(report['temperature_c']-check['temperature_c'])>2:raise ValueError('sensor still warming too quickly')
    report.update(schema='IMU_STARTUP_BIAS_V1',
        holdout_bias_separation_dps=separation,accel_axis_std=accel_std,
        fit_samples=len(fit),holdout_samples=len(holdout),total_samples=len(rows),
        startup_stationary_required=True,physical_center_established=False,axis_changed=False)
    return report,holdout

def prepare(raw_path,base_path,directory,replay):
    directory=Path(directory);axis=read_axis(base_path)
    with Path(raw_path).open(newline='') as f:rows=list(csv.DictReader(f))
    report,holdout=validate_recording(rows,axis)
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

def stop_child(child):
    if child is None or child.poll() is not None:
        return
    child.terminate()
    try:
        child.wait(timeout=2)
    except subprocess.TimeoutExpired:
        child.kill()
        child.wait()


def serve(sock, peer, root, argv, index, feedback_port):
    session = 0
    seen = set()
    child = None
    directory = None
    expiry = deadline = 0
    print("IMU_WAIT_S_R_CONFIRMATION", flush=True)
    try:
        while True:
            try:
                packet, source = sock.recvfrom(64)
            except socket.timeout:
                packet, source = b"", None
            now = time.monotonic()
            if directory is not None and (now >= expiry or now >= deadline):
                stop_child(child)
                child = directory = None
                print("IMU_CALIBRATION_ABORTED confirmation or capture timed out", flush=True)
            if source == (peer, feedback_port) and len(packet) == REQUEST.size:
                magic, requested = REQUEST.unpack(packet)
                if magic == b"SCZ1" and requested and requested not in seen:
                    seen.add(requested)
                    stop_child(child)
                    session = requested
                    expiry, deadline = now + REQUEST_TIMEOUT_S, now + CAPTURE_TIMEOUT_S
                    directory = root / "calibration/startup_runs" / f"{time.time_ns()}_{session}"
                    directory.mkdir(parents=True)
                    raw = directory / "raw.csv"
                    with (directory / "capture.log").open("x") as log:
                        child = subprocess.Popen([str(root / "build/imu_dump"),
                            "--backend", "fpga", "--samples", str(SAMPLES), "--interval-ms", "1",
                            "--print-every", "100", "--wait-device", "--csv", str(raw)],
                            stdout=log, stderr=subprocess.STDOUT)
                    print(f"IMU_STARTUP_CALIBRATING session={session} seconds=12", flush=True)
                elif magic == b"SCZ1" and requested == session and directory is not None:
                    expiry = now + REQUEST_TIMEOUT_S

            if child is None or child.poll() is None:
                continue
            if directory is None:
                print("IMU_FEEDBACK_EXITED waiting for a new S/R confirmation", flush=True)
                child = None
                continue
            try:
                if child.returncode:
                    raise ValueError("startup raw capture failed")
                candidate, report = prepare(raw, argv[index], directory, root / "build/replay_stationary")
                if time.monotonic() >= expiry:
                    raise ValueError("parking confirmation expired during validation")
                current = root / "calibration/startup_bias.current.cal"
                atomic_write(current, candidate.read_bytes())
                atomic_write(current.with_suffix(".cal.json"), json.dumps(report, indent=2).encode())
                if time.monotonic() >= expiry:
                    raise ValueError("parking confirmation expired before publishing feedback")
                args = argv.copy()
                args[index] = str(candidate)
                env = dict(os.environ, STEERING_IMU_SESSION=str(session))
                child = subprocess.Popen([str(root / "build/angle_monitor"), *args], env=env)
                print(f"IMU_STARTUP_READY session={session} bias={report['bias']} mechanical_zero=0", flush=True)
            except (OSError, ValueError, subprocess.SubprocessError) as error:
                print(f"IMU_STARTUP_REJECTED {error}; no valid angle published", file=sys.stderr, flush=True)
                child = None
            directory = None
    finally:
        stop_child(child)


def main(argv=None):
    argv=list(sys.argv[1:] if argv is None else argv)
    if argv==['--help']:print(__doc__);return 0
    if os.name=='nt':raise ValueError('service startup runs on Linux; validation functions are portable')
    if '--stationary-hold' not in argv or '--backend' not in argv or argv[argv.index('--backend')+1]!='fpga':raise ValueError('real FPGA stationary-hold service required')
    if '--zero-at-center' in argv:raise ValueError('rate calibration cannot assert a mechanical center')
    if argv.count('--calibration')!=1:raise ValueError('one explicit base calibration required')
    index=argv.index('--calibration')+1
    read_axis(argv[index])
    root=Path(__file__).resolve().parents[1]
    peer=argv[argv.index("--send-to")+1]
    feedback_port=int(argv[argv.index("--port")+1]) if "--port" in argv else 54321
    def stopped(*_):
        raise SystemExit(130)
    signal.signal(signal.SIGTERM,stopped);signal.signal(signal.SIGINT,stopped)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind(("", CALIBRATION_PORT))
        sock.settimeout(.1)
        serve(sock, peer, root, argv, index, feedback_port)

if __name__=='__main__':
    try:raise SystemExit(main())
    except (OSError,ValueError,IndexError,subprocess.SubprocessError) as error:
        print('IMU_STARTUP_REJECTED '+str(error)+'; no uncalibrated valid UDP angle published',file=sys.stderr,flush=True)
        raise SystemExit(1)
