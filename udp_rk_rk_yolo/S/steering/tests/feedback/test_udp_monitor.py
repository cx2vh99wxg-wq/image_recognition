"""Native Linux UDP integration/fault tests. No SPI, sensor or actuator is opened."""
import argparse
import csv
import re
import select
import socket
import struct
import subprocess
import tempfile
import time
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PROGRAM = ROOT / 'feedback/build/steering_feedback_monitor'
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--m-tool', type=Path, help='Optional independently built M angle_monitor for mock end-to-end test')
args = parser.parse_args()


def frame(session, seq, flags=1, angle=-4321, stamp=None, age=0):
    data = struct.pack('!4sHHIIIiiIQI', b'STF1', 1, 48, session, seq, flags,
                       angle, 0, age, stamp if stamp is not None else 10**9 + seq*10**7, 0)
    return data + struct.pack('!I', zlib.crc32(data))


def start(path, *extra):
    p = subprocess.Popen([str(PROGRAM), '--bind', '127.0.0.1', '--source', '127.0.0.1',
         '--port', '0', '--duration-ms', '1200', '--timeout-ms', '100', '--print-every', '1',
         '--csv', str(path), *extra], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if not select.select([p.stdout], [], [], 3)[0]:
        p.kill(); raise AssertionError('Monitor did not start')
    line = p.stdout.readline()
    match = re.search(r'port=(\d+)', line)
    if not match:
        out, err = p.communicate(timeout=3)
        raise AssertionError((line, out, err))
    return p, ('127.0.0.1', int(match.group(1)))


with tempfile.TemporaryDirectory(prefix='feedback_udp_') as directory:
    path = Path(directory)/'faults.csv'
    p, target = start(path)
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            def send(data):
                sock.sendto(data, target); time.sleep(.01)
            send(frame(1, 1))
            send(frame(1, 1))  # duplicate cannot renew expiry
            send(frame(1, 0))  # old sequence
            bad = bytearray(frame(1, 2)); bad[-1] ^= 1; send(bad)
            send(frame(1, 2)[:-1])
            send(frame(1, 2) + b'x')
            send(frame(1, 2, flags=3, angle=12000))
            send(frame(2, 3, flags=1, angle=0))  # runtime relative zero; new session
            send(frame(1, 4))  # retired session
            send(frame(2, 4, flags=5, angle=500))  # simulated never becomes real
            send(frame(2, 5, flags=1, age=1000))  # sender-expired
            send(frame(2, 6, flags=1, angle=-9876))
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as foreign:
            foreign.bind(('127.0.0.2',0)); foreign.sendto(frame(99,1),target)
        out, err = p.communicate(timeout=4)
        assert p.returncode == 0, (out, err)
        assert 'session_changes=1' in out and 'final_status=STALE' in out, out
        assert 'accepted=6 rejected=6' in out and 'actuator_enabled=0' in out, out
        assert 'foreign=1' in out, out
        with path.open() as handle:
            rows = list(csv.DictReader(handle))
        good = [r for r in rows if r['accepted'] == '1']
        receive_stamps = [int(v) for v in re.findall(r'\brx_local_ns=(\d+)', out)]
        assert receive_stamps == [int(r['rx_local_ns']) for r in good], out
        assert [r['cache_status'] for r in good] == ['REAL_RELATIVE','REAL_ZERO_VALID',
            'REAL_RELATIVE','SIMULATED','STALE','REAL_RELATIVE'], good
        assert good[-1]['raw_relative_deg'] == '-9.876'
        assert not any(r['cache_status'] == 'REAL_ZERO_VALID' and r['flags'] != '3' for r in good)
    finally:
        if p.poll() is None: p.kill(); p.communicate()
    saved = path.read_bytes()
    refused = subprocess.run([str(PROGRAM), '--port', '0', '--duration-ms', '1', '--csv', str(path)],
                             capture_output=True, timeout=3)
    assert refused.returncode == 1 and path.read_bytes() == saved
    silent = subprocess.run([str(PROGRAM), '--bind', '127.0.0.1', '--port', '0', '--duration-ms', '80'],
                            capture_output=True, text=True, timeout=3)
    assert silent.returncode == 1 and 'NO_SAMPLE' in silent.stdout
    if args.m_tool:
        p, target = start(Path(directory)/'m_sender.csv')
        try:
            sender = subprocess.run([str(args.m_tool), '--backend', 'mock', '--samples', '20',
                '--interval-ms', '10', '--mock-rate-dps', '30', '--send-to', target[0],
                '--port', str(target[1])], text=True, capture_output=True, timeout=4)
            assert sender.returncode == 0, (sender.stdout, sender.stderr)
            out, err = p.communicate(timeout=4)
            assert p.returncode == 0 and 'accepted=20 rejected=0' in out, (out, err)
            assert 'real_relative=0 zero_flagged=0 simulated=20' in out, out
            with (Path(directory)/'m_sender.csv').open() as handle:
                received = list(csv.DictReader(handle))
            assert len(received) == 20 and all(r['cache_status'] == 'SIMULATED' for r in received)
            assert float(received[-1]['raw_relative_deg']) > float(received[0]['raw_relative_deg'])
        finally:
            if p.poll() is None: p.kill(); p.communicate()
        print('PASS production M mock sender -> production S monitor: 20 ordered STF1 samples remain simulated')

print('PASS UDP monitor: relative/zero/simulated isolation, CRC/length, duplicate/order/session faults, expiry, silence, signed angles and CSV protection')
