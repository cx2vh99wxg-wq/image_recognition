#!/bin/bash
# start_m.sh — M 端板卡启动脚本（【人员 B】集成）
#
# 流程：加载 PCIe 驱动 → 配置网卡 IP（与 S 端网线直连）→ 启动感知进程 → 启动 UDP 发送器
#
# 用法（在 M 端 RK3568 上执行，建议用 root 或 sudo）：
#   ./start_m.sh [模型路径] [驱动ko路径]
# 默认模型路径：model/yolopv2_Nx3x480x640_rk3568.rknn（相对运行目录）
# 默认驱动路径：当前目录 pango_pci_driver.ko
set -e

MODEL_PATH="${1:-model/yolopv2_Nx3x480x640_rk3568.rknn}"
KO_PATH="${2:-pango_pci_driver.ko}"
NET_IF="${NET_IF:-eth0}"
M_IP="192.168.100.10"

echo "==== M 端启动 ===="

# 1) 加载 PCIe 驱动（若已加载则跳过）
if lsmod | grep -q pango_pci_driver; then
    echo "[1] pango_pci_driver 已加载"
else
    echo "[1] 加载驱动: $KO_PATH"
    sudo insmod "$KO_PATH"
fi

# 2) 配置网卡 IP（与 S 端直连，同一子网；IP 值必须与 driving_config.h 的 UDP_IP_M 一致）
echo "[2] 配置 $NET_IF -> $M_IP/24"
sudo ifconfig "$NET_IF" "$M_IP" netmask 255.255.255.0 up
# 可选：禁用网络管理器的自动管理，避免抢 IP
# sudo nmcli dev set "$NET_IF" managed no

# 3) 启动感知进程（A 交付；--stub 可在无硬件时联调）
echo "[3] 启动 perception_main (stub 模式请加 --stub)"
sudo ./perception_main --model "$MODEL_PATH" &

# 4) 启动 UDP 发送器（B 交付）
echo "[4] 启动 udp_m_send_main"
sudo ./udp_m_send_main &

echo "==== M 端启动完成（Ctrl+C 结束前台任务，全部停止请用 stop_all.sh）===="
