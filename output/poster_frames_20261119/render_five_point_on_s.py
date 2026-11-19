from pathlib import Path
import subprocess
import tarfile

root = Path('/userdata/poster_frames_20261119/five_point_s')
root.mkdir(exist_ok=True)
app = Path('/home/linaro/S/x11/csrc')
crop = 'crop=728:546:254:224'
with tarfile.open(root / 'inputs.tar.gz') as archive:
    for member in archive.getmembers():
        assert member.isfile() and Path(member.name).name == member.name
        assert member.name.endswith('.png')
        (root / member.name).write_bytes(archive.extractfile(member).read())
outputs = []
for tile in sorted(root.glob('input-*.png')):
    stem = tile.stem.removeprefix('input-')
    frame = root / 'combined.ppm'
    dashboard = root / 'dashboard.ppm'
    output = root / (stem + '.png')
    common = ['ffmpeg', '-loglevel', 'error', '-nostdin', '-y', '-threads', '1', '-filter_threads', '1']
    # The S application loads conventional RGB565 but displays this UDP plane as BGR565.
    subprocess.run(common + ['-i', tile, '-vf', 'pad=1280:480:960:240:black',
                              '-frames:v', '1', frame], check=True)
    proc = subprocess.run([app/'build/eight_camera_renderer', '--preview', dashboard, '--input',
                           frame, '--preview-mode', '0x88'], cwd=app, text=True, capture_output=True)
    if proc.returncode:
        raise RuntimeError(proc.stdout + proc.stderr)
    subprocess.run(common + ['-i', dashboard, '-vf', crop, '-frames:v', '1', output], check=True)
    outputs.append(output)
assert outputs
with tarfile.open(root / 'screens.tar.gz', 'w:gz') as archive:
    for output in outputs:
        archive.add(output, arcname=output.name)
print('S_FRONT_RENDERED', len(outputs), (root / 'screens.tar.gz').stat().st_size)
