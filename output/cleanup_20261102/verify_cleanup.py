"""Verify retained-file hashes, dependency presence, deletion scope and Git refs."""
from collections import Counter
import hashlib
import json
import os
from pathlib import Path
import subprocess

OUT = Path(__file__).resolve().parent
ROOT = OUT.parents[1]
plan = json.loads((OUT/'cleanup_plan.json').read_text(encoding='utf-8'))
before = json.loads((OUT/'inventory_before.json').read_text(encoding='utf-8'))
deleted = json.loads((OUT/'deleted_files.json').read_text(encoding='utf-8-sig'))
planned = {x['path'] for x in plan['files']}
assert planned == {x['path'] for x in deleted}
assert not any((ROOT/name).exists() for name in planned)
failures = []
for number, item in enumerate(plan['preserved_sha256'], 1):
    path = ROOT/item['path']
    try:
        with path.open('rb') as stream:
            sha = hashlib.file_digest(stream, 'sha256').hexdigest()
        if sha != item['sha256']:
            failures.append(dict(path=item['path'], error='hash mismatch'))
    except OSError as error:
        failures.append(dict(path=item['path'], error=str(error)))
    if number % 2000 == 0:
        print('HASH_VERIFY %u/%u' % (number,len(plan['preserved_sha256'])), flush=True)
dependency_changes = [x for x in plan['PDS_setup_dependencies']
                      if (ROOT/x['path']).is_file() != x['exists']]
branch = subprocess.check_output(['git','branch','--show-current'], cwd=ROOT, text=True).strip()
head = subprocess.check_output(['git','rev-parse','HEAD'], cwd=ROOT, text=True).strip()
refs = subprocess.check_output(['git','for-each-ref','--format=%(refname) %(objectname)'], cwd=ROOT, text=True)
old_refs = (OUT/'git_refs_before.txt').read_text(encoding='utf-8-sig')
refs_unchanged = refs.splitlines() == old_refs.splitlines()
sizes = Counter()
file_count = 0
for folder, directories, filenames in os.walk(ROOT, followlinks=False):
    for filename in filenames:
        path = Path(folder)/filename
        relative = path.relative_to(ROOT)
        sizes[relative.parts[0] if len(relative.parts)>1 else '(root files)'] += path.stat().st_size
        file_count += 1
by_reason = Counter()
for item in deleted:
    by_reason[item['reason']] += item['bytes']
result = dict(date='2026-11-02', current_branch=branch, head=head,
              branch_and_HEAD_unchanged=(branch==plan['branch'] and head==plan['head']),
              all_Git_refs_unchanged=refs_unchanged,
              boards_modified=False, board_program_restart_or_deployment=False,
              removed_file_count=len(deleted), removed_bytes=sum(x['bytes'] for x in deleted),
              removed_GiB=sum(x['bytes'] for x in deleted)/2**30,
              before_bytes=before['total_bytes'], after_bytes=sum(sizes.values()),
              before_GiB=before['total_bytes']/2**30, after_GiB=sum(sizes.values())/2**30,
              after_without_Git_GiB=(sum(sizes.values())-sizes['.git'])/2**30,
              retained_file_hashes_checked=len(plan['preserved_sha256']),
              retained_hash_failures=failures,
              PDS_input_references_checked=len(plan['PDS_setup_dependencies']),
              PDS_dependency_changes=dependency_changes,
              preserved_all_runtime_models_HMI_calibration_source_IP_and_firmware=(not failures and not dependency_changes),
              remaining_top_level_bytes=dict(sizes), removed_bytes_by_reason=dict(by_reason),
              working_tree_file_count=file_count,
              verification_method='Retained-file SHA256 equality and PDS input presence; no board functional test performed because no executable or source changed',
              submission_without_Git_and_tmp_GiB=(sum(sizes.values())-sizes['.git']-sizes['tmp'])/2**30)
result['passed'] = (not failures and not dependency_changes and refs_unchanged and result['branch_and_HEAD_unchanged'])
(OUT/'verification.json').write_text(json.dumps(result, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
report = '''本地代码提交清理结果

日期：2026-11-02
当前分支：{branch}

清理前：{before:.3f} GiB
清理后：{after:.3f} GiB（含保留的 Git 历史）
实际删除：{removed:.3f} GiB，共 {count} 个文件。
清理后不含 .git 的工作文件：{without:.3f} GiB。

已删除的内容：可重新生成的 PDS 编译数据库、布局布线输出、仿真编译缓存、安装轮子包、文档渲染预览，以及已确认有相同原件的临时副本。

保留内容：S/M 和公共源码、Makefile/构建脚本、第三方库、全部 RKNN 模型、HMI 工程及 TFT、IMU 校准文件、转向标定表、机械零点/运行配置、FPGA 源码、IP、工程及引脚/布局约束、全部最终烧录文件、独有文档、测试样本、部署工具和验收记录。

校验：对 {checked} 个保留文件核对 SHA-256，差异 {failed} 项；对 FPGA 工程 {deps} 处输入引用核对存在性，变化 {depchanges} 项。当前分支、HEAD 及所有 Git 分支/标签指针保持不变。

S、M 板上的程序均未修改，也没有连接、重启或烧录板卡；本次不需要重新更新板端。

PDS 中间结果已清理，下次重新生成烧录文件应从编译/综合开始完整运行，不能依赖旧缓存直接跳到布局布线或位流导出。工程输入和已交付的 .sbit/.sfc 均保留。

提交注意：.git 保存本机版本历史，仍约 {git:.3f} GiB，不是提交评审的源码内容。复制工程或打包交付时排除 .git 和 tmp 后，交付文件约 {submission:.3f} GiB；模型、HMI、源码及烧录文件仍包含在内。

tmp 内保留了独有文档、样本和历史工具，避免误删。现有 .gitignore 已有 tmp 规则，但以前已经被 Git 跟踪的文件不会自动取消跟踪。上述排除体积指复制/打包交付目录，不代表 Git 历史体积。本次没有暂存、提交、推送、重写历史或修改其他分支；当前工作区留下的删除项供提交时审阅。

详细文件清单及前后哈希见同目录 cleanup_plan.json、deleted_files.json 和 verification.json。
'''.format(branch=branch,before=result['before_GiB'],after=result['after_GiB'],removed=result['removed_GiB'],
           count=len(deleted),without=result['after_without_Git_GiB'],checked=result['retained_file_hashes_checked'],
           failed=len(failures),deps=result['PDS_input_references_checked'],depchanges=len(dependency_changes),git=sizes['.git']/2**30,
           submission=result['submission_without_Git_and_tmp_GiB'])
(OUT/'清理结果与提交说明.txt').write_text(report,encoding='utf-8')
print('FINAL_VERIFICATION '+json.dumps({k:result[k] for k in ['passed','current_branch','removed_file_count','removed_GiB','before_GiB','after_GiB','after_without_Git_GiB','retained_file_hashes_checked','retained_hash_failures','PDS_dependency_changes','all_Git_refs_unchanged']},ensure_ascii=False),flush=True)
if not result['passed']:
    raise SystemExit(1)
