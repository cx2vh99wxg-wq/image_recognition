"""Reference acceptance tests only. No processes, network or hardware access."""
from pathlib import Path
import copy
import sys
import unittest
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools/calibration'))
from confirm_center import validate_center

class CenterTests(unittest.TestCase):
    def setUp(self):
        self.samples=[dict(session=7,sequence=i+1,rx_local_ns=1000000000+i*10000000,
                           raw_relative_deg=.204,rate_dps=0,source_age_ms=3) for i in range(100)]
    def test_measured_reference_preserves_raw_offset(self):
        self.assertEqual(validate_center(self.samples),(7,.204))
    def test_missing_and_short_baseline(self):
        with self.assertRaises(ValueError):validate_center(self.samples[:20])
        for s in self.samples:s['rx_local_ns']=1000000000+s['sequence']*1000000
        with self.assertRaises(ValueError):validate_center(self.samples)
    def test_session_gap_duplicates_and_motion(self):
        for field,value in [('session',8),('rx_local_ns',1),('sequence',1),
                            ('source_age_ms',100),('raw_relative_deg',1.),('rate_dps',3.),('rate_dps',float('nan'))]:
            with self.subTest(field=field,value=value):
                samples=copy.deepcopy(self.samples);samples[50][field]=value
                with self.assertRaises(ValueError):validate_center(samples)

if __name__=='__main__':unittest.main()
