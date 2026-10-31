# B：M 端 MPU-9250 原始采集、台架测角和反馈发送

实现：MPU-9250 → FMC A20/B20 → 独立 M FPGA I2C/快照 → M RK3568 → STF1 UDP。
原有 S 端、HMI、视觉算法、视频数据格式和共同协议不改。
硬件与 IMU1 格式见 [HARDWARE_AND_PROTOCOL.md](HARDWARE_AND_PROTOCOL.md)。

## 构建

```sh
cd /home/linaro/M/imu
python3 tools/build.py --test
python3 tests/test_tools.py
```

本机源码位于 `udp_rk_rk_yolo/M/imu`，公共接口引用 `udp_rk_rk_yolo/common/steering`。
单独编译不依赖 S；Linux 为真实硬件目标，Windows/mock 用于工具开发。
FPGA 顶层在独立 `project_m_imu` 工程，不能把仅更新 ARM 软件当作已更新 FPGA。

## 使用前停止单一采集服务

```sh
sudo systemctl stop m-steering-imu
./build/imu_probe
```

工具之间有硬件进程锁；不要同时启动两个真实采集工具。probe 必须看到 IMU1、WHO=71、flags=7、样本序号增长。
错误码、旧 FPGA、CRC 错、无 ACK、超时、过期、重复数据均不伪造成角度。

## 原始记录和零偏

先让模块与车身静止，通电稳定后采集；终端低频打印，完整样本写 CSV：

```sh
./build/imu_dump --backend fpga --samples 2000 --interval-ms 1 --print-every 500 --csv stationary.csv
python3 tools/calibrate.py --input stationary.csv --output mounting.cal --stationary --axis z --sign 1
```

`--axis z --sign 1` 仅是示例，必须按安装转轴改。calibrate 检查静止时长、角速度方差、加速度模长和模拟标志；
保存偏置、方差、温度与样本报告，但不建立机械中位。连续匀速旋转无法仅靠方差排除，静止必须由物理条件保证。

需要从已知左向旋转标定任意安装轴时，另录一段固定车身、只沿转向轴向左转动至少15度的真实原始数据：

```sh
python3 tools/calibrate.py --input stationary.csv --output mounting.cal --stationary --positive-left-csv known_left_sweep.csv
```

转轴向量由去偏后的角速度积分求得，左向决定正号。输入必须是独立确认的左向动作，不能用软件输出预期角度充当真值。

## 相对积分和显式机械归零

### 静止漂移抑制（可选台架模式）

`angle_monitor --stationary-hold` 使用约2秒窗口，同时检查三轴去偏角速度的均值/方差、
加速度模长/方差和姿态稳定性。确认静止后保持当前角度，输出 `static_hold=1`，
保持角度而不把它拉回0；新样本仍推进序号/时间。检测到运动后恢复积分。
不改写标定文件或安装轴，不授予机械中位，原始 gyro 日志保留实测值。
已进入静止保持且证据仍有效时，以5秒时间常数跟踪内存中的残余零偏，三轴矢量上限0.20°/s。
开始运动前先做运动检查，运动期间禁止学习；补偿的是角速度偏差，不把角度拉回0。

这是显式的静止约束，不能凭单个 IMU 区分接近噪声水平的恒定慢转与残余零偏。
运动判定使用30ms低通，滤波后三轴合成角速度超过0.18°/s连续3帧退出保持；
大于1°/s的明显转动立即退出。低通只用于判定，积分使用实测角速度减固定及已学到的残余零偏。
约2秒证据窗口后再确认进入；普通噪声引起的窗口失败需持续约150ms才退出，避免hold反复切换。
持续0.25°/s的模拟慢转已验证在100ms内退出保持；更慢的转动仍可能被抑制或延迟。
当前台架服务可通过 `/etc/default/m-steering-imu` 启用；正式装车须验证安装轴及最低转速，
不应将静止时显示稳定等同于绝对角度准确。该选项与重力辅助实验选项不同时使用。

运行中把当前位置设为相对 0°，无需重启传感器或服务：

本次 M 板已安装短命令 `sudo imu-zero`（源文件 `tools/imu_zero.sh`）。
完整等价命令如下：

