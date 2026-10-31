# S 端角度遥测与控制接入边界

传感器接在 M 端，本目录不实现本地 MPU I²C 驱动。
合并版本新增只读UDP监测工具，接收M端实际STF1角度并记录CSV，不初始化FSPI或发送执行器命令。
`steering_feedback_read()`及正式角度控制API仍为NOT_READY，不把尚未安装/标定的IMU直接接入实车控制。

在S板的`/home/linaro/S/steering`运行：

```sh
python3 tools/build.py --monitor
./feedback/build/steering_feedback_monitor --bind 192.168.100.20 --source 192.168.100.10 --port 54321 --duration-ms 10000 --csv logs/imu_telemetry_001.csv
```

`--duration-ms 0` 可持续监听直到显式停止。2026-10-31 恢复网络修复时，
已启动一个无CSV的持续被动监听器，日志在
`/home/linaro/imu_network_restore_20261031/S_receiver.log`，PID记录在同目录的
`S_receiver_pid`。它不自动接入控制、不写共享车控状态，也没有新增开机服务。
若后续闭环工具需要独占54321端口，先停止这个已知被动监听进程，避免两个接收者抢占端口。

CSV须使用新文件名；`raw_relative_deg`是源端报告的相对角度，不代表装车精度已经验收。输出始终显示`actuator_enabled=0`。

- `REAL_RELATIVE`：真实、VALID，未声明机械零位；可观察相对角度，不能作为已归中反馈。
- `REAL_ZERO_VALID`：源端设置了ZERO_VALID标记；接收工具仍不授权任何动作。
- `SIMULATED`、`INVALID`、`STALE`、`NO_SAMPLE`：明确区分模拟、无效、过期和未收到数据，不伪造实际0°。

`feedback_tracker`验证共同编解码结果、序号回绕、重复/乱序、源时间递增和数据年龄。重复及坏包不刷新本地接收时刻；源时钟异常在本会话中锁存，换会话才清除。最近16个退役会话被拒绝；这是遥测诊断缓存，不是完整重放防护或实车控制权限管理。

年龄判定只使用发送时年龄加S本地持有时长，不直接相减M/S时钟，**不包含尚未独立测定的网络传输时间**。源IP过滤也不构成身份认证。因此不能仅凭该工具状态就宣布正式闭环反馈可用；控制接入还需轴向/零位确认、延迟约束、会话授权和统一FSPI管理。

软件检查：`python3 tools/build.py --test`验证纯逻辑；`python3 tests/feedback/test_udp_monitor.py`验证原生UDP故障场景。可通过`--m-tool /path/to/M/imu/build/angle_monitor`追加生产M模拟发送器到生产S接收工具的端到端检查，测试始终不打开SPI。

B的发送器见`M/imu/feedback/`，共同线协议见`common/steering/contracts/README.md`；合并结果与实机证据见上级集成测试报告。
