# M/S 自研 RTL 重构交付

范围按两套实际 PDS 的并集核对，包含 135 个自研 RTL 文件。文件清单另列 22 个 PCIe 厂商支持源文件、14 个带明确上游作者或版权标识的源文件，以及 1 个显示时序宏配置。工程仍使用原 ARM 程序、厂商 IP、引脚和时钟约束。四文件阶段的记录见 [初版说明](README.md)。

源码直接维护在仓库的 `fpga/fpga_pcie_ov5640/source` 下，两套工程引用原有路径。之前生成的 RTL 替换压缩包已删除。若需核对重构前的代码，可查看 [原始快照](original)。工作区已有的一份 FPS 字形表改动经逐拍对照发现会改变初始化行为，现已保留原表和触发语义，重构其显示选码与输出阶段。

## 高重合模块与 Setup 优化

| 自研模块 | 改动 | 时序与数据核对 |
|---|---|---|
| 字形叠加 | 将 624 行标牌字形和 230 行数字字形按字符成组赋值，重命名两组行存储 | 全部 854 行逐项等值。保持常量过程的原触发方式、读取索引和叠加寄存拍 |
| Gamma 共用查表 | 每个固定页独立按低位译码，再由高位选择字节 | 保持一级寄存器和未知输入保持逻辑。曲线数据、评分阈值及亮度选择与前版相同 |
| 三路 AXI 帧缓存 | 由一套通道连接规则生成四个 FIFO 实例 | 相机 0 仍供第 0、3 路使用，四条 AXI 读路径独立。复位、时钟与端口映射逐项核对 |

本轮自研 RTL 本地词元重合度由 **41.25%** 降至 **36.23%**，FPGA 应用及板级源码为 **51.51%**，完整 FPGA 源码目录为 **97.70%**。统计阈值为至少 40 个连续相同词元，参考目录为 `image_recognition_origin`。[自研 RTL 前后对照](active_scope_setup.json) · [目录统计](setup_static/directory_comparison_20261103.json)

上一轮 S/M 最差 Setup 均位于厂商写 FIFO 的读指针到像素时钟域首级同步器，关键延迟以布线为主。此轮保留厂商 IP、CDC 拍数、引脚和约束，使用同一份 RTL 分别测布局布线种子 3、5，按 Setup 裕量及 Hold 状态比较。

原版、新版各 12 个既有用例和 5 套重点逐拍对照均通过。扩展的 19 模块、2 个 AXI 模块、73 张 Gamma 表和完整 SCCB 配置序列对照也通过。[本轮仿真日志](results_setup)

种子 3 的 M/S 均完成编译、综合、器件映射、布局布线和时序报告，退出码均为 0。与上一轮相比，两端最差 Setup 在快慢角均改善，两端 Hold 为正。单位为 ns。

| 工程 | 慢角 Setup，上一轮 → 本轮 | 快角 Setup，上一轮 → 本轮 | 本轮慢角 Hold | 本轮快角 Hold |
|---|---:|---:|---:|---:|
| S | −8.905 → −6.960 | −6.525 → −4.088 | +0.130 | +0.089 |
| M | −8.006 → −7.103 | −5.731 → −4.459 | +0.123 | +0.063 |

Setup 仍有负裕量。两端 LUT 分别从 S 的 13,523 增至 13,675、M 的 14,442 增至 14,573。板级功能尚未验收。[种子 3 构建结果](pds_setup_seed3/results.json) · [种子 3 时序明细](pds_setup_seed3/timing_comparison.json)

种子 5 的 M/S 同样完成实现，快慢角 Hold 均为正。其慢角 Setup 分别为 S −7.339 ns、M −8.448 ns，快角分别为 S −5.504 ns、M −5.627 ns。两块板均以快慢角 Setup 都更好的种子 3 作为本轮结果。[种子 5 构建结果](pds_setup_seed5/results.json) · [种子 5 时序明细](pds_setup_seed5/timing_comparison.json)

