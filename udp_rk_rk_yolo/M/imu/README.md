# B：M 端 MPU-9250 采集与转角估计

目标路径：MPU → FMC 扩展板 → M FPGA 的 I²C 主机 → M RK3568 计算转角 → UDP → S 端接收与控制。
本目录完整归 B：传感器采样、测角、工具、测试和后续 UDP 发送都放在 M 端。
FMC 上所选两根信号不是 RK 原生 I²C；不能凭线名假设某个 `/dev/i2c-*` 已连接到它们。

## 当前边界

- `mpu9250_transport.h` 定义读寄存器/写寄存器回调及 WHO_AM_I 检查。
- `mpu9250_transport.c` 中的 `mpu9250_fpga_open()` 返回 NOT_READY，尚未定义实际 FSPI 操作码/寄存器布局。
- `imu_source.c` 的 mock 可运行，FPGA 路径仍明确拒绝产生测量值。
- `steering_angle.c` 只有台架单轴相对积分参考算法，不是完整姿态解算。
- 公共头来自 `common/steering/contracts/`，不复制协议，不依赖 S 端源码。
- 工具源码在本目录 `tools/`，测试在 `tests/`，编译产物在 `build/`。
- `feedback/` 说明后续 M→S UDP 发送职责；当前没有实时发送器。

## 独立构建和检查

从仓库根目录执行，不会编译 S 端：

```powershell
python udp_rk_rk_yolo/M/imu/tools/build.py --test
.\udp_rk_rk_yolo\M\imu\build\imu_dump.exe --backend mock --samples 100
.\udp_rk_rk_yolo\M\imu\build\angle_monitor.exe --backend mock --mock-rate-dps 30 --samples 100
```

M 端 Linux 使用 `python3`，程序名去掉 `.exe`；在本目录也可 `make` 或 `make test`。
需要同时拥有 `M/imu/` 和 `common/steering/`，无需 S 目录即可独立构建。

## B 的实现顺序

1. 核对接线与 IO 电平，新增 FPGA MPU 模块；先读 WHO_AM_I=0x71，明确无 ACK/超时错误。
2. 实现 RK↔FPGA 的独立读回协议，先让 `mpu9250_read_identity()` 通过真实传输调用成功。
3. 完成唤醒、量程/滤波/采样率配置，读取连续完整加速度、角速度和温度样本。
4. FPGA 记录采样序号与采集时刻，读回保持一帧一致，不能将多次读取的字节拼成混合样本。
5. 在 `imu_source_open/read()` 接入真实快照和单位换算，去掉该路径的 NOT_READY。
6. 更新工具的真实/模拟显示分支；真实数据绝不带 SIMULATED，失败不刷新旧样本年龄。
7. 做静态偏置、转轴和机械中位标定，并保存配置与独立物理参照结果。
8. 验证静置漂移、正反转回零、方向盘锁定时整车转动/倾斜，以及电机开关影响。
9. 在本目录接入 M→S UDP 发送器；A 在 `S/steering/feedback/` 接收，双方遵守共同接口的时间/会话/有效性规则。

电路/引脚任务见 `fpga/fpga_pcie_ov5640/source/mpu9250/README.md`。
第一版建议 I²C 100kHz，目标采样 100Hz；这是初始配置，不是已实测性能。
四线方案不包含独立 INT，需通过数据就绪状态或 FIFO 等机制区分新样本。
积分使用采样时间间隔，不能用终端打印或网络接收间隔替代。

## FSPI 与既有功能

M 端已有程序会初始化 FSPI。初次 bring-up 可独立运行；与视觉程序同时运行前，
需统一总线访问所有者/串行化配置及事务，不允许两个进程抢同一设备模式。
不要直接复用 S 端“运动寄存器”的地址含义，也不要把传感器数据塞入摄像头 DMA 格式。
FPGA 采集建议默认关闭，只有 M 端配置明确启用；避免影响 S 端既有控制。
