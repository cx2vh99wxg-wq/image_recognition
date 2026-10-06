#!/bin/bash
# setup_network_s.sh — S 端（接收端）网络配置（【人员 B】集成）
#
# 网线直连 M 端。直连网口 end1 配静态 IP 192.168.100.20/24，
# 与 driving_config.h 的 UDP_IP_S 保持一致；end0 配管理 IP（开发期 SSH 用，可选）。
#
# 用法（S 端 RK3568 上，需 root）：
#   sudo ./setup_network_s.sh
# 可选环境变量：
#   NET_IF   直连网口（默认 end1）
#   S_IP     本端直连 IP（默认 192.168.100.20）
#   M_IP     对端直连 IP（默认 192.168.100.10，仅用于连通性测试）
set -e

NET_IF="${NET_IF:-end1}"
S_IP="${S_IP:-192.168.100.20}"
M_IP="${M_IP:-192.168.100.10}"
MGMT_IP="${MGMT_IP:-192.168.137.222}"   # end0 管理 IP（留空则跳过）

if [ "$(id -u)" -ne 0 ]; then
    echo "错误：请用 root 权限运行（sudo $0）"
    exit 1
fi

if ! ip link show "$NET_IF" >/dev/null 2>&1; then
    echo "错误：网络接口 $NET_IF 不存在。可用接口："
    ip link show | grep -E '^[0-9]+:' | awk '{print $2}' | sed 's/://g'
    exit 1
fi

echo "==== S 端网络配置 ===="
echo "直连网口: $NET_IF -> $S_IP/24"
echo "源 M 端: $M_IP:8888"

# 1) NetworkManager 不托管直连网口
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
ip addr add "${S_IP}/24" dev "$NET_IF"

# 3) 配置管理网口（可选）
if [ -n "$MGMT_IP" ] && ip link show end0 >/dev/null 2>&1; then
    ip link set end0 up 2>/dev/null || true
    ip addr flush dev end0 2>/dev/null || true
    ip addr add "${MGMT_IP}/24" dev end0 2>/dev/null || true
    echo "管理网口 end0 -> ${MGMT_IP}/24"
fi

# 4) 验证
echo "接口状态:"
ip addr show "$NET_IF" | grep -E 'inet |state '
echo ""
if ping -c 3 -W 2 "$M_IP" >/dev/null 2>&1; then
    echo "✓ 与 M 端 ($M_IP) 连通正常"
else
    echo "⚠ 暂无法连通 M 端 ($M_IP)，请确认 M 端已运行 setup_network_m.sh"
fi
echo "==== S 端网络配置完成 ===="
