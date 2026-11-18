# image_recognition 孪生系统项目

## 项目简介
本仓库为图像识别孪生系统代码仓库，采用Git + GitHub协作开发。
> 协作规范：**禁止直接在 `master` 分支开发**。`master` 分支仅存放稳定、可正常运行的版本，所有新功能、调试代码均在个人开发分支完成，通过PR合并到master。

## HDMI 前视输入

笔记本的 HDMI 输出连接到支持 V4L2 单平面 YUYV 的 HDMI 采集卡，采集卡的 USB 接 M 板。板载 HDMI OUT 仍接显示器。S、M 两端软件须配套更新。M 端默认在异步推理模式下监听已验证的 MacroSilicon 采集卡（`/dev/v4l/by-id/usb-MACROSILICON_USB_Video-video-index0`）。使用其他采集卡时可用 `VISION_HDMI_DEVICE` 指定设备路径，显式设为空值可仅使用真实摄像头。`v4l2-ctl --list-devices` 可用于核对采集设备。

M 端收到两个有效采集帧后切换前视输入，采集设备报告失锁或 500 毫秒收不到有效新帧时回到相机。选型时需确认采集卡在拔掉 HDMI 后能报告失锁或停止送帧。持续输出占位图且不报告失锁的设备无法通过标准 V4L2 状态识别为断线。

M 端按采集卡实际交付的帧等比例缩放到 320×240。16:10 的有效画面为 320×200，上下各补 20 行，16:9 为 320×180，上下各补 30 行。HDMI 前视按无镜头畸变处理，车道分割、行人和交通灯检测读取前视画面，车道掩码和画面再做逆透视变换（IPM）。M 端四路拼接左上格与 S 端 `Front Center` 显示同一鸟瞰结果。M 端右下格在 HDMI 模式显示笔记本原始画面，切回摄像头时恢复原来的相机预览。切回摄像头时沿用原相机镜头和地面标定。

HDMI 默认使用有效画面内的参考梯形生成鸟瞰预览，仅供回放观察。斑马线、车道角度和行人落点的几何精度取决于原视频视角。需要定量使用时，在 M 端程序工作目录放入该视频的 `front_hdmi_ipm.conf`。沿用 `front_ipm_test.conf` 的格式，依次填写地面矩形左上、右上、右下、左下四点在**补边后 320×240 前视窗口**中的归一化 x、y，最后填写地面矩形的实际长宽比，九个数以逗号分隔。M 日志中的 `IPM=calibrated` 表示使用专用标定，`IPM=nominal` 表示使用参考梯形。两种情况下均排除程序添加的黑边。HDMI 回放的识别结果沿用显示用途。

Linux 上可用 `make -C udp_rk_rk_yolo/M/pcie test-hdmi-front` 运行模拟采集、断流和元数据回归。

## 环境准备
1. 安装 Git
2. 配置 GitHub SSH密钥（推荐，避免每次推送输入账号密码）
3. 仓库管理员已添加你为本仓库协作者，接受GitHub邮件邀请获得访问权限

## 首次拉取仓库（成员第一次操作，仅执行一次）
```bash
# 克隆仓库（SSH地址）
git clone git@github.com:cx2vh99wxg-wq/image_recognition.git
cd image_recognition
