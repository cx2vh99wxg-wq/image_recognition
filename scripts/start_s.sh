#!/bin/bash
# start_s.sh — S 端板卡启动脚本（【人员 B】集成）
#
# 流程：配置网卡（调用 setup_network_s.sh）→ 启动决策+显示进程
#       （planning_main 内含 UDP 接收 / 行人检测 / 决策 / LCD）
#
# 用法（在 S 端 RK3568 上，需 root）：
#   sudo ./scripts/start_s.sh [--model 行人模型绝对路径] [--no-lcd]
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BIN_DIR="${BIN_DIR:-$REPO_ROOT/bin}"

# 行人检测模型（仓库实际文件为 model/yolov5s-640-640.rknn）
MODEL="$REPO_ROOT/model/yolov5s-640-640.rknn"
NO_LCD=0

while [ $# -gt 0 ]; do
    case "$1" in
        --model)  MODEL="$2"; shift 2 ;;
        --no-lcd) NO_LCD=1; shift ;;
        *)        shift ;;
    esac
done

# RKNN 运行时库路径（librknnrt.so）
export LD_LIBRARY_PATH="$REPO_ROOT/lib:$LD_LIBRARY_PATH"

if [ "$(id -u)" -ne 0 ]; then
    echo "提示：网络配置需要 root 权限，建议用 sudo 运行"
fi

echo "==== S 端启动 ===="

# 1) 配置网卡（独立脚本：end1 + ip 命令 + NetworkManager 规避）
echo "[1] 配置网络"
"$SCRIPT_DIR/setup_network_s.sh" || true

# 2) 启动决策+显示进程
ARGS=(--model "$MODEL")
[ "$NO_LCD" = 1 ] && ARGS+=(--no-lcd)
echo "[2] 启动 planning_main ${ARGS[*]}"
nohup "$BIN_DIR/planning_main" "${ARGS[@]}" \
    > "$REPO_ROOT/planning.log" 2>&1 &

sleep 1
echo "==== S 端启动完成 ===="
echo "日志: planning.log"
echo "全部停止请用: ./scripts/stop_all.sh"
