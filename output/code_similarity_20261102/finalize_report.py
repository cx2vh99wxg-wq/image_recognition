"""Verify the narrow change set and write an honest source-overlap report."""
import hashlib
import json
from pathlib import Path
import subprocess

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
before=json.loads((OUT/'audit_before.json').read_text(encoding='utf-8'))
after=json.loads((OUT/'audit_after.json').read_text(encoding='utf-8'))
allowed={'udp_rk_rk_yolo/S/pcie/include/camera_orientation.h',
         'udp_rk_rk_yolo/M/pcie/csrc/yolop_opencv_integration.cpp'}
changed=[]
for item in before['files']:
    actual=hashlib.sha256((ROOT/item['path']).read_bytes()).hexdigest()
    if actual!=item['sha256']:
        changed.append(item['path'])
assert set(changed)==allowed,changed
tests=json.loads((OUT/'tests.json').read_text(encoding='utf-8'))
assert len(tests)==8 and all(test['returncode']==0 for test in tests)
font_name='udp_rk_rk_yolo/M/pcie/csrc/yolop_opencv_integration.cpp'
old=(OUT/'baseline'/font_name).read_text(encoding='utf-8')
new=(ROOT/font_name).read_text(encoding='utf-8')
assert old.split('void init_font() {')[0]==new.split('void init_font() {')[0]
assert old.split('// 简单文字绘制函数',1)[1]==new.split('// 简单文字绘制函数',1)[1]
branch=subprocess.check_output(['git','branch','--show-current'],cwd=ROOT,text=True).strip()
verification=dict(current_root=str(ROOT),origin_root=before['origin_root'],branch=branch,
    changed_existing_source_files=changed,unchanged_existing_source_files=len(before['files'])-len(changed),
    added_tests=after['added_sources_not_used_to_dilute_comparison'],
    origin_modified=False,boards_updated=False,FPGA_HMI_models_control_protocols_modified=False,
    fixed_comparison_population=len(before['files']),before_summary=before['summary'],after_summary=after['summary'],
    test_runs=tests,meaning='Scoped equivalence tests passed; no hardware test or official plagiarism determination',
    public_interfaces_unchanged=True,source_attribution_or_copyright_removed=False)
(OUT/'verification.json').write_text(json.dumps(verification,ensure_ascii=False,indent=2),encoding='utf-8')
summary=before['summary']; latest=after['summary']
report='''代码对比与等价重构说明
核对日期：2026-11-02

当前工程：{root}
只读对照：{origin}
修改所在分支：{branch}

一、指标含义
本次是本地相似性审计，不是比赛官方查重结果，也不能据此认定作者或代码权属。
排除了 .git、tmp、output、pds_work、编译缓存、日志及烧录输出。统计代码及构建脚本，不统计模型权重、图像、文档和 HMI 二进制。
前后使用相同的 {population} 个当前工程文件作为比较集合；新增测试未加入分母，避免用新增代码稀释指标。

二、可复核结果
对照目录有 {origin_count} 个源码/构建脚本文件。
完全相同文件数（SHA-256）：修改前 {old_exact}/{population}（{old_percent:.2f}%），修改后 {new_exact}/{population}（{new_percent:.2f}%）。
按词法 token 比较相同的文件数：修改前 {old_tokens}，修改后 {new_tokens}。
按路径初步排除 IP/PCIe 支持代码、库 API 和测试后的应用/支持文件共 {business} 个，其中完全相同文件修改前 {old_business} 个、修改后 {new_business} 个。这是粗分类，不是“自研率”或官方查重率。

词法分析主要面向 C/C++/Verilog；脚本结果仅供参考。部分相似度使用同名或同路径候选的连续七 token 片段，可能漏掉改名、移动或拆分后的相似实现。文件数占比也不等于代码行、字节或语义占比。
整体数值包含多份 IP 副本。共有接口声明、硬件 IP、驱动接口、标准算法不能通过换名、改注释或调整排版变成新的来源。

三、实际修改
1. udp_rk_rk_yolo/S/pcie/include/camera_orientation.h
   将成对图块行和奇数中间行的镜像交换合并为内部函数，保留原来的输入检查、图块位置、行遍历顺序、交换次序及对外函数签名。
2. udp_rk_rk_yolo/M/pcie/csrc/yolop_opencv_integration.cpp
   仅将 init_font 中 25 个字符、每个 7 行的逐项赋值改为固定字模记录表和统一装载；保留空白字符默认值、初始化幂等性及字模原始字节。其余模型初始化、推理、阈值、NMS、分割和绘制函数未改动。

四、验证与边界
修改前后的图块旋转均通过 1,098 组几何与无效输入检查，逐像素匹配独立参考映射，保护区不变，旋转两次恢复原图。
修改前后字体均通过 889 组字符、裁剪位置及像素检查，完整字模和绘制缓冲区指纹均为 4d619b0417d4d08c；空指针、空文本和重复初始化也检查通过。
完整 M 端被修改的翻译单元在修改前后均通过 C++11 语法检查。
原有 {unchanged} 个其他源码/构建脚本文件的 SHA-256 保持一致。未删除来源或版权标识。
没有修改 image_recognition_origin，没有连接、部署、重启或烧录 S/M 板，也没有改变 FPGA、HMI、车控、协议、模型或标定配置。
验证为本地离线对照，不等同于整套 ARM 程序重新链接或板端实测；当前板端仍运行原先的程序。

五、交付核对
相同/相近文件仍然存在，本次没有将它们全部伪装成形式不同，也不承诺通过某一查重阈值。
main_old.cpp 等历史文件及接口声明仍在明细中；第三方和继承部分应按真实来源说明，新增业务功能应单独描述。
修改前的两个原文件保存在 output/code_similarity_20261102/baseline，可单独恢复；机器明细见 audit_before.json、audit_after.json、tests.json、verification.json。
'''.format(root=ROOT,origin=before['origin_root'],branch=branch,population=summary['current_source_files'],
    origin_count=summary['origin_source_files'],old_exact=summary['exact_file_matches'],new_exact=latest['exact_file_matches'],
    old_percent=100*summary['exact_file_fraction'],new_percent=100*latest['exact_file_fraction'],
    old_tokens=summary['token_identical_file_matches'],new_tokens=latest['token_identical_file_matches'],
    business=summary['business_and_support_files'],old_business=summary['exact_business_file_matches'],
    new_business=latest['exact_business_file_matches'],unchanged=verification['unchanged_existing_source_files'])
(ROOT/'代码对比与重构说明_20261102.txt').write_text(report,encoding='utf-8')
lines=['相同或相近源码文件明细','指标：同名/同路径七 token 片段覆盖率，不是官方查重分数。','']
for item in sorted(after['files'],key=lambda x:x['path']):
    match=item['best_same_name_or_path_match']
    if item['exact_matches'] or (match and match['current_shingle_overlap']>=0.7):
        lines.extend([item['path'],'分类：'+item['category'],
            '完全相同：'+str(bool(item['exact_matches'])),
            '对照文件：'+('; '.join(item['exact_matches']) if item['exact_matches'] else match['path']),
            '当前词片段覆盖率：'+('%.2f%%'%(100*match['current_shingle_overlap']) if match else '不适用'),''])
(OUT/'重复与相近文件明细.txt').write_text('\n'.join(lines),encoding='utf-8')
print('VERIFIED_ONLY_TWO_EXISTING_SOURCES_CHANGED others_unchanged=%u tests=8 board_updates=0 origin_updates=0'%verification['unchanged_existing_source_files'])
