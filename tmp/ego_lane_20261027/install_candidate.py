"""Install validated sources/binaries with a recoverable backup; no motion commands."""
import pathlib,os,sys,json,hashlib,shutil,struct,signal,time,subprocess
role=sys.argv[1];assert role in ('M','S')
root=pathlib.Path('/home/linaro')/role;work=root/'ego_lane_20261027';candidate=work/'candidate'
assert 'CANDIDATE_BUILD_PASS '+role in (work/'prepare.log').read_text()
if role=='M':assert 'ARM_SAVED_VALIDATION_PASS' in (work/'validation.log').read_text()
expected=json.loads((work/'source_manifest.json').read_text())
binaries=['pcie/build/pcie_capture','udp/build/udp_sender','pcie/build/test_line_style','pcie/build/test_ego_lane','pcie/build/line_style_replay','pcie/build/line_style_model_probe'] if role=='M' else ['udp/build/video_receiver','x11/csrc/build/eight_camera_renderer']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
for name in binaries:
    data=(candidate/name).read_bytes();assert data[:4]==b'\x7fELF' and struct.unpack_from('<H',data,18)[0]==183
    expected[name]=sha(candidate/name)
for name,digest in expected.items():assert sha(candidate/name)==digest,name
backup=root/'backup_ego_lane_20261027';assert not backup.exists(),'Never overwrite a backup'
backup.mkdir()
record=dict(role=role,root=str(root),candidate=str(candidate),previous={},installed={},binaries=binaries)
for name in expected:
    src=root/name
    if src.exists():
        dst=backup/name;dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(str(src),str(dst));record['previous'][name]=sha(dst)
    else:record['previous'][name]=None
(backup/'manifest.json').write_text(json.dumps(record,indent=2))
programs=['pcie/build/pcie_capture','udp/build/udp_sender'] if role=='M' else ['pcie/build/app','udp/build/video_receiver','fspi/build/vehicle_control','x11/csrc/build/eight_camera_renderer']
targets={str(root/p) for p in programs}
def running():
    found=[]
    for p in pathlib.Path('/proc').iterdir():
        if p.name.isdigit():
            try:
                if os.readlink(str(p/'exe')).replace(' (deleted)','') in targets:found.append(int(p.name))
            except (FileNotFoundError,PermissionError,ProcessLookupError):pass
    return found
for pid in running():
    print('STOP_PROCESS',role,pid,flush=True)
    try:os.kill(pid,signal.SIGTERM)
    except ProcessLookupError:pass
# The original S make-run shell has an EXIT trap. Let its complete stop
# sequence finish before starting any new process it could otherwise kill.
time.sleep(9 if role=='S' else 3)
assert not running(),'A process has not exited; preserve backup and abort installation'
for name,digest in expected.items():
    target=root/name;target.parent.mkdir(parents=True,exist_ok=True)
    temp=target.with_name(target.name+'.ego_install_tmp');assert not temp.exists()
    shutil.copy2(str(candidate/name),str(temp))
    if name in binaries:temp.chmod(0o755)
    assert sha(temp)==digest;os.replace(str(temp),str(target));assert sha(target)==digest
    record['installed'][name]=digest
    (backup/'manifest.json').write_text(json.dumps(record,indent=2))
os.sync();(work/'installed_manifest.json').write_text(json.dumps(record,indent=2))
print('EGO_INSTALL_VERIFIED',role,len(expected),flush=True)
subprocess.run(['python3',str(work/'start_verified.py'),role],check=True)
