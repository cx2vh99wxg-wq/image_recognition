import hashlib
import os
from pathlib import Path
import re
import subprocess
import sys
import tarfile
import time
import urllib.request

root = Path('/userdata/poster_frames_20261119/paired_current')
app = Path('/home/linaro/S/x11/csrc')
samples = root / 'evidence'
screens = root / 'screens'
for path in (samples, screens):
    path.mkdir(exist_ok=True)
stage = sys.argv[1]
archive = root / (stage + '_selection.tar.gz')

if stage == 'batch':
    for selection in ('current_base', 'current_actors'):
        subprocess.run(['python3','-u',__file__,selection,'reuse'],check=True)
    print('S_BATCH_COMPLETE',flush=True)
    sys.exit(0)

def build_feeder():
    if not (root/'replay_s').is_file():
        subprocess.run(['g++','-std=c++14','-O2','-pthread','-I/home/linaro/S/include',
                        '-I/home/linaro/common/perception/include',root/'paired_replay_s.cpp',
                        app/'build/shared_memory.o','-lrt','-o',root/'replay_s'],check=True)

if stage == 'build':
    build_feeder()
    print('S_FEEDER_BUILT', flush=True)
    sys.exit(0)

if len(sys.argv) == 2 or sys.argv[2] == 'reuse':
    if len(sys.argv) == 2:
        download = urllib.request.build_opener(urllib.request.ProxyHandler({}))
        endpoint = 'http://192.168.100.10:43879/' + archive.name
        with download.open(endpoint, timeout=15) as response:
            content = response.read(12*1024*1024)
            expected = response.headers['X-SHA256']
        assert hashlib.sha256(content).hexdigest() == expected
        archive.write_bytes(content)
        print('S_INPUT_VERIFIED', archive.name, len(content), flush=True)
    else:
        assert archive.is_file()
    build_feeder()
    process = subprocess.run(['unshare','--ipc','--mount','--propagation','private','--fork',
                              'python3','-u',__file__,stage,'private'],check=True)
    sys.exit(process.returncode)

assert os.readlink('/proc/self/ns/ipc') != os.readlink('/proc/1/ns/ipc')
assert os.readlink('/proc/self/ns/mnt') != os.readlink('/proc/1/ns/mnt')
subprocess.run(['mount','-t','tmpfs','-o','mode=1777','tmpfs','/dev/shm'],check=True)
with tarfile.open(archive) as source:
    for item in source.getmembers():
        assert item.isfile() and Path(item.name).name == item.name
        (samples / item.name).write_bytes(source.extractfile(item).read())
env = dict(os.environ,DISPLAY=':0',XAUTHORITY='/home/linaro/.Xauthority',
           VISION_KEEP_DISPLAY_MODE='1',VISION_RENDER_BACKEND='putimage')
binary = app/'build/eight_camera_renderer'
print('S_RUNTIME_SHA',hashlib.sha256(binary.read_bytes()).hexdigest(),flush=True)
with (root/(stage+'_x11.log')).open('w') as log:
    replay = subprocess.Popen([root/'replay_s',samples,samples/(stage+'_shots.txt')],cwd=root,
        stdin=subprocess.PIPE,stdout=subprocess.PIPE,text=True)
    renderer = None
    try:
        for line in replay.stdout:
            if line.strip() == 'FEED_READY':
                break
            print(line.rstrip(),flush=True)
        else:
            raise RuntimeError(f'Replay feeder exited before startup ({replay.wait(timeout=4)})')
        renderer = subprocess.Popen([binary,'--fullscreen'],cwd=root,env=env,stdin=subprocess.DEVNULL,
                                    stdout=log,stderr=log)
        for attempt in range(100):
            assert renderer.poll() is None
            match = re.search(r'PERCEPTION_WINDOW_ID=(0x[\da-f]+)',Path(log.name).read_text(errors='replace'))
            if match:
                window = match[1]
                print('S_REPLAY_WINDOW',window,flush=True)
                break
            time.sleep(.07)
        else:
            raise TimeoutError('Replay X11 window did not appear')
        for attempt in range(64):
            assert renderer.poll() is None
            status = Path(log.name).read_text(errors='replace')
            if '切换动画完成' in status:
                break
            time.sleep(.2)
        else:
            raise TimeoutError('Front-center view never finished its zoom transition')
        geometry = subprocess.run(['xwininfo','-id',window],env=env,check=True,text=True,capture_output=True).stdout
        for line in geometry.splitlines():
            if any(key in line for key in ('Width:','Height:','Absolute upper-left')):
                print(line.strip(),flush=True)
        replay.stdin.write('\n')
        replay.stdin.flush()
        captured = []
        for line in replay.stdout:
            if not line.startswith('CAPTURE '):
                continue
            _, name, style = line.split()
            xwd = screens/(name+'_'+style+'.xwd')
            png = xwd.with_suffix('.png')
            time.sleep(.20)
            subprocess.run(['xwd','-silent','-id',window,'-out',xwd],env=env,check=True,timeout=4)
            captured.append((xwd,png))
            print('CAPTURED',name,style,flush=True)
        assert replay.wait(timeout=8) == 0 and captured
    finally:
        if replay.poll() is None:
            replay.terminate()
            replay.wait(timeout=5)
        if renderer and renderer.poll() is None:
            renderer.terminate()
            try:
                renderer.wait(timeout=8)
            except subprocess.TimeoutExpired:
                renderer.kill()
                renderer.wait(timeout=4)
output = root/(stage+'_screens.tar.gz')
with tarfile.open(output,'w:gz',compresslevel=2) as result:
    for xwd,png in captured:
        subprocess.run(['ffmpeg','-loglevel','error','-nostdin','-y','-threads','1','-i',xwd,
                        '-frames:v','1',png],check=True)
        result.add(png,arcname=png.name)
    result.add(samples/(stage+'_selection.json'),arcname='selection.json')
print('S_X11_COMPLETE',len(captured), output.stat().st_size,flush=True)
