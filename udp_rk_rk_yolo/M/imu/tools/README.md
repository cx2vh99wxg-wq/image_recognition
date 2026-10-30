# B 工具

- imu_probe：验证 IMU1 FPGA 协议、WHO_AM_I、状态、序号及错误码。
- imu_dump：真实/模拟原始数据、时间戳、实际 dt、序号缺口及 CSV。
- angle_monitor：偏置和任意安装轴投影、梯形积分、多圈相对角度、显式机械归零、可选重力实验校正、STF1 UDP。
- calibrate.py：静止记录质量检查、零偏、物理左向扫转的轴向估计；不把启动位置当中位。
- analyze_recording.py：采样统计、漂移、参考终点误差。
- feedback_inspect：已有 STF1 文件解码。

用法和现场验收步骤见上级 README。所有 CSV 使用新文件名，禁止覆盖已有测量记录。
硬件目标是 M/Linux；mock 始终带 SIMULATED，不能通过真实标定，也不能被置为已归零。
