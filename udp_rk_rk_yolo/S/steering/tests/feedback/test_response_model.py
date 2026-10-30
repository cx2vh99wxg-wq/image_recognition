import importlib.util
from pathlib import Path
import unittest
import sys

p=Path(__file__).resolve().parents[2]/'tools/calibration/response_model.py'
sys.path.insert(0,str(p.parent))
spec=importlib.util.spec_from_file_location('response_model',p)
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)


class ModelTests(unittest.TestCase):
    def test_new_session_requires_explicit_confirmed_physical_zero_binding(self):
        model=dict(observed_sessions=[7])
        self.assertFalse(m.reference_matches(model,8,0))
        model['reference_binding']=dict(schema='STEER_BENCH_REFERENCE_V1',
            active_session=8,zero_raw_deg=-.359,verified_same_physical_zero_sessions=[7,8],
            user_same_physical_zero_confirmed=True)
        self.assertTrue(m.reference_matches(model,8,-.359))
        self.assertFalse(m.reference_matches(model,9,-.359))
        self.assertFalse(m.reference_matches(model,8,0))
        model['reference_binding']['user_same_physical_zero_confirmed']=False
        self.assertFalse(m.reference_matches(model,8,-.359))
    def model(self):
        return dict(schema=m.SCHEMA,cells=[dict(direction='left',priority=0,n=3,moved=3,
          min_deg=4,max_deg=6,mean_deg=5,start_min_deg=-4,start_max_deg=2,duration_us=20000)])

    def test_measured_interval_improves_large_error(self):
        d=m.choose(self.model(),-3,2)
        self.assertEqual(d['status'],'PULSE')
        self.assertEqual(d['duration_us'],20000)

    def test_small_unreachable_error_is_not_oscillated(self):
        self.assertEqual(m.choose(self.model(),0,1.5)['status'],'RESOLUTION_LIMIT')

    def test_noop_jump_average_is_not_fictitious_fine_motion(self):
        model=self.model();c=model['cells'][0]
        c.update(n=2,moved=1,min_deg=0,max_deg=5.5,mean_deg=2.75,responses_deg=[0,5.5])
        self.assertEqual(m.choose(model,0,2.75)['status'],'RESOLUTION_LIMIT')

    def test_large_goal_does_not_silently_expand_default_limits(self):
        with self.assertRaises(ValueError):m.choose(self.model(),0,15)
        decision=m.choose(self.model(),0,15,angle_limit=18,target_limit=15,request_limit_us=60000)
        self.assertEqual(decision['status'],'PULSE')

    def test_noop_and_undersampled_cells_cannot_claim_motion(self):
        model=self.model();model['cells'][0]['moved']=0
        self.assertEqual(m.choose(model,-3,2)['status'],'RESOLUTION_LIMIT')
        model=self.model();model['cells'][0]['n']=1
        self.assertEqual(m.choose(model,-3,2)['status'],'INSUFFICIENT_MODEL')

    def test_no_extrapolation_to_wrong_priority_or_far_start_or_outside_envelope(self):
        self.assertEqual(m.choose(self.model(),-3,2,priority=10)['status'],'INSUFFICIENT_MODEL')
        self.assertEqual(m.choose(self.model(),-9,-3)['status'],'INSUFFICIENT_MODEL')
        with self.assertRaises(ValueError):m.choose(self.model(),0,9)
        self.assertEqual(m.choose(self.model(),2,2.5)['status'],'REACHED')


if __name__=='__main__':unittest.main()
