"""Consolidate existing letterbox bodies; preserve the S-only row-copy path."""
import hashlib
import json
from pathlib import Path

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
if (OUT/'before.json').exists():raise SystemExit('Existing snapshot must not be overwritten')
names=['udp_rk_rk_yolo/M/pcie/csrc/preprocess.c','udp_rk_rk_yolo/S/pcie/csrc/preprocess.c',
       'udp_rk_rk_yolo/S/fspi/csrc/main.c']
before=[]
for name in names:
    path=ROOT/name;raw=path.read_bytes()
    destination=OUT/'before'/name;destination.parent.mkdir(parents=True,exist_ok=True);destination.write_bytes(raw)
    before.append(dict(path=name,sha256=hashlib.sha256(raw).hexdigest()))
protected=['udp_rk_rk_yolo/M/pcie/include/preprocess.h','udp_rk_rk_yolo/S/pcie/include/preprocess.h',
    'udp_rk_rk_yolo/M/pcie/include/postprocess.h','udp_rk_rk_yolo/S/pcie/include/postprocess.h',
    'udp_rk_rk_yolo/S/steering/runtime.json','udp_rk_rk_yolo/S/steering/homing.json',
    'fpga/fpga_pcie_ov5640/project/project.fdc','fpga/fpga_pcie_ov5640/project_m_imu_nack_20261101/mpu9250_m.fdc']
for name in protected:
    before.append(dict(path=name,sha256=hashlib.sha256((ROOT/name).read_bytes()).hexdigest(),protected=True))
(OUT/'before.json').write_text(json.dumps(before,ensure_ascii=False,indent=2),encoding='utf-8')
m=(ROOT/names[0]).read_text(encoding='utf-8')
s=(ROOT/names[1]).read_text(encoding='utf-8')
start=s.index('    /* The production 640x480 -> 640x640 letterbox')
end=s.index('    for (int y = 0; y < new_h; ++y)',start)
fast=s[start:end]
assert s[:start]+s[end:]==m,'The implementations differ beyond the already existing fast path'
public_include='#include "preprocess.h"\n'
assert m.startswith(public_include)
body=m[len(public_include):]
offset=body.index('    for (int y = 0; y < new_h; ++y)')
shared=body[:offset]+'#if PERCEPTION_LETTERBOX_UNIT_SCALE_COPY\n'+fast+'#endif\n\n'+body[offset:]
shared='''/* Shared RGB888 letterbox implementation.
 * The caller supplies BOX_RECT through its unchanged public preprocess header.
 * M retains scalar nearest-neighbour sampling; S retains its existing exact-
 * scale row-copy branch. This file is compiled through those two wrappers.
 */
#ifndef PERCEPTION_LETTERBOX_UNIT_SCALE_COPY
#error "Include this implementation only from a configured preprocess.c"
#endif
'''+shared
target=ROOT/'udp_rk_rk_yolo/common/image/letterbox_rgb888_impl.inc'
assert not target.exists()
target.parent.mkdir(parents=True,exist_ok=True);target.write_text(shared,encoding='utf-8')
for board,flag,name in [('M',0,names[0]),('S',1,names[1])]:
    wrapper='''#include "preprocess.h"

/* Keep this board's public entry point and existing sampling policy. */
#define PERCEPTION_LETTERBOX_UNIT_SCALE_COPY {flag}
#include "../../../common/image/letterbox_rgb888_impl.inc"
#undef PERCEPTION_LETTERBOX_UNIT_SCALE_COPY
'''.format(flag=flag)
    (ROOT/name).write_text(wrapper,encoding='utf-8')

# Correct a stale module overview only; author/history and all executable
# tokens, including diagnostic strings, remain intact.
control=ROOT/names[2]
text=control.read_text(encoding='utf-8');old_end=text.index('*/')+2
overview='''/*
 * S 端统一车控入口。
 * 作者：科研轮转
 *
 * 本进程读取共享内存中的车道、行人及交通灯状态，按现有策略更新 FSPI。
 * 请求闭环模式时，目标角、反馈新鲜度、参考会话和脉冲期限由 steering_runtime
 * 处理；初始化或反馈无效不会退回兼容模式的长时间转向脉冲。
 * 未请求闭环模式时，保留原有位置状态和定时转向分支。
 *
 * 人员通道发布原始刹停请求；AUTO 使能、STOP 锁存和人工解除仍由 FPGA 仲裁。
 * 实线变道权限、红灯 GO 门控及档位告警继续走各自既有监视入口。
 * 显示命令保持六路视角 22/33/44/66/77/88、全景 99，以及 11/55 兼容归并。
 *
 * 历史创建时间：2025年10月20日。
 * 历史修改记录：2025年11月18日曾调整前后控制与转向、制动的职责。
 * 本次仅校正模块说明，不修改上述分支、控制参数或日志字串。
 */'''
control.write_text(overview+text[old_end:],encoding='utf-8')
print('SHARED_LETTERBOX_CREATED M_scalar=0 S_existing_row_copy=1; public signatures unchanged')
