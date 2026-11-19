"""Transfer poster screenshots through the existing board DEBUG consoles."""

import argparse
import base64
import ctypes
from ctypes import wintypes
import hashlib
from pathlib import Path
import re
import runpy
import subprocess
import time
import uuid
import zlib

HELPER = Path(__file__).resolve().parents[1] / 'imu_com11_20261116/watch_s_imu.py'
SerialConsole = runpy.run_path(str(HELPER))['SerialConsole']
ANSI = re.compile(r'\x1b\[[0-?]*[ -/]*[@-~]')

def encoded_argument(value):
    return '"' + re.sub('fiq', lambda m: m[0][:2] + '"+"' + m[0][2:], value,
                        flags=re.IGNORECASE) + '"'


def execute(side, script, timeout=45):
    port = {'M': 'COM7', 'S': 'COM11'}[side]
    subprocess.run(['mode.com', port, 'BAUD=1500000', 'PARITY=N', 'DATA=8', 'STOP=1',
                    'XON=OFF', 'ODSR=OFF', 'OCTS=OFF', 'DTR=OFF', 'RTS=OFF'],
                   check=True, stdout=subprocess.DEVNULL)
    tag = 'POSTER_' + uuid.uuid4().hex
    body = ("import base64,zlib\n"
            f"print({tag + '_BEGIN'!r},flush=True)\n"
            "try:\n"
            f" exec(zlib.decompress(base64.b64decode({base64.b64encode(zlib.compress(script.encode())).decode()!r})))\n"
            "except Exception:\n import traceback;traceback.print_exc()\n"
            f"print({tag + '_END'!r},flush=True)\n")
    encoded = base64.b64encode(zlib.compress(body.encode())).decode()
    command = f"python3 -u -c 'import base64,zlib;exec(zlib.decompress(base64.b64decode({encoded_argument(encoded)})))'\r"
    if len(command) > 3800:
        raise ValueError('Serial console command exceeds line limit')
    link = SerialConsole(port)
    link.api.SetupComm.argtypes = [wintypes.HANDLE, wintypes.DWORD, wintypes.DWORD]
    link.api.SetupComm(link.handle, 1048576, 1048576)
    try:
        link.write(b'\r')
        prompt = ANSI.sub('', link.read_for(.35))
        if not re.search(r'root@[^#\r\n]+#\s*$', prompt):
            raise RuntimeError(f'{side} console is busy: {prompt[-200:]}')
        link.write(command.encode())
        text = ''
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            text += link.read().decode('utf-8', 'replace').replace('\r', '')
            if tag + '_END\n' in text:
                return ANSI.sub('', text.split(tag + '_BEGIN\n', 1)[1].split(tag + '_END\n', 1)[0])
            time.sleep(.01)
        raise TimeoutError(f'{side} command timed out: {text[-1200:]}')
    finally:
        link.close()


def get(side, remote, local):
    script = ("from pathlib import Path\nimport base64,hashlib,zlib,time\n"
              "console=Path('/proc/sys/kernel/printk')\nprevious=console.read_text()\nconsole.write_text('1')\n"
              "try:\n"
              f" b=Path({remote!r}).read_bytes()\n"
              " packed=zlib.compress(b)\n"
              f" Path({remote + '.poster-z'!r}).write_bytes(packed)\n"
              " print('FILE_SHA',hashlib.sha256(b).hexdigest(),len(b),len(packed),flush=True)\n"
              " for i in range(0,len(packed),768):\n"
              "  part=packed[i:i+768]\n"
              "  print('DATA',i//768,format(zlib.crc32(part),'08x'),base64.b64encode(part).decode(),flush=True)\n"
              "  time.sleep(.006)\n"
              " print('FILE_END',flush=True)\n"
              "finally:console.write_text(previous)\n")
    output = execute(side, script, 55)
    header = re.search(r'^FILE_SHA ([a-f0-9]{64}) (\d+) (\d+)$', output, re.MULTILINE)
    if not header:
        raise RuntimeError(output[:1000])
    expected, length, packed_length = header.groups()
    count = (int(packed_length)+767)//768
    chunks = {}

    def collect(text):
        for match in re.finditer(r'^DATA (\d+) ([a-f0-9]{8}) ([A-Za-z0-9+/=]+)$', text,
                                 re.MULTILINE):
            try:
                block = base64.b64decode(match[3], validate=True)
                if zlib.crc32(block) == int(match[2], 16):
                    chunks[int(match[1])] = block
            except ValueError:
                continue

    collect(output)
    for attempt in range(3):
        missing = [index for index in range(count) if index not in chunks]
        if not missing:
            break
        repair = ("from pathlib import Path\nimport zlib,base64,time\n"
                  f"b=Path({remote + '.poster-z'!r}).read_bytes()\n"
                  f"for i in {missing!r}:\n"
                  " part=b[i*768:(i+1)*768]\n"
                  " print('DATA',i,format(zlib.crc32(part),'08x'),base64.b64encode(part).decode(),flush=True)\n"
                  " time.sleep(.008)\n")
        collect(execute(side, repair, 55))
    if len(chunks) != count:
        raise RuntimeError('Incomplete screenshot transfer')
    data = zlib.decompress(b''.join(chunks[index] for index in range(count)))
    if len(data) != int(length) or hashlib.sha256(data).hexdigest() != expected:
        raise RuntimeError('Downloaded screenshot failed its hash check')
    destination = Path(local)
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(data)
    print(destination, len(data), expected)


