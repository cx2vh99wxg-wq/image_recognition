# 转向控制共同框架 v1

这是任务 1、2 分支前共用的起点。原有视觉、UDP、自动转向主程序、FPGA 顶层和引脚约束均未接入或替换。

整车工程复核见 [INTEGRATION_REVIEW.md](INTEGRATION_REVIEW.md)。当前框架适用于独立台架；正式接入应由现有 S 端控制进程统一管理 FSPI，保留行人、红灯、实线、HMI 和调速任务。S 端正式控制 API 仍待补齐控制权、门控与非阻塞更新设计，不能把本框架直接当作完整控制进程替代品。

认识基本正确：A 做 left/right 有效电平时长测试，B 接入并跑通 MPU。
需要纠正的是 **FMC 信号接到 FPGA，B 的真实采集在 M 端**；不能直接把它当成 RK3568 的原生 I²C 口。
**M 端采集并计算方向盘转角，通过 UDP 发给 S 端；S 端接收反馈并控制执行器。**
两端工具和测试各归各端，共同协议放在 `common/steering/`，M 的构建不依赖 S 的目录。

```text
MPU-9250 → M端FMC/FPGA采集 → M端RK3568测角 → UDP → S端反馈接收 → S端闭环 → left/right
```

这是目标数据流。真实 FPGA 采集、实时 UDP 收发和角度闭环目前仍待实现，不把模拟或格式检查称为链路已跑通。

## 目录与所有权

```text
udp_rk_rk_yolo/
  common/steering/
    contracts/              两端共用：单位、状态、原始样本和 STF1 反馈协议
    tests/                  协议测试
    tools/check_scaffold.py 整体软件检查，包含两端独立构建验证
    build_support.py        通用编译辅助；不引用 M/S 实现
    README.md               本文：总体架构与分工
  M/imu/
    include/                B：MPU 传输抽象、采样和测角接口
    csrc/                   B：采样与测角实现；mock 可用，FPGA 后端待实现
    tools/                  B：imu_dump、angle_monitor、反馈包检查和独立构建脚本
    tests/                  B：采样边界和相对角度积分测试
    feedback/               B：后续 M→S UDP 发送接入说明
    Makefile                B：只构建 M 端
  S/steering/
    actuator/               A：脉冲状态机、模拟/FSPI 后端
    feedback/               A/集成：S 端 UDP 反馈接收入口，目前 NOT_READY
    controller/             合并后：角度闭环及其 S 端 API，目前 NOT_READY
    tools/actuator/         A：steering_step 独立工具
    tools/build.py          A：只构建 S 端
    tests/                  A：执行器与 S 端控制边界测试
fpga/fpga_pcie_ov5640/source/mpu9250/
    README.md               B：引脚方案、FPGA 采集实现任务
```

所有角度定义为：**方向盘/转向轴相对真实机械中位，左正右负**。
方向盘角度、前轮角度、车身朝向是不同的量。释放 left/right 只表示撤销动作，不表示回中。

## 已实现与待实现

| 项目 | 当前状态 |
|---|---|
| A 的可输入时长脉冲工具 | 已实现；默认 dry-run，硬件版在 Linux/RK 单独编译 |
| 非阻塞脉冲状态机 | 已实现；单调时钟截止、取消、限时、互斥、失败释放 |
| FSPI 接入 | 复用 `S/fspi` 现有驱动，只写左右寄存器 2/3；尚未上板验证 |
| B 的模拟原始采样、CSV | 已实现；输出始终带 SIMULATED 标记 |
| 单轴相对转角积分 | 已实现台架参考版；实际时间间隔、轴投影、偏置参数、断流故障 |
| MPU 芯片身份读取接口 | 已实现驱动边界；依赖 B 补齐真实传输 |
| FPGA I²C 主机和快照读回 | 待 B 实现；硬件后端直接返回 NOT_READY，不产生假数据 |
| 反馈包编解码 | 已实现 STF1 v1，含有符号角度、序号、源时刻、源年龄和 CRC32 |
| 实时 UDP 反馈收发、S 端反馈缓存 | 待集成；现有视频协议未改动 |
| 给角度自动到位 | 阶段 3；当前 API 明确返回 NOT_READY |
| 机械回中行为、步长、精度 | 待实测，框架不预设结论 |

## 先在本机验证框架

依赖 Python 3 和 GCC；不需要第三方 Python 包。从仓库根目录执行：

