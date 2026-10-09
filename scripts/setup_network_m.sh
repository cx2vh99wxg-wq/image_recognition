#!/bin/bash
# setup_network_m.sh — M 端（发送端）网络配置（【人员 B】集成）
#
# 网线直连 S 端，无路由器。自动检测可用的有线网口（优先 end0 → end1 → eth0），
# 配静态 IP 192.168.100.10/24，与 driving_config.h 的 UDP_IP_M 保持一致。
#
# 用法（M 端 RK3568 上，需 root）：
#   sudo ./setup_network_m.sh
# 可选环境变量：
#   NET_IF   直连网口（留空则自动检测）
#   M_IP     本端直连 IP（默认 192.168.100.10）
#   S_IP     对端直连 IP（默认 192.168.100.20，仅用于连通性测试）
set -e

M_IP="${M_IP:-192.168.100.10}"
S_IP="${S_IP:-192.168.100.20}"

if [ "$(id -u)" -ne 0 ]; then
    echo "错误：请用 root 权限运行（sudo $0）"
    exit 1
fi

# 自动检测可用的有线网口：优先 end0，其次 end1，最后 eth0
auto_detect_netif() {
    local iface
    for iface in "$@"; do
        if ip link show "$iface" >/dev/null 2>&1; then
            echo "$iface"
            return 0
        fi
    done
    return 1
}

if [ -n "${NET_IF:-}" ]; then
    NET_IF="$NET_IF"
elif ! NET_IF="$(auto_detect_netif end0 end1 eth0)"; then
    echo "错误：未找到可用的有线网口（end0/end1/eth0 均不存在）。可用接口："
    ip link show | grep -E '^[0-9]+:' | awk '{print $2}' | sed 's/://g'
    exit 1
fi

echo "==== M 端网络配置 ===="
echo "直连网口: $NET_IF -> $M_IP/24"
echo "目标 S 端: $S_IP:8888"

# 1) 让 NetworkManager 不托管直连网口，避免抢 IP / 自动配置冲突
mkdir -p /etc/NetworkManager/conf.d
cat > /etc/NetworkManager/conf.d/99-unmanaged-devices.conf <<EOF
[keyfile]
unmanaged-devices=interface-name:${NET_IF}
EOF
systemctl reload NetworkManager 2>/dev/null || true

# 2) 配置直连网口
ip link set "$NET_IF" down 2>/dev/null || true
ip addr flush dev "$NET_IF" 2>/dev/null || true
ip link set "$NET_IF" up
ip addr add "${M_IP}/24" dev "$NET_IF"

# 3) 验证
echo "接口状态:"
ip addr show "$NET_IF" | grep -E 'inet |state '
echo ""
if ping -c 3 -W 2 "$S_IP" >/dev/null 2>&1; then
    echo "✓ 与 S 端 ($S_IP) 连通正常"
else
    echo "⚠ 暂无法连通 S 端 ($S_IP)，请确认 S 端已运行 setup_network_s.sh"
fi
echo "==== M 端网络配置完成 ===="