def put(side, local, remote):
    data = Path(local).read_bytes()
    payload = base64.b64encode(zlib.compress(data)).decode()
    for start in range(0, len(payload), 2000):
        part = payload[start:start + 2000]
        path = remote + '.b64'
        execute(side, f"from pathlib import Path\np=Path({path!r})\np.parent.mkdir(parents=True,exist_ok=True)\n"
                f"with p.open({'w' if start == 0 else 'a'!r}) as f:f.write({part!r})")
    result = execute(side, "from pathlib import Path\nimport base64,zlib,hashlib\n"
                     f"p=Path({remote!r})\np.write_bytes(zlib.decompress(base64.b64decode(Path({remote + '.b64'!r}).read_text())))\n"
                     f"assert hashlib.sha256(p.read_bytes()).hexdigest()=={hashlib.sha256(data).hexdigest()!r}\n"
                     "print('UPLOAD_OK',len(p.read_bytes()))")
    print(result)


def put_archive(side, local, remote):
    """Stream hex so the board's reserved FIQ console escape cannot occur."""
    data = Path(local).read_bytes()
    payload = data.hex()
    rows = [payload[i:i+2000] for i in range(0, len(payload), 2000)]
    digest = hashlib.sha256(data).hexdigest()
    tag = 'PUT_' + uuid.uuid4().hex
    script = ("import base64,hashlib,sys\nfrom pathlib import Path\n"
              f"print({tag + '_READY'!r},flush=True)\n"
              f"s=''.join(sys.stdin.readline().strip() for _ in range({len(rows)}))\n"
              "b=bytes.fromhex(s)\n"
              f"assert hashlib.sha256(b).hexdigest()=={digest!r}\n"
              f"p=Path({remote!r});p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(b)\n"
              f"print({tag + '_OK'!r},flush=True)\n")
    encoded = base64.b64encode(script.encode()).decode()
    command = f"stty -echo; python3 -u -c 'import base64;exec(base64.b64decode({encoded_argument(encoded)}))'; stty echo\r"
    port = {'M': 'COM7', 'S': 'COM11'}[side]
    link = SerialConsole(port)
    uploading = False
    try:
        link.write(b'\r')
        if 'root@' not in link.read_for(.35):
            raise RuntimeError('Board console is busy')
        link.write(command.encode())
        output = ''
        deadline = time.monotonic()+8
        while tag+'_READY\n' not in output:
            output += link.read().decode('utf-8','replace').replace('\r','')
            if time.monotonic()>deadline:raise TimeoutError(output[-1000:])
            time.sleep(.01)
        uploading = True
        for row in rows:
            blob = (row+'\n').encode()
            for offset in range(0,len(blob),256):
                part = blob[offset:offset+256]
                count = wintypes.DWORD()
                if not link.api.WriteFile(link.handle,part,len(part),ctypes.byref(count),None):
                    raise ctypes.WinError(ctypes.get_last_error())
                if count.value!=len(part):raise OSError('Short archive write')
                time.sleep(.004)
        deadline = time.monotonic()+12
        while tag+'_OK' not in output:
            output += link.read().decode('utf-8','replace')
            if time.monotonic()>deadline:raise TimeoutError(output[-1200:])
            time.sleep(.01)
        uploading = False
        print('ARCHIVE_OK',len(data),digest)
    finally:
        if uploading:
            link.write(b'\x03')
            link.write(b'stty echo\r')
        link.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('operation', choices=['run', 'get', 'put'])
    parser.add_argument('side', choices=['M', 'S'])
    parser.add_argument('source')
    parser.add_argument('destination', nargs='?')
    args = parser.parse_args()
    if args.operation == 'run':
        print(execute(args.side, Path(args.source).read_text(encoding='utf-8')))
    elif args.operation == 'get':
        get(args.side, args.source, args.destination)
    else:
        put(args.side, args.source, args.destination)
