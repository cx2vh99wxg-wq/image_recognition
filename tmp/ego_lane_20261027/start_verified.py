#!/usr/bin/env python3
"""Start one instance of the existing production pipeline after installation."""
import os, pathlib, subprocess, sys, time
role = sys.argv[1]
assert role in ('M', 'S')
root = pathlib.Path('/home/linaro') / role
assert pathlib.Path('/dev/pango_pci_driver').exists()
assert (root / 'backup_ego_lane_20261027/manifest.json').exists()
programs = [('pcie', 'pcie_capture'), ('udp', 'udp_sender')] if role == 'M' else [
    ('pcie', 'app'), ('udp', 'video_receiver'), ('fspi', 'vehicle_control'),
    ('x11/csrc', 'eight_camera_renderer')]
for folder, name in programs:
    executable = root / folder / 'build' / name
    assert executable.is_file() and os.access(str(executable), os.X_OK), str(executable)
    for entry in pathlib.Path('/proc').iterdir():
        if not entry.name.isdigit():
            continue
        try:
            assert os.readlink(str(entry / 'exe')) != str(executable), 'Already running: '+name
        except (FileNotFoundError, ProcessLookupError, PermissionError):
            pass
logs = root / 'runtime_ego_lane_20261027'
logs.mkdir(exist_ok=True)
env = dict(os.environ, DISPLAY=':0', XAUTHORITY='/home/linaro/.Xauthority')
started = []
for folder, name in programs:
    with (logs / (name+'.log')).open('ab', buffering=0) as output:
        proc = subprocess.Popen(['./build/'+name], cwd=str(root / folder), env=env,
            stdin=subprocess.DEVNULL, stdout=output, stderr=subprocess.STDOUT,
            start_new_session=True, close_fds=True)
    started.append((name, proc))
    (logs / (name+'.pid')).write_text(str(proc.pid)+'\n')
    if role == 'S' and folder in ('pcie', 'udp', 'fspi'):
        (root / folder / (folder+'.pid')).write_text(str(proc.pid)+'\n')
    time.sleep(2 if folder == 'pcie' else 1)
    assert proc.poll() is None, 'Startup failed: '+name+' see '+str(logs / (name+'.log'))
    print('STARTED', role, name, proc.pid, flush=True)
for name, proc in started:
    assert proc.poll() is None, 'Process exited during startup: '+name
print('PIPELINE_RUNNING', role, flush=True)
