# S 端 UDP 反馈接收入口

传感器接在 M 端，本目录不实现本地 MPU I²C 驱动。
`steering_feedback_read()` 是后续专用 UDP 接收/回放模块的接入口。
当前清除有效标志并返回 NOT_READY，绝不把缺失数据报告成方向盘 0°。

A 在阶段 3 扩展本目录的接收端；B 的发送端位于 `M/imu/feedback/`。
共同规范见 `udp_rk_rk_yolo/common/steering/contracts/README.md`。
第一阶段先独立验证执行器、M 端采样和测角，实时收发留到双方联调。
