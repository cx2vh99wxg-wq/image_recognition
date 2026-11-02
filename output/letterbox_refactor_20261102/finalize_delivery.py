"""Preserve the three static/compiler evidence layers and update delivery notes."""
import hashlib
import json
from pathlib import Path
import subprocess

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
static=json.loads((OUT/'static_inspection.json').read_text(encoding='utf-8'))
boards={board:json.loads((OUT/(board+'_compile_result.json')).read_text(encoding='utf-8')) for board in ['M','S']}
sources=json.loads((OUT/'candidate_source_manifest.json').read_text(encoding='utf-8'))
for entry in sources:
    assert hashlib.sha256((ROOT/'udp_rk_rk_yolo'/entry['path']).read_bytes()).hexdigest()==entry['sha256'],entry['path']
for board,result in boards.items():
    assert result['compiled'] and result['preprocess_object_text_identical']
    assert result['preprocess_object_text_before_sha256']==result['preprocess_object_text_after_sha256']
    assert not result['runtime_tests_run'] and not result['production_files_replaced']
assert boards['S']['binaries']['fspi/build/vehicle_control']['sha256']=='91326d9538b66a29d6dc02fb92718fd6214f4d4bd20e152f5576895bb2194ab9'
assert boards['S']['binaries']['steering/actuator/build/steering_step_fspi']['sha256']=='477fd6c06f2b1669027a4b840f3fa6c00b4fd3b3ef2d4d2fcb35ad9a998608c5'
assert boards['M']['binaries']['imu/build/angle_monitor']['sha256']=='d7b8952fc336fe7824bdb745efbb4d7ab4dfc429674f415029366df67d54ea0f'
firmware=json.loads((ROOT/'output/application_refactor_20261102/firmware_candidates.json').read_text(encoding='utf-8'))
for image in firmware:
    for kind in ['sbit','sfc']:
        assert hashlib.sha256(Path(image[kind]).read_bytes()).hexdigest()==image[kind+'_sha256']
record=dict(date='2026-11-02',project=str(ROOT),branch=subprocess.check_output(['git','branch','--show-current'],cwd=ROOT,text=True).strip(),
    change='Shared letterbox implementation with unchanged M/S compile-time policies; control overview comments corrected',
    static_inspection=static,ARM_compile_and_text_sections=boards,local_source_package_files_checked=len(sources),
    source_package_matches_local=True,runtime_tests_run=False,programs_started_or_stopped=False,
    production_files_replaced=False,FPGA_or_HMI_changed=False,previous_SBIT_SFC_sha256_unchanged=True,
    origin_directory_changed=False)
(OUT/'delivery.json').write_text(json.dumps(record,ensure_ascii=False,indent=2),encoding='utf-8')
note='''图像预处理共享重构及核对说明
日期：2026-11-02
项目：D:\\Study_Object\\image_recognition\\image_recognition

保持现有逻辑和功能不变，以本地修改前的实际实现为基准。

本次修改
M/pcie/csrc/preprocess.c 与 S/pcie/csrc/preprocess.c 改为调用同一份 common/image/letterbox_rgb888_impl.inc 实现。
M 的编译选择为 0，仍使用原逐像素路径；S 的选择为 1，保留原 scale==1、尺寸相同时的行拷贝分支。公开函数、BOX_RECT、比例、补边、灰色值、浮点表达式、截断与通道顺序不变。
S/fspi/csrc/main.c 仅校正开头与实际闭环实现不符的旧说明，保留作者和历史记录，执行语句及日志字串不变。

三层核对
1. 保存修改前的三个文件，并核对公开头文件、运行配置和引脚约束的 SHA-256 不变。
2. 用 C 编译器分别预处理 M/S 修改前后版本，有效 token 序列完全相同。车控文件除注释外的 token 同样完全相同。
3. 在 M/S 板各自的新候选目录完成 ARM 编译，两端新旧 preprocess.o 的非空 .text 机器码段 SHA-256 完全相同。S 端车控、转向步进工具及 M 端角度监视器完整二进制哈希仍与前一轮记录一致。

范围说明
这些是静态、编译和产物数据核对，未运行采集、识别、车控、传感器监视或动作测试，不能描述为整车运行验收通过。
正式 /home/linaro/S、/home/linaro/M 目录未覆盖，服务和程序未重启。最新源码与编译产物分别在 /home/linaro/refactor_letterbox_candidate_20261102/S、M，公共模块位于同级 common。
本地已同步全部 193 个候选源码/构建文件并核对包清单。前一轮候选目录保留供对比。
本次没有修改 FPGA、HMI 或标定数据。此前生成的 S/M application_refactor_20261102.sbit/.sfc 内容及哈希未变，无需因本次软件实现共享再生成位流。

原版本和证据
修改前文件及 SHA-256：output/letterbox_refactor_20261102/before、before.json。
静态预处理核对：static_inspection.json。
板端编译和机器码比较：M_compile_result.json、S_compile_result.json。
完整记录：delivery.json。本次没有修改 image_recognition_origin，也没有通过删除版权或来源标记改变归属说明。
'''
(ROOT/'图像预处理共享重构核对说明_20261102.txt').write_text(note,encoding='utf-8')
previous=ROOT/'应用工程重构交付说明_20261102.txt'
text=previous.read_text(encoding='utf-8')
marker='同日后续更新：图像预处理共享实现'
if marker not in text:
    previous.write_text(text+'\n\n'+marker+'\n最新候选目录为 /home/linaro/refactor_letterbox_candidate_20261102/{S|M}；193 个候选源文件与本地一致，三层核对见“图像预处理共享重构核对说明_20261102.txt”。先前候选及编译记录保留为历史，正式运行目录仍未替换，仍未运行功能测试。\n',encoding='utf-8')
print('DELIVERY_OK local_source_files=193 static_preprocessed_equal=2 ARM_text_sections_equal=2 production_changes=0 runtime_tests=0')
