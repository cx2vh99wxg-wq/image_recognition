# image_recognition — 辅助驾驶小车（三人从零重写）

M/S 双 RK3568 + FPGA 三级流水线：感知（M 板）→ 决策（S 板）→ 执行（S 板 FSPI → 电机）。
本仓库为**从零重写版本**，不存放旧代码拷贝；旧实现仅作学习参考（见文末）。

## 模块结构（按分工方案 v2.0）

```
image_recognition/
├── common/        # 公共契约库（B 主责：配置/共享内存/UDP协议/日志）
│   ├── include/   driving_types.h(A冻结) driving_config.h(B维护) shm_ipc.h udp_proto.h log.h time_util.h
│   ├── csrc/      shm_ipc.c udp_proto.c log.c
│   └── test/      test_udp_proto.c
├── perception/    # 感知层（A）：PCIe 取帧 + YOLOPv2 车道线 + 弯道 + 红绿灯/虚实线/斑马线
├── planning/      # 决策+通信+显示（B）：UDP收发 + 行人检测 + 决策状态机 + LCD(X11)
│   ├── csrc/      decision.c person_detect.c render_lcd_x11.c udp_sender.c udp_receiver.c
│   │              main_planning.c(S端) udp_m_send_main.c(M端)
│   └── test/      test_decision.c test_udp.c udp_mock.py
├── control/       # 执行层（C，待交付）：FSPI 主机驱动 + 100ms 控制循环 + 安全
├── fpga/          # FPGA RTL（C）：PCIe 封包 / FSPI 从机 / 串口屏 UART
├── HMI/           # 陶晶驰串口屏工程（C）
├── scripts/       # 启动/停止脚本（B）：start_m.sh start_s.sh stop_all.sh
├── model/         # 板端模型（本地文件，不入 git）
│   ├── yolopv2_Nx3x480x640_rk3568.rknn    # M 板车道线（A）
│   └── yolov5s-640-640.rknn                # S 板行人（B）
├── Makefile       # 顶层一键构建
└── docs/          # 分工方案等文档（见旧仓库 docs/）
```

## 构建

```bash
make            # 本机：全模块编译自检 + 全部单元测试（common/planning/perception）
make board      # 板端交叉编译（需 aarch64-linux-gnu- + 板端 librknnrt/libX11）
make clean
```

单元测试：协议编解码、决策状态机（11 用例）、UDP 乱序重组、感知 8 套——全部 PASS（-Werror 零告警）。

## 上板启动（不控电机，验证感知链路）

```bash
# M 板：insmod 驱动 → 配 IP 192.168.100.10 → 感知 + UDP 发送
./scripts/start_m.sh <模型绝对路径> <驱动ko路径>
# S 板：配 IP 192.168.100.20 → 决策 + UDP 接收 + LCD 显示（--no-lcd 可无屏跑）
./scripts/start_s.sh --model /path/to/yolov5s-640-640.rknn
# 清理
./scripts/stop_all.sh
```

PC 联调（M2 验收）：`python3 planning/test/udp_mock.py 192.168.100.20 8888 100 left`

## 数据流（重构后）

```
A(perception@M板) --shm_lane/shm_pcie_img--> B(udp_m_send_main@M板) --UDP:8888-->
B(planning_main@S板) --> shm_udp_img + shm_lane(转发) --> 决策(车道+行人) -->
shm_cmd --> C(control@S板) --> FSPI --> FPGA --> 电机
```

## 关键契约（三人必须一致）

| key | 名称 | 写 | 读 |
|---|---|---|---|
| 0x12345679 | shm_pcie_img | A(M板) | B 显示 |
| 0x1234567A | shm_udp_img | B(S板) | LCD |
| 0x1234567B | **shm_cmd**（控制命令） | B | **C** |
| 0x1234567C | shm_display（显示控制） | B | LCD |
| 0x1234567D | shm_person（行人） | B | B、C |
| 0x1234567E | shm_lane（弯道） | A(M板)/B转发(S板) | B、C |
| 0x1234567F/80/81 | 红绿灯/斑马线/虚实线 | A(M板) | B（经 UDP v2 转发，预留） |

## 旧代码参考

旧实现（含 FPGA 原工程、旧 UDP/感知代码）在仓库**外**：`E:\image_recognition\udp_rk_rk_yolo\`。
仅作阅读参考，**禁止**把旧文件拷回本仓库——协议/结构以 `common/` 唯一真源为准。