```bash
export SIM_RESULTS_DIR="$PWD/output/rtl_refactor_20261103/results_setup"
python3 output/rtl_refactor_20261103/run_rom_compare.py
python3 output/rtl_refactor_20261103/run_expanded_compare.py --suite modules
python3 output/rtl_refactor_20261103/run_expanded_compare.py --suite axi
bash output/rtl_refactor_20261103/run_all.sh
```

PDS 复跑时将 `PINEAPPLE_PNR_SEED` 设为 3 或 5，并分别将 `PINEAPPLE_PDS_OUTPUT` 设为 `pds_setup_seed3` 或 `pds_setup_seed5`。两套工程使用原 PDS 入口。

## 查表模块深度重构

本轮直接修改 75 个本地 RTL 文件。两套工程文件和约束仍引用原路径。

| 路径 | 原结构 | 本地实现 |
|---|---|---|
| `cnn/Curve_Gamma_18.v` 至 `Curve_Gamma_90.v` | 每条曲线各自实现时钟过程和离散 case 项 | 每条曲线只描述 256 个字节，共用 16 页查表逻辑和一级输出寄存器。未知地址仍保持上次输出 |
| `cnn/gamma_judge.v` | 每条曲线声明独立标量网络，流水级信号以通用后缀区分 | 曲线结果按编号收进一个数组。评分、原亮度与帧行有效信号按流水级命名。阈值树及取样边沿沿用原行为 |
| `ov5640/rtl/reg_config.v` | 配置字以独立 case 项散布在时序过程里 | 以 24 位字打包配置表。保持索引变动触发和非阻塞更新，非法或未知索引返回全一 |

对照同一个 `image_recognition_origin`，按至少 40 个连续相同词元统计，135 个自研 RTL 的本地词元重合度由本轮开始时的 **58.81%** 降到 **41.25%**。FPGA 应用及板级源码为 55.12%，包含历史工程和厂商支持的 FPGA 源码目录为 97.82%。[本轮自研 RTL 前后对照](active_scope_deep_rom.json) · [目录统计](deep_rom_static/directory_comparison_20261103.json)

静态核对了全部 18,688 个 Gamma 表项和 251 个有效 SCCB 配置字。Verilator 逐拍比较了 73 条 Gamma 曲线、全部 512 个 SCCB 索引，以及从复位到配置完成的总线和寄存器字。[查表对照日志](results_deep_rom/rom_tables.run.log)

同一批回归用例在原版和新版各通过 12 项，重点逐拍对照通过 5 项。综合报告的 LUT 数量从 S 端 11,858 增至 13,523，从 M 端 12,758 增至 14,442。[本轮回归日志](results_deep_rom)

本轮 M/S 均完成编译、综合、器件映射、布局布线和时序报告，退出码均为 0。两端快慢角 Hold 均为正，S 端上一轮的慢角 Hold 违例在本轮结果中消失。最差 Setup 仍为负，而且比上一轮更低。S 慢角为 −8.905 ns、快角为 −6.525 ns，M 慢角为 −8.006 ns、快角为 −5.731 ns。功能对照使用 RTL 仿真，尚未上板。[本轮 PDS 结果](pds_deep_rom/results.json) · [本轮时序对照](pds_deep_rom/timing_comparison.json)

```bash
python3 output/rtl_refactor_20261103/run_rom_compare.py
SIM_RESULTS_DIR="$PWD/output/rtl_refactor_20261103/results_deep_rom" bash output/rtl_refactor_20261103/run_all.sh
```

在 Windows 复跑本轮 PDS 时，将 `PINEAPPLE_PDS_OUTPUT` 设为 `pds_deep_rom`。约束文件的哈希与上一轮一致，PDS 快照的全部输入哈希与工作区一致。

## 注释与常量整理

上一轮把 135 个自研 RTL 文件的普通注释整理为 UTF-8，并保留原作者、日期、版权和综合指令。倒数表中的 257 个完整 32 位二进制常量改用 8 个十六进制数字表示，保留 32 位宽度。该轮逐文件比对了归一化后的 RTL 和综合指令。[注释整理明细](comment_refresh_results.json)

整理版的 12 个既有回归、5 套重点逐拍对照和包含倒数表的 19 模块扩展对照均通过。[整理版回归与对照日志](results_comment_verified) · [独立模块对照日志](results_expanded/expanded_modules.run.log)

