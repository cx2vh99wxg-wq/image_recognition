#!/usr/bin/env python3
"""Read-only board release inventory; never records credentials or full environments."""
import argparse
import hashlib
import json
import os
import time
from pathlib import Path

KEYS = frozenset('VISION_ASYNC VISION_CAPTURE_FPS VISION_DMA_ROWS VISION_LANE_MODEL VISION_TRAFFIC VISION_TRAFFIC_MODEL VISION_LANE_OUTPUT_MODE VISION_DIRECT_MASK_IPM VISION_RENDER_BACKEND VISION_SHM_SYNC S_VISION_ASYNC S_YOLO_SUBMIT_INTERVAL LANE_CURVE_MODE LANE_RADIAL_LAMBDA LANE_RADIAL_PREVIEW LANE_GROUND_RECT LANE_IPM_TEST LANE_POINT_TRACKS TRAFFIC_NPU_BUDGET_MS TRAFFIC_LOCATOR_INTERVAL_MS TRAFFIC_NPU_INTERVAL_MS'.split())

def digest(path):
    path = Path(path)
    if not path.is_file():
        return {'path': str(path), 'exists': False}
    h = hashlib.sha256()
    with path.open('rb') as f:
        for block in iter(lambda: f.read(1024 * 1024), b''):
            h.update(block)
    return dict(path=str(path), resolved=str(path.resolve()), bytes=path.stat().st_size, sha256=h.hexdigest())

def collect(root, side):
    processes = []
    names = {'pcie_capture', 'app', 'udp_sender', 'video_receiver', 'eight_camera_renderer', 'vehicle_control'}
    for entry in Path('/proc').iterdir():
        if not entry.name.isdigit():
            continue
        try:
            executable = (entry / 'exe').resolve()
            if executable.name not in names or root not in executable.parents:
                continue
            env = dict(x.split(b'=', 1) for x in (entry / 'environ').read_bytes().split(b'\0') if b'=' in x)
            processes.append(dict(pid=int(entry.name), executable=digest(executable), cwd=str((entry / 'cwd').resolve()), settings={k: env[k.encode()].decode(errors='replace') for k in sorted(KEYS) if k.encode() in env}))
        except (OSError, ValueError):
            continue
    files = []
    for pattern in (side+'/pcie/*.conf', side+'/pcie/model/**/*.rknn', side+'/pcie/lib/librknnrt.so', side+'/x11/assets/car_third_person.jpg', side+'/drivers/*.ko', 'drivers/*.ko', 'common/image/camera_valid_rows.h', 'tools/release_manifest.py'):
        files.extend(digest(p) for p in sorted(root.glob(pattern)))
    clocks = {}
    for name in ('/sys/class/devfreq/*/cur_freq', '/sys/devices/system/cpu/cpufreq/policy*/scaling_governor', '/sys/devices/system/cpu/cpufreq/policy*/scaling_max_freq'):
        for p in Path('/').glob(name.lstrip('/')):
            try:
                clocks[str(p)] = p.read_text().strip()
            except OSError:
                pass
    calibration = {}
    for name in ('front_radial.conf', 'front_ipm_test.conf', 'front_ground.conf'):
        p = root / side / 'pcie' / name
        if p.is_file() and p.stat().st_size <= 4096:
            calibration[name] = dict(digest(p), value=p.read_text())
    return dict(side=side, collected_unix_ns=time.time_ns(), kernel=os.uname().release, processes=processes, artifacts=files, clocks=clocks, calibration=calibration,
                safeguards='Inventory only; no IPC writes, GPIO, rearm, model substitution or frequency changes.')

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--root', type=Path, default=Path('/home/linaro'))
    p.add_argument('--side', choices=('M', 'S'), required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    data = collect(a.root.resolve(), a.side)
    a.output.write_text(json.dumps(data, ensure_ascii=False, indent=2)+'\n')
    print('RELEASE_MANIFEST', a.side, len(data['processes']), len(data['artifacts']))

if __name__ == '__main__':
    main()
