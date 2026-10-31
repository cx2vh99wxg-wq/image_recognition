# M → S 反馈

发送器已集成在 `angle_monitor --send-to IPv4 --port 54321`。复用共同目录的 STF1 v1 编码器；不修改视频 UDP 及 curve_angle。

- 新样本才推进序号，重复 FPGA 快照不生成新反馈。
- sample_time_ns 来自 FPGA 采集时间差映射的源单调时钟；不能与 S 的时钟直接相减。
- source_age_ms 使用 FPGA 样本年龄与本机后续耗时，CRC/断流/时钟异常不刷新有效性。
- 默认没有 ZERO_VALID；只有显式标定安装轴、固定车身、确认机械中位的台架运行才允许置位。
- service 自动启动始终不声明机械中位。停止/异常后由 S 的新鲜度机制撤销权限，不能靠无限重复最后一包维持控制。
- S已有持续被动接收工具；2026-10-31新增主FSPI进程内的单消费者角度接收器和闭环入口。
- 新控制器沿用48字节STF1，无需改变视频/IMU线格式；M在hold时仍持续推进真实采样序号和时间戳。
- S的操作员物理零点偏移绑定一个M会话，M重启或imu-zero导致会话变化时必须重新确认。
  S不会因为收到REAL_RELATIVE自动将当时角度当作物理零点，也不自动修改M的ZERO_VALID。

服务可从 `/etc/default/m-steering-imu` 的 `IMU_CALIBRATION_ARGS` 读取
`--calibration /绝对路径/安装标定.cal`。本次实板仅应用测得的静止零偏，Z 轴仍是待验证参考轴，
服务没有 `--zero-at-center`；不能将其相对角度作为已确认的机械中位角度。

新增只读健康检查：`python3 tools/feedback_health.py --seconds 3`，检查常驻进程和样本是否持续推进；
不重启采集、不访问SPI、不设置零位。它是低频诊断，不替代S闭环100ms的新鲜度检查。
