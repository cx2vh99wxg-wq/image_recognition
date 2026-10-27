from pathlib import Path
import hashlib, json, struct

workspace = Path(__file__).resolve().parents[2]
root = workspace / 'udp_rk_rk_yolo'
receipt_path = Path(__file__).resolve().parent / 'deployment_receipt_20261027.json'
receipt = json.loads(receipt_path.read_text(encoding='utf-8')) if receipt_path.exists() else {}
deployed = receipt.get('production_installed', False)
m = ['include/line_style.h', 'include/line_style_ipc.h', 'include/line_style_wire.h',
     'pcie/include/line_style_analyzer.h', 'pcie/include/line_style_snapshot.h',
     'pcie/include/latest_lane_worker.h', 'pcie/csrc/main.cpp',
     'pcie/tests/test_line_style.cpp', 'pcie/tools/line_style_replay.cpp',
     'pcie/tools/line_style_model_probe.cpp', 'pcie/Makefile', 'udp/csrc/main.cpp']
s = ['include/line_style.h', 'include/line_style_ipc.h', 'include/line_style_wire.h',
     'udp/csrc/main.cpp', 'x11/csrc/main.cpp', 'x11/csrc/include/dashboard_ui.h']
binaries = {
    'M/pcie/build/pcie_capture': '6839766f55108628f26df615ed131da86ee5e0cc0d1e764ab00cd7d51d13492a',
    'M/udp/build/udp_sender': '6a7019ed8cd796715d33c7f822c05f9e4fc8d1ac85b7c16d5194496b34d4e8c1',
    'S/udp/build/video_receiver': '8be7b441907e47c02ceb6908d5059208d0f4121050edc58bd50053ccdcd0c5a9',
    'S/x11/csrc/build/eight_camera_renderer': 'fe6c4d4a45317e15d5b78ef7efcf435f67660c318f370ff4f32f1521faca17fd',
    'M/pcie/build/line_style_replay': '42818e7ebe1ecc79a51852c7927cbb58754b32b9580347a55974d1ba4fe17304',
    'M/pcie/build/line_style_model_probe': '9cf2809f9e45fb27cefcb8b22722b10f3de22b7bb61e62fad30db2514d415cb3',
    'M/pcie/build/test_line_style': 'bba6458ade9e07f08b73e6a9c4e64436b43560e3270cd76d25c14613afdf5f11',
}
for name, expected in binaries.items():
    data = (root / name).read_bytes()
    assert data[:4] == b'\x7fELF' and struct.unpack_from('<H', data, 18)[0] == 183, name
    assert hashlib.sha256(data).hexdigest() == expected, name
for name in m[:3]:
    assert (root / 'M' / name).read_bytes() == (root / 'S' / name).read_bytes(), name

files = ['M/'+x for x in m] + ['S/'+x for x in s] + list(binaries)
files += ['实虚线UNKNOWN修复说明_20261027.txt']
rows = ['实虚线 UNKNOWN 修复文件清单（2026-10-27）',
        '本目录为本地直接修改的源码和板上编译取回的 ARM 程序。' + ('已通过 DEBUG 安装到两板生产目录并启动。' if deployed else '候选未安装至板上生产目录。'),
        '路径相对于 udp_rk_rk_yolo；只更新对应的 M/S 目录。前四个 ELF 为运行程序，其余三个为离线工具。',
        'HMI、FPGA、S/fspi 车控逻辑此次未改。', '']
for name in files:
    data = (root / name).read_bytes()
    rows.append(hashlib.sha256(data).hexdigest() + '  ' + name)
(root / 'line_style_fix_manifest_20261027.txt').write_text('\n'.join(rows)+'\n', encoding='utf-8')

state = dict(date='2026-10-27', fix='observed-track + original-RGB paint analysis + diagnostics',
    local_sources_updated=True, ARM_builds_verified=True, ARM_binaries_downloaded=True,
    board_production_installed=deployed, board_production_restarted=receipt.get('pipeline_started',False),
    last_serial_online_BJT=receipt.get('verified_BJT','2026-10-27 00:05'), continuous_monitoring=False,
    board_candidate_root='/home/linaro/{M,S}/line_style_fix_20261026/candidate',
    actual_road_frames=3, actual_results=['DASHED/SOLID', 'DASHED/SOLID', 'DASHED/SOLID (live ROI fix)'],
    transformed_checks=dict(boundaries=48, correct=42, unknown=6, opposite=0),
    independent_reference='UNKNOWN/UNKNOWN (insufficient visible paint evidence)',
    ARM_postprocess_mean_ms=2.222, hardware_HMI_or_pin_test=False,
    document=str(root/'实虚线UNKNOWN修复说明_20261027.txt'), binaries=binaries)
if deployed:
    state['deployment_receipt'] = receipt
(Path(__file__).resolve().parent / 'delivery_state.json').write_text(json.dumps(state, ensure_ascii=False, indent=2)+'\n',encoding='utf-8')
print('PASS: 7 AArch64 files match board download hashes; 3 shared headers identical; manifest has', len(files), 'files')
