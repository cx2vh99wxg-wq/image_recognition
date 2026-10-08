# C 部分：控制与 FPGA（FSPI 主机 + 串口屏 UART + RTL）

> 本文档说明人员 C 负责的模块如何建立在已重构的 A、B 代码之上，以及需要衔接的接口。

## 一、目录与职责划分（v1.0 统一）

| 目录 | 主责 | 内容 |
|------|------|------|
| `common/` | B | 公共接口与「魔法数字」真源；`fpga_regmap.h` 由 **C 起草**、B 集成 |
| `perception/` | A | 感知（车道/红绿灯/斑马线/虚实线 + ISP 参数表） |
| `planning/` | B | 决策融合（`decision.c`）+ UDP + LCD 渲染 |
| `control/` | **C** | FSPI 主机驱动 + 控制主循环（读 B 的命令并落地执行器） |
| `fpga/` | **C** | RTL：FSPI 从机 + 串口屏 UART + 顶层连线 |
| `HMI/` | **C** | 陶晶驰串口屏工程（二进制 `.HMI`，不参与文本重构） |
| `scripts/` | B | 启动/清理/网络 |
| `docs/` + `Makefile` | 顶层 | 文档 + 一键编译 |

## 二、C 需要衔接的已重构 A/B 接口

C 的 `control/` 模块**只读** B 的共享内存与公共类型，不写任何共享段：

| 接口 | 来源 | 用途 |
|------|------|------|
| `ControlCommandMsg` / `ControlCommand`（`CMD_*`） | `common/include/driving_types.h`（A 起草） | B 写 `shm_cmd`，C 读并执行 |
| `PersonState` | `common/include/driving_config.h`（B 暂存契约） | B 写 `shm_person`，C 读做行人安全兜底 |
| `shm_read_cmd()` / `shm_read_person()` | `common/include/shm_ipc.h`（B 实现） | 读共享段（内部双读校验 + 失败退避） |
| `SHM_KEY_CMD` / `SHM_KEY_PERSON` / `DEC_BRAKE_CONF_MIN` / `DEC_LOOP_MS` | `common/include/driving_config.h` | 配置真源 |
| `LOGI/LOGW/LOGE/LOGD` | `common/include/log.h` | 统一日志（`LOG_TAG=CONTROL`） |
| `now_us_mono()` | `common/include/time_util.h` | 单调时钟（预留） |
| `fpga_regmap.h` | `common/include/fpga_regmap.h`（**C 起草**） | FSPI 寄存器契约，`control/` 与 `fpga/` 共用 |

**职责迁移要点**：旧版 `udp_rk_rk_yolo/S/fspi/main.c` 自行读弯道/行人做转向与制动决策；
新版决策已上移到 B（`planning/csrc/decision.c` 输出 `ControlCommandMsg`），C 只负责**执行**
（`CMD_GO/BACK/LEFT/RIGHT → 前进/后退/转向`，`CMD_STOP/BRAKE → 急停`）。

## 三、FSPI 寄存器映射契约（fpga_regmap.h）

- 物理层：QUAD SPI 4 线、Mode3、100MHz、写帧 `[0x00][addr][data]`。
- 命令字节：`0x00`=写 / `0x80`=读（读路径历史保留，主机未触发）。
- 寄存器（低有效，0=运动/1=停止）：`0`=前进 `1`=后退 `2`=左转 `3`=右转 `4`=急停。
- 急停寄存器为**下降沿触发**：写 0 触发 FPGA 1s 停车脉冲，写 1 复位。

## 四、构建

```bash
make control                        # 本机：纯逻辑编译 + 单测
make control USE_STUB=1             # 桩模式
make CROSS=aarch64-linux-gnu- -C control bin   # 板端：control_main
make board                          # 顶层一键板端构建（含 control_main）
```
