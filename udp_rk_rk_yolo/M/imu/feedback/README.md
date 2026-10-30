# M → S 反馈

发送器已集成在 `angle_monitor --send-to IPv4 --port 54321`。复用共同目录的 STF1 v1 编码器；不修改视频 UDP 及 curve_angle。

- 新样本才推进序号，重复 FPGA 快照不生成新反馈。
- sample_time_ns 来自 FPGA 采集时间差映射的源单调时钟；不能与 S 的时钟直接相减。
- source_age_ms 使用 FPGA 样本年龄与本机后续耗时，CRC/断流/时钟异常不刷新有效性。
- 默认没有 ZERO_VALID；只有显式标定安装轴、固定车身、确认机械中位的台架运行才允许置位。
- service 自动启动始终不声明机械中位。停止/异常后由 S 的新鲜度机制撤销权限，不能靠无限重复最后一包维持控制。
- S 当前不在本任务连接范围，未安装/修改 S 接收器，也未宣称 M→S 闭环已验收。

服务可从 `/etc/default/m-steering-imu` 的 `IMU_CALIBRATION_ARGS` 读取
`--calibration /绝对路径/安装标定.cal`。本次实板仅应用测得的静止零偏，Z 轴仍是待验证参考轴，
服务没有 `--zero-at-center`；不能将其相对角度作为已确认的机械中位角度。
