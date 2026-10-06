#!/bin/bash
# start_s.sh — S 端板卡启动脚本（【人员 B】集成）
#
# 流程：配置网卡 IP → 启动决策+显示进程（内含 UDP 接收 / 行人检测 / 决策 / LCD）
#
# 用法（在 S 端 RK3568 上执行，建议 root 或 sudo）：
#   ./start_s.sh [--model 行人模型路径] [--no-lcd]
set -e

NET_IF="${NET_IF:-eth0}"
S_IP="192.168.100.20"
EXTRA=""

# 修正默认模型路径：仓库实际文件为 model/yolov5s-640-640.rknn
while [ $# -gt 0 ]; do
    case "$1" in
        --model)
            EXTRA="$EXTRA --model ${2:-model/yolov5s-640-640.rknn}"
            shift 2
            ;;
        --no-lcd)
            EXTRA="$EXTRA --no-lcd"
            shift
            ;;
        *)
            shift
            ;;
    esac
done

echo "==== S 端启动 ===="

# 1) 配置网卡 IP（与 M 端直连；IP 值必须与 driving_config.h 的 UDP_IP_S 一致）
echo "[1] 配置 $NET_IF -> $S_IP/24"
sudo ifconfig "$NET_IF" "$S_IP" netmask 255.255.255.0 up

# 2) 启动决策+显示进程
echo "[2] 启动 planning_main$EXTRA"
sudo ./planning_main $EXTRA &

echo "==== S 端启动完成 ===="
