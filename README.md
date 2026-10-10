# image_recognition — 辅助驾驶小车（三人从零重写）

> 2026-10-10 新闭环：[六路感知、串口屏、UDP/FSPI、PDS 工程、修改清单与验收步骤](docs/partC-adas-closed-loop.md)。主前视为上排 M 中间 cmos5；使用 `drive_6ch` 位流和 `start_m.sh --adas` / `start_s.sh --adas`，默认预览停车。真实 RKNN、PDS 时序和电机动作仍待实板验证，论文 CNN 等效增强尚未完成。
> 2026-10-09 历史核查：[论文复现缺口、三路引脚表、六宫格验证步骤](docs/partC-paper-reproduction-audit.md)。当前状态以 10-10 说明为准。
> 六路模拟：S 用 `sudo bash scripts/start_s.sh --local-stub`，M 用 `sudo bash scripts/start_m.sh --stub`；上排 M、下排 S，窗口 960×480。
> 旧 `start_s.sh` 默认仍是本地模拟。仅验证真实 S 采集可用 `--local-pcie --no-person --no-overlay`；完整感知与控制请切换上述 `--adas` 入口。
> J8 双目实拍测试：[完整 PDS 新建工程、28 根摄像头引脚、下载及双板操作指南](docs/partC-stereo-j8-guide.md)。M 使用独立 `stereo_j8` 位流后运行 `start_m.sh --stereo-pcie`，S 使用 `start_s.sh --local-stub`；上排为左实拍/黑色/右实拍。

M/S 双 RK3568 + FPGA 三级流水线：感知（M 板）→ 决策（S 板）→ 执行（S 板 FSPI → 电机）。
本仓库为**从零重写版本**，旧实现仅作学习参考（`udp_rk_rk_yolo/`，见文末）。

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
├── scripts/       # 启动/停止/网络/调优/诊断脚本（B）
│   ├── start_m.sh start_s.sh stop_all.sh
│   ├── setup_network_m.sh setup_network_s.sh check_system.sh
│   ├── tune_net.sh   # 放大内核 UDP 缓冲（rmem_max）——"收帧恒 0"必查项
│   └── board/     # 板卡部署：bashrc.m/s 自启模板 + deploy_board.sh
├── drivers/       # 板端 PCIe 驱动（二进制，来自参考工程）pango_pci_driver.ko
├── lib/           # 板端 RKNN 运行时（二进制，来自参考工程）librknnrt.so
├── bin/           # make board 生成的板端可执行文件（部署布局，含 staging）
├── model/         # 板端模型（本地文件，不入 git）
│   ├── yolopv2_Nx3x480x640_rk3568.rknn    # M 板车道线（A）
│   └── yolov5s-640-640.rknn                # S 板行人（B）
├── Makefile       # 顶层一键构建 + 板端 run/stop/status 命令
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

板端运行依赖文件已随仓库分发（见上文 `drivers/`、`lib/`）：

| 文件 | 用途 |
|---|---|
| `drivers/pango_pci_driver.ko` | PCIe 采集内核驱动（insmod） |
| `lib/librknnrt.so` | RKNN 推理运行时（运行时 `LD_LIBRARY_PATH` 已由脚本设置） |

### 一次性部署（每块板卡做一次）

```bash
# 把仓库（或 bin/ drivers/ lib/ model/ scripts/）拷到板卡后，在板卡上执行：
sudo ./scripts/board/deploy_board.sh m    # M 板：装自启(.bashrc)+驱动+网络
sudo ./scripts/board/deploy_board.sh s    # S 板：同上
# 此后登录即自动加载驱动、配置直连网络、设置 LD_LIBRARY_PATH
```

### 启动 / 停止

```bash
# M 板：insmod 驱动 → 配 IP 192.168.100.10(end0/end1 自动探测) → 感知 + UDP 发送
sudo ./scripts/start_m.sh [模型绝对路径] [驱动ko绝对路径]
# S 板：配 IP 192.168.100.20 → 内核 UDP 缓冲调优 → 决策 + UDP 接收 + LCD（--no-lcd 无屏跑）
sudo bash ./scripts/start_s.sh --local-pcie --no-person --no-overlay

# 或顶层 Makefile 等价命令（板卡上）：make run-m / run-s / stop / status
# 网络单独配置：sudo ./scripts/setup_network_m.sh / setup_network_s.sh
# 内核 UDP 缓冲调优：sudo ./scripts/tune_net.sh [--persist]
# 诊断：./scripts/check_system.sh
# 清理
./scripts/stop_all.sh
```

### 排障：S 端"收帧"恒为 0（LCD 帧却在涨）

一帧 = 1 帧头 + 439 个数据块 = 440 个 UDP 包（614400B / 1400B），M 端在毫秒级
突发里发完（桩模式 5 帧/s ≈ 2200 包/s）。S 端若消费跟不上，内核接收队列会
**静默丢包**，439 块永远凑不齐 → 收帧恒 0（LCD 帧不受影响，照常增长）。

必查三项（planning_main 每 2s 打印一行统计，已内置分层诊断）：

| 现象 | 结论 |
|---|---|
| `socket收包=0` 不涨 | 包没进进程：IP/网线/端口（先用 Python 在 8888 计数验证） |
| `socket收包` 涨、`重组接受块` 不涨 | 包到了但被丢：看"内核队列溢出"计数 |
| `重组接受块` 涨、`收帧=0` | 块收不齐（丢包）→ 按下面两步修 |
| `收帧` 增长 | 链路正常 |

```bash
sudo ./scripts/tune_net.sh          # ① 提高 net.core.rmem_max（默认仅 ~208KB，装不下一帧）
# ② 确认 planning_main 启动日志 SO_RCVBUF ≥ 16MB，且每轮把内核队列收干（代码已内置）
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

旧实现（含 FPGA 原工程、旧 UDP/感知代码）在仓库**内** `udp_rk_rk_yolo/`（也见 `E:\image_recognition\udp_rk_rk_yolo\`）。
**源代码**仅作阅读参考，禁止拷回——协议/结构以 `common/` 唯一真源为准。
**二进制依赖**（`pango_pci_driver.ko`、`librknnrt.so`）无法从源码重建，已复制到 `drivers/`、`lib/` 随仓库分发。
