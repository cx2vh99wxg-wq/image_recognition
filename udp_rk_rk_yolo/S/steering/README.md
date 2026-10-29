# A：S 端转向执行与反馈控制

S 端负责输出 left/right、接收 M 端转角反馈，以及阶段 3 的角度闭环。
MPU 在 M 端；采样、测角和传感器工具全部位于 `M/imu/`。
整体架构、分工和共同基线提交步骤见 `../../common/steering/README.md`。
整车接入条件见 `../../common/steering/INTEGRATION_REVIEW.md`：正式控制要接入现有 `S/fspi/csrc/main.c` 的统一 IO 和功能协调流程；独立脉冲工具只用于台架。

```text
M端测角 → UDP → S/steering/feedback → controller → actuator → left/right
```

## S 端目录

- `actuator/`：已实现的脉冲状态机和 dry-run/FSPI 后端。
- `tools/actuator/`：已实现的独立 `steering_step` 工具。
- `feedback/`：M→S 反馈接收入口，目前没有 socket，明确返回 NOT_READY。
- `controller/`：S 端角度控制 API 及预留实现，目前明确返回 NOT_READY。
- `tests/`：S 端执行器和未就绪边界测试。
- `tools/build.py`：只构建 S 端，依赖公共头，不使用 M 端源码。

## 独立构建和检查

从仓库根目录：

```powershell
python udp_rk_rk_yolo/S/steering/tools/build.py --test
.\udp_rk_rk_yolo\S\steering\actuator\build\steering_step.exe --direction left --duration-ms 20
```

Windows 产物带 `.exe`；Linux 使用 `python3` 和无扩展名的程序。
也可在本目录 `make` 或 `make test`。本目录默认构建不会编译 M 的 MPU 工具。

S 端 Linux/RK 的真实控制：

```sh
cd udp_rk_rk_yolo/S/steering
python3 tools/build.py --hardware
sudo ./actuator/build/steering_step_fspi --backend fspi --direction left --duration-ms 20
sudo ./actuator/build/steering_step_fspi --backend fspi --direction release
```

默认仍是模拟。硬件测试需固定车辆、退出旧 FSPI 控制程序，详细限制和日志解释见 `tools/actuator/README.md`。
释放控制不代表方向盘回中，工具不会报告实际角度已到位。

当前只完成脉冲工具、共同协议和软件检查。真实测角反馈、网络链路及闭环精度仍待双方接入验证。
