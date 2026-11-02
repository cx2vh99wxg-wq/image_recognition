"""Rigid-mount fixed-chassis characterization and independent goal validation.

Only the bounded native action child owns SPI. Calibration may explore a new
start partition with an explicitly conservative measured global envelope;
runtime validation uses only observed local starts, never that exploration.
"""
from pathlib import Path
import argparse,hashlib,json,math,os,signal,statistics,subprocess,time
from partition_response import fit,choose,choose_reposition
from steering_bench import Telemetry,baseline_of,status

class Rig:
    def __init__(self,a):
        self.a=a;self.dest=Path(a.output);self.dest.mkdir(exist_ok=False,parents=True)
        ref=Path(a.reference).read_text().split()
        if len(ref)!=4 or ref[0]!='STEER_PHYSICAL_REFERENCE_V1' or ref[3]!='1':raise ValueError('invalid physical reference')
        self.session=int(ref[1]);self.zero=float(ref[2]);self.current=0.;self.child=None;self.paths=[];self.goals=[];self.index=0
        self.model=fit([]);self.source=[]
        if a.continue_from:
            old=json.loads(Path(a.continue_from).read_text())
            if old['schema']!='STEER_PARTITION_RESPONSE_V2' or old['reference']!=[self.session,self.zero]:raise ValueError('mount/session reference mismatch')
            for s in old['sources']:
                p=Path(s['file'])
                if hashlib.sha256(p.read_bytes()).hexdigest()!=s['sha256']:raise ValueError('changed source observation')
                self.paths.append(p)
            self.model=fit(self.paths,a.width_deg)
        self.frozen=json.loads(json.dumps(self.model));self.save('PREFLIGHT')
    def save(self,state):
        out=dict(schema='STEER_RIG_PARTITION_RUN_V1',state=state,session=self.session,zero_raw_deg=self.zero,
            stage=self.a.stage,current_deg=self.current,physical_limit_deg=18,center_tolerance_deg=3,target_tolerance_deg=2,
            fixed_chassis=True,goals=self.goals,new_action_count=self.index,model_qualified_for_road_control=False)
        tmp=self.dest/'progress.tmp';tmp.write_text(json.dumps(out,indent=2));tmp.replace(self.dest/'progress.json')
        p=self.dest/'response_model.json';tmp=self.dest/'model.tmp';tmp.write_text(json.dumps(self.model,indent=2));tmp.replace(p)
    def position(self):
        prefix=self.dest/('observe_%d'%time.monotonic_ns())
        t=Telemetry(prefix,self.session,self.zero,18)
        try:t.collect(.85,allow_initial=True);self.current=baseline_of(t.samples[-60:],self.zero);return self.current
        finally:t.close()
    def envelope(self,duration,direction):
        rs=[]
        for c in self.model['cells']:
            if c['direction']==direction and c['first_us']==duration and not c['brake_us'] and c['priority']==10:rs.append(c)
        count=sum(c['n'] for c in rs)
        peak=max([max(abs(c['peak_low_deg']),abs(c['peak_high_deg'])) for c in rs]+[0])
        # 反复读到零响应不能作为到达机械止挡的凭证。
        return max(4,peak*1.35+.7) if count>=3 and peak>.15 else 12.
    def pulse(self,direction,duration,purpose='CALIBRATION',bounds=None):
        if self.index>=self.a.max_actions:raise RuntimeError('action budget exhausted')
        self.index+=1;prefix=self.dest/('%03d_%s_%s_%d'%(self.index,purpose,'left' if direction==1 else 'right',duration))
        if bounds is None:
            g=self.envelope(duration,direction);bounds=(0,g) if direction==1 else (-g,0)
        if self.current+bounds[0]<-18 or self.current+bounds[1]>18:return False
        args=[self.a.action_tool,'--backend','fspi','--fixed-chassis','--direction','left' if direction==1 else 'right',
              '--first-us',str(duration),'--brake-us','0','--gap-us','2000','--priority','10',
              '--reference',self.a.reference,'--output-prefix',str(prefix),'--guard-low',str(bounds[0]),'--guard-high',str(bounds[1])]
        with prefix.with_suffix('.console.log').open('x') as log:
            self.child=subprocess.Popen(args,stdout=log,stderr=subprocess.STDOUT,start_new_session=True)
            try:code=self.child.wait(timeout=12)
            except BaseException:
                self.child.send_signal(signal.SIGTERM);self.child.wait(timeout=4);raise
            finally:self.child=None
        r=json.loads(prefix.with_suffix('.json').read_text());self.source.append(str(prefix))
        print('RIG_ACTION %03d purpose=%s dir=%d us=%d before=%s after=%s delta=%s result=%s'%(
            self.index,purpose,direction,duration,r['before_deg'],r['after_deg'],r['delta_deg'],r['result']),flush=True)
        if code or r['result']!='RECORDED':raise RuntimeError('action failed; no automatic replay: '+r['error'])
        if r['session']!=self.session or r['zero_raw_deg']!=self.zero:raise RuntimeError('reference changed')
        self.current=r['after_deg']
        if self.a.stage!='validate':
            self.paths.append(prefix.with_suffix('.json'));self.model=fit(self.paths,self.a.width_deg)
        self.save('MEASURING');time.sleep(.25);return True
    def pilot_choice(self,target):
        error=target-self.current;d=1 if error>0 else 2;sign=1 if d==1 else -1;candidates=[]
        for us in [19000,20000,25000,30000]:
            cells=[c for c in self.model['cells'] if c['direction']==d and c['first_us']==us and not c['brake_us'] and c['priority']==10]
            obs=[v for c in cells for v in c['responses_deg']]
            if len(obs)<3 or not any(v*sign>.2 for v in obs):continue
            guard=self.envelope(us,d)
            if not -18<=self.current+sign*guard<=18:continue
            expected=statistics.mean(abs(error-v) for v in obs)
            worst=max(abs(error-v) for v in obs)
            if expected>=abs(error)-.05 or worst>max(abs(error)+1.,4.):continue
            candidates.append((expected,worst,us))
        return (d,min(candidates)[2]) if candidates else None
    def goal(self,target,repeat):
        tolerance=3 if target==0 else 2
        record=dict(target_deg=target,tolerance_deg=tolerance,repeat=repeat,before_deg=self.position(),actions=[],status='STARTED')
        self.goals.append(record);previous_direction=0;reversals=0;no_progress=0;reposition_used=False
        for _ in range(8):
            error=target-self.current
            if abs(error)<=tolerance:record['status']='REACHED';break
            reposition=False
            if self.a.stage=='validate':
                try:
                    try:c=choose(self.frozen,self.current,target,tolerance,18,10,self.a.minimum_samples)
                    except ValueError:
                        if reposition_used:raise
                        c=choose_reposition(self.frozen,self.current,target,tolerance,18,10,self.a.minimum_samples)
                    if not c:record['status']='REACHED';break
                    reposition=c.get('_reposition',False)
                    d,us=c['direction'],c['first_us']
                    low=min(0,c['peak_low_deg'])*1.2-.3;high=max(0,c['peak_high_deg'])*1.2+.3
                except ValueError:record['status']='RESOLUTION_OR_COVERAGE_LIMIT';break
            else:
                c=self.pilot_choice(target)
                if c is None:record['status']='RESOLUTION_OR_COVERAGE_LIMIT';break
                d,us=c;g=self.envelope(us,d);low,high=(0,g) if d==1 else (-g,0)
            if previous_direction and previous_direction!=d:reversals+=1
            if reversals>2:record['status']='REVERSAL_LIMIT';break
            previous_direction=d;before=self.current
            if not self.pulse(d,us,'VALIDATION' if self.a.stage=='validate' else 'PARTITION_PILOT',(low,high)):
                record['status']='ENVELOPE_LIMIT';break
            record['actions'].append(dict(direction=d,duration_us=us,before_deg=before,after_deg=self.current))
            if reposition and abs(self.current-before)>=.15:reposition_used=True;no_progress=0
            else:no_progress=no_progress+1 if abs(error)-abs(target-self.current)<.15 else 0
            if no_progress>=2:record['status']='NO_PROGRESS';break
        else:record['status']='ACTION_LIMIT'
        if abs(target-self.current)<=tolerance:record['status']='REACHED'
        record.update(after_deg=self.current,error_deg=target-self.current)
        self.save('TARGET_COMPLETE');print('RIG_GOAL '+json.dumps(record),flush=True)
        return record['status']=='REACHED'
    def run(self):
        status();self.position()
        if abs(self.current)>18:raise RuntimeError('outside physical envelope')
        if self.a.stage=='center':
            if abs(self.current)>.5:raise RuntimeError('new rigid mount reference is not centered')
            for repeat in range(self.a.repeats):
                for us in [30000,20000,25000,19000]:
                    for d in ([2,1] if self.current>0 else [1,2]):
                        if abs(self.current)>4:
                            if not self.pulse(2 if self.current>0 else 1,30000,'CENTER_POSITION'):raise RuntimeError('no safe center start')
                        if not self.pulse(d,us):raise RuntimeError('calibration envelope unavailable')
            self.goal(0,0)
        elif self.a.stage=='supplement':
            for repeat in range(self.a.repeats):
                self.goal(-12,repeat)
                if -14.5<=self.current<=-9.1:
                    self.pulse(1,self.a.fine_return_us,'NEGATIVE_FINE_RETURN',(-.3,8))
                # 向外表征仅从具有完整瞬态余量的起点开始，
                # 不能从受保护端点继续向外推动。
                for _ in range(2):
                    if self.current>=-11.1:break
                    self.pulse(1,self.a.fine_return_us,'NEGATIVE_FINE_RETURN',(-.3,8))
                if -11.1<=self.current<=-9.1:
                    self.pulse(2,25000,'NEGATIVE_OUTWARD_WINDOW',(-6.8,.3))
                self.goal(-15,repeat)
                self.goal(0,repeat)
                self.goal(12,repeat)
                if 9.1<=self.current<=14.5:self.pulse(2,25000,'POSITIVE_FINE_RETURN',(-8,.3))
                self.goal(15,repeat);self.goal(0,repeat)
                self.goal(-6,repeat)
                if -8.5<=self.current<=-3.1:self.pulse(2,19000,'NEGATIVE_FINE_WINDOW',(-8,.3))
                self.goal(0,repeat)
                self.goal(6,repeat)
                if 3.1<=self.current<=8.5:self.pulse(1,19000,'POSITIVE_FINE_WINDOW',(-.3,8))
                self.goal(0,repeat)
        else:
            for repeat in range(self.a.repeats):
                for target in [6,12,15,12,6,0,-6,-12,-15,-12,-6,0]:self.goal(target,repeat)
        self.save('COMPLETE');print('RIG_RUN_COMPLETE',self.a.stage,self.index,flush=True)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--fixed-chassis',action='store_true',required=True)
    p.add_argument('--reference',required=True);p.add_argument('--action-tool',required=True);p.add_argument('--output',required=True)
    p.add_argument('--continue-from');p.add_argument('--stage',choices=['center','sweep','supplement','validate'],required=True)
    p.add_argument('--repeats',type=int,default=3);p.add_argument('--max-actions',type=int,default=100)
    p.add_argument('--width-deg',type=float,default=6);p.add_argument('--minimum-samples',type=int,default=3)
    p.add_argument('--fine-return-us',type=int,choices=[19000,25000],default=25000);a=p.parse_args()
    if os.name=='nt':p.error('Characterization runs only on Linux; partition_response.py fits/replays on Windows')
    if not 1<=a.repeats<=5 or not 1<=a.max_actions<=160 or not .5<=a.width_deg<=6:p.error('invalid experiment bounds')
    rig=None
    def interrupted(*_):raise KeyboardInterrupt('operator cancelled')
    signal.signal(signal.SIGTERM,interrupted);signal.signal(signal.SIGINT,interrupted)
    try:rig=Rig(a);rig.run()
    except BaseException as e:
        if rig:rig.save('STOPPED');(rig.dest/'error.txt').write_text(str(e))
        raise
if __name__=='__main__':main()
