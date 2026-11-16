"""Run M's foreground angle_monitor and display its output in Windows."""

import argparse
import re
import subprocess
import time

from watch_s_imu import ANSI, SerialConsole


START_MONITOR = (
    'if ip -4 -o addr show | grep -q "192.168.100.10/"; then '
    'systemctl stop m-steering-imu && '
    '/home/linaro/M/imu/build/angle_monitor --backend fpga '
    '--calibration /home/linaro/M/imu/calibration/startup_bias.current.cal '
    '--stationary-hold --samples 0 --interval-ms 1 --print-every 10 --wait-device; '
    'else echo "M-board IP address 192.168.100.10 was not found"; false; fi; '
    'echo "M_IMU_EXIT=$?"\r'
)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', default='COM7', help='M-board DEBUG serial port (default COM7)')
    args = parser.parse_args()
    if not re.fullmatch(r'COM[1-9][0-9]*', args.port, re.I):
        parser.error('Expected a COM port such as COM7')

    subprocess.run(['mode.com', args.port, 'BAUD=1500000', 'PARITY=N', 'DATA=8', 'STOP=1',
                    'XON=OFF', 'ODSR=OFF', 'OCTS=OFF', 'DTR=OFF', 'RTS=OFF'],
                   check=True, stdout=subprocess.DEVNULL)
    link = SerialConsole(args.port)
    running = False
    try:
        link.write(b'\r')
        prompt = ANSI.sub('', link.read_for(.5))
        if not re.search(r'root@[^@#\r\n]+:[^#\r\n]*#\s*$', prompt):
            raise RuntimeError(f'Open the M-board root shell in the serial console first. Prompt: {prompt!r}')

        print(f'{args.port} 1500000 8N1 | M IMU | read relative_deg | Ctrl+C to stop', flush=True)
        link.write(START_MONITOR.encode('ascii'))
        running = True
        pending = ''
        while True:
            pending += link.read().decode('utf-8', 'replace').replace('\r', '')
            lines = pending.split('\n')
            pending = lines.pop()
            for line in lines:
                line = ANSI.sub('', line)
                status = re.fullmatch(r'M_IMU_EXIT=(\d+)', line.strip())
                if status:
                    running = False
                    return int(status.group(1))
                if line and 'echo "M_IMU_EXIT=' not in line:
                    print(line, flush=True)
            time.sleep(.02)
    finally:
        try:
            if running:
                link.write(b'\x03')
                link.read_for(.25)
        finally:
            link.close()


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except KeyboardInterrupt:
        pass