```sh
sudo systemctl kill --kill-who=main --signal=USR2 m-steering-imu.service
sudo journalctl -u m-steering-imu.service -n 5 -f
```

仅适用于已安装支持 SIGUSR2 的本版本工具。日志立即输出 `RELATIVE ZERO set`。
采样序号、时间戳和进程继续运行，反馈 session 改变，`ZERO_VALID` 清零；不重新标定偏置。
请在想设零的位置保持不动后发送命令。此操作是相对参考点，不能当作机械中位验收。
若在重力辅助台架模式下使用，会关闭该辅助，避免继续依赖旧机械参考。

不声明中位时：

```sh
./build/angle_monitor --backend fpga --calibration mounting.cal --samples 6000 --interval-ms 1 --csv relative.csv
```

该模式只表示沿标定轴的相对转动，ZERO_VALID=0。积分使用 FPGA 采集时刻差，支持多圈角度；不会用串口接收间隔积分。

只有车身固定、安装轴已确认、实际机构放在机械中位时，才运行：

```sh
./build/angle_monitor --backend fpga --calibration mounting.cal --fixed-chassis --zero-at-center --samples 0 --interval-ms 1 --send-to 192.168.100.20 --port 54321 --csv centered.csv
```

运行中重新放到真实中位后，可向该进程发 SIGUSR1。仅上述显式台架模式接受归零；归零更换反馈 session。
不要把 `--zero-at-center` 写入自动重启服务，否则不能证明重启时恰好在机械中位。
断流、时钟/会话异常会退出并停止有效反馈，必须重新确认中位，不能自动把旧角度当0度。

可选 `--gravity-gain 0.02` 是固定车身台架的重力辅助实验功能，默认关闭。
转轴接近重力方向时不参与校正；加速度模长异常时也不校正。它不补偿整车转动、倾斜、振动和电机影响。
单只 IMU 不能自动分离车身姿态与方向盘相对车身的转角，未完成整车运动干扰验收前不可声称可用于行驶闭环。

## UDP 和开机服务

`feedback/m-steering-imu.service` 默认发送 192.168.100.20:54321，使用已有48字节 STF1 v1。
service 默认没有机械归零，ZERO_VALID=0，不向 S 发出任何执行器命令。A 的接收器必须遵守共同协议，只允许新鲜、真实且已归零的反馈进入控制。
服务与 NPU/视频任务独立，SPI 每笔24MHz（板载驱动最低支持档），传感器100Hz；不修改 M 原有视觉进程。
S 不在线时仍可本地采集，UDP 发送失败不会阻塞采样。
发送使用不绑定源地址的`sendto`，由每个数据包的当前路由选择源IP。
这是为了兼容开机地址随后被`setup_network.sh`切换为192.168.100.10的情况；
长期`connect`的UDP会话可能保留切换前的地址，导致Network is unreachable。
错误按2秒限频并累计，恢复后打印`IMU UDP recovered`；不能仅凭本地send成功认定S已收到。
S接收工具可用`--duration-ms 0`持续查看，仍不访问执行器。

主机读取快照超过20ms时丢弃本次快照并在既有100ms期限内重试，不重建角度会话。
此处理只针对主机调度延迟；传感器时钟反向、会话变化、重复/过期数据及持续断流仍拒绝。

```sh
sudo systemctl start m-steering-imu
journalctl -u m-steering-imu -n 20 --no-pager
```

## 实物验收，不能由仿真替代

1. 指针/量角器给定 ±15、±30、±60度，记录正负号和最大误差。
2. 往返至少5次回到同一刻度，记录回零误差。
3. 固定机构静置60秒，记录漂移；记录温度与零偏变化。
4. 保持机构相对车身锁定，转动/倾斜车身，记录混入角度。
5. 比较电机关闭/开启时方差、丢样和漂移；架空后由现场人员操作。
6. 拔传感器、重启采集、停止网络，确认反馈失效和 session 改变；后续由 A 检查输出撤销。

统计工具：

```sh
python3 tools/analyze_recording.py --input relative.csv --expected-final-deg 0 --output return_zero_report.json
```

真实静止记录只能证明当时的数据与漂移，不能证明转角精度、轴向和运动干扰已经通过。现场误差容差由 A/B 商定。
