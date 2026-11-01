"""Automation/export guards; Windows and Linux, no hardware commands."""
import argparse,json,sys,tempfile,unittest,os,subprocess,socket
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools/calibration'))
from steering_auto_calibrate import Batch,release_outputs
from unittest.mock import Mock,patch
from export_runtime import export
from response_model import fit,SCHEMA
from steering_bench import settled_after_pulse,pulse_elapsed_ms,timing_valid
class AutoTests(unittest.TestCase):
    def test_raw_timestamp_boundary_is_not_hidden_by_print_rounding(self):
        p=dict(enable_end_ns='1000000000',release_end_ns='1042000292',software_elapsed_ms='42.000')
        self.assertGreater(pulse_elapsed_ms(p),42)
        self.assertFalse(timing_valid(40000,pulse_elapsed_ms(p)))
        self.assertTrue(timing_valid(40000,pulse_elapsed_ms(dict(software_elapsed_ms='42.000'))))
    def test_exact_guard_requires_three_nonzero_measurements_and_keeps_zero_risk(self):
        b=Batch.__new__(Batch);b.a=argparse.Namespace(seed_guard_deg=8,realtime_priority=0)
        short=dict(direction='left',duration_us=19000,priority=0,n=2,moved=1,min_deg=0,max_deg=8,mean_deg=4)
        repeated=dict(direction='left',duration_us=30000,priority=0,n=3,moved=3,min_deg=4,max_deg=5.5,mean_deg=4.8)
        b.model=dict(cells=[short,repeated]);self.assertAlmostEqual(b.guard('left',30000),9)
        repeated['moved']=2;self.assertGreater(b.guard('left',30000),18)
        # A missed/zero response changes progress probability, not the maximum
        # observed reach; runtime choose still retains every zero observation.
        repeated.update(n=4,moved=3,min_deg=0);self.assertAlmostEqual(b.guard('left',30000),9)
    def test_calibration_preposition_collects_safe_A_samples_without_runtime_gate(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as folder:
            a=argparse.Namespace(output=Path(folder)/'batch',run_id='202610310088',reference_session=7,zero_raw_deg=0,
                fixed_chassis=True,confirm_zero=True,simulate=True,angle_limit_deg=18,seed_guard_deg=8,
                realtime_priority=0,durations_us=[20000],repeats=2,max_total_pulses=6)
            b=Batch(a);b.current=8
            b.model['cells']=[dict(direction='right',duration_us=20000,priority=0,n=1,moved=1,
                min_deg=4,max_deg=4,mean_deg=4)]
            # The formal controller cannot use n=1. A safe calibration move
            # can still gather the next sample, and remains inside the bound.
            self.assertTrue(b.prepare_start('left',50000,14))
            self.assertEqual(len(b.report['trials']),2)
            self.assertLessEqual(b.current+14,18)
            self.assertTrue(all(t['id'].startswith('A') for t in b.report['trials']))
    def test_settle_wait_can_extend_without_relaxing_or_hiding_stale_feedback(self):
        clock=[0.];monitor=Mock();monitor.samples=[]
        monitor.collect.side_effect=lambda seconds:clock.__setitem__(0,clock[0]+seconds)
        with patch('steering_bench.time.monotonic',side_effect=lambda:clock[0]),patch('steering_bench.baseline_of',side_effect=[RuntimeError('Steering is not settled'),1.25]):
            value,wait=settled_after_pulse(monitor,0)
        self.assertEqual(value,1.25);self.assertAlmostEqual(wait,2.5)
        monitor.collect.side_effect=RuntimeError('No fresh feedback within 100ms')
        with self.assertRaisesRegex(RuntimeError,'No fresh feedback'):settled_after_pulse(monitor,0)
    def test_final_release_failure_invalidates_completion(self):
        batch=Mock();batch.report=dict(state='COMPLETE')
        with patch('steering_auto_calibrate.subprocess.run',side_effect=subprocess.TimeoutExpired('release',5)):
            self.assertFalse(release_outputs(batch))
        self.assertEqual(batch.report['state'],'STOPPED');self.assertTrue(batch.report['release_state_unknown'])
        batch.save.assert_called_once()
    def test_continuation_rejects_unconfirmed_session(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as folder:
            p=Path(folder)/'previous.json'
            c=dict(direction='left',duration_us=20000,priority=0,n=2,moved=2,min_deg=4,max_deg=6,mean_deg=5,
                start_min_deg=-2,start_max_deg=2,responses_deg=[4,6])
            p.write_text(json.dumps(dict(schema=SCHEMA,cells=[c],records=[],observed_sessions=[6])))
            a=argparse.Namespace(output=Path(folder)/'next',run_id='202610310003',reference_session=7,zero_raw_deg=0,
                fixed_chassis=True,confirm_zero=True,simulate=False,angle_limit_deg=18,continue_model=p)
            with self.assertRaisesRegex(ValueError,'same explicitly confirmed'):Batch(a)
    @unittest.skipIf(os.name=='nt','POSIX runtime; portable core tested separately')
    def test_runtime_integration_with_fake_registers(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as folder:
            table=Path(folder)/'response.table';ref=Path(folder)/'physical.reference'
            ref.write_text('STEER_PHYSICAL_REFERENCE_V1 7 0 1\n')
            cells=[dict(direction=d,duration_us=10000,priority=0,n=3,moved=3,min_deg=4,max_deg=6,mean_deg=5,
                start_min_deg=-18,start_max_deg=18,responses_deg=[4,5,6]) for d in ['left','right']]
            table.write_text(export(dict(schema=SCHEMA,cells=cells)))
            with socket.socket(socket.AF_INET,socket.SOCK_DGRAM) as s:s.bind(('127.0.0.1',0));port=s.getsockname()[1]
            # Added short-valid-frame scenarios contain >7s of intentional
            # waits. The old 5s wrapper killed a passing fixture before its
            # final assertions; this is only a test-process deadline.
            result=subprocess.run([str(ROOT/'build/tests/test_runtime_integration'),str(table),str(ref),str(port)],capture_output=True,text=True,timeout=15)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
    def test_simulated_batch_stays_bounded_and_cannot_export(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as folder:
            a=argparse.Namespace(output=Path(folder)/'batch',run_id='202610310099',reference_session=7,zero_raw_deg=0,
                fixed_chassis=True,confirm_zero=True,simulate=True,angle_limit_deg=18,seed_guard_deg=8,
                realtime_priority=0,durations_us=[5000,10000,15000,17500,20000,30000,60000],repeats=2)
            b=Batch(a);b.run()
            self.assertIn(b.report['state'],['COMPLETE','PARTIAL_COVERAGE'])
            self.assertTrue(b.model['simulation_only'])
            for r in b.report['trials']:
                self.assertLessEqual(abs(r['before_deg']),18);self.assertLessEqual(abs(r['after_deg']),18)
            with self.assertRaises(ValueError):export(b.model)
            self.assertEqual(fit(b.paths)['cells'],[])
    def test_runtime_export_keeps_empirical_distribution_and_rejects_forged_statistics(self):
        cell=dict(direction='left',duration_us=20000,priority=0,n=2,moved=1,min_deg=0,max_deg=6,mean_deg=3,
            start_min_deg=-2,start_max_deg=2,responses_deg=[0,6])
        m=dict(schema=SCHEMA,cells=[cell]);text=export(m)
        self.assertTrue(text.startswith('STEER_RESPONSE_TABLE_V1 1\n'))
        cell['mean_deg']=2
        with self.assertRaises(ValueError):export(m)
    def test_invalidated_obstructed_sample_cannot_fit_or_export(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as folder:
            p=Path(folder)/'trial.json';p.write_text(json.dumps(dict(trial='A202610310002_001',requested_us=5000,invalidated=True,invalidation_reason='OBSTRUCTION')))
            self.assertEqual(fit([p])['cells'],[])
            self.assertEqual(fit([p])['excluded'][0]['reason'],'OBSTRUCTION')
        with self.assertRaises(ValueError):export(dict(schema=SCHEMA,invalidated=True,cells=[]))
    def test_portable_C_loader_uses_distribution_and_rejects_malformed_table(self):
        cell=dict(direction='left',duration_us=20000,priority=0,n=2,moved=1,min_deg=0,max_deg=6,mean_deg=3,
            start_min_deg=-2,start_max_deg=2,responses_deg=[0,6])
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as folder:
            table=Path(folder)/'model.table';table.write_text(export(dict(schema=SCHEMA,cells=[cell])))
            exe=ROOT/'actuator/build'/('steering_goal.exe' if os.name=='nt' else 'steering_goal')
            good=subprocess.run([str(exe),'--backend','dry-run','--model',str(table),'--target-deg','10'],capture_output=True,text=True)
            self.assertEqual(good.returncode,0,good.stderr);self.assertIn('duration_us=20000',good.stdout)
            small=subprocess.run([str(exe),'--backend','dry-run','--model',str(table),'--target-deg','1.5'],capture_output=True,text=True)
            self.assertEqual(small.returncode,1);self.assertIn('duration_us=0',small.stdout)
            table.write_text(table.read_text().replace('0 6 3 ','0 9 3 '))
            bad=subprocess.run([str(exe),'--backend','dry-run','--model',str(table),'--target-deg','10'],capture_output=True)
            self.assertEqual(bad.returncode,1)
if __name__=='__main__':unittest.main()
