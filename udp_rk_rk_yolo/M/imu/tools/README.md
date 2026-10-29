# B：独立采样与测角工具

工具、采样与测角实现均位于 `M/imu/`，最终在 M 端运行。
本目录的构建只依赖 M 端源码和 `common/steering/` 公共接口，不依赖 S 目录。

从仓库根目录：

```sh
python3 udp_rk_rk_yolo/M/imu/tools/build.py
./udp_rk_rk_yolo/M/imu/build/imu_dump --backend mock --samples 100
./udp_rk_rk_yolo/M/imu/build/angle_monitor --backend mock --mock-rate-dps 30 --axis z --samples 100
```

Windows 用 `python` 和 `.exe`。在 M/imu 中也可 `make`。
选项：`--samples`、`--interval-ms`、`--print-every`、`--csv`、`--axis x|y|z`、`--axis-sign 1|-1`。
mock 是恒定 Z 轴角速度、Z 轴重力、25°C 的人工数据，明确带 SIMULATED 标志。
CSV 保存每个样本，终端默认每 10 个打印一次。

`angle_monitor` 是可测试的台架参考积分器：沿标定轴减偏置后按真实采样间隔做梯形积分。
它从启动时记相对 0°，ZERO_VALID 始终为 0。尚无静态偏置自动标定、重力融合、车身运动补偿，不能用于实车闭环。
重复样本、旧序号、源时钟异常、会话变化和过大采样间隔会报故障，需要显式重启/重新建立基准。

```sh
./udp_rk_rk_yolo/M/imu/build/imu_dump --backend fpga --samples 1
```

当前此命令必须失败并提示 NOT_READY；没有打开 SPI，也不会把 mock 当真数据。
下一步先实现 FPGA 身份读回，再补传感器初始化、完整样本及数据源。

`feedback_inspect --file packet.bin` 仅解码一个完整 STF1 文件，不连接网络，不判断反馈是否可用于控制。
