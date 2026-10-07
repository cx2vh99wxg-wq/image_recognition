#!/bin/bash
# start_m.sh — M 端板卡启动脚本（【人员 B】集成）
#
# 流程：加载 PCIe 驱动 → 配置网卡（调用 setup_network_m.sh）→ 启动感知进程 → 启动 UDP 发送器
#
# 用法（在 M 端 RK3568 上，需 root）：
#   sudo ./scripts/start_m.sh [模型绝对路径] [驱动ko绝对路径]
# 默认模型：model/yolopv2_Nx3x480x640_rk3568.rknn（仓库根下）
# 默认驱动：drivers/pango_pci_driver.ko
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BIN_DIR="${BIN_DIR:-$REPO_ROOT/bin}"

MODEL_PATH="${1:-$REPO_ROOT/model/yolopv2_Nx3x480x640_rk3568.rknn}"
KO_PATH="${2:-$REPO_ROOT/drivers/pango_pci_driver.ko}"

# RKNN 运行时库路径（librknnrt.so）
export LD_LIBRARY_PATH="$REPO_ROOT/lib:$LD_LIBRARY_PATH"

if [ "$(id -u)" -ne 0 ]; then
    echo "提示：驱动加载/网络配置需要 root 权限，建议用 sudo 运行"
fi

echo "==== M 端启动 ===="

# 1) 加载 PCIe 驱动（若已加载则跳过）
if lsmod 2>/dev/null | grep -q pango_pci_driver; then
    echo "[1] pango_pci_driver 已加载"
else
    echo "[1] 加载驱动: $KO_PATH"
    insmod "$KO_PATH"
fi

# 2) 配置网卡（独立脚本：自动探测 end0/end1 + ip 命令 + NetworkManager 规避）
echo "[2] 配置网络"
"$SCRIPT_DIR/setup_network_m.sh" || true

# 3) 内核 UDP 缓冲调优（发送端突发写入；接收端 S 板更要调，见 start_s.sh）
echo "[3] 内核 UDP 缓冲调优"
if [ -x "$SCRIPT_DIR/tune_net.sh" ]; then
    "$SCRIPT_DIR/tune_net.sh" || echo "警告：tune_net.sh 执行失败"
fi

# 4) 启动感知进程（A 交付；--stub 可在无硬件时联调）
echo "[4] 启动 perception_main (stub 模式请加 --stub)"
nohup "$BIN_DIR/perception_main" --model "$MODEL_PATH" \
    > "$REPO_ROOT/perception.log" 2>&1 &

# 5) 启动 UDP 发送器（B 交付）
echo "[5] 启动 udp_m_send_main"
nohup "$BIN_DIR/udp_m_send_main" \
    > "$REPO_ROOT/udp_send.log" 2>&1 &

sleep 1
echo "==== M 端启动完成 ===="
echo "日志: perception.log / udp_send.log"
echo "全部停止请用: ./scripts/stop_all.sh"