## 已落地的结构调整

| 模块组 | 原结构 | 当前工作区 |
|---|---|---|
| Gamma 曲线表 | 73 个模块分别为每个输入写一次寄存器赋值 | 共用同输出的 case 分支。静态展开核对了全部 18,688 项，保留上升沿更新与 X/Z 不命中时保持 |
| 卷积层对齐 | 七层和四通道延迟模块分别复制 RGB/Y 延迟连接 | 同一生成结构处理四路，保留现有参数、四个数据寄存级和原同步信号取样 |
| 卷积乘法 | 单通道和双通道实现各自手写九个寄存乘法 | 六组乘法使用共用参数化乘法级，原 tap 顺序、结果宽度及后续加减分别保留 |
| CNN 顶层 | 每个处理边界分别维护 R/G/B/Y 四根总线，RGB 直通分别打拍 | 处理边界按 RGBY 打包，RGB 用同一个三拍对齐过程，保留原 HREF 对 Y/Cb/Cr 的门控 |
| RGB 输出 | R/G/B 三组分散寄存器 | 按通道生成相同的数据级，原 R/B 异步复位、G 同步复位和饱和边界分别保留 |
| FSPI | S/M 各有完整的四线命令、返回状态机和寄存器桥接 | 共用传输引擎和寄存器端点，按板端参数维持返回索引宽度和 IMU 扩展，PLL 实例保留在板端包装层 |
| HMI UART | 配置、挡位、牵引租期在同一个过程 | 独立更新配置、挡位和牵引输出，完整的 04 命令仍优先于租期到期 |
| M 端 IMU | 微秒时间基和采集状态混在一起，I²C 分频与总线状态共用过程 | 分出采集时间基和 I²C 计数，命令判定和快照调度分开，沿用原优先级 |
| M/S 顶层 DMA | 相同的行填充与帧请求各维护一份 | 两端引用同一个格式化模块，复制前逐词核对了两份逻辑。旧实现填充值保持为全 1 |
| FPS | 两路像素域采样分散，十进制位单独求模和除法 | 合并两个采样移位过程，十进制位复用商并求余，静态遍历 512 个输入 |
| 字符叠加 | 每个字符分支分别给四个寄存器赋值，三路描边状态分开维护 | 将字符数字与使能编成同一个控制字，保留原字形表及描边到 RGB 的寄存延迟 |
| 相机与 SCCB | 三路相机复制像素时钟测量，初始化控制用数字编码 | 相机共用测频模块，按原时钟域和取样间隔独立实例化，SCCB 使用有名称的事务阶段 |
| AXI 写入 | 单路 EOF 与总线状态共用过程，多路位置依赖公共状态机内的动态索引写入 | 独立保存 EOF 请求和每路写入位置，保留帧同步与末拍相遇时的覆盖优先级 |
| ISP 窗口与形态学 | 两种位宽各自实现边缘扩展，膨胀与腐蚀各有窗口和像素延迟 | 共用窗口边缘级、二值形态学与两级组合，按各路径参数保留开闭运算顺序 |
| 颜色、识别与行缓存 | 视频时序、位置与 RGB 分开打拍，分类条件分散在平行表 | 按像素束推进侧带，分类规则用同一条记录保存，行缓存保留两路不同的地址及使能延迟 |
| UART 收发 | RX/TX 各有相同的周期计数器和逐位常量分支 | 共用位周期计数，数据位用索引访问，保持起止位边沿以及 RX 采样与 TX 忙信号时序 |

[源文件和哈希盘点](expanded_source_inventory.json) 记录了两套 PDS 实际引用的源文件、修改前后哈希和上游文件来源。

## 与原目录的比较

以下为结构重构阶段的统计，以相邻 `image_recognition_origin` 的源码为对照，忽略注释与空白，统计被至少 40 个连续相同词元覆盖的比例。135 个自研 RTL 的覆盖率由 94.53% 降至 59.70%。同一批文件按板端统计，S 为 60.25%，M 为 59.70%。包含历史工程和厂商支持的 FPGA 源码目录为 97.83%，应用及板级源码目录为 66.53%。本地各口径均未达到 10%。[自研 RTL 明细](active_scope_final.json) · [目录统计](final_static/directory_comparison_20261103.json)

