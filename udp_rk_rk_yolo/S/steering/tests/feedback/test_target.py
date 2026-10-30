"""Exercise controller orchestration using fake receiver/actuator, never hardware."""
from contextlib import ExitStack,redirect_stdout
import io
import json
from pathlib import Path
import signal
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools/calibration'))
import steering_target as t
from response_model import SCHEMA


class TargetTests(unittest.TestCase):
    def run_case(self,mode):
        build=ROOT/'build';build.mkdir(exist_ok=True)
        folder=tempfile.TemporaryDirectory(prefix='target_unit_',dir=build)
        directory=Path(folder.name).resolve()
        directory.relative_to(ROOT.resolve())  # Verify workspace confinement before recursive cleanup.
        old_handler=signal.getsignal(signal.SIGTERM)
        try:
            (directory/'logs').mkdir()
            model=dict(schema=SCHEMA,observed_sessions=[7],cells=[dict(direction='left',priority=0,
                n=3,moved=3,min_deg=4,max_deg=6,mean_deg=5,start_min_deg=-4,start_max_deg=2,duration_us=20000)])
            if mode=='insufficient':model['cells'][0]['n']=1
            (directory/'model.json').write_text(json.dumps(model))
            children=[]
            class Receiver:
                def __init__(self,*args):
                    self.samples=[dict(rx_local_ns=1000000000+i*10000000,
                        raw_relative_deg=0,rate_dps=0) for i in range(60)]
                def collect(self,*args,**kwargs):pass
                def close(self):pass
            class Child:
                returncode=0
                def __init__(self,command,**kwargs):
                    self.command=command;children.append(self)
                    trial=command[command.index('--trial')+1]
                    r=dict(result='RECORDED',before_deg=0,after_deg=0 if mode=='no_progress' else 5)
                    if mode=='fault':r['result']='STOPPED';self.returncode=1
                    (directory/'logs'/(trial+'.json')).write_text(json.dumps(r))
                def wait(self,**kwargs):return self.returncode
                def poll(self):return self.returncode
            argv=['target','--trial','target_test','--target-deg','5','--model',str(directory/'model.json'),
                  '--reference-session','7','--zero-raw-deg','0']
            with ExitStack() as stack:
                stack.enter_context(patch.object(t,'ROOT',directory))
                stack.enter_context(patch.object(t,'Telemetry',Receiver))
                stack.enter_context(patch.object(t,'status',lambda:'released'))
                stack.enter_context(patch.object(t.subprocess,'Popen',Child))
                stack.enter_context(patch.object(sys,'argv',argv))
                stack.enter_context(redirect_stdout(io.StringIO()))
                code=t.main()
                # Reusing a target ID must not launch any further child.
                count=len(children)
                with self.assertRaises(FileExistsError):t.main()
                self.assertEqual(len(children),count)
            report=json.loads((directory/'logs/target_test.json').read_text())
            return code,report,len(children)
        finally:
            signal.signal(signal.SIGTERM,old_handler)
            folder.cleanup()

    def test_reaches_only_from_measured_post_pulse_angle(self):
        code,r,count=self.run_case('reach')
        self.assertEqual((code,r['status'],count),(0,'REACHED',1))
        self.assertEqual(r['final_deg'],5)

    def test_two_no_progress_results_stop(self):
        code,r,count=self.run_case('no_progress')
        self.assertEqual((code,r['status'],count),(1,'NO_PROGRESS',2))

    def test_insufficient_model_never_launches_actuator(self):
        code,r,count=self.run_case('insufficient')
        self.assertEqual((code,r['status'],count),(1,'INSUFFICIENT_MODEL',0))

    def test_child_fault_does_not_reuse_old_angle_or_retry(self):
        code,r,count=self.run_case('fault')
        self.assertEqual((code,r['status'],count),(1,'FEEDBACK_OR_ACTUATOR_FAULT',1))
        self.assertIsNone(r['final_deg'])


if __name__=='__main__':unittest.main()
