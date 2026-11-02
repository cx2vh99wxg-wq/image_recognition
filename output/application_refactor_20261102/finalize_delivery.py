"""Record compiled artifacts and limitations without claiming runtime validation."""
import hashlib
import json
from pathlib import Path
import subprocess

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
firmware=json.loads((OUT/'firmware_candidates.json').read_text(encoding='utf-8'))
static=json.loads((OUT/'static_invariants.json').read_text(encoding='utf-8'))
timing=json.loads((OUT/'compiler_timing_summary.json').read_text(encoding='utf-8'))
boards={board:json.loads((OUT/(board+'_compile_result.json')).read_text(encoding='utf-8')) for board in ['S','M']}
for item in firmware:
    assert item['compiled'] and item.get('sfc')
    for kind in ['sbit','sfc']:
        path=Path(item[kind])
        assert path.is_file() and hashlib.sha256(path.read_bytes()).hexdigest()==item[kind+'_sha256']
for board,result in boards.items():
    assert result['compiled'] and not result['runtime_tests_run'] and not result['production_files_replaced']
assert boards['S']['binaries']['fspi/build/vehicle_control']['sha256']=='91326d9538b66a29d6dc02fb92718fd6214f4d4bd20e152f5576895bb2194ab9'
assert boards['S']['binaries']['steering/actuator/build/steering_step_fspi']['sha256']=='477fd6c06f2b1669027a4b840f3fa6c00b4fd3b3ef2d4d2fcb35ad9a998608c5'
assert boards['M']['binaries']['imu/build/angle_monitor']['sha256']=='d7b8952fc336fe7824bdb745efbb4d7ab4dfc429674f415029366df67d54ea0f'
baseline=json.loads((OUT/'baseline_manifest.json').read_text(encoding='utf-8'))
changed=[]
for item in baseline['files']:
    path=ROOT/item['path']
    if path.is_file() and hashlib.sha256(path.read_bytes()).hexdigest()!=item['sha256']:changed.append(item['path'])
branch=subprocess.check_output(['git','branch','--show-current'],cwd=ROOT,text=True).strip()
assert branch==baseline['branch']
delivery=dict(date='2026-11-02',current_project=str(ROOT),branch=branch,
    scope='Shared-module consolidation, RTL organization and consistent application code layout; not a claim of new authorship',
    modified_existing_files=changed,shared_implementations=7,compatible_include_paths=14,
    relocated_RTL_modules=7,formatted_application_files=static['formatted_files_with_unchanged_tokens'],
    body_token_inspection=static,ARM_compile_results=boards,firmware=firmware,compiler_timing=timing,
    candidate_sources_on_boards='/home/linaro/refactor_candidate_20261102/{S|M}, with adjacent common/',
    production_source_or_executables_replaced=False,processes_started_or_stopped=False,
    runtime_tests_run=False,simulations_run=False,FPGA_programmed=False,
    test_omission='User explicitly requested no testing; compiled/static-inspected candidates only',
    origin_directory_modified=False,copyright_or_attribution_removed=False,
    preexisting_board_state='Both normal applications stopped; M m-steering-imu was failed with NRestarts=3 before refactor; not repaired or restarted here')
(OUT/'delivery.json').write_text(json.dumps(delivery,ensure_ascii=False,indent=2),encoding='utf-8')
public=ROOT/'fpga/fpga_pcie_ov5640/project/generate_bitstream'
(public/'application_refactor_20261102_manifest.json').write_text(json.dumps(dict(
    firmware=firmware,compiler_timing=timing,hardware_programmed=False,runtime_tested=False,
    functional_source_tokens_preserved=True,notes='S and M are different images. Existing files preserved. See root delivery note.'),ensure_ascii=False,indent=2),encoding='utf-8')
