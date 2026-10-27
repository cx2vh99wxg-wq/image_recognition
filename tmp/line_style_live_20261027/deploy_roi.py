#!/usr/bin/env python3
"""Install the already tested candidate; never starts hardware processes."""
import base64, hashlib, json, os, pathlib, shutil, struct, sys, signal, time, subprocess

config = json.loads(base64.b64decode(sys.argv[1]))
role = config['role']
assert role in ('M', 'S')
root = pathlib.Path('/home/linaro') / role
assert root.is_dir() and not (root.parent / ('S' if role == 'M' else 'M')).exists()
candidate = root / 'line_style_fix_20261026/candidate'
expected = config['files']
backup = root / 'backup_line_roi_20261027'
assert not backup.exists(), 'Backup directory already exists; do not overwrite it'

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

targets = set()
for name, checksum in expected.items():
    source, target = candidate / name, root / name
    assert candidate.resolve() in source.resolve().parents and root.resolve() in target.resolve().parents
    assert source.is_file() and digest(source) == checksum, 'Candidate mismatch: '+name
    if name.split('/')[-1] in config['executables']:
        data = source.read_bytes()
        assert data[:4] == b'\x7fELF' and struct.unpack_from('<H', data, 18)[0] == 183
        targets.add(str(target.resolve()))
for entry in pathlib.Path('/proc').iterdir():
    if not entry.name.isdigit(): continue
    try:
        if os.readlink(str(entry/'exe')) in targets:
            print('TERMINATE',entry.name,flush=True);os.kill(int(entry.name),signal.SIGTERM)
    except (FileNotFoundError,ProcessLookupError,PermissionError): pass
time.sleep(1)
for proc in pathlib.Path('/proc').iterdir():
    if not proc.name.isdigit():
        continue
    try:
        exe = os.readlink(str(proc / 'exe')).removesuffix(' (deleted)') if sys.version_info >= (3, 9) else os.readlink(str(proc / 'exe')).replace(' (deleted)', '')
        assert exe not in targets, 'Target is running: PID '+proc.name
    except (FileNotFoundError, PermissionError, ProcessLookupError):
        pass

backup.mkdir()
record = {'role': role, 'root': str(root), 'candidate': str(candidate), 'previous': {}, 'installed': {}}
for name in expected:
    target = root / name
    if target.exists():
        saved = backup / name
        saved.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(str(target), str(saved))
        record['previous'][name] = digest(saved)
    else:
        record['previous'][name] = None
(backup / 'manifest.json').write_text(json.dumps(record, indent=2))
for name, checksum in expected.items():
    source, target = candidate / name, root / name
    target.parent.mkdir(parents=True, exist_ok=True)
    temporary = target.with_name(target.name+'.line_style_install_tmp')
    assert not temporary.exists()
    shutil.copy2(str(source), str(temporary))
    if name.split('/')[-1] in config['executables']:
        temporary.chmod(0o755)
    assert digest(temporary) == checksum
    os.replace(str(temporary), str(target))
    assert digest(target) == checksum
    record['installed'][name] = checksum
    (backup / 'manifest.json').write_text(json.dumps(record, indent=2))
    print('INSTALLED', name, checksum, flush=True)
os.sync()
print('BACKUP', str(backup), flush=True)
print('DEPLOY_VERIFIED', role, len(expected), flush=True)

log=root/'runtime_line_style_20261027/pcie_capture.log'
with log.open('ab',buffering=0) as output:
    child=subprocess.Popen(['./build/pcie_capture'],cwd=str(root/'pcie'),env=dict(os.environ,DISPLAY=':0',XAUTHORITY='/home/linaro/.Xauthority'),stdin=subprocess.DEVNULL,stdout=output,stderr=subprocess.STDOUT,start_new_session=True,close_fds=True)
(root/'runtime_line_style_20261027/pcie_capture.pid').write_text(str(child.pid)+'\n')
time.sleep(2)
assert child.poll() is None
print('ROI_CAPTURE_RUNNING',child.pid,flush=True)