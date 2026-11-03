# 菠萝快跑 FPGA RTL 重构

本文记录已通过验证的四文件阶段。扩大到两套 PDS 的全部自研 RTL 后，工程目录中还有正在重构的源码。这里的替换包、仿真日志和布线结果均对应四文件阶段。[扩大范围进度](EXPANDED_STATUS.md)

日期 2026-11-03。对照基准是本轮开始时工作区内两套 PDS 实际引用的 RTL。上游作者、日期和许可信息保留在源文件头。

| 工程 | PDS 入口 | 顶层 | 已解析输入引用 |
|---|---|---|---:|
| S | project_s_application_refactor_20261102/project_s_refactor.pds | image_pcie_capture | 343 |
| M | project_m_application_refactor_20261102/project_m_refactor.pds | image_pcie_capture_m_imu | 348 |

引用及 SHA-256 见 [pds_inputs.json](pds_inputs.json)。工程入口、器件和顶层与 2026-11-02 的构建清单一致。两端引用同一批 167 个 `source` 路径，其中包括本次四个改动文件。M 端另有五个独有文件，分别是 `image_pcie_capture_m_imu.v`、`fspi_slave_top_m_imu.v`、`fspi_slave_m_imu.v`、`imu_i2c_master.v` 和 `mpu9250_sampler.v`，均位于 `source/mpu9250`。S 端无独有的 `source` 路径，两套工程仍分别引用各自的约束与厂商 IP 目录。摄像头采集使用 `source/ov5640/rtl` 下的版本，历史 `ov5640_dri` 下有同名文件。M 端专用 FSPI 接入共用的 `spi_ram_crtl`。两端顶层均实例化挡位模块，并经 `cnn_top` 使用 Gamma。静态检查扫描了现行约束中的模块和调试信号引用。SPI 的原状态编码、调试寄存器名称和标记保留。

| 文件 | 原结构 | 本次结构及维护收益 |
|---|---|---|
| `fspi/spi_ram_crtl.v` | 分开的读写高地址入口和低地址处理分支 | 在同一有效字节入口解码命令，读写共用事务高字节寄存器和低字节解析阶段。地址锁存时预解码寄存器范围，写提交和读完成各自处理。地址拼接只维护一处 |
| `remote_vehicle_control.v` | CDC、消抖、估计和调度混在一个时序过程。HMI 逐字节列 ASCII 常量 | 分成 CDC、触点估计、调度三组寄存器。HMI 用两个可读的报文前缀和动态字节组织数据。事件优先级与 UART 发送状态机仍沿用原次序 |
| `ov5640/rtl/cmos_capture_data.v` | X/Y 计数、启动帧计数和门控分别散落在多个过程 | X/Y 共用列末条件，启动计数与门控集中，字节组装与有效相位集中。每个时序过程对应一个用途 |
| `cnn/gamma_judge.v` | 顺序比较全部阈值 | 按阈值范围分层选择，并显式分开旁路与曲线路径。减少阅读单条长优先链的负担，原曲线表、实例与取样级保持一致 |

## 基线中的易误判行为

- SPI 以 `wr_clk` 下降沿工作。写数据阶段置位的 `ram_wr_en` 在下一下降沿由寄存器组消费，提交时使用当时已有的 `wr_data_reg`。片选撤销经寄存后令解析器回到空闲并保持地址。已排队的写使能仍可能被消费。复位时地址 0 至 4 为 1，地址 5 和 6 为 0。地址 0 至 6 的内部读回给出当前 HMI 选择码，其他地址给出 FF。S 端 ARM 写缓冲区仍为地址高、地址低、数据三字节，SMDT 插入操作码。
- M 端 ARM 快照请求负载为 `4D 01 A5`，寄存器组对这个地址越界，IMU 采样器另行观察该命令。SMDT 原生 `80` 读取的 8 字节状态前缀加 64 字节 IMU 快照由 M 专用传输器输出，快照在开始返回时锁存。独立 `81 00 05` 返回 AUTO 状态。
- 挡位调度每拍先应用原状态动作，再处理目标、人工接管、最后处理符合空闲条件的 A5。A5 本身不清脉冲计数器。人工长按由同步后的输入电平直通，挡位只按有效物理接点边沿累计。STOP 与 REARM 在本文件的独立控制和仲裁模块中实现。对应模块与快照逐字节一致，并由两版的既有 STOP、方向仲裁和 HMI 联调用例覆盖。
- HMI 文本命令仍发送 `page0.tGear.txt=`、引号、挡位字节、GB2312 的 `B5 B5`、引号和三字节 FF。滑块就绪时追加 `page0.vaGearActual.val=`、数值字节和三字节 FF。未知档位的滑块数值仍是字符 0。发送请求、忙等待和字节间隔逻辑原样保留。
- 摄像头默认在第 11 个检测到的 VSYNC 上升沿打开门控。VSYNC/HREF 由原两级寄存器输出。奇数字节后的首个消隐周期可能产生一次有效脉冲，仍输出先前组装的字。坐标只按已送出的有效脉冲推进，VSYNC 不清坐标。中途复位恢复原计数和门控状态。
- Gamma 分数使用延迟一级的 `per_data_1`，570 及以下旁路取当前输入 `per_img_Y`，曲线取原曲线模块已寄存的值。是否更新由延迟一级的 HREF 判定，无效时输出保持。分数比较使用包含上界，超过 1012 选择 Gamma 18。对无法通过任何关系判断的未知分数保留保持分支。

