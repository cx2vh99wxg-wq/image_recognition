"""Display and record S-board IMU telemetry through an existing root DEBUG shell."""

import argparse
import base64
import ctypes as ct
from ctypes import wintypes as win
from datetime import datetime
from pathlib import Path
import re
import subprocess
import time


ANSI = re.compile(r'\x1b\[[0-?]*[ -/]*[@-~]')


# Capture the same packets as the installed passive reference checker. This
# socket does not bind the UDP port used by vehicle_control on the S board.
BOARD = '''
import json, socket, struct, sys, time
from pathlib import Path
sys.path.insert(0, '/home/linaro/S/steering/tools/calibration')
from terminal_center import receive_socket, decode_packet

try:
    cfg = json.loads(Path('/home/linaro/S/steering/runtime.json').read_text())
    ref = Path(cfg['environment']['STEERING_REFERENCE_FILE']).read_text().split()
    ref_session, zero = (int(ref[1]), float(ref[2])) if ref[0] == 'STEER_PHYSICAL_REFERENCE_V1' and ref[3] == '1' else (0, 0)
except (OSError, ValueError, KeyError, IndexError):
    ref_session, zero = 0, 0

with receive_socket() as sock:
    sock.settimeout(0.25)
    if ref_session:
        print(f'IMU_LISTENING saved_reference_session={ref_session} saved_zero_offset_deg={zero:.3f}; no live sample yet')
    else:
        print('IMU_LISTENING no saved zero reference; no live sample yet')
    deadline = time.monotonic() + SECONDS
    next_display = 0
    previous = None
    while time.monotonic() < deadline:
        try:
            packet = sock.recv(2048)
        except socket.timeout:
            print('IMU_WAIT no incoming packet in 250 ms')
            continue
        session = decode_packet(packet)
        if session is None:
            continue
        header_end = 22 + (packet[14] & 15) * 4
        seq, flags, angle, rate, age, sample_ns = struct.unpack_from('!IIiiIQ', packet, header_end + 12)
        angle /= 1000
        if previous and session == previous[0]:
            step = (seq - previous[1]) & 0xffffffff
            if not 0 < step < 0x80000000 or sample_ns <= previous[3]:
                continue
            peak = max(peak, abs(angle - previous[2]))
            gap = max(gap, step - 1)
            low, high = min(low, angle), max(high, angle)
        else:
            peak, gap, low, high = 0., 0, angle, angle
            next_display = 0
        previous = session, seq, angle, sample_ns
        now = time.monotonic()
        if now >= next_display:
            wheel = f'{angle - zero:+.3f}' if session == ref_session else f'{angle:+.3f}' if flags & 2 else 'NA'
            print(f'IMU session={session} seq={seq} raw={angle:+.3f} wheel={wheel} '
                  f'rate={rate/1000:+.3f} age={age} step={peak:.3f} gap={gap} low={low:+.3f} high={high:+.3f}')
            next_display = now + 0.1
            peak, gap, low, high = 0., 0, angle, angle
print('IMU_DONE')
'''


class Timeouts(ct.Structure):
    _fields_ = [(name, win.DWORD) for name in (
        'interval', 'read_multiplier', 'read_constant', 'write_multiplier', 'write_constant'
    )]


