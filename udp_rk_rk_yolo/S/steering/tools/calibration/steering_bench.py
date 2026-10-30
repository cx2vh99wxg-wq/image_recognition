"""Record ONE human-observed bench pulse with real M->S telemetry; never replay it.

Uses the production C receiver and bounded C actuator. Relative marked zero is
bound to one M session; this does not grant mechanical ZERO_VALID or road control.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import csv
import json
import math
import os
from pathlib import Path
import re
import selectors
import signal
import statistics
import subprocess
import time

ROOT = Path(__file__).resolve().parents[2]


def timing_valid(requested_us, actual_ms):
    return math.isfinite(actual_ms) and -.05 <= actual_ms-requested_us/1000 <= 2


def fields(line):
    return dict(re.findall(r'(\w+)=([^\s]+)', line))


def checked_sample(line, session, now_ns, timeout_ms=100):
    if not line.startswith('RX '):
        return None
    f = fields(line)
    if int(f['session']) != session:
        raise RuntimeError('M reference session changed; re-confirm physical zero')
    flags = int(f['flags'])
    if flags not in (1, 3) or f['status'] not in ('REAL_RELATIVE', 'REAL_ZERO_VALID'):
        raise RuntimeError('Feedback is not fresh real angle telemetry')
    stamp = int(f['rx_local_ns'])
    age = int(f['source_age_ms']) * 1_000_000 + now_ns - stamp
    if stamp <= 0 or now_ns < stamp or age < 0 or age >= timeout_ms * 1_000_000:
        raise RuntimeError('Feedback expired (network transit delay is not measured)')
    angle, rate = float(f['raw_relative_deg']), float(f['rate_dps'])
    if not math.isfinite(angle) or not math.isfinite(rate):
        raise RuntimeError('Nonfinite feedback')
    return dict(rx_local_ns=stamp, session=session, sequence=int(f['seq']),
                raw_relative_deg=angle, rate_dps=rate, source_age_ms=int(f['source_age_ms']))


def validate_request(direction, duration_us, guard_step, limit, request_limit_us=20000):
    if direction not in ('left', 'right') or not 1 <= duration_us <= request_limit_us <= 60000:
        raise ValueError('Bench request exceeds the explicit request limit (absolute cap 60000us)')
    if not all(math.isfinite(v) for v in (guard_step,limit)) or not 0 < limit <= 18 or not 0 < guard_step <= 2*limit:
        raise ValueError('Invalid bench envelope')


def validate_pulse(direction, duration_us, baseline, guard_step, limit, request_limit_us=20000):
    validate_request(direction,duration_us,guard_step,limit,request_limit_us)
    if not math.isfinite(baseline):
        raise ValueError('Invalid baseline')
    predicted = baseline + (guard_step if direction == 'left' else -guard_step)
    if abs(baseline) > limit or abs(predicted) > limit:
        raise RuntimeError('Refusing outward pulse: insufficient planned angle margin')


def baseline_of(samples, zero):
    if len(samples) < 30 or samples[-1]['rx_local_ns'] - samples[0]['rx_local_ns'] < 400_000_000:
        raise RuntimeError('Insufficient continuous stationary baseline')
    angles = [s['raw_relative_deg'] for s in samples]
    if max(angles) - min(angles) > .4 or max(abs(s['rate_dps']) for s in samples) > 2:
        raise RuntimeError('Steering is not settled')
    return statistics.median(angles) - zero


def status():
    if not Path('/tmp/steering-bench.active').is_file():
        raise RuntimeError('Bench isolation marker missing')
    p = subprocess.run(['pgrep', '-x', 'vehicle_control'], capture_output=True)
    if p.returncode != 1:
        raise RuntimeError('Legacy controller running or process check failed')
    p = subprocess.run(['fuser', '/dev/spidev4.0'], capture_output=True)
    if p.returncode != 1:
        raise RuntimeError('SPI owner present or ownership check failed')
    p = subprocess.run([str(ROOT/'actuator/build/steering_status')],
                       text=True, capture_output=True, timeout=3)
    if p.returncode:
        raise RuntimeError('No valid control snapshot: ' + p.stdout + p.stderr)
    f = fields(p.stdout)
    expected = dict(go_n='1', back_n='1', left_n='1', right_n='1', stop_n='1',
                    stop='0', auto='0', fs_armed='1', reset_released='1',
                    left_denied='0', right_denied='0')
    if any(f.get(k) != v for k, v in expected.items()):
        raise RuntimeError('Contacts/gates are not ready: ' + p.stdout)
    return p.stdout


def status_while_receiving(monitor):
    # SPI status initialization can take >100ms. Keep draining the receiver;
    # never relabel old pipe contents as fresh or relax the feedback deadline.
    with ThreadPoolExecutor(max_workers=1) as executor:
        check = executor.submit(status)
        while not check.done():
            monitor.collect(.04)
        result = check.result()
    monitor.collect(.05)
    return result


class Telemetry:
    def __init__(self, prefix, session, zero, limit):
        self.samples, self.pending = [], b''
        self.session, self.zero, self.limit = session, zero, limit
        self.log = open(str(prefix)+'.receiver.log', 'x', buffering=1)
        self.process = subprocess.Popen([str(ROOT/'feedback/build/steering_feedback_monitor'),
            '--bind', '192.168.100.20', '--source', '192.168.100.10', '--port', '54321',
            '--duration-ms', '20000', '--print-every', '1', '--csv', str(prefix)+'.telemetry.csv'],
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        self.selector = selectors.DefaultSelector()
        self.selector.register(self.process.stdout, selectors.EVENT_READ)

    def collect(self, seconds, allow_initial=False, actuator=None):
        end = time.monotonic() + seconds
        start_count = len(self.samples)
        while time.monotonic() < end:
            if self.process.poll() is not None:
                raise RuntimeError('Telemetry monitor exited unexpectedly')
            for key, _ in self.selector.select(.01):
                data = os.read(key.fileobj.fileno(), 65536)
                if not data:
                    raise RuntimeError('Telemetry pipe closed')
                self.pending += data
                while b'\n' in self.pending:
                    raw, self.pending = self.pending.split(b'\n', 1)
                    line = raw.decode('utf-8', errors='replace')
                    self.log.write(line+'\n')
                    if line.startswith('SESSION_CHANGE '):
                        raise RuntimeError('Reference session changed')
                    if line.startswith('STATUS ') and line.split()[1] not in ('REAL_RELATIVE', 'REAL_ZERO_VALID'):
                        if self.samples or not allow_initial:
                            raise RuntimeError('Telemetry status fault: '+line)
                    sample = checked_sample(line, self.session, time.monotonic_ns())
                    if sample:
                        if self.samples and sample['rx_local_ns'] <= self.samples[-1]['rx_local_ns']:
                            raise RuntimeError('Local receive clock did not advance')
                        self.samples.append(sample)
                        if abs(sample['raw_relative_deg']-self.zero) > self.limit:
                            raise RuntimeError('Observed angle exceeds planned bench envelope')
            now = time.monotonic_ns()
            if self.samples:
                last = self.samples[-1]
                if now-last['rx_local_ns'] + last['source_age_ms']*1_000_000 >= 100_000_000:
                    raise RuntimeError('No fresh feedback within 100ms')
            elif not allow_initial:
                raise RuntimeError('No initial angle feedback')
            if actuator is not None and actuator.poll() is not None:
                if actuator.returncode:
                    raise RuntimeError('Pulse failed; never retry automatically')
                break
        if len(self.samples) == start_count and actuator is None:
            raise RuntimeError('No new samples in observation window')

    def close(self):
        if self.process.poll() is None:
            self.process.send_signal(signal.SIGTERM)
        self.process.wait(timeout=3)
        self.selector.close()
        self.log.close()


def main():
    def interrupted(signum, frame):
        raise KeyboardInterrupt('Bench interrupted')
    signal.signal(signal.SIGTERM, interrupted)
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--trial', required=True)
    parser.add_argument('--direction', required=True, choices=['left', 'right'])
    parser.add_argument('--duration-us', required=True, type=int)
    parser.add_argument('--reference-session', required=True, type=int)
    parser.add_argument('--zero-raw-deg', required=True, type=float)
    parser.add_argument('--guard-step-deg', type=float, default=8)
    parser.add_argument('--angle-limit-deg', type=float, default=10,
                        help='Explicit observed-angle envelope; absolute cap 18deg')
    parser.add_argument('--request-limit-us', type=int, default=20000,
                        help='Explicit pulse request envelope; absolute cap 60000us')
    parser.add_argument('--expected-start-deg', type=float,
                        help='Stop before action if fresh baseline differs by >0.5deg')
    parser.add_argument('--realtime-priority', type=int, default=0,
                        help='0: normal; 1..30: FIFO only for bounded actuator child')
    args = parser.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9_-]{1,80}', args.trial):
        parser.error('Use a new simple trial identifier')
    validate_request(args.direction,args.duration_us,args.guard_step_deg,args.angle_limit_deg,args.request_limit_us)
    if not 0 < args.reference_session <= 0xffffffff or not math.isfinite(args.zero_raw_deg):
        parser.error('Invalid reference')
    if not 0 <= args.realtime_priority <= 30:
        parser.error('Realtime priority must be 0..30')
    prefix = ROOT/'logs'/args.trial
    prefix.parent.mkdir(exist_ok=True)
    report = dict(trial=args.trial, reference_session=args.reference_session,
                  zero_raw_deg=args.zero_raw_deg, direction=args.direction,
                  requested_us=args.duration_us, max_request_us=args.request_limit_us,
                  planned_angle_limit_deg=args.angle_limit_deg, guard_step_deg=args.guard_step_deg,
                  actuator_realtime_priority=args.realtime_priority,
                  mechanical_zero_valid_granted=False, absolute_network_delay_measured=False,
                  pulse_launch_attempted=False, result='RESERVED')
    # A reservation survives errors/timeouts. Reusing its ID can NEVER launch again.
    with open(str(prefix)+'.reservation.json', 'x') as f:
        json.dump(report, f, indent=2)
    monitor, actuator, pulse_log = None, None, None
    try:
        report['preflight'] = status()
        if args.realtime_priority:
            subprocess.run(['chrt', '-f', str(args.realtime_priority), 'true'], check=True, timeout=3)
        monitor = Telemetry(prefix, args.reference_session, args.zero_raw_deg, args.angle_limit_deg)
        monitor.collect(1, allow_initial=True)
        report['pre_pulse_status'] = status_while_receiving(monitor)
        before = baseline_of(monitor.samples[-60:], args.zero_raw_deg)
        if args.expected_start_deg is not None:
            if not math.isfinite(args.expected_start_deg) or abs(before-args.expected_start_deg)>.5:
                raise RuntimeError('Fresh baseline differs from target-controller decision')
        validate_pulse(args.direction,args.duration_us,before,args.guard_step_deg,args.angle_limit_deg,args.request_limit_us)
        if abs((monitor.samples[-1]['raw_relative_deg']-args.zero_raw_deg)-before) > .4:
            raise RuntimeError('Baseline moved before pulse')
        report['before_deg'] = before
        report['pulse_launch_attempted'] = True
        report['launch_local_ns'] = time.monotonic_ns()
        pulse_log = open(str(prefix)+'.actuator.log', 'x')
        command = [str(ROOT/'actuator/build/steering_step_fspi'),
            '--backend', 'fspi', '--direction', args.direction, '--duration-us', str(args.duration_us),
            '--max-pulse-ms', str((args.request_limit_us+999)//1000), '--csv', str(prefix)+'.pulse.csv']
        if args.realtime_priority:
            command = ['chrt', '-f', str(args.realtime_priority)] + command
        actuator = subprocess.Popen(command, stdout=pulse_log, stderr=subprocess.STDOUT)
        monitor.collect(2, actuator=actuator)
        if actuator.poll() is None:
            raise RuntimeError('Actuator did not finish promptly')
        report['actuator_finished_local_ns'] = time.monotonic_ns()
        monitor.collect(2.3)
        report['after_deg'] = baseline_of(monitor.samples[-60:], args.zero_raw_deg)
        report['delta_deg'] = report['after_deg'] - before
        if report['delta_deg']*(1 if args.direction=='left' else -1)<-.4:
            raise RuntimeError('Measured movement opposes commanded direction')
        report['postflight'] = status()
        report['result'] = 'RECORDED'
    except (Exception, KeyboardInterrupt) as exc:
        report['result'], report['error'] = 'STOPPED', str(exc) or type(exc).__name__
    finally:
        if actuator is not None and actuator.poll() is None:
            try:
                actuator.send_signal(signal.SIGTERM)
                actuator.wait(timeout=3)
            except Exception as exc:
                report['result'] = 'STOPPED'
                report['release_state_unknown'] = True
                report['actuator_cleanup_error'] = str(exc)
        if pulse_log:
            pulse_log.close()
        if monitor:
            try:
                monitor.close()
            except Exception as exc:
                report['result'] = 'STOPPED'
                report['monitor_cleanup_error'] = str(exc)
        pulse_csv = Path(str(prefix)+'.pulse.csv')
        if pulse_csv.exists():
            with pulse_csv.open() as f:
                report['pulse_records'] = list(csv.DictReader(f))
            if report['result']=='RECORDED':
                records=report['pulse_records']
                if len(records)!=1 or not timing_valid(args.duration_us,float(records[0]['software_elapsed_ms'])):
                    report['result']='TIMING_FAULT'
                    report['error']='Software pulse timing outside request -0.05/+2ms; exclude from response model'
        if monitor and monitor.samples:
            report['samples'] = len(monitor.samples)
            report['observed_min_deg'] = min(s['raw_relative_deg']-args.zero_raw_deg for s in monitor.samples)
            report['observed_max_deg'] = max(s['raw_relative_deg']-args.zero_raw_deg for s in monitor.samples)
            report['last_observed_deg'] = monitor.samples[-1]['raw_relative_deg']-args.zero_raw_deg
        with open(str(prefix)+'.json', 'x') as f:
            json.dump(report, f, indent=2)
        print('BENCH_REPORT='+json.dumps(report), flush=True)
    return 0 if report['result']=='RECORDED' else 1


if __name__ == '__main__':
    raise SystemExit(main())
