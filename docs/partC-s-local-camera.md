# S 板双目 OV5640 本地显示测试（六宫格下排）

本测试在 **S 板**上把 J8 双目 OV5640 的画面经 **S 板自带 FPGA → 板内 PCIe → RK3568** 直接显示到 S 板 HDMI 六宫格窗口的**下排**：

| 位置 | 内容 |
| --- | --- |
| 上排（M 板数据源） | 本阶段不接，保持黑色（正常） |
| 下排左一格 | **CAM1 实拍** |
| 下排中间格 | **固定黑色占位**（该象限无摄像头） |
| 下排右一格 | **CAM2 实拍** |

不需要 M 板、不需要网线/UDP、不运行模型推理。它与 M 板测试（`docs/partC-m-local-camera.md`）互为镜像：那份是"相机在 M、显示在 S"，这份是"相机在 S、显示也在 S"。

显示取块关系（已与 stereo_j8 RTL 逐项核对）：FPGA 输出的 640×480 画布 =
[左上=CAM1 | 右上=常量黑 // 左下=CAM2 | 右下=不上屏]；渲染端只取
[左上, 右上, 左下] 三个 320×240 子块排成下排 = [CAM1, 黑, CAM2]。

## 1 前置条件

1. **S 板 FPGA 已下载 stereo_j8 位流**（与 M 板测试同一份 `.sbit`）：
   - 文件（本机）：`pds_work/stereo_j8_batch/generate_bitstream/image_pcie_capture.sbit`
   - SHA-256：`b40ce54f97422297fe118a3e6c99de5b2348dbec2586f6ab03afcf6841cf348a`
     （Bank L4=1.8V 修正版；此前 3.3V 版本已改名隔离，**严禁下载**）
   - 下载步骤同 `docs/partC-stereo-j8-guide.md` 第 5 节：JTAG（J12，2×5 针）→
     PDS Configuration → Scan Device 应见 `PG2L100H` → 选上述 `.sbit` → Program（SRAM）。
   - ⚠ `.sbit` 经 JTAG 配置是**易失**的：S 板断电/重启后需重新下载（除非已按指南
     §5.3 固化 Flash）。`pds_work/` 不入 git；换电脑需按指南重建位流或单独拷贝并核对 SHA。
2. S 板 `lspci -nn` 应能看到 FPGA 端点 `[0755:0755]`；看不到时先
   `echo 1 | sudo tee /sys/bus/pci/rescan`，再查 `sudo dmesg | tail -40`。
3. **双目模块插在 S 板 J8**：断电装接，J17.1 对准 J8.1（以 PCB 1 脚标识/方焊盘为准）。
   28 根信号映射与电平注意事项（Bank L4=1.8V + RS0108 电平转换；J8.2 为 5V，不要改接）
   见指南第 2、3 节。模块自带 24 MHz XCLK、PWDN 已下拉，**无需 FPGA 提供时钟**。
4. 仓库最新代码已同步到 S 板；`drivers/pango_pci_driver.ko` 与运行内核匹配
   （`modinfo -F vermagic drivers/pango_pci_driver.ko` 对比 `uname -r`）。
5. 显示器接 S 板 HDMI、进入 X11 桌面，从**桌面终端**运行（`start_s.sh` 会自动把图形
   进程降权到桌面用户，避免 sudo 拿不到 X 授权而黑屏）。

## 2 运行

在 S 板、仓库根目录（桌面终端）：

```bash
cd ~/image_recognition
sudo bash scripts/stop_all.sh
make -C planning bin
sudo bash scripts/start_s.sh --local-pcie --no-person --no-overlay
```

- `--local-pcie`：`planning_main` **直接读取本板 PCIe**（脚本自动 insmod 驱动、
  给桌面用户授权设备节点）。不会静默回退到模拟图——初始化失败即报错退出。
- `--no-person --no-overlay`：本次只看画面（不载模型推理、不叠箭头/红框）；
  正式感知时去掉这两个参数。
- 左右眼反了（遮挡镜头确认后不合预期）：启动命令追加 `--swap-eyes` —— 纯显示侧
  对调下排左/右两格，不改位流与 FDC。
- 想先逐眼放大确认两颗摄像头成像（带 PPM 快照）：见第 5 节。

## 3 预期结果

- 出现 960×480 窗口，标题含 `6CH: M top / S bottom`。
- **下排**：左一格 CAM1 实时、中间格纯黑、右一格 CAM2 实时；**上排黑色属正常**。
- 遮挡/移动任意一枚镜头前的物体，对应格子应实时变化。
- 下载后**前 10~30 秒可能全黑**：FPGA 先复位摄像头并写配置寄存器（约 251 个），
  再等若干次 VSYNC 开放采集、读侧统计满 3 帧才放行数据。不要立即判失败。
- `planning.log` 每 2 秒一行统计，其中 `--local-pcie` 专属行：

```text
本地采集=N 帧 前导[白=xxxxx 非零=xxxxx]/57600（白多=PCIe 读数通路正常）
```

  `前导` = FPGA 每行加的 120 个 `0xFFFF` 填充像素（480 行共 57600 个）。
  **白≈57600 说明 FPGA→DDR→PCIe→驱动整条读数通路正常**；此时若画面仍黑，
  问题在摄像头侧而不是链路层。

## 4 判读（分层定位）

| 现象 | 先查 |
| --- | --- |
| 报"驱动已加载但无设备节点"或启动即退 `S 本地 PCIe 初始化失败` | 位流未下载 / PCIe 未枚举：`lspci -nn`、`echo 1 | sudo tee /sys/bus/pci/rescan`、`sudo dmesg | tail -40` |
| 窗口没有出现 | X11：必须从桌面终端运行；看 `planning.log` 的 `XOpenDisplay` 提示与 `[3] 图形身份` 行 |
| 下排全黑 + 前导白≈0 | PCIe/DMA 通路：核对下载的是本页 SHA 的位流、驱动与内核是否匹配 |
| 下排全黑 + 前导白正常 | 摄像头侧：J8 装接方向、模块供电、RS0108 两侧、SCCB（NACK 会重试）、复位、PCLK 质量 |
| 只有一格有图 | 遮挡确认是哪只眼；查该路 data0~7 / SDA / SCL / RESET 映射与镜头排线 |
| 中间格不是纯黑 | 用的不是 stereo_j8 位流（右上格由常量 0 填充）；核对 SHA |
| 画面卡住不更新 | 摄像头失帧时旧画面不会自动变黑；移动物体确认，查该路 PCLK/VSYNC |
| 上排也亮了 | 说明另有 M 板在发 UDP（不影响本测试）；纯 S 测试可忽略上排 |

## 5 可选：逐眼放大与快照预检

`stereo_local_preview`（`make -C planning local-preview`）读**同一个** PCIe 设备、
开两个 640×480 放大窗口，支持 `--snapshot /tmp/s-ov5640` 存 PPM 快照。它与六宫格
**不能同时运行**（同一设备互斥）：

```bash
sudo bash scripts/stop_all.sh
cd ~/image_recognition && make -C planning local-preview
./planning/stereo_local_preview --snapshot /tmp/s-ov5640 --frames 300
```

以**桌面用户**运行即可（设备节点已在 `--local-pcie` 流程授权）。
`pad-white / nonblack / hash` 的判读方法见 `docs/partC-m-local-camera.md` 的
"怎样判断哪一层出了问题"。

## 6 参考

- 位流工程、28 根引脚表、JTAG 下载与故障定位：`docs/partC-stereo-j8-guide.md`
- M 板镜像测试（相机在 M、显示在 S）：`docs/partC-m-local-camera.md`
- 实现：`planning/csrc/main_planning.c`（`--local-pcie` / `--swap-eyes` / 采集诊断）、
  `planning/csrc/frame_compose.c`（下排取块）、`planning/csrc/stereo_local_preview.c`