report='''应用工程重构交付说明
日期：2026-11-02
工程：D:\\Study_Object\\image_recognition\\image_recognition
分支：{branch}

完成内容
1. 将 S/M 两份车道、交通灯共享实现和视频 UDP 实现收为七个公共模块，位于 udp_rk_rk_yolo/common/perception 和 common/transport。十四个旧头文件路径保留为兼容入口，对外数据结构、字段、共享键、线协议和超时常量不变。
2. 将 FPGA 相机顶层尾部七个车控模块搬到 source/remote_vehicle_control.v，原模块端口、参数和模块体语句保持一致；三个正常 PDS 入口均已包含新源文件。
3. 对 132 个应用源文件统一代码布局，保留非注释 token 和函数式宏含义。编译器无法安全格式化的内容、厂商 IP/SDK、版权与来源说明保留。此项是格式整理，不是算法重写，不证明原创，也不承诺比赛查重分数。
4. 四个 Makefile 增加传递头文件依赖，公共头文件变更时会重新编译相应对象文件。

保持不变
模型、HMI 工程、FMC 引脚约束、IMU 校准文件、转向映射表、运行配置、现有阈值及控制规则未修改。静态文件核对通过；公共模块实现和拆分 RTL 的非注释 token 保持一致。

编译结果
S、M 两端已在各自 ARM 板的独立候选目录完成完整应用编译；S 的车控、转向步进工具及 M 的 angle_monitor 与重构前记录的二进制 SHA-256 相同。
板端候选目录分别是 /home/linaro/refactor_candidate_20261102/S 和 /home/linaro/refactor_candidate_20261102/M，公共代码在同级 common 中。当前正式 /home/linaro/S、/home/linaro/M 的源码和程序未覆盖，也没有启动、停止或重启这些程序。

FPGA 烧录候选文件
统一目录：fpga/fpga_pcie_ov5640/project/generate_bitstream
S 端临时加载：image_pcie_capture_s_application_refactor_20261102.sbit
S 端固化：image_pcie_capture_s_application_refactor_20261102.sfc
M 端临时加载：image_pcie_capture_m_application_refactor_20261102.sbit
M 端固化：image_pcie_capture_m_application_refactor_20261102.sfc
S/M 不能互换。原先交付的烧录文件未覆盖。

独立候选 PDS 工程
S：fpga/fpga_pcie_ov5640/project_s_application_refactor_20261102/project_s_refactor.pds
M：fpga/fpga_pcie_ov5640/project_m_application_refactor_20261102/project_m_refactor.pds
这些目录内的 compile/synthesize/place_route/report_timing 等是本轮编译生成的中间结果。

验证边界
按用户最后要求，本轮没有运行程序测试、仿真或小车动作测试，没有烧录 FPGA。
时序报告没有新增失败时钟对，Hold 均无失败；旧工程已有的摄像头/DDR 异步域 Setup 负裕量仍存在，不能将“成功生成位流”理解为全部时序约束通过。
开始重构前只读记录显示，M 端 IMU 服务已为 failed、NRestarts=3，两端正常采集/车控均未运行。本轮未改动这项状态，也未将它描述为重构引入或已经修复。

提交与恢复
提交时完整保留 udp_rk_rk_yolo/common，包括新增 perception/transport 目录；只复制 M/S 而漏掉 common 会缺少头文件。
FPGA source/remote_vehicle_control.v 和更新后的工程文件须一起提交。
修改前本地文件备份及清单在 output/application_refactor_20261102/baseline 与 baseline_manifest.json；编译日志、静态核对和完整交付记录在同目录。只读对照目录 image_recognition_origin 未修改。
本说明明确区分模块化重构、格式整理、编译成功和未进行的运行测试，不将继承代码描述为新的原创来源。
'''.format(branch=branch)
(ROOT/'应用工程重构交付说明_20261102.txt').write_text(report,encoding='utf-8')
(public/'application_refactor_20261102_烧录文件说明.txt').write_text(report,encoding='utf-8')
print('DELIVERY_RECORDED files_changed=%u shared=7 rtl_modules=7 ARM_compile=2 firmware_pairs=2 no_tests_no_programming_no_production_install'%len(changed))