```powershell
python udp_rk_rk_yolo/common/steering/tools/check_scaffold.py
```

Windows 输出为 `.exe`，Linux 输出无扩展名。检查不会打开设备或发出真实转向脉冲。
Linux 可执行 `make -C udp_rk_rk_yolo/common/steering test`，或将上面的 `python` 换成 `python3`。
整体检查还会把 M+common、S+common 分别复制到临时目录，验证缺少另一端目录时各自仍可编译并通过单端测试。

两人分别构建/检查自己的代码：

```powershell
python udp_rk_rk_yolo/S/steering/tools/build.py --test
python udp_rk_rk_yolo/M/imu/tools/build.py --test
```

第一条只构建和检查 S，第二条只构建和检查 M；整体检查入口才同时构建两端。

Windows 运行 A 的模拟工具：

```powershell
.\udp_rk_rk_yolo\S\steering\actuator\build\steering_step.exe --direction left --duration-ms 20
```

Windows 运行 B 的模拟工具：

```powershell
.\udp_rk_rk_yolo\M\imu\build\imu_dump.exe --backend mock --samples 100
.\udp_rk_rk_yolo\M\imu\build\angle_monitor.exe --backend mock --mock-rate-dps 30 --samples 100
```

B 的 `--backend fpga` 目前必须报 NOT_READY。这代表硬件待接通，不能将 mock 的运行成功当作 MPU 已跑通。

## A 上板的起点

把共同基线代码同步到 S 端 Linux/RK，先固定车身、架空测试并退出旧 `vehicle_control`。
新工具没有接入旧程序里的行人检测/制动轮询，不应在行驶中替代原程序。

```sh
cd udp_rk_rk_yolo/S/steering
python3 tools/build.py --hardware
./actuator/build/steering_step_fspi --direction left --duration-ms 20
sudo ./actuator/build/steering_step_fspi --backend fspi --direction left --duration-ms 20
sudo ./actuator/build/steering_step_fspi --backend fspi --direction release
```

硬件版仍默认模拟，必须显式 `--backend fspi` 才会接触设备。
具体测试顺序、日志含义和限制见 `S/steering/tools/actuator/README.md`。

## 分支前与合并约定

本次不自动创建 A/B 分支、不提交或推送。先把下面四个新目录作为**同一个共同基线提交**共享给 B，
再各自从该提交开分支。不要把工作区已有的其他未跟踪目录一起提交。

```sh
git add udp_rk_rk_yolo/common/steering udp_rk_rk_yolo/S/steering udp_rk_rk_yolo/M/imu fpga/fpga_pcie_ov5640/source/mpu9250
git commit -m "搭建转向执行与MPU反馈共同框架"
```

将该提交同步到双方能访问的远端开发分支，并记录 `git rev-parse HEAD` 的结果。
A 从共同提交创建 `codex/steering-actuator`，B 从同一提交创建 `codex/steering-imu`。
不要各自在不同版本上另造一份 contracts。

- A 负责 `S/steering/`，第一轮重点是执行器；阶段 3 在 S 接收反馈并闭环。
- B 负责 `M/imu/` 和新 FPGA MPU 目录；采样、测角、工具、测试、UDP 发送都在 M 端。
- `common/steering/` 与协议版本变更由一人统一提交；建议 A 负责协调。
- 两端各自维护自己的构建脚本，不需要互相修改源码或构建入口。
- FPGA 顶层、工程入口和约束由 B 统一接入。新增采集默认关闭，仅在 M 端启用；不得覆盖 S 端原控制配置。
- 第一轮两人都不改原有 `S/fspi/csrc/main.c`、M 端视觉主程序或视频 UDP 格式。
- 构建产物与 `logs/` 已忽略。A 的每次脉冲可能累积转角，不会自动重置起点。

## 第一轮共同验收

1. A：证明实际低电平时长、松开是否保持/回中、最小可重复动作及左右差异。
2. B：证明真实 WHO_AM_I、原始数据、时间间隔和读取失败状态正确，然后对照物理刻度测角。
3. 两边各自通过后，再实现 M 的 `feedback/` 发送、S 的 `feedback/` 接收和 S 的 `controller/`。
4. 集成时测网络延迟与抖动；模拟包默认不得进入实车角度闭环。

软件检查验证逻辑和格式，不代表板端电平、遥控链路、机械角度或控制精度已通过实测。
