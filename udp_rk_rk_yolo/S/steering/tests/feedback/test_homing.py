"""Synthetic plants only; homing safety/state tests never access hardware."""
from pathlib import Path
import sys
import unittest
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools/calibration'))
from homing_core import run_homing,HomeError,HomeLimits
from terminal_home import already_homed
import tempfile

class Plant:
    def __init__(self,start=0,left=20,right=-20,mode='normal'):
        self.angle=start;self.origin=start;self.left=left;self.right=right;self.mode=mode
        self.released=False;self.calls=0
    def stable(self):
        if self.mode=='stale' and self.calls>2:raise HomeError('stale')
        return self.angle-self.origin
    def pulse(self,direction,us):
        self.calls+=1
        if self.mode=='manual' and self.calls==3:raise HomeError('manual takeover')
        if self.mode=='no_power':return
        if self.mode=='wrong_sign':direction=-direction
        self.angle=max(self.right,min(self.left,self.angle+direction*3))
    def center(self,mid,tol,budget):
        self.angle=self.origin+mid+1
        return self.stable(),[30000]
    def release(self):self.released=True

class HomingTests(unittest.TestCase):
    def test_startup_receipt_requires_boot_session_and_real_reference(self):
        folder=ROOT/'build/tests';folder.mkdir(parents=True,exist_ok=True)
        with tempfile.TemporaryDirectory(dir=folder) as d:
            ref=Path(d)/'center.reference';ref.write_text('STEER_PHYSICAL_REFERENCE_V1 8 1.25 1')
            receipt=dict(schema='STEER_ENDPOINT_HOME_V1',passed=True,S_boot_id='boot-a',session=8,
                physical_straight_visually_confirmed=True,reference_file=str(ref))
            self.assertTrue(already_homed(receipt,'boot-a',8,ref))
            self.assertFalse(already_homed(receipt,'boot-b',8,ref))
            self.assertFalse(already_homed(receipt,'boot-a',9,ref))
            for key in ('passed','physical_straight_visually_confirmed'):
                changed=dict(receipt);changed[key]=False
                self.assertFalse(already_homed(changed,'boot-a',8,ref))
            ref.write_text('STEER_PHYSICAL_REFERENCE_V1 8 0 0')
            self.assertFalse(already_homed(receipt,'boot-a',8,ref))
    def test_arbitrary_boot_origins_and_endpoints(self):
        for start in [-20,-12,0,9,20]:
            with self.subTest(start=start):
                io=Plant(start);r=run_homing(io)
                self.assertTrue(r['passed']);self.assertAlmostEqual(r['span_deg'],40)
                self.assertAlmostEqual(r['midpoint_raw_deg'],-start)
                self.assertLessEqual(abs(io.angle),3);self.assertTrue(io.released)
    def test_power_failure_obstacle_wrong_direction_and_takeover(self):
        for mode in ['no_power','wrong_sign','stale','manual']:
            with self.subTest(mode=mode):
                io=Plant(mode=mode)
                with self.assertRaises(HomeError):run_homing(io)
                self.assertTrue(io.released)
        io=Plant(left=10)
        with self.assertRaises(HomeError):run_homing(io)
        self.assertTrue(io.released)
    def test_endpoint_rebound_does_not_hide_opposite_travel(self):
        class ReboundPlant(Plant):
            def pulse(self,direction,us):
                old=self.angle
                super().pulse(direction,us)
                if old==self.angle and abs(self.angle)>=18:
                    self.angle-=direction*1.1
        for start in [-20,0,20]:
            io=ReboundPlant(start);r=run_homing(io)
            self.assertTrue(r['passed']);self.assertLess(abs(r['midpoint_raw_deg']+start),1)
            self.assertTrue(io.released)
        class WeakWrongPlant(Plant):
            def pulse(self,direction,us):
                self.calls+=1;self.angle-=direction*.7
        io=WeakWrongPlant()
        with self.assertRaises(HomeError):run_homing(io)
        self.assertLessEqual(io.calls,3);self.assertTrue(io.released)
    def test_budget_and_post_center_measurement(self):
        io=Plant();io.stable=lambda:float('nan')
        with self.assertRaises(HomeError):run_homing(io)
        self.assertEqual(io.calls,0);self.assertTrue(io.released)
        io=Plant()
        with self.assertRaises(HomeError):run_homing(io,HomeLimits(max_total_us=400000))
        self.assertTrue(io.released)
        io=Plant();io.center=lambda *a:(100,[30000])
        with self.assertRaises(HomeError):run_homing(io)
        self.assertTrue(io.released)

    def test_large_rebound_needs_three_real_samples_and_full_stroke(self):
        class ElasticPlant(Plant):
            def pulse(self,direction,us):
                old=self.angle
                super().pulse(direction,us)
                if direction<0 and old==self.right and self.angle==self.right:self.angle+=2.55
        io=ElasticPlant();r=run_homing(io)
        self.assertTrue(r['passed']);self.assertTrue(io.released)
        self.assertTrue(any(p['verified_full_stroke_rebound'] for p in r['pulse_records']))
        self.assertLessEqual(abs(r['error_deg']),3)
        class NearLowerStroke(Plant):
            def pulse(self,direction,us):
                old=self.angle
                super().pulse(direction,us)
                if direction<0 and old==self.right and self.angle==self.right:self.angle+=2.77
        # One unloaded point may fall below the stroke lower limit after a
        # real stop was reached. The three-point median must still qualify.
        io=NearLowerStroke(left=15.6,right=-17.5)
        r=run_homing(io,HomeLimits(min_span_deg=31.763,max_span_deg=37.763))
        self.assertTrue(r['passed']);self.assertTrue(io.released)
        # The same reverse jump in mid-travel must not acquire a reference.
        class MidTravelReverse(Plant):
            def pulse(self,direction,us):
                if direction<0 and -5<=self.angle<=5:
                    self.calls+=1;self.angle+=2.55
                else:super().pulse(direction,us)
        io=MidTravelReverse()
        with self.assertRaises(HomeError):run_homing(io)
        self.assertTrue(io.released)

if __name__=='__main__':unittest.main()
