# FMC 接线引脚清单

核对日期：2026-11-01。

本清单记录当前系统外接遥控器、HMI 串口屏和 MPU-9250 使用的 FMC 引脚，供接线核对。S、M 两端分别列出。

**J3-xx 表示 FMC 扩展板 J3 排针的针脚编号；FPGA 球位是主板芯片的封装球位，两者不能混用。**

## S 端 遥控器与 HMI 串口屏

| 用途／连接对象 | FMC 扩展板针脚 | FMC 信号 | 主板 FPGA 球位 | 代码端口／方向 |
|---|---|---|---|---|
| 遥控器 GO 前进信号端 | **J3-20** | LA27_P | G17 | `go`，输出 |
| 遥控器 BACK 后退信号端 | **J3-22** | LA26_P | J14 | `back`，输出 |
| 遥控器 LEFT 左转信号端 | **J3-24** | LA22_P | F15 | `left`，双向 |
| 遥控器 RIGHT 右转信号端 | **J3-26** | LA23_P | E22 | `right`，双向 |
| 遥控器 STOP 刹停信号端，对应 S/R | **J3-28** | LA19_P | H13 | `stop`，输出 |
| 遥控器 SPEED 调速信号端 | **J3-34** | LA14_P | A18 | `speed_key_n`，双向 |
| **接 HMI TX**：屏幕向 FPGA 发送按键命令 | **J3-30** | LA20_P | C22 | `RXD`，FPGA 输入 |
| **接 HMI RX**：FPGA 向屏幕发送档位显示信息 | **J3-29** | LA20_N | B22 | `hmi_status_txd`，FPGA 输出 |
| 遥控器、HMI 与 S 板共地 | **J3-1、J3-37 或 J3-38** | GND | — | 地线 |

### 信号与接线说明

- 遥控器 GO、BACK、LEFT、RIGHT、STOP、SPEED 均为**低电平有效**。
- LEFT、RIGHT、SPEED 有效时驱动低电平；释放时输出高阻 `Z`，由遥控器原有上拉恢复高电平。高阻不等于 FPGA 主动输出高电平。
- GO、BACK 沿用现有锁存和控制门控规则，不能仅凭“低电平有效”推断松开 HMI 后必然立即变高。
- S/R 和自动刹停共同使用 STOP 输出，没有另外一根 REARM 实体信号线。
- **SPEED 当前接 J3-34／LA14_P／A18；旧 J3-27／LA19_N／G13 不再作为 SPEED 接线。不要同时连接新旧两个针脚。**
- HMI 串口交叉连接：**HMI TX → J3-30；HMI RX ← J3-29**。RX 接收的是 FPGA 发出的档位显示更新，不是摄像头图像或 MPU 原始角度。

## M 端 MPU-9250 陀螺仪

| MPU-9250 模块接点 | FMC 扩展板针脚 | FMC 信号 | 主板 FPGA 球位 | 代码端口／用途 |
|---|---|---|---|---|
| SCL | **J3-36** | LA13_P | B20 | `mpu_scl`，I²C 时钟 |
| SDA | **J3-35** | LA13_N | A20 | `mpu_sda`，I²C 数据 |
| GND | **J3-37** | GND | — | 模块与 M 板共地 |
| 已确认支持 3.3V 输入的模块电源脚 | **J3-39** | A3V3 | — | 3.3V 供电 |

### 信号与接线说明

- SCL、SDA 为开漏 `0/Z` 接口，上拉到 **3.3V**，不能上拉到 5V。
- 电源应接模块上已确认支持 3.3V 输入的电源脚，按实际模块标识核对，不能仅凭 VCC 或 VIN 名称推断。
- MPU 角度由 M 端采集后通过两板网络上的 UDP 传给 S 端，**不需要另接 M→S 的 FMC 角度信号线**。
- 需要改接线时先断电；传感器线缆应留出随转向机构运动的余量。

## 核对依据

- [S 端当前引脚约束](fpga/fpga_pcie_ov5640/project/project.fdc)
- [M 端当前 NACK 重试版引脚约束](fpga/fpga_pcie_ov5640/project_m_imu_nack_20261101/mpu9250_m.fdc)
- [SPEED 最新接线说明](udp_rk_rk_yolo/S/SPEED_RIGHT同路实现与接线_20261029.txt)
- [M 端 MPU 硬件与协议说明](udp_rk_rk_yolo/M/imu/HARDWARE_AND_PROTOCOL.md)
- [主板硬件手册](fpga/fpga_pcie_ov5640/doc/doc/RK3568_MES2L50H_100H_硬件使用手册_v1.1.pdf)：FMC 信号与本机 FPGA 球位对应关系。
- [FMC 子卡手册](fpga/fpga_pcie_ov5640/doc/doc/202506161424259.pdf)：J3 针脚与 FMC 信号对应关系。该手册中的其他封装球位不能替代本机 `.fdc` 球位。

本次仅新增接线清单文档，未修改代码、引脚约束、HMI 或烧录文件。
