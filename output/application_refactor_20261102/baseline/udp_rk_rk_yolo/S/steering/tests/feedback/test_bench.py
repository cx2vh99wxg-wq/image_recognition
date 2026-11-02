"""No hardware: reject unsafe reference/freshness and preserve measured increments."""
import importlib.util
from pathlib import Path
import unittest
from unittest.mock import patch
import time

path = Path(__file__).resolve().parents[2]/'tools/calibration/steering_bench.py'
spec = importlib.util.spec_from_file_location('steering_bench', path)
b = importlib.util.module_from_spec(spec)
spec.loader.exec_module(b)


class BenchTest(unittest.TestCase):
    def test_timing_outlier_not_used_as_requested_duration(self):
        self.assertTrue(b.timing_valid(20000,20.3))
        self.assertFalse(b.timing_valid(17750,28.898))
        self.assertFalse(b.timing_valid(20000,17.5))
        self.assertFalse(b.timing_valid(20000,float('nan')))

    def test_wide_range_requires_explicit_envelope_and_margin(self):
        b.validate_pulse('left',40000,-10,22,18,60000)
        with self.assertRaises(ValueError):b.validate_pulse('left',40000,-10,22,18)
        with self.assertRaises(RuntimeError):b.validate_pulse('left',40000,0,22,18,60000)
        with self.assertRaises(ValueError):b.validate_pulse('left',61000,-10,22,18,60000)
        with self.assertRaises(ValueError):b.validate_pulse('left',20000,0,8,21,60000)

    def line(self, **changes):
        fields = dict(session=7, seq=20, flags=1, raw_relative_deg=-2.5,
                      rate_dps=0, source_age_ms=3, status='REAL_RELATIVE', rx_local_ns=1000000000)
        fields.update(changes)
        return 'RX '+' '.join(str(k)+'='+str(v) for k,v in fields.items())

    def test_real_relative_allowed_without_granting_mechanical_zero(self):
        s = b.checked_sample(self.line(), 7, 1001000000)
        self.assertEqual(s['raw_relative_deg'], -2.5)

    def test_bad_session_stale_future_invalid_simulated_and_nonfinite_rejected(self):
        for changes in [dict(session=8), dict(source_age_ms=100), dict(rx_local_ns=1),
                        dict(rx_local_ns=2000000000), dict(flags=5), dict(flags=0),
                        dict(status='STALE'), dict(raw_relative_deg='nan'), dict(rate_dps='inf')]:
            with self.subTest(changes=changes), self.assertRaises(RuntimeError):
                b.checked_sample(self.line(**changes), 7, 1001000000)

    def test_initial_outward_margin_and_inward_return(self):
        b.validate_pulse('left', 17750, 0, 8, 10)
        b.validate_pulse('right', 17750, 4, 8, 10)
        with self.assertRaises(RuntimeError):
            b.validate_pulse('left', 17750, 4, 8, 10)
        for duration in [0, 20001]:
            with self.assertRaises(ValueError):
                b.validate_pulse('left', duration, 0, 8, 10)

    def test_baseline_must_be_settled_and_long_enough(self):
        samples = [dict(rx_local_ns=1000000000+i*10000000,
                        raw_relative_deg=.151, rate_dps=0) for i in range(60)]
        self.assertAlmostEqual(b.baseline_of(samples, .151), 0)
        with self.assertRaises(RuntimeError):
            b.baseline_of(samples[:5], .151)
        samples[-1]['raw_relative_deg'] = 1.5
        with self.assertRaises(RuntimeError):
            b.baseline_of(samples, .151)

    def test_slow_status_does_not_pause_receiver(self):
        class Receiver:
            calls = 0
            def collect(self, seconds):
                self.calls += 1
                time.sleep(.01)
        receiver = Receiver()
        def slow_status():
            time.sleep(.15)
            return 'released'
        with patch.object(b, 'status', slow_status):
            self.assertEqual(b.status_while_receiving(receiver), 'released')
        self.assertGreater(receiver.calls, 3)


if __name__=='__main__':
    unittest.main()
