# A：独立脉冲测试

## 操作

在 `S/steering` 中构建：

```sh
python3 tools/build.py
./actuator/build/steering_step --direction left --duration-ms 20
./actuator/build/steering_step --direction right --duration-ms 40 --repeat 3 --gap-ms 1000
```

默认只输出 DRY RUN。Windows 使用 `python` 和对应 `.exe`。

真实 S 端 Linux/RK 构建与运行：

```sh
python3 tools/build.py --hardware
mkdir -p logs
sudo ./actuator/build/steering_step_fspi --backend fspi --direction left --duration-ms 20 --csv logs/A_left_20ms_001.csv
sudo ./actuator/build/steering_step_fspi --backend fspi --direction release
```

CSV 必须指定新文件名，不覆盖旧记录。完整选项通过 `--help` 查看。
默认单次 1 个脉冲、最大 200ms；显式 `--max-pulse-ms` 可调整，但硬上限为 1000ms。
先验证小脉冲，不要因看不到动作就直接放大到机械限位。

## 电平与退出

- LEFT：先释放右 reg3=1，再请求左 reg2=0；RIGHT 相反。
- 每次动作前释放两侧，到截止时间或正常取消时释放两侧。
- 只写寄存器 2/3，不写 GO、BACK、STOP，不清除已有 STOP 锁存。
- Ctrl+C/SIGTERM 只在处理流程中撤销输出；进程被强杀、设备失联或掉电不能依靠软件释放。
- 当前没有新增 FPGA 最大脉冲超时。下地前需要独立的超时/停止保障。
- 硬件锁只约束本工具的其他实例，旧 `vehicle_control` 不遵守这个锁。
  真实测试前必须退出旧自动控制程序，固定车辆，保留可操作的物理 STOP。
- FPGA 仍可能通过 STOP、人工接管等门控拒绝请求；写事务成功不等于方向盘运动。

## 先测什么

1. 用物理标记确认初始角度，短脉冲后观察释放即刻、0.5s、2s 的角度。
2. 判断松开后保持还是自动回中；这由机构决定，不是路面算法参数。
3. 在安全范围内逐步改变时长，分别重复左/右动作，并记录反向回差。
4. 使用量角器/固定指针/独立视频刻度测角，逻辑分析仪测实际触点低电平时长。
5. 重复脉冲会累积转角；程序不会自动回中。每次要么记录真实起点，要么人工重置起点。

## CSV 的边界

记录请求时长，以及 enable/release 写调用的开始和返回时刻。
`software_elapsed_ms` 是释放返回与使能返回之间的时间，含部分通信耗时。
这些值都不是实际引脚波形，更不是角度反馈；实际边沿位于写调用的时间窗口内。
操作系统调度、驱动重试会拉长时间，工具不能承诺硬实时。

请另行记录物理初始/终止角、电池状态、负载、是否反向，以及实际波形时长。
