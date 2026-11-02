"""List local changes and check hashes against the board candidate source package."""
from collections import defaultdict
import hashlib
import json
from pathlib import Path

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
delivery=json.loads((OUT/'delivery.json').read_text(encoding='utf-8'))
latest=ROOT/'output/letterbox_refactor_20261102/candidate_source_manifest.json'
sources=json.loads((latest if latest.exists() else OUT/'candidate_source_manifest.json').read_text(encoding='utf-8'))
candidate_root='/home/linaro/refactor_letterbox_candidate_20261102' if latest.exists() else '/home/linaro/refactor_candidate_20261102'
mismatches=[]
for entry in sources:
    path=ROOT/'udp_rk_rk_yolo'/entry['path']
    actual=hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else None
    if actual!=entry['sha256']:mismatches.append(entry['path'])
assert not mismatches,mismatches

groups=defaultdict(list)
for name in delivery['modified_existing_files']:
    if name.startswith('udp_rk_rk_yolo/M/'):group='M 端'
    elif name.startswith('udp_rk_rk_yolo/S/'):group='S 端'
    elif name.startswith('udp_rk_rk_yolo/common/'):group='两端公共模块'
    else:group='FPGA 源码与工程入口'
    assert (ROOT/name).is_file(),name
    groups[group].append(name)
additions=['.clang-format','udp_rk_rk_yolo/common/README.md',
    *['udp_rk_rk_yolo/common/perception/include/'+name for name in [
        'line_style.h','line_style_wire.h','line_style_ipc.h',
        'traffic_light.h','traffic_light_wire.h','traffic_light_ipc.h']],
    'udp_rk_rk_yolo/common/transport/include/video_transport_v2.h',
    'udp_rk_rk_yolo/common/image/letterbox_rgb888_impl.inc',
    'udp_rk_rk_yolo/common/image/README.txt',
    'fpga/fpga_pcie_ov5640/source/remote_vehicle_control.v',
    'fpga/fpga_pcie_ov5640/source/REMOTE_CONTROL_LAYOUT.txt']
lines=['本次重构文件清单','日期：2026-11-02','本地工程：'+str(ROOT),
    '分支：'+delivery['branch'],'',
    '本轮修改已有文件 %d 个。原有代码直接在上述本地工程修改，并从这里生成板端候选源码。'%len(delivery['modified_existing_files']),
    '已逐文件核对候选包中的 %d 个源码/构建文件，本地 SHA-256 全部一致。'%len(sources),
    '两板最新候选位置：'+candidate_root+'/{S 或 M}，公共代码位于同级 common。',
    '两板正式运行目录未覆盖。本轮未运行功能测试、未烧录 FPGA。','',
    '修改性质：七组接口公共实现去重、十四个兼容头文件入口、双端 letterbox 实现共享（保留 S 既有快路径）、七个 RTL 车控模块分离、四个 Makefile 依赖修正，以及应用代码格式整理。',
    '模型、HMI、FMC 引脚约束、标定数据和控制阈值不变；并非每个列出的文件都发生算法改动。','']
for group in ['M 端','S 端','两端公共模块','FPGA 源码与工程入口']:
    names=groups[group]
    lines.extend([group+'：修改已有文件 %d 个'%len(names),''])
    lines.extend(str(ROOT/name) for name in names)
    lines.append('')
lines.extend(['新增源码和工程说明',''])
for name in additions:
    assert (ROOT/name).is_file(),name
    lines.append(str(ROOT/name))
lines.extend(['','新增独立构建入口','',
    str(ROOT/'fpga/fpga_pcie_ov5640/project_s_application_refactor_20261102/project_s_refactor.pds'),
    str(ROOT/'fpga/fpga_pcie_ov5640/project_m_application_refactor_20261102/project_m_refactor.pds'),
    '','生成的最终烧录文件',''])
for item in delivery['firmware']:
    lines.extend([item['sbit'],item['sfc']])
lines.extend(['','其余说明见本地“应用工程重构交付说明_20261102.txt”；逐文件机器清单见 output/application_refactor_20261102/delivery.json。'])
path=ROOT/'本次重构文件清单_20261102.txt'
path.write_text('\n'.join(lines)+'\n',encoding='utf-8')
result=dict(local_root=str(ROOT),source_files_checked=len(sources),mismatches=[],
    modified_existing_count=len(delivery['modified_existing_files']),counts={key:len(value) for key,value in groups.items()},
    board_candidate_source_package_matches_local=True,production_updated=False,output=str(path))
(OUT/'local_sync_verification.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False,indent=2))
