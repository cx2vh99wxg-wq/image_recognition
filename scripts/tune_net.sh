#!/bin/bash
# tune_net.sh — 放大内核 UDP socket 缓冲上限（【人员 B】UDP 链路调优，必需）
#
# 为什么需要它（"收帧恒 0"的根因之一）：
#   一帧图像 = 614400B = 1 帧头 + 439 个数据块 = 440 个 UDP 包，M 端在一个突发里
#   连续发出（桩模式 5 帧/s ≈ 2200 包/s）。S 端 socket 请求了 UDP_RECV_BUF_BYTES，
#   但 Linux 内核会先把请求值翻倍、再按 net.core.rmem_max **静默截断**；
#   rmem_max 出厂默认只有 212992 字节（≈208KB，约 100 个包），连一帧都装不下。
#   → S 端稍微晚一点去取包，多出来的包就被内核丢掉（RcvbufErrors），
#     439 块永远凑不齐，于是"收帧"一直是 0，而 LCD 帧照常增长。
#
# 用法（S 端必须、M 端建议，需 root）：
#   sudo ./scripts/tune_net.sh            # 立即生效（重启后失效）
#   sudo ./scripts/tune_net.sh --persist  # 立即生效 + 写入 /etc/sysctl.d 永久生效
#
# 验证：
#   sysctl net.core.rmem_max
#   # 然后启动 planning_main，日志里 SO_RCVBUF 应显示 >= 16MB
set -e

RMEM_MAX="${RMEM_MAX:-16777216}"   # 16MB
WMEM_MAX="${WMEM_MAX:-16777216}"   # 16MB（发送端突发写入用）
RMEM_DEF="${RMEM_DEF:-1048576}"    # 1MB
PERSIST=0
[ "$1" = "--persist" ] && PERSIST=1

if [ "$(id -u)" -ne 0 ]; then
    echo "错误：需要 root 权限（sudo $0）"
    exit 1
fi

read_kern() { cat "/proc/sys/net/core/$1" 2>/dev/null || echo "?"; }

echo "==== 内核 UDP 缓冲调优 ===="
echo "调整前: rmem_max=$(read_kern rmem_max)  wmem_max=$(read_kern wmem_max)  rmem_default=$(read_kern rmem_default)"

# 优先用 sysctl；没有 sysctl（精简 rootfs）就直接写 /proc/sys
if command -v sysctl >/dev/null 2>&1; then
    sysctl -w net.core.rmem_max="$RMEM_MAX"      >/dev/null
    sysctl -w net.core.wmem_max="$WMEM_MAX"      >/dev/null
    sysctl -w net.core.rmem_default="$RMEM_DEF"  >/dev/null
else
    echo "$RMEM_MAX"      > /proc/sys/net/core/rmem_max
    echo "$WMEM_MAX"      > /proc/sys/net/core/wmem_max
    echo "$RMEM_DEF"      > /proc/sys/net/core/rmem_default
fi

echo "调整后: rmem_max=$(read_kern rmem_max)  wmem_max=$(read_kern wmem_max)  rmem_default=$(read_kern rmem_default)"

# 参考量：一帧 440 包，按每包内核记账 ~2KB 估算约需 0.9MB，留几帧余量 → 16MB 足够
if [ "$PERSIST" = 1 ]; then
    PERSIST_FILE="/etc/sysctl.d/99-adas-udp.conf"
    mkdir -p /etc/sysctl.d
    cat > "$PERSIST_FILE" <<EOF
# ADAS 小车 UDP 链路调优（由 scripts/tune_net.sh 生成）
# 一帧图像 440 个 UDP 包突发到达，接收缓冲必须能吞下整帧，否则内核静默丢包。
net.core.rmem_max = $RMEM_MAX
net.core.wmem_max = $WMEM_MAX
net.core.rmem_default = $RMEM_DEF
EOF
    echo "已写入 $PERSIST_FILE（重启后自动生效）"
else
    echo "提示：当前仅本次开机有效；要重启不丢请加 --persist"
fi

echo "==== 完成：现在重启 planning_main 再看日志里的 SO_RCVBUF ===="
