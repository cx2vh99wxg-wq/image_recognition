# M 板双目摄像头本地显示测试

本测试在 **M 板**上把 J8 双 OV5640 的画面经 FPGA、PCIe、RK3568 直接显示到 M 板 HDMI。它不启动 S 板、不发 UDP，也不运行模型或电机控制。程序会打开两个独立的 640×480 窗口，分别显示 CAM1 与 CAM2；每个窗口把 FPGA 画布里的 320×240 子画面放大两倍。这样可分别遮挡镜头验证两颗传感器。

## 运行前核对

1. M 板的 FPGA 使用此次 `stereo_j8` 工程生成并下载的位流；仓库旧 `project/generate_bitstream` 中的位流不能代替它。双目模块 J17 直插 M 底板 J8，1 脚对 1 脚；显示器接 M 的 RK3568 HDMI，打开 M 的 X11 图形桌面终端。
2. M 板 Linux 仓库拉取了本次代码，`drivers/pango_pci_driver.ko` 与板端运行内核匹配。运行 `uname -r`、`modinfo -F vermagic drivers/pango_pci_driver.ko`、`lspci -nn`。当前仓库内的 ko 编译标识是 `6.1.99 SMP mod_unload aarch64`，PCIe IP 的设备 ID 是 `0755:0755`。
3. 用 `sudo bash scripts/stop_all.sh` 停止此前 M/S 联调留下的 M 板采集程序，防止两个程序同时操作 `/dev/pango_pci_driver`。此命令只在 M 板执行；不需要启动 S 板。

## 第一次在 M 板运行

下列命令在 **M 板 HDMI 图形桌面的普通用户终端**执行，先把示例仓库路径改为 M 板实际路径：

```bash
cd /home/linaro/image_recognition
sudo bash scripts/stop_all.sh
bash scripts/start_m_local_camera.sh --snapshot /tmp/m-ov5640
```

启动脚本会确认 `DISPLAY`、检查 `0755:0755` PCIe 端点、按需加载驱动、给当前桌面用户 `/dev/pango_pci_driver` 的本次启动读写权限、编译只依赖 X11 的本地预览程序，然后直接运行。编译要求 M 板有 GCC、make、X11 开发头文件和库；无需 RKNN 开发库。不要用 `sudo bash scripts/start_m_local_camera.sh` 启动整个图形程序，否则 root 可能无法连接桌面 X11。若驱动已经加载，脚本不会把磁盘上的新 ko 自动替换进内核。

程序首先可能显示黑色，FPGA 至少需要若干实际相机帧才会将采集画面标为可读。运行约五帧后，程序会在 `/tmp/` 写 `m-ov5640-cam1.ppm` 和 `m-ov5640-cam2.ppm`。这两张文件保存的是本地预览画面，不需要经过网络；可用于区分图像数据本来是黑色还是只有 X11 显示出了问题。按 Ctrl-C 结束。若只需固定次数，可在脚本后加 `--frames 30`；计划生成快照时帧数至少设为 5。

`stereo_j8` 的 FPGA DDR 内部画布是 640×480 RGB565，但顶层 `image_pcie_capture.v` 在 **PCIe DMA 输出**时为每行加入 120 个前导像素（共 760 像素、1520 字节）。因此本地预览继续使用 `lead_px=120`，按每行 1520 字节读取并跳过前 240 字节。这里以实际顶层输出协议为准，不能只按 DDR 内部尺寸推断主机读取长度。若画面横向偏移或错行，应保存快照及当前下载位流对应的 RTL，再核对该位流是否确实包含此前导插入逻辑。

## 怎样判断哪一层出了问题

| 看到的现象 | 优先检查 |
|---|---|
| 脚本提示 `0755:0755` 不存在 | FPGA 是否下载正确位流、RK `lspci -nn` 是否枚举到端点、PCIe 时钟和复位；驱动无法凭空创建端点 |
| ko 版本与 `uname -r` 不同或 `insmod` 报 `invalid module format` | 换成当前内核匹配的驱动；不能只修改 ko 文件名 |
| 有端点但没有 `/dev/pango_pci_driver` | `lspci -nnk` 查看绑定驱动，`sudo dmesg | tail -80` 查看 probe 错误 |
| `ioctl PANGO_IO_MAP` 或 `PANGO_IO_DMA_WR` 失败 | 保存程序报错行号、errno 与 dmesg；继续核对驱动/FPGA DMA 协议 |
| 两个窗口都黑，日志 `nonblack=0`，但 `pad-white` 很多 | PCIe 至少取到了 FPGA 加入的前导数据；优先核对 J8/J17 供电和方向、SCCB 初始化、RESET、PCLK、VSYNC、DDR 写入及 read_ready。仍需核对有效图像区的 DMA 对齐 |
| 两个窗口都黑，且 `pad-white=0`、`pad-nonzero=0` | 连前导数据也没取到；优先核对当前位流、FPGA 的 DMA 输出协议、驱动与板端 PCIe 通路，不能直接判断摄像头坏了 |
| 一个窗口有变化，另一个黑 | 分别遮挡 CAM1/CAM2，核对该眼的 SCCB、DVP 信号和排针 |
| 日志 `nonblack` 非零而显示黑 | 查看两张 PPM 快照，核对桌面 X11 像素格式和显示窗口 |
| 两个窗口出现画面但不更新 | 遮挡与移动目标，看日志中的逐眼 hash 是否变化；静态 hash 可能是停在旧帧 |

`pad-white` 和 `pad-nonzero` 统计每帧 480 行、每行前 120 个填充像素（总计 57600 个）；顶层 RTL 设定填充为 `0xffff`，所以正常情况下 `pad-white` 应明显大于零，精确数量仍受输出对齐影响。`nonblack` 表示 320×240 子图中不为 RGB565 零值的像素数；`hash` 是该子图的快速变化指纹，不是图像有效性的单独证明。镜头完全遮黑也可能使它们保持很低，应结合遮挡前后观察。程序每五帧打印一次统计。

若仍全黑，请保存 M 板以下输出，便于把问题定位到 PCIe、驱动或传感器：

```bash
uname -r
modinfo drivers/pango_pci_driver.ko | head -25
lspci -nnk
ls -l /dev/pango_pci_driver
sudo dmesg | tail -80
```

还请提供本地预览终端的完整输出、两张 PPM、PDS 本次位流的生成/下载记录。已有离线单元测试验证了两颗相机在 FPGA 帧中取的位置、RGB565 转换与放大逻辑；尚未用这台 M 板证实 FPGA 摄像头初始化或 DMA 实际有效，因此以镜头移动和遮挡后的实时变化为出图验收依据。
