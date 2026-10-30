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
- `feedback/`：遥测缓存校验与只读UDP监测工具；正式控制接入函数仍返回NOT_READY。
- `tools/feedback/`：Linux专用`steering_feedback_monitor`，记录M端相对角度，不访问执行器。
- `tools/calibration/`：实测脉冲记录、经验响应模型和阶段三台架目标角度工具，见该目录说明。
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

真实M→S角度遥测已恢复并完成带反馈的脉冲测试，可用`python3 tools/build.py --monitor`构建接收工具；用法及状态见`feedback/README.md`。2026-10-30用户确认MPU固定、左正右负，机械端点读数约±20°。第一阶段32次标定脉冲中有1次软件计时异常被模型排除；7次独立目标试验有5次进入±1°容差，另两次分别因无进展、分辨率限制退出，尚不能承诺任意角度精度。证据见`tests/feedback/evidence/precision_20261030.json`。

用户要求覆盖大角度后，已增加显式±18°观测范围、最长60ms请求的台架选项，并完成A060～A063；随后M会话变化使A064在发脉冲前停止。重新置零必须先确认原物理标记并记录新会话参考，不能直接拼接不同会话的角度。连续较长脉冲与±15°目标覆盖尚未完成，正式整车控制API仍为NOT_READY。