## 替换与复跑

本地工程直接使用 `fpga/fpga_pcie_ov5640/source` 下的 RTL。原文件快照在 [original](original)，可按同样的 `source` 相对路径回退。对照参考文件在 [reference](reference)，用于仿真编译。两套 PDS 的工程文件继续使用原路径。

在 Linux 或 WSL 中从工程根目录运行以下命令。需要 Verilator 5.x、C++ 编译器和 make。

```bash
bash output/rtl_refactor_20261103/run_sim.sh baseline
bash output/rtl_refactor_20261103/run_sim.sh revised
bash output/rtl_refactor_20261103/run_sim.sh compare
```

可用 `ONLY_TEST=compare_vehicle` 选择单项，用 `SIM_TIMEOUT` 调整单个仿真的宿主超时。对照用例在 [tests](tests)，日志在 [results](results)。原版 UART 长按测试首次达到 30 秒超时。该用例含两次超过一亿拍的等待，延长到 180 秒后通过，实测约 41 秒。对照测试初次采用 UART 8 倍时钟时，原版 UART 的停止计数条件无法达成，覆盖断言失败。改用原用例的 64 倍时钟后，通过字节数和并发覆盖检查。

`run_all.sh` 先运行原版，随后并行运行新版和逐拍对照。可通过 `SIM_RESULTS_DIR` 指定独立的日志目录，保留旧记录。
在扩大范围后的工作区重跑四文件阶段时，可将 `RTL_SOURCE_DIR` 指向 `$PWD/output/rtl_refactor_20261103/pds_recheck/source`。

| RTL 检查 | 结果 |
|---|---|
| 既有 `sim/test_*.v` | 原版 12/12，新版 12/12 |
| SPI | 2,642,760 个下降沿，穷举全部 65,536 个地址的读写，另含 CS 中断和复位。逐拍比较全部输出、状态、地址、两类高字节调试寄存器、写使能和字节计数 |
| M 专用 FSPI | 5,086 个寄存器时钟下降沿，含 72 字节状态与 IMU 快照、非零地址写入、越界触发和 AUTO 状态读回。PLL 输出用理想时钟模型驱动 |
| 挡位与 HMI | 88,299 拍，129 个实际 UART 字节发送事件。检查目标与 A5 同拍、接点回绕、人工接管、同挡无脉冲。额外逐一比较 64×256 个字节索引及档位输入 |
| OV5640 | 616,906 拍，并行检查默认 640×480 与 5×3 实例。默认坐标完成一次整帧回绕，覆盖奇数字节、消隐和中途复位 |
| Gamma | 263,172 拍，全部 1,024×256 分数和亮度组合及消隐复位。逐拍比较 RGB、Y、延迟 Y、VSYNC、HREF |

本次用 Verilator 5.048 在独立日志目录复跑，既有用例原版 12/12、新版 12/12，逐拍对照 5/5，退出码为 0。RTL 对照使用两态模型，覆盖表中输入和复位序列。跨时钟域的亚稳态及实际引脚、电气时序留给板级检查。[复跑日志](results_recheck)

Windows PowerShell 中可运行下列命令。脚本构建隔离副本，使用 2026-11-02 构建相同的实现参数。默认沙箱中的首次启动出现 WinError 623 系统 DLL 重定位错误，经授权在默认沙箱外执行后 PDS 正常启动。

```powershell
$env:PINEAPPLE_PDS_OUTPUT = 'pds_address_predecode'
& 'D:/Anaconda/python.exe' -X utf8 'output/rtl_refactor_20261103/build_pds.py'
& 'D:/Anaconda/python.exe' -X utf8 'output/rtl_refactor_20261103/summarize_pds_timing.py'
```

