"""Automated FIXED-CHASSIS pulse mapping; fresh telemetry before every action.

Linux hardware path reuses steering_bench/steering_target. Windows --simulate
checks orchestration only and is explicitly forbidden from runtime export.
"""
import argparse,contextlib,hashlib,json,math,os,re,signal,subprocess,sys,time
from pathlib import Path
from response_model import fit,choose,reference_matches,SCHEMA
from export_runtime import export
from steering_bench import ROOT,status,validate_pulse

def atomic_text(path,text,encoding='utf-8'):
    tmp=path.with_suffix('.tmp')
    with tmp.open('w',encoding=encoding,newline='\n') as f:
        f.write(text);f.flush();os.fsync(f.fileno())
    for attempt in range(8):
        try:tmp.replace(path);break
        except PermissionError:
            if os.name!='nt' or attempt==7:raise
            time.sleep(.01*(attempt+1))
    if os.name!='nt':
        fd=os.open(path.parent,os.O_RDONLY|os.O_DIRECTORY)
        try:os.fsync(fd)
        finally:os.close(fd)

class Batch:
    def __init__(self,args):
        self.a=args;self.index=0;self.position_index=0;self.child=None;self.current=0.;self.records=[];self.paths=[];self.steps=[]
        self.dest=Path(args.output);self.dest.mkdir(parents=True,exist_ok=False)
        self.model={'schema':SCHEMA,'cells':[],'records':[],'observed_sessions':[]}
        continuation=getattr(args,'continue_model',None)
        if continuation:
            if args.simulate:raise ValueError('Measured continuation cannot run in simulation')
            previous=json.loads(Path(continuation).read_text())
            export(previous) # validate measured distribution, counts and bounds
            if not reference_matches(previous,args.reference_session,args.zero_raw_deg):
                raise ValueError('Continuation requires the same explicitly confirmed physical reference/session')
            for record in previous['records']:
                name=record['trial']
                if not re.fullmatch(r'A\d+_[A-Za-z0-9_]+',name):raise ValueError('Invalid continuation A trial identity')
                path=ROOT/'logs'/(name+'.json')
                if hashlib.sha256(path.read_bytes()).hexdigest()!=record['sha256']:raise ValueError('Continuation trial changed')
                self.paths.append(path)
            self.model=fit(self.paths)
            if self.model['records']!=previous['records']:raise ValueError('Continuation records are not reproducible')
        self.report=dict(schema='STEER_AUTO_CALIBRATION_V1',run=args.run_id,reference_session=args.reference_session,
            zero_raw_deg=args.zero_raw_deg,user_fixed_chassis_confirmed=args.fixed_chassis,
            user_physical_zero_confirmed=args.confirm_zero,simulation_only=args.simulate,
            angle_limit_deg=args.angle_limit_deg,request_limit_us=60000,trials=[],coverage_skips=[],state='RESERVED',
            requested_durations_us=getattr(args,'durations_us',[]),requested_repeats=getattr(args,'repeats',None))
        self.report['guard_policy']='Unmeasured durations: max observed response-rate x requested duration, margin1.5. Exact duration: at least3 observed nonzero responses, maximum observed motion x1.5+0.75deg. Zero responses stay in model/planner; always at least seed guard.'
        if continuation:self.report.update(continued_from=str(continuation),inherited_measured_trials=len(self.paths))
        self.save()
        if continuation:self.write_model()
    def save(self):
        atomic_text(self.dest/'batch.json',json.dumps(self.report,indent=2))
    def command(self,argv,log):
        with log.open('x') as f:
            self.child=subprocess.Popen(argv,stdout=f,stderr=subprocess.STDOUT,start_new_session=os.name!='nt')
            try:code=self.child.wait(timeout=45)
            except BaseException:
                self.child.send_signal(signal.SIGTERM)
                self.child.wait(timeout=5);raise
            finally:self.child=None
        return code
    def measured_position(self):
        if self.a.simulate:return self.current
        # Receiver is a fresh process, never reuses an old buffered angle.
        from steering_bench import Telemetry,baseline_of
        self.position_index+=1
        prefix=self.dest/('position_%03d'%self.position_index)
        monitor=Telemetry(prefix,self.a.reference_session,self.a.zero_raw_deg,self.a.angle_limit_deg)
        try:monitor.collect(.8,allow_initial=True);return baseline_of(monitor.samples[-60:],self.a.zero_raw_deg)
        finally:monitor.close()
    def write_model(self):
        if self.a.simulate:
            # Same aggregation contract; explicitly not a measured plant model.
            self.model=fit(self.paths,allow_simulated=True);self.model['simulation_only']=True
        else:self.model=fit(self.paths)
        self.model['reference_binding']=dict(schema='STEER_BENCH_REFERENCE_V1',active_session=self.a.reference_session,
            zero_raw_deg=self.a.zero_raw_deg,verified_same_physical_zero_sessions=[self.a.reference_session],user_same_physical_zero_confirmed=True)
        p=self.dest/'response_model.json';atomic_text(p,json.dumps(self.model,indent=2))
        if self.model['cells'] and not self.a.simulate:
            table=self.dest/'response_model.table';atomic_text(table,export(self.model),encoding='ascii')
            (self.dest/'model_identity.json').write_text(json.dumps(dict(json_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),
                table_sha256=hashlib.sha256(table.read_bytes()).hexdigest()),indent=2))
    def guard(self,direction,us):
        exact=[c for c in self.model['cells'] if c['direction']==direction and c['duration_us']==us and
            c['priority']==self.a.realtime_priority and c['n']>=3 and c['moved']>=3]
        if exact:return max(self.a.seed_guard_deg,max(c['max_deg'] for c in exact)*1.5+.75)
        known=[c for c in self.model['cells'] if c['direction']==direction and c['max_deg']>.15]
        if not known:return self.a.seed_guard_deg
        # Conservative exploration bound, not a claimed time/angle function.
        rate=max(c['max_deg']/c['duration_us'] for c in known)
        return max(self.a.seed_guard_deg,(rate*us+.5)*1.5)
    def prepare_start(self,direction,us,guard):
        """Calibration exploration, not a goal-controller bootstrapping loop.

        Never weakens the runtime's sample/start-range requirements. These
        bounded exploratory moves are themselves A trials, with the same fresh
        feedback, timing, session and predictive envelope checks as any sample.
        """
        opposite='right' if direction=='left' else 'left';sign=1 if opposite=='left' else -1
        no_progress=0
        for _ in range(4):
            guard=max(guard,self.guard(direction,us))
            self.current=self.measured_position()
            try:validate_pulse(direction,us,self.current,guard,self.a.angle_limit_deg,60000);return True
            except RuntimeError:pass
            if len(self.report['trials'])>=getattr(self.a,'max_total_pulses',120):return False
            candidates=[]
            for c in self.model['cells']:
                if c['direction']!=opposite or c['priority']!=self.a.realtime_priority or not c['moved'] or c['min_deg']<-.15:continue
                bound=self.guard(opposite,c['duration_us'])
                try:validate_pulse(opposite,c['duration_us'],self.current,bound,self.a.angle_limit_deg,60000)
                except (ValueError,RuntimeError):continue
                candidates.append((c['mean_deg'],-c['duration_us'],c['duration_us'],bound))
            if not candidates:return False
            _,_,duration,bound=max(candidates)
            before=self.current;self.report['state']='EXPLORE_START';self.save();self.pulse(opposite,duration,bound)
            no_progress=no_progress+1 if (self.current-before)*sign<.15 else 0
            if no_progress>=2:return False
        try:validate_pulse(direction,us,self.current,guard,self.a.angle_limit_deg,60000);return True
        except RuntimeError:return False
    def reposition(self,target):
        if abs(self.current-target)<=1:return True
        if self.a.simulate:
            for _ in range(6):
                d=choose(self.model,self.current,target,angle_limit=self.a.angle_limit_deg,target_limit=16,request_limit_us=60000,priority=self.a.realtime_priority)
                if d['status']=='REACHED':return True
                if d['status']!='PULSE':return False
                self.current+=(1 if d['direction']=='left' else -1)*self.sim_response(d['duration_us'])
            return abs(self.current-target)<=1
        self.index+=1;ident='T'+self.a.run_id+'_%03d'%self.index
        code=self.command([sys.executable,str(Path(__file__).with_name('steering_target.py')),'--trial',ident,'--model',str(self.dest/'response_model.json'),
            '--target-deg',str(target),'--reference-session',str(self.a.reference_session),'--zero-raw-deg',str(self.a.zero_raw_deg),
            '--angle-limit-deg',str(self.a.angle_limit_deg),'--target-limit-deg','16','--request-limit-us','60000','--max-pulses','6',
            '--realtime-priority',str(self.a.realtime_priority)],self.dest/(ident+'.console.log'))
        report=json.loads((ROOT/'logs'/(ident+'.json')).read_text());self.report.setdefault('reposition',[]).append(report)
        if report['status'] not in ['REACHED','RESOLUTION_LIMIT','INSUFFICIENT_MODEL','PULSE_LIMIT','NO_PROGRESS','REVERSAL_LIMIT']:
            raise RuntimeError('Reposition feedback/actuator failure: '+report['status'])
        self.current=self.measured_position();return code==0
    @staticmethod
    def sim_response(us):return 0. if us<17000 else min(14.,3.+(us-17000)*.0003)
    def pulse(self,direction,us,guard):
        if len(self.report['trials'])>=getattr(self.a,'max_total_pulses',120):raise RuntimeError('Calibration total pulse budget exhausted')
        self.index+=1;ident='A'+self.a.run_id+'_%03d_%s_%dus'%(self.index,direction,us)
        if self.a.simulate:
            before=self.current;delta=(1 if direction=='left' else -1)*self.sim_response(us);self.current+=delta
            report=dict(trial=ident,result='RECORDED',direction=direction,requested_us=us,reference_session=self.a.reference_session,
                before_deg=before,after_deg=self.current,delta_deg=delta,launch_local_ns=self.index*1000000000,
                actuator_realtime_priority=self.a.realtime_priority,pulse_records=[dict(result='COMMAND_COMPLETED',software_elapsed_ms=str(us/1000))],simulation_only=True)
            path=self.dest/(ident+'.json');path.write_text(json.dumps(report,indent=2))
        else:
            code=self.command([sys.executable,str(Path(__file__).with_name('steering_bench.py')),'--trial',ident,'--direction',direction,
                '--duration-us',str(us),'--reference-session',str(self.a.reference_session),'--zero-raw-deg',str(self.a.zero_raw_deg),
                '--guard-step-deg',str(guard),'--angle-limit-deg',str(self.a.angle_limit_deg),'--request-limit-us','60000',
                '--expected-start-deg',str(self.current),'--realtime-priority',str(self.a.realtime_priority)],self.dest/(ident+'.console.log'))
            path=ROOT/'logs'/(ident+'.json');report=json.loads(path.read_text())
            if code or report['result']!='RECORDED':raise RuntimeError('Pulse failed; no automatic replay: '+report.get('error',report['result']))
            self.current=report['after_deg']
        self.paths.append(path);self.report['trials'].append(dict(id=ident,direction=direction,requested_us=us,before_deg=report['before_deg'],after_deg=report['after_deg'],delta_deg=report['delta_deg']))
        self.write_model();self.save()
    def run(self):
        self.report['state']='PREFLIGHT';self.save()
        if not self.a.simulate:status()
        self.current=self.measured_position()
        self.report['initial_angle_deg']=self.current;self.save()
        if not getattr(self.a,'continue_model',None) and abs(self.current)>.5:raise RuntimeError('Initial angle differs from operator-confirmed physical center')
        if abs(self.current)>self.a.angle_limit_deg:raise RuntimeError('Continuation starts outside the declared angle envelope')
        for us in self.a.durations_us:
            for repeat in range(self.a.repeats):
                directions=['right','left'] if self.current>0 else ['left','right']
                for direction in directions:
                    self.current=self.measured_position();guard=self.guard(direction,us);sign=1 if direction=='left' else -1
                    if guard>2*self.a.angle_limit_deg:
                        self.report['coverage_skips'].append(dict(direction=direction,us=us,reason='UNMEASURED_EXPLORATION_BOUND_TOO_LARGE'));continue
                    if abs(self.current+sign*guard)>self.a.angle_limit_deg:
                        self.prepare_start(direction,us,guard)
                        guard=max(guard,self.guard(direction,us))
                    if guard>2*self.a.angle_limit_deg:
                        self.report['coverage_skips'].append(dict(direction=direction,us=us,reason='UPDATED_EXPLORATION_BOUND_TOO_LARGE'));continue
                    try:validate_pulse(direction,us,self.current,guard,self.a.angle_limit_deg,60000)
                    except RuntimeError:
                        self.report['coverage_skips'].append(dict(direction=direction,us=us,reason='NO_SAFE_START_POSITION',current=self.current));continue
                    self.report['state']='MEASURE';self.save();self.pulse(direction,us,guard)
        self.report['state']='RETURN_NEAR_CENTER';self.save();self.report['returned_within_tolerance']=self.reposition(0) if self.model['cells'] else abs(self.current)<=1
        self.report['final_angle_deg']=self.current
        ready=all(any(c['direction']==d and c['n']>=2 and c['moved']>=2 for c in self.model['cells']) for d in ['left','right'])
        self.report['both_directions_measured']=ready
        any_motion=any(c['moved'] for c in self.model['cells'])
        self.report['state']=('INSUFFICIENT_MEASURED_COVERAGE' if any_motion else 'NO_MEASURED_MOTION') if not ready else 'COMPLETE' if not self.report['coverage_skips'] and self.report['returned_within_tolerance'] else 'PARTIAL_COVERAGE'
        self.save();print('AUTO_CALIBRATION_RESULT='+json.dumps({k:self.report[k] for k in ['state','both_directions_measured','returned_within_tolerance','final_angle_deg']}),flush=True)

