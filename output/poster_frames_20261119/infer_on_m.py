import subprocess
import tarfile
from pathlib import Path

p = Path('/userdata/poster_frames_20261119')
inputs = p / 'inputs'
outputs = p / 'outputs'
inputs.mkdir(exist_ok=True)
outputs.mkdir(exist_ok=True)
with tarfile.open(p / 'lane_inputs.tar') as archive:
    for member in archive.getmembers():
        assert member.isfile() and '/' not in member.name and member.name.endswith('.jpg')
        (inputs / member.name).write_bytes(archive.extractfile(member).read())

rows = []
for frame in sorted(inputs.glob('*.jpg')):
    raw = frame.with_suffix('.bgr')
    subprocess.run(['ffmpeg', '-loglevel', 'error', '-nostdin', '-y', '-i', str(frame),
                    '-vf', 'pad=320:240:0:30:black', '-f', 'rawvideo', '-pix_fmt', 'bgr24',
                    str(raw)], check=True)
    rows.append(str(raw) + ' ' + str(outputs / (frame.stem + '.ppm')))
(p / 'inputs.txt').write_text('\n'.join(rows) + '\n')
with (p / 'lane_run.log').open('w') as log:
    result = subprocess.run([
        str(p / 'lane_frames'),
        '/home/linaro/M/pcie/model/yolopv2_front_320x240_mixed_error_guided_rk3568.rknn',
        str(p / 'inputs.txt')
    ], stdout=log, stderr=log)
assert result.returncode == 0, (p / 'lane_run.log').read_text()[-2000:]
for frame in outputs.glob('*.ppm'):
    subprocess.run(['ffmpeg', '-loglevel', 'error', '-nostdin', '-y', '-i', str(frame),
                    '-frames:v', '1', str(frame.with_suffix('.png'))], check=True)
with tarfile.open(p / 'lane_outputs.tar', 'w') as archive:
    for frame in outputs.glob('*.png'):
        archive.add(frame, arcname=frame.name)
    archive.add(p / 'lane_run.log', arcname='lane_run.log')
print('OUTPUTS', len(list(outputs.glob('*.png'))), (p / 'lane_outputs.tar').stat().st_size)