## 当前验证边界

复跑命令在仓库根目录执行，使用 WSL、Verilator 5.048 和 C++ 构建工具。

```bash
SIM_RESULTS_DIR="$PWD/output/rtl_refactor_20261103/results_final" bash output/rtl_refactor_20261103/run_all.sh
python3 output/rtl_refactor_20261103/run_expanded_compare.py --suite modules
python3 output/rtl_refactor_20261103/run_expanded_compare.py --suite pipeline
python3 output/rtl_refactor_20261103/run_expanded_compare.py --suite axi
python3 output/rtl_refactor_20261103/run_expanded_compare.py --suite board
```

逐拍对照中两版同时实例化，在时钟边沿后的采样点比较端口。Gamma 使用独立原曲线，M 端 FSPI 使用独立原传输实现。ISP 和 AXI 扩展套件对两版提供相同的 FIFO 仿真模型。板级仍需验证相机冷启动、视频与 DDR 传输、制动与挡位、S/M FSPI 和 M 端 IMU 快照。

最终版既有仿真原版 12/12、新版 12/12，首批逐拍套件 5/5。扩展逐拍对照共 30 个模块，19 个独立模块、7 个 CNN 或邻域流水线模块和 2 个 AXI 控制模块各做 20,000 次采样。另有 2 个板级连接模块各做 60,000 次采样。[全量回归日志](results_final) · [扩展对照日志](results_expanded)

扩展回归初次发现三个问题。字符字形表的循环加载改变了旧常量过程在 Verilator 中的触发条件，现已保留原表和触发。RGB 输出末级的旧赋值会在复位拍覆盖绿、蓝清零，现已保留覆盖行为。FSPI 端口包装恢复了已有 HMI 用例使用的 `status_tx_started` 层次探针。

## PDS 复跑

Windows PowerShell 从仓库根目录执行。流程使用两套工程原有约束与厂商 IP，在独立目录执行编译、综合、器件映射、布局布线和时序报告。

```powershell
$env:PINEAPPLE_PDS_OUTPUT = 'pds_comment_refresh'
& 'D:/Anaconda/python.exe' -B -X utf8 'output/rtl_refactor_20261103/build_pds.py'
& 'D:/Anaconda/python.exe' -B -X utf8 'output/rtl_refactor_20261103/summarize_pds_timing.py' 'output/rtl_refactor_20261103/pds_comment_refresh/timing_comparison.json'
```

对照发现显示初始化和复位差异后，停止了使用早期快照的 `pds_expanded`。结构重构快照在 `pds_final` 完成实现，并核对了四个约束输入的 SHA-256。注释整理版以该快照为基准做 RTL 归一化比对。下表仍使用结构重构阶段的 PDS 报告。

两端五个 PDS 阶段均完成，工具退出码均为 0。下表以 ns 为单位。全设计 Setup 仍有负裕量，S 端还存在慢角 Hold 违例。时序尚未收敛。

| 工程 | 慢角最差 Setup | 快角最差 Setup | 慢角最差 Hold | 快角最差 Hold | FSPI 同域慢角 Setup |
|---|---:|---:|---:|---:|---:|
| S | −6.961 | −4.192 | −0.723 | +0.099 | +0.253 |
| M | −7.085 | −4.170 | +0.033 | +0.025 | +0.203 |

与 11 月 2 日基线相比，新增负裕量为 S 端 `pclk → pclk` 慢角 Setup −0.312 ns、`ddrphy_sysclk → cmos5_pclk` 慢角 Hold −0.723 ns，以及 M 端 `ddrphy_sysclk → phy_dq_sysclk_0` 慢角 Setup −0.007 ns。S 端布线工具已尝试修复 Hold，最终报告仍记录该违例。[PDS 阶段结果](pds_final/results.json) · [快慢角逐时钟对照](pds_final/timing_comparison.json)

本次完成离线验证，实车与上板验收尚未执行。