def release_outputs(batch):
    """Release failure invalidates batch completion, including an otherwise good map."""
    try:
        release=subprocess.run([str(ROOT/'actuator/build/steering_step_fspi'),'--backend','fspi','--direction','release','--max-pulse-ms','60'],capture_output=True,text=True,timeout=5)
        batch.report['release_verified_by_command']=release.returncode==0
    except Exception as exc:
        batch.report['release_verified_by_command']=False
        batch.report['release_cleanup_error']=str(exc)
    good=batch.report['release_verified_by_command']
    if not good:
        batch.report.update(state='STOPPED',release_state_unknown=True,error='Final actuator release could not be verified')
    batch.save();return good

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--run-id',required=True);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--fixed-chassis',action='store_true',required=True);p.add_argument('--confirm-zero',action='store_true',required=True)
    p.add_argument('--reference-session',type=int,required=True);p.add_argument('--zero-raw-deg',type=float,required=True)
    p.add_argument('--durations-us',default='5000,10000,15000,17500,18000,20000,25000,30000,40000,50000,60000')
    p.add_argument('--repeats',type=int,default=2);p.add_argument('--angle-limit-deg',type=float,default=18)
    p.add_argument('--seed-guard-deg',type=float,default=8);p.add_argument('--realtime-priority',type=int,default=0);p.add_argument('--simulate',action='store_true')
    p.add_argument('--continue-model',type=Path,help='Verified measured A records with SAME confirmed session/zero; permits a fresh nonzero start')
    p.add_argument('--max-total-pulses',type=int,default=120,help='Batch A pulse cap, including exploratory prepositioning; max160')
    a=p.parse_args();a.durations_us=[int(v) for v in a.durations_us.split(',')]
    if not 1<=a.max_total_pulses<=160:p.error('Total pulse budget must be1..160')
    if not re.fullmatch(r'\d{6,12}',a.run_id) or not 0<a.reference_session<=0xffffffff or not math.isfinite(a.zero_raw_deg) or not 1<=a.repeats<=4 or not 2<=a.angle_limit_deg<=18 or not 1<=a.seed_guard_deg<=a.angle_limit_deg or not 0<=a.realtime_priority<=30 or not 1<=len(a.durations_us)<=32 or any(not 1<=v<=60000 for v in a.durations_us) or a.durations_us!=sorted(set(a.durations_us)):
        p.error('Invalid run/reference/duration/envelope')
    if os.name=='nt' and not a.simulate:p.error('Hardware path is Linux-only; Windows supports --simulate')
    batch=None
    def interrupt(*_):raise KeyboardInterrupt('Calibration interrupted')
    signal.signal(signal.SIGTERM,interrupt)
    lock=None;exit_code=1
    try:
        if not a.simulate:
            import fcntl
            lock=open('/tmp/steering-calibration.lock','a');fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
        batch=Batch(a);batch.run();exit_code=0 if batch.report['state']=='COMPLETE' else 2
    except (Exception,KeyboardInterrupt) as e:
        if batch:batch.report.update(state='STOPPED',error=str(e));batch.save()
        print('AUTO_CALIBRATION_STOPPED',str(e),flush=True);exit_code=1
    finally:
        if not a.simulate and batch:
            # Normal failure/cancellation always tries both direction releases.
            if not release_outputs(batch):exit_code=1
        if lock:lock.close()
    return exit_code
if __name__=='__main__':raise SystemExit(main())
