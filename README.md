# image_recognition 孪生系统项目

## 项目简介
本仓库为图像识别孪生系统代码仓库，采用Git + GitHub协作开发。
> 协作规范：**禁止直接在 `master` 分支开发**。`master` 分支仅存放稳定、可正常运行的版本，所有新功能、调试代码均在个人开发分支完成，通过PR合并到master。

## 环境准备
1. 安装 Git
2. 配置 GitHub SSH密钥（推荐，避免每次推送输入账号密码）
3. 仓库管理员已添加你为本仓库协作者，接受GitHub邮件邀请获得访问权限

## 首次拉取仓库（成员第一次操作，仅执行一次）
```bash
# 克隆仓库（SSH地址）
git clone git@github.com:cx2vh99wxg-wq/image_recognition.git
cd image_recognition

## 人员 A（感知层）模块说明

A 负责自动驾驶小车的**感知层**：图像采集 → 预处理 → 车道线分割 → 弯道判定。
代码分布在两个采集端：`udp_rk_rk_yolo/M/pcie/`（M 端）与 `udp_rk_rk_yolo/S/pcie/`（S 端），
两份实现遵循**同一套新契约**，三人在根目录 `common/` 统一拼接即可运行。

### A 对外唯一契约：`LaneResult`
A 只向决策层（B）输出 `LaneResult`（定义见 `common/include/driving_types.h`），由 B 经
共享内存 `shm_lane`（key `0x1234567E`）读取；A 同时把原始图写入 `shm_pcie_img`
（key `0x12345679`）供 B 显示/转发。A **不**做 X11 渲染、UDP、共享内存底层实现、SPI/电机
（分别归 B、C）。

### 算法优化（相对旧实现的硬性改动）
1. **真实置信度**：`confidence` 由车道线覆盖率 + 远端集中度推算（`coverage*600 + far_ratio*40`，封顶 100），
   不再硬编码 90/75/60/40。
2. **像素偏移替代角度**：弯道统一用 `curve_offset`（带符号像素偏移，正值=右弯），
   不再输出无物理意义的 25° 角度。
3. **剥离前导像素**：采集端逐行丢弃每行 120 个前导像素（`PCIE_LEAD_PIXELS`，定义于 `common/include/driving_config.h`），
   交给下游的是干净的 640x480。
4. **遵循 `LaneResult` 新接口**：不再写旧的弯道共享内存，改为填充 `LaneResult` 由 `shm_write_lane()` 送出。

### 目录与构建
```
common/include/   driving_types.h(契约)  driving_config.h(配置)  shm_ipc.h(B提供的接口声明)
M/pcie| S/pcie:
  include/  pcie_capture.h  preprocess.h  yolo_integration.h  pcie_dma_read_test.h(ABI不变)
  csrc/     preprocess.c  yolo_integration.c  main.cpp
  Makefile  仅编译 A 源，链接 B 的 shm_ipc.c 与 librknnrt
```
构建（板端 aarch64，交叉编译前缀见 `driving_config.h`）：
```bash
cd udp_rk_rk_yolo/M/pcie && make      # 或 S/pcie
```
> 注意：`common/csrc/shm_ipc.c`（B 实现）与 `lib/librknnrt.so` 需在拼接时提供；
> 本机（MinGW）仅能对新 A 源做语法自检，完整链接/运行需在板端 Linux 进行。

### 关于 `pcie_dma_read_test.h`
该头是 A 与 FPGA 驱动的 ioctl/结构体契约，**命令号与结构体布局必须逐字节保持原样**，
否则硬件失联。本次只重排注释/分组并调整 include 守卫；S 端相对 M 端额外保留了
`PCI_GET_DEVICE_INFO_CMD`(命令12) 与 `VEISION` 标签以对齐 S 板真实驱动。

### 已剥离的“非 A”职责
旧 S 端曾把 YOLOv5s 行人检测（person_alert_effects / 5x7 字体 / IoU 去重 /
`emergency_brake_*` 共享内存）混入 A。按分工方案，行人检测与紧急制动属 B，本次已从 A 文件剥离，
A 仅保留纯车道线分割与 `LaneResult` 产出。