首轮实现已完成编译、综合、布局布线和报告，S/M 的 FSPI 同域慢角分别出现 −0.179 ns 和 −0.157 ns 的新增负裕量。S 端最差路径从 `ram_addr[6]` 到控制寄存器写入。最终 RTL 把地址范围判断提前到地址锁存沿，并保留原写提交沿。首轮日志在 [pds](pds)，最终运行在 [pds_address_predecode](pds_address_predecode)。

## 最终 PDS 结果

两端的编译、综合、器件映射、布局布线和时序报告均完成。布局布线完成与时序收敛分别记录。以下裕量单位为 ns。

四文件阶段在隔离副本中重新构建，M/S 两端均退出 0，重新生成的时序对照与下表一致。[PDS 复跑结果](pds_recheck/results.json)

| 工程 | FSPI 同域慢角 Setup | 全设计慢角最差 Setup | 全设计快角最差 Setup | 慢角最差 Hold | 快角最差 Hold |
|---|---:|---:|---:|---:|---:|
| S | +0.200 | −7.242 | −4.085 | +0.066 | +0.026 |
| M | +0.202 | −8.735 | −6.059 | +0.137 | +0.100 |

原有摄像头与 DDR 等跨时钟路径仍有 Setup 负裕量。相比 11 月 2 日工程，S 端新出现两组慢角负裕量，分别是 DDR PHY 路径 −0.103 ns 和 PCIe `pclk` 同域路径 −0.272 ns。M 端新出现一组 DDR PHY 慢角路径 −0.220 ns。三个新负裕量的起止点都在原厂 IP 实例内部，需要进一步完成时序收敛再作最终板级验收。两端快慢角 Hold 均无负裕量。[逐时钟对结果](timing_comparison.json)

S 的 112 条、M 的 114 条 `def_port` 记录与基线逐项一致，比较涵盖引脚位置和 IO 属性。[引脚对照](pin_comparison.json)

SPI 调试寄存器名及 RTL 标记仍可用于仿真层次访问。PDS 器件映射会清理部分无扇出的位，例如 S 端的 `addr_high[7:0]` 和 `ram_addr[15:3]`。现行约束未引用这些位，若用片上逻辑分析仪观察它们，应按实现网表重新确认探针。

工作区、替换包和最终 PDS 输入里的四个 RTL 文件逐字节一致。[替换文件哈希](replacement_hashes.json)

按此前同目录词元口径复测，软件源码目录为 35.46%，FPGA 源码目录为 99.17%。统计器跳过构建输出和二进制文件，受保护的 `.vp` 单列文件哈希。过滤已识别的厂商代码和测试后，软件应用及工具为 28.57%，FPGA 应用与板级支持为 92.58%。本次复扫结果与原统计 JSON 一致。[目录重合统计](results_recheck/directory_comparison_20261103.json)

## 与原始目录的重合度

功能基准使用本交付的 `original` 快照。重合度以相邻的 `D:\Study_Object\image_recognition\image_recognition_origin` 为参考。下表比较同路径 RTL 中被至少 40 个连续相同词元覆盖的词元比例，忽略注释和空白，分母分别使用重构前后各自文件的词元总数。

| PDS 共用 RTL | 重构前 | 重构后 |
|---|---:|---:|
| `fspi/spi_ram_crtl.v` | 79.29% | 50.00% |
| `ov5640/rtl/cmos_capture_data.v` | 100.00% | 54.38% |
| `cnn/gamma_judge.v` | 100.00% | 73.54% |
| `remote_vehicle_control.v` | 原目录无同路径文件 | 原目录无同路径文件 |

从工程根目录运行 `python output/rtl_refactor_20261103/compare_origin.py` 可复测。[逐文件数据](origin_target_files.json)

以 PDS 清单筛选实际引用的应用及板级源码，排除统计器识别的厂商 IP、测试和生成目录，再与原目录的 FPGA 应用源码跨文件匹配。S 端 145 个文件，覆盖率 94.31%。M 端 150 个文件，覆盖率 92.20%。四个目标文件之外的匹配词元已分别占 S 和 M 当前总词元的 92.75% 和 90.71%。

整目录统计中，仅 `ipcore` 下已记录的长匹配区间就有 901,515 个词元，占 FPGA 全目录 4,046,400 个词元的 22.28%。在保留厂商 IP 和工程内容、只重构这四处的范围内，FPGA 全目录未达到 10%。上述结果为本地统计，实际查重结果取决于平台口径。

## 板上验证

当前交付到 RTL、离线仿真及 PDS 实现报告，实板验收尚未执行。

上板需分别核对 S/M 的 FSPI 读写及片选中断，M 端 IMU 快照 CRC 与 I²C 重试计数，档位长按、回绕、滑块与 A5 校准，以及摄像头冷启动后的首帧、分辨率和 Gamma 画面。STOP 与 REARM 验收时同时检查牵引和转向门控。
