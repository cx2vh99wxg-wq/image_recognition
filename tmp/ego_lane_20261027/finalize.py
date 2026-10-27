from pathlib import Path
import json,hashlib,struct,re,datetime
work=Path(__file__).resolve().parent
repo=work.parent.parent
root=repo/'udp_rk_rk_yolo'
files={};pids={};guards=json.loads((work/'protected_hashes.json').read_text(encoding='utf-8'))
for role in ['M','S']:
    manifest=json.loads((work/(role+'-installed_manifest.json')).read_text(encoding='utf-8'))
    receipt=json.loads((work/(role+'-runtime_receipt.json')).read_text(encoding='utf-8'))
    assert all(receipt['files_verified'].values())
    assert all(len(p)==1 for p in receipt['pids'].values())
    pids[role]=receipt['pids']
    for name,h in manifest['installed'].items():
        path=root/role/name
        assert hashlib.sha256(path.read_bytes()).hexdigest()==h,(role,name)
        if name in manifest['binaries']:assert struct.unpack_from('<H',path.read_bytes(),18)[0]==183
        files[role+'/'+name]=h
    for name,h in receipt['protected_sha256'].items():assert guards['udp_rk_rk_yolo/'+role+'/'+name]==h
    observed=json.loads((work/(role+'-passive_state.json')).read_text(encoding='utf-8'))
    assert all(r['detected']['ttl_ms']>0 for r in observed)
    assert len({r['detected']['source'] for r in observed})>=4
for name,h in guards.items():assert hashlib.sha256((repo/name).read_bytes()).hexdigest()==h
cases=json.loads((work/'saved_results.json').read_text(encoding='utf-8'));assert len(cases)==170 and all(r['ok'] for r in cases)
arm=json.loads((work/'M-validation-arm_saved_results.json').read_text(encoding='utf-8'))
ms=[float(re.search(r'postprocess_ms=([\d.]+)',r['detail'])[1]) for r in arm]
assert sum(r['got'][0]=='SOLID' for r in arm)==11 and not any(r['got'][0]=='DASHED' for r in arm)
record=dict(verified_at=datetime.datetime.now().astimezone().isoformat(),production_installed=True,local_synced=True,
    files_verified=len(files),M_port='COM8',S_port='COM7',pids=pids,
    backups={r:'/home/linaro/'+r+'/backup_ego_lane_20261027' for r in ['M','S']},
    left_saved_frames=dict(total=12,solid=11,unknown=1,wrong_dashed=0),
    saved_regressions=dict(total=170,classified_sides=258,total_sides=340,opposite_style=0),
    arm_postprocess_ms=dict(min=min(ms),max=max(ms),mean=sum(ms)/len(ms)),
    real_model_reinference='sample_00: LEFT SOLID, RIGHT DASHED',
    wire_bytes=60,ipc_data_bytes=56,ipc_segment_bytes=64,protected_sources_unchanged=guards,
    live_test_scope='passive frame/metadata freshness and pipeline performance; no new real-video LEFT red-X rate or physical steering measurement',
    installed=files)
(work/'deployment_receipt.json').write_text(json.dumps(record,indent=2,ensure_ascii=False),encoding='utf-8')
lines=['本车道归属与左实线修复：当前文件清单（2026-10-27）',
       '已通过 DEBUG 安装两板生产目录并恢复运行；本地29个对应源码/ARM文件逐项SHA256一致。',
       'M: /home/linaro/M；S: /home/linaro/S。以下路径相对于 udp_rk_rk_yolo。',
       '备份：两端各自的 /home/linaro/{M,S}/backup_ego_lane_20261027，manifest.json记录旧版与新增文件。',
       '该清单取代 line_style_fix_manifest_20261027.txt 中较早的同名文件哈希。','']
lines += [h+'  '+name for name,h in sorted(files.items())]
(root/'ego_lane_manifest_20261027.txt').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print('FINAL LOCAL/BOARD CONSISTENCY PASS',len(files),'files; ARM mean ms',record['arm_postprocess_ms']['mean'])
