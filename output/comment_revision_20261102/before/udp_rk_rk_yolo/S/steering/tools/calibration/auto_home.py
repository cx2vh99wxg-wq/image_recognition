"""Real endpoint-midpoint homing. Only LEFT/RIGHT contacts are pulsed.

Normal vehicle control is isolated and restored on exit. A failed home leaves
an explicitly unconfirmed reference, so other FSPI policies resume without
granting automatic steering. Commissioning requires one visual straight check.
"""
from pathlib import Path
import argparse,csv,json,os,re,select,selectors,signal,subprocess,time
from homing_core import HomeLimits,HomeError,run_homing
from steering_bench import Telemetry,baseline_of,status_while_receiving,timing_valid,pulse_elapsed_ms
ROOT=Path(__file__).resolve().parents[2]


class HomeTelemetry(Telemetry):
    def __init__(self,prefix,session,origin=None):
        self.samples=[];self.pending=b'';self.session=session;self.zero=origin or 0;self.limit=2000 if origin is None else 50
        self.log=open(str(prefix)+'.receiver.log','x',buffering=1)
        self.process=subprocess.Popen([str(ROOT/'feedback/build/steering_feedback_monitor'),
            '--bind','192.168.100.20','--source','192.168.100.10','--port','54321',
            '--duration-ms','0','--print-every','1','--csv',str(prefix)+'.csv'],
            stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        self.selector=selectors.DefaultSelector();self.selector.register(self.process.stdout,selectors.EVENT_READ)


class HardwareIO:
    def __init__(self,out,session,model):
        self.out=out;self.session=session;self.model=model;self.counter=0;self.monitor_counter=0
        self.child=None;self.monitor=None;self.origin=None;self.after=False;self.start=time.monotonic()
        try:
            self.open_monitor();self.stable();self.origin=baseline_of(self.monitor.samples[-60:],0)
            self.monitor.zero=self.origin;self.monitor.limit=50
        except BaseException:
            self.close()
            raise
    def open_monitor(self):
        self.monitor_counter+=1
        self.monitor=HomeTelemetry(self.out/('feedback_%d'%self.monitor_counter),self.session,self.origin)
    def stable(self):
        if time.monotonic()-self.start>110:raise HomeError('Homing wall-clock deadline')
        self.monitor.collect(.8 if self.after else .65,allow_initial=not self.monitor.samples)
        self.after=False;end=time.monotonic()+3
        while True:
            try:
                value=baseline_of(self.monitor.samples[-60:],0)
                print('AUTO_HOME_STABLE raw_deg=%.3f'%value,flush=True)
                return value
            except RuntimeError as e:
                if str(e) not in ('Insufficient continuous stationary baseline','Steering is not settled'):raise
                if time.monotonic()>end:raise HomeError('Steering did not settle')
                self.monitor.collect(.1)
    def pulse(self,direction,us):
        status_while_receiving(self.monitor)
        self.counter+=1;prefix=self.out/('pulse_%02d'%self.counter)
        command=['chrt','-f','10',str(ROOT/'actuator/build/steering_step_fspi'),
                 '--backend','fspi','--direction','left' if direction>0 else 'right',
                 '--duration-us',str(us),'--max-pulse-ms','30','--csv',str(prefix)+'.csv']
        with open(str(prefix)+'.log','x') as log:
            self.child=subprocess.Popen(command,stdout=log,stderr=subprocess.STDOUT)
            end=time.monotonic()+3
            try:
                while self.child.poll() is None:
                    if time.monotonic()>end:raise HomeError('Actuator did not finish')
                    self.monitor.collect(.04,actuator=self.child)
                if self.child.returncode:raise HomeError('Pulse rejected; never treat it as an endpoint')
            finally:
                if self.child.poll() is None:self.child.send_signal(signal.SIGTERM);self.child.wait(timeout=3)
                self.child=None
        with open(str(prefix)+'.csv') as f:records=list(csv.DictReader(f))
        if len(records)!=1 or not timing_valid(us,pulse_elapsed_ms(records[0])):
            raise HomeError('Actuator timing outside the measured pulse envelope')
        if 'requested_contact_low=YES' not in Path(str(prefix)+'.log').read_text(errors='replace'):
            raise HomeError('FPGA did not accept the requested contact')
        self.after=True
        print('AUTO_HOME_PULSE direction=%s requested_us=%d'%('left' if direction>0 else 'right',us),flush=True)
    def center(self,mid,tolerance,budget):
        reference=self.out/'candidate.reference'
        reference.write_text('STEER_PHYSICAL_REFERENCE_V1 %u %.6f 1\n'%(self.session,mid))
        # This reference is only for the isolated goal tool until verification.
        self.monitor.close();self.monitor=None
        tool=ROOT/'actuator/build/steering_goal_fspi'
        env=dict(os.environ,STEERING_PRIORITY='10',STEERING_MAX_CORRECTION_PULSES=str(min(6,budget//60000)))
        path=self.out/'fine_center.log'
        with path.open('x') as log:
            self.child=subprocess.Popen([str(tool),'--backend','fspi','--fixed-chassis',
                '--model',str(self.model),'--reference',str(reference),
                '--target-deg','0','--tolerance-deg',str(tolerance)],env=env,stdout=log,stderr=subprocess.STDOUT)
            try:code=self.child.wait(timeout=28)
            finally:
                if self.child.poll() is None:self.child.send_signal(signal.SIGTERM);self.child.wait(timeout=3)
                self.child=None
        text=path.read_text(errors='replace')
        match=re.search(r'GOAL_RESULT .*measured_deg=([-0-9.]+).*state_name=SETTLED',text)
        requests=[int(n) for n in re.findall(r'\[STEER_PULSE_BEGIN\].*duration_us=(\d+)',text)]
        if code or not match:raise HomeError('Measured fine center failed; reference not installed')
        self.open_monitor();self.after=True
        return mid+float(match.group(1)),requests
    def release(self):
        if self.child is not None and self.child.poll() is None:
            self.child.send_signal(signal.SIGTERM);self.child.wait(timeout=3)
        result=subprocess.run([str(ROOT/'actuator/build/steering_step_fspi'),
            '--backend','fspi','--direction','release'],capture_output=True,text=True,timeout=4)
        (self.out/'release.log').write_text(result.stdout+result.stderr)
        if result.returncode:raise HomeError('Contact release failed')
    def close(self):
        if self.monitor is not None:self.monitor.close();self.monitor=None


def atomic_json(path,data):
    temp=path.with_name(path.name+'.new')
    with temp.open('w') as f:json.dump(data,f,indent=2);f.write('\n');f.flush();os.fsync(f.fileno())
    temp.replace(path)


def start_normal(out,environment):
    with (out/'production.log').open('x') as log:
        process=subprocess.Popen(['stdbuf','-oL','python3',str(ROOT/'tools/launch_vehicle_control.py')],
            cwd=ROOT.parent/'fspi',env=environment,stdin=subprocess.DEVNULL,
            stdout=log,stderr=subprocess.STDOUT,start_new_session=True)
    (ROOT.parent/'fspi/fspi.pid').write_text(str(process.pid)+'\n')
    return process.pid


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--session',type=int);p.add_argument('--fixed-chassis',action='store_true')
    p.add_argument('--endstops-verified',action='store_true');p.add_argument('--commission',action='store_true')
    p.add_argument('--diagnose-single',choices=['left','right'],help='One isolated 30ms contact; never qualifies homing')
    a=p.parse_args()
    if not a.fixed_chassis or not a.endstops_verified or (a.session is not None and not 0<a.session<=0xffffffff):
        p.error('Fixed chassis, verified mechanical stops and fresh session required')
    if os.name=='nt' or os.geteuid()!=0:p.error('Run on S Linux as root')
    if a.diagnose_single and not a.commission:p.error('A diagnostic pulse cannot enable production homing')
    if a.session is None:
        from terminal_center import receive_socket,decode_packet
        with receive_socket() as receiver:
            end=time.monotonic()+3
            while time.monotonic()<end:
                if select.select([receiver],[],[],.1)[0]:
                    a.session=decode_packet(receiver.recv(65535))
                    if a.session:break
            else:raise HomeError('No fresh real M telemetry session')
    import fcntl
    lock=os.open('/tmp/steering-auto-home.lock',os.O_CREAT|os.O_RDWR|os.O_CLOEXEC|os.O_NOFOLLOW,0o600)
    fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
    profile=json.loads((ROOT/'homing.json').read_text())
    if not profile.get('mechanical_endstops_verified'):raise HomeError('Mechanical stops not verified')
    if not a.commission and not (profile.get('enabled') and profile.get('qualified')):
        raise HomeError('One-time endpoint commissioning is required')
    limits=HomeLimits(min_span_deg=profile['min_span_deg'],max_span_deg=profile['max_span_deg'])
    limits.validate()
    config_path=ROOT/'runtime.json';config=json.loads(config_path.read_text())
    model=Path(config['environment']['STEERING_RESPONSE_TABLE'])
    if model.read_text().splitlines()[0].split()[0]!='STEER_RESPONSE_TABLE_V2':raise HomeError('Qualified V2 fine model required')
    ids=subprocess.run(['pgrep','-x','vehicle_control'],capture_output=True,text=True).stdout.split()
    if len(ids)>1:raise HomeError('Multiple FSPI owners')
    env=dict(os.environ)
    if ids:
        proc=Path('/proc')/ids[0]
        if (proc/'exe').resolve()!=(ROOT.parent/'fspi/build/vehicle_control').resolve():raise HomeError('Unexpected control executable')
        env=dict(v.split('=',1) for v in (proc/'environ').read_text().split(chr(0)) if '=' in v)
    out=ROOT/'logs'/('auto_home_'+str(time.time_ns()));out.mkdir()
    (out/'runtime.before.json').write_bytes(config_path.read_bytes())
    marker=Path('/tmp/steering-bench.active');token='auto_home pid='+str(os.getpid())+'\n'
    marker_created=False;io=None;paused=False;success=False;result={}
    def stopped(*_):raise KeyboardInterrupt('Homing interrupted')
    signal.signal(signal.SIGINT,stopped);signal.signal(signal.SIGTERM,stopped)
    try:
        with marker.open('x') as f:f.write(token)
        marker_created=True
        if ids:
            os.kill(int(ids[0]),signal.SIGTERM);end=time.monotonic()+4
            while (Path('/proc')/ids[0]).exists() and time.monotonic()<end:time.sleep(.02)
        if subprocess.run(['pgrep','-x','vehicle_control'],stdout=subprocess.DEVNULL).returncode!=1:raise HomeError('Controller did not release ownership')
        paused=True
        # Declared=0 means UNKNOWN, never a fabricated mechanical zero. Normal
        # FSPI policies can resume on failure, with closed-loop steering blocked.
        pending=out/'unconfirmed.reference'
        pending.write_text('STEER_PHYSICAL_REFERENCE_V1 %u 0 0\n'%a.session)
        config['environment']['STEERING_REFERENCE_FILE']=str(pending);atomic_json(config_path,config)
        io=HardwareIO(out,a.session,model)
        if a.diagnose_single:
            before=io.stable();io.pulse(1 if a.diagnose_single=='left' else -1,30000)
            after=io.stable();io.release()
            result=dict(schema='STEER_HOME_BUS_DIAGNOSTIC_V1',passed=True,direction=a.diagnose_single,
                before_raw_deg=before,after_raw_deg=after,delta_deg=after-before,requested_us=30000,
                automatic_homing_qualified=False)
        else:result=run_homing(io,limits)
        result.update(session=a.session,S_boot_id=Path('/proc/sys/kernel/random/boot_id').read_text().strip(),
            commissioning=a.commission,physical_straight_visually_confirmed=not a.commission,
            source='real_M_telemetry_and_real_FSPI_no_perception_injection')
        success=True
        if not a.commission:
            reference=out/'center.reference'
            reference.write_text('STEER_PHYSICAL_REFERENCE_V1 %u %.6f 1\n'%(a.session,result['midpoint_raw_deg']))
            config['environment']['STEERING_REFERENCE_FILE']=str(reference);atomic_json(config_path,config)
            result['reference_file']=str(reference);atomic_json(ROOT/'auto_home.current.json',result)
        else:result['pending_visual_check']=not bool(a.diagnose_single)
    except (Exception,KeyboardInterrupt) as e:
        result.update(passed=False,error=str(e),session=a.session)
        if io is not None:
            try:io.release()
            except Exception as release_error:result['release_error']=str(release_error)
    finally:
        if io is not None:
            try:io.close()
            except Exception as e:result['receiver_cleanup_error']=str(e)
        if marker_created and marker.exists() and marker.read_text()==token:marker.unlink()
        if paused:
            try:result['restored_controller_pid']=start_normal(out,env)
            except Exception as e:
                result['restore_error']=str(e);result['passed']=False;success=False
        os.close(lock)
        atomic_json(out/'result.json',result)
    print('AUTO_HOME_RESULT '+json.dumps(dict(out=str(out),**result)),flush=True)
    return 0 if success else 1


if __name__=='__main__':
    try:raise SystemExit(main())
    except (OSError,ValueError,HomeError,subprocess.SubprocessError) as e:
        print('AUTO_HOME_REJECTED',str(e),flush=True);raise SystemExit(1)
