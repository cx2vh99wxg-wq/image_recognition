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
BACKLOG="${BACKLOG:-20000}"        # net.core.netdev_max_backlog（软中断前队列）
BUDGET="${BUDGET:-600}"            # net.core.netdev_budget（每轮软中断处理包数）
BUDGET_US="${BUDGET_US:-8000}"     # net.core.netdev_budget_usecs
IFACE="${IFACE:-}"                 # 留空则自动探测
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
    # 软中断（NAPI）队列：包从网卡进内核后、尚未交给协议栈/分配到 socket 之前
    # 的暂存队列。默认仅 1000，440 包微突发 + 主循环占满 CPU 时会在此溢出，
    # 丢包位置在 socket 接收缓冲"之前"——这时怎么调 rmem_max 都没用。
    sysctl -w net.core.netdev_max_backlog="$BACKLOG"  >/dev/null
    sysctl -w net.core.netdev_budget="$BUDGET"        >/dev/null
    sysctl -w net.core.netdev_budget_usecs="$BUDGET_US" >/dev/null
else
    echo "$RMEM_MAX"      > /proc/sys/net/core/rmem_max
    echo "$WMEM_MAX"      > /proc/sys/net/core/wmem_max
    echo "$RMEM_DEF"      > /proc/sys/net/core/rmem_default
    echo "$BACKLOG"       > /proc/sys/net/core/netdev_max_backlog
    echo "$BUDGET"        > /proc/sys/net/core/netdev_budget
fi

echo "调整后: rmem_max=$(read_kern rmem_max)  wmem_max=$(read_kern wmem_max)  rmem_default=$(read_kern rmem_default)"
echo "       netdev_max_backlog=$(read_kern netdev_max_backlog)  netdev_budget=$(read_kern netdev_budget)"

# ----------------------------------------------------------------------
# 网卡 RX ring：UDP 突发丢包的"第一现场"
#
# 一帧 440 个包在 ~5ms 内打到 S 端网卡（千兆线下 630KB ≈ 5ms）。包要经过
#   网卡 RX ring → NAPI(软中断) → softnet backlog → IP/UDP → socket 接收队列
# 这条链。ring 描述符默认常常只有 256~512 个；S 端主线程正忙于 X11 渲染时
# 软中断被拖延，ring 就会溢出 —— 此时丢的包**根本进不到 socket 队列**，
# 所以 RcvbufErrors 不涨（看起来"内核没丢包"），只有网卡侧计数能看到。
# 把这些环节放大，是"收帧恒 0 且调 rmem_max 无效"的必查项。
# ----------------------------------------------------------------------
echo
echo "==== 网卡 RX ring 调优（突发丢包的第一现场） ===="
if [ -z "$IFACE" ]; then
    IFACE=$(ip -o -4 addr show 2>/dev/null | awk '$0 !~ /127\.0\.0\.1/ {print $2; exit}')
fi

if [ -n "$IFACE" ] && command -v ethtool >/dev/null 2>&1; then
    MAX_RX=$(ethtool -g "$IFACE" 2>/dev/null | awk '/^Pre-set maximums:/{s=1;next} /^Current hardware settings:/{s=0} s&&/^RX:/{print $2; exit}')
    MAX_TX=$(ethtool -g "$IFACE" 2>/dev/null | awk '/^Pre-set maximums:/{s=1;next} /^Current hardware settings:/{s=0} s&&/^TX:/{print $2; exit}')
    CUR_RX=$(ethtool -g "$IFACE" 2>/dev/null | awk '/^Current hardware settings:/{s=1;next} s&&/^RX:/{print $2; exit}')
    echo "网卡=$IFACE  当前 RX ring=$CUR_RX  驱动允许最大 RX=$MAX_RX"
    if [ -n "$MAX_RX" ] && [ -n "$CUR_RX" ] && [ "$MAX_RX" -gt "$CUR_RX" ]; then
        if ethtool -G "$IFACE" rx "$MAX_RX" tx "${MAX_TX:-$MAX_RX}" 2>/dev/null; then
            echo "已把 RX ring 提到 $MAX_RX（440 包突发不再挤爆 ring）"
        else
            echo "ethtool -G 失败（驱动不支持动态调整），忽略；不影响其余调优"
        fi
    else
        echo "RX ring 已是驱动允许的最大值，无需调整"
    fi
else
    echo "跳过（未识别到网卡，或系统里没有 ethtool）"
    echo "  可手动执行： ethtool -g end0  然后  sudo ethtool -G end0 rx 4096 tx 4096"
fi

# 参考量：一帧 440 包，按每包内核记账 ~2KB 估算约需 0.9MB，留几帧余量 → 16MB 足够
if [ "$PERSIST" = 1 ]; then
    PERSIST_FILE="/etc/sysctl.d/99-adas-udp.conf"
    mkdir -p /etc/sysctl.d
    cat > "$PERSIST_FILE" <<EOF
# ADAS 小车 UDP 链路调优（由 scripts/tune_net.sh 生成）
# 一帧图像 440 个 UDP 包以微突发到达，接收链路上任何一环太小都会静默丢包。
net.core.rmem_max = $RMEM_MAX
net.core.wmem_max = $WMEM_MAX
net.core.rmem_default = $RMEM_DEF
net.core.netdev_max_backlog = $BACKLOG
net.core.netdev_budget = $BUDGET
net.core.netdev_budget_usecs = $BUDGET_US
EOF
    echo "已写入 $PERSIST_FILE（重启后自动生效；注意网卡 RX ring 不会随 sysctl 恢复，"
    echo "  重启后需重跑本脚本，或用 ethtool -G 自行持久化）"
else
    echo "提示：当前仅本次开机有效；要重启不丢请加 --persist"
fi

echo "==== 完成：现在重启 planning_main 再看日志里的 SO_RCVBUF ===="