class SerialConsole:
    def __init__(self, port):
        self.api = ct.WinDLL('kernel32', use_last_error=True)
        self.api.CreateFileW.argtypes = [win.LPCWSTR, win.DWORD, win.DWORD, win.LPVOID,
                                        win.DWORD, win.DWORD, win.HANDLE]
        self.api.CreateFileW.restype = win.HANDLE
        self.api.CloseHandle.argtypes = [win.HANDLE]
        self.api.SetCommTimeouts.argtypes = [win.HANDLE, ct.POINTER(Timeouts)]
        for name in ('ReadFile', 'WriteFile'):
            getattr(self.api, name).argtypes = [win.HANDLE, win.LPVOID, win.DWORD,
                                              ct.POINTER(win.DWORD), win.LPVOID]
        self.handle = self.api.CreateFileW(rf'\\.\{port}', 0xc0000000, 0, None, 3, 0, None)
        if self.handle == ct.c_void_p(-1).value:
            raise ct.WinError(ct.get_last_error())
        if not self.api.SetCommTimeouts(self.handle, ct.byref(Timeouts(0xffffffff, 0, 0, 0, 2000))):
            error = ct.get_last_error()
            self.close()
            raise ct.WinError(error)

    def close(self):
        self.api.CloseHandle(self.handle)

    def read(self):
        buffer = ct.create_string_buffer(8192)
        count = win.DWORD()
        if not self.api.ReadFile(self.handle, buffer, len(buffer), ct.byref(count), None):
            raise ct.WinError(ct.get_last_error())
        return buffer.raw[:count.value]

    def write(self, data):
        for offset in range(0, len(data), 64):
            part = data[offset:offset + 64]
            count = win.DWORD()
            if not self.api.WriteFile(self.handle, part, len(part), ct.byref(count), None):
                raise ct.WinError(ct.get_last_error())
            if count.value != len(part):
                raise OSError('Short serial write')
            time.sleep(0.004)

    def read_for(self, seconds):
        received = bytearray()
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            received.extend(self.read())
            time.sleep(0.02)
        return received.decode('utf-8', 'replace')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', default='COM11')
    parser.add_argument('--seconds', type=int, choices=range(5, 601), default=120, metavar='5..600')
    args = parser.parse_args()
    if not re.fullmatch(r'COM[1-9][0-9]*', args.port, re.I):
        parser.error('Expected a COM port such as COM11')
    subprocess.run(['mode.com', args.port, 'BAUD=1500000', 'PARITY=N', 'DATA=8', 'STOP=1',
                    'XON=OFF', 'ODSR=OFF', 'OCTS=OFF', 'DTR=OFF', 'RTS=OFF'],
                   check=True, stdout=subprocess.DEVNULL)
    log_path = Path(__file__).with_name(datetime.now().strftime('imu_%Y%m%d_%H%M%S.log'))
    print(f'{args.port} 1500000 8N1 | live IMU | {log_path}', flush=True)
    link = SerialConsole(args.port)
    running = False
    try:
        link.write(b'\r')
        startup = ANSI.sub('', link.read_for(0.5))
        if not re.search(r'root@[^\r\n]+#\s*$', startup):
            raise RuntimeError(f'Expected the existing S-board root shell, got {startup!r}')
        encoded = base64.b64encode((f'SECONDS={args.seconds}\n' + BOARD).encode()).decode()
        command = ('ip -4 -o addr show | grep -q "192.168.100.20/" && '
                   f'python3 -B -u -c \'import base64;exec(base64.b64decode("{encoded}"))\' '
                   '|| echo IMU_ERROR\r')
        link.write(command.encode('ascii'))
        running = True
        pending = ''
        with log_path.open('w', encoding='utf-8', buffering=1) as log:
            deadline = time.monotonic() + args.seconds + 8
            while time.monotonic() < deadline:
                pending += link.read().decode('utf-8', 'replace').replace('\r', '')
                lines = pending.split('\n')
                pending = lines.pop()
                for line in lines:
                    line = ANSI.sub('', line)
                    if line.startswith(('IMU', 'Traceback', 'ModuleNotFoundError', 'PermissionError')):
                        record = f'{datetime.now().strftime("%H:%M:%S.%f")[:-3]} {line}'
                        print(record, flush=True)
                        log.write(record + '\n')
                    if line == 'IMU_DONE' or line == 'IMU_ERROR':
                        running = False
                        return 0 if line == 'IMU_DONE' else 1
                time.sleep(0.02)
        raise TimeoutError('No end marker from the board IMU monitor')
    finally:
        try:
            if running:
                link.write(b'\x03')
        finally:
            link.close()


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except KeyboardInterrupt:
        pass
