import pathlib,hashlib,os,json,sys,time
role=sys.argv[1];assert role in ('M','S')
root=pathlib.Path('/home/linaro')/role;work=root/'ego_lane_20261027'
installed=json.loads((work/'installed_manifest.json').read_text())['installed']
verified={name:hashlib.sha256((root/name).read_bytes()).hexdigest()==h for name,h in installed.items()}
names=['pcie/build/pcie_capture','udp/build/udp_sender'] if role=='M' else ['pcie/build/app','udp/build/video_receiver','fspi/build/vehicle_control','x11/csrc/build/eight_camera_renderer']
pids={name:[] for name in names}
for p in pathlib.Path('/proc').iterdir():
    if not p.name.isdigit():continue
    try:
        exe=os.readlink(str(p/'exe'))
        for name in names:
            if exe==str(root/name):pids[name].append(int(p.name))
    except (FileNotFoundError,ProcessLookupError,PermissionError):pass
logs={}
for name in names:
    path=root/'runtime_ego_lane_20261027'/(pathlib.Path(name).name+'.log')
    with path.open('rb') as f:f.seek(max(0,path.stat().st_size-12000));logs[path.name]=f.read().decode('utf-8','replace').splitlines()[-25:]
guards=['pcie/csrc/yolo_integration.c'] if role=='M' else ['fspi/csrc/main.c','fspi/csrc/fspi_module.c']
record=dict(role=role,monotonic_ns=time.monotonic_ns(),files_verified=verified,pids=pids,protected_sha256={p:hashlib.sha256((root/p).read_bytes()).hexdigest() for p in guards},logs=logs)
(work/'runtime_receipt.json').write_text(json.dumps(record,indent=2,ensure_ascii=False))
assert all(verified.values())
assert all(len(p)==1 for p in pids.values()),pids
print('RUNNING_FILE_HASHES_AND_SINGLE_INSTANCES_PASS',role,len(verified),pids,flush=True)
