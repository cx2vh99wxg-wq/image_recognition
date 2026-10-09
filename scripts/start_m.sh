#!/bin/bash
# start_m.sh — M 端板卡启动脚本（【人员 B】集成）
#
# 流程：加载 PCIe 驱动 → 配置网卡（调用 setup_network_m.sh）→ 启动感知进程 → 启动 UDP 发送器
#
# 用法（在 M 端 RK3568 上，需 root）：
#   sudo ./scripts/start_m.sh [--stub] [模型绝对路径] [驱动ko绝对路径]
#   --stub：无 FPGA / 无 A 硬件时联调用，只起 UDP 发送桩（发灰色渐变图）
# 默认模型：model/yolopv2_Nx3x480x640_rk3568.rknn（仓库根下）
# 默认驱动：drivers/pango_pci_driver.ko
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BIN_DIR="${BIN_DIR:-$REPO_ROOT/bin}"

# 参数：支持 --stub（跳过驱动/感知，只跑发送桩）；其余位置参数保持兼容
STUB=0
POS=()
for a in "$@"; do
    case "$a" in
        --stub) STUB=1 ;;
        *)      POS+=("$a") ;;
    esac
done
MODEL_PATH="${POS[0]:-$REPO_ROOT/model/yolopv2_Nx3x480x640_rk3568.rknn}"
KO_PATH="${POS[1]:-$REPO_ROOT/drivers/pango_pci_driver.ko}"

# RKNN 运行时库路径（librknnrt.so）
export LD_LIBRARY_PATH="$REPO_ROOT/lib:$LD_LIBRARY_PATH"

if [ "$(id -u)" -ne 0 ]; then
    echo "提示：驱动加载/网络配置需要 root 权限，建议用 sudo 运行"
fi

echo "==== M 端启动 ===="

# 1) 加载 PCIe 驱动（桩模式不需要；若已加载则跳过）
if [ "$STUB" = 1 ]; then
    echo "[1] --stub：跳过 PCIe 驱动加载（无 FPGA 采集，发送器直接造图）"
elif lsmod 2>/dev/null | grep -q pango_pci_driver; then
    echo "[1] pango_pci_driver 已加载"
else
    echo "[1] 加载驱动: $KO_PATH"
    insmod "$KO_PATH"
fi

# 2) 配置网卡（独立脚本：自动探测 end0/end1 + ip 命令 + NetworkManager 规避）
#    用 bash 显式调起：脚本若丢了可执行位（Windows 克隆/提交时常见），
#    直接 exec 会报 "Permission denied"。
echo "[2] 配置网络"
bash "$SCRIPT_DIR/setup_network_m.sh" || echo "警告：网络配置未成功（请检查网线是否插在 end0）"

# 3) 内核 UDP 缓冲调优（发送端突发写入；接收端 S 板更要调，见 start_s.sh）
#    注意用 -f 而非 -x：可执行位缺失时 -x 会让调优被"静默跳过"
echo "[3] 内核 UDP 缓冲调优"
if [ -f "$SCRIPT_DIR/tune_net.sh" ]; then
    bash "$SCRIPT_DIR/tune_net.sh" || echo "警告：tune_net.sh 执行失败"
fi

# 4) 启动感知进程（A 交付）；--stub 时不需要（无 FPGA 采集，跳过 A 的模型）
if [ "$STUB" = 1 ]; then
    echo "[4] --stub：跳过 perception_main（不依赖 A 的模型/FPGA）"
else
    echo "[4] 启动 perception_main"
    nohup "$BIN_DIR/perception_main" --model "$MODEL_PATH" \
        > "$REPO_ROOT/perception.log" 2>&1 &
fi

# 5) 启动 UDP 发送器（B 交付）；--stub 时发灰色渐变桩图，无需摄像头
SEND_ARGS=()
[ "$STUB" = 1 ] && SEND_ARGS+=(--stub)
echo "[5] 启动 udp_m_send_main ${SEND_ARGS[*]}"
nohup "$BIN_DIR/udp_m_send_main" "${SEND_ARGS[@]}" \
    > "$REPO_ROOT/udp_send.log" 2>&1 &

sleep 1
echo "==== M 端启动完成 ===="
echo "日志: perception.log / udp_send.log"
echo "全部停止请用: ./scripts/stop_all.sh"
