#!/bin/bash

# S 端视频接收网络初始化。脚本会更改管理网口、视频网口及 NetworkManager 配置。
# end1 固定为 192.168.100.20/24，接收 M 端 192.168.100.10 的视频。
# 适用于项目使用的 Debian 10 / RK3568 主机。
# 维护：科研轮转。
# 原始配置日期：2025 年 10 月 13 日。

# 这些地址用于本端配置、状态展示及向 M 端发送连通性探测。
INTERFACE="end1"
S_IP="192.168.100.20"      # 接收端在 end1 上使用的地址。
NETMASK="255.255.255.0"    # 地址规划所用子网掩码。
NETWORK="192.168.100.0/24" # 两端视频链路共用的网络段。
M_IP="192.168.100.10"      # 视频发送端地址，用于连通性检查。

# 终端颜色常量只作用于状态提示。
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # 重置终端颜色。

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}    S端网络配置脚本 (接收端)${NC}"
echo -e "${BLUE}======================================${NC}"
echo -e "接口: ${YELLOW}${INTERFACE}${NC}"
echo -e "本机IP: ${YELLOW}${S_IP}${NC}"
echo -e "源IP: ${YELLOW}${M_IP}${NC}"
echo -e "网络段: ${YELLOW}${NETWORK}${NC}"
echo ""

# 在更改网口及系统配置文件之前核对管理员权限。
if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}错误: 请使用root权限运行此脚本${NC}"
    echo "使用方法: sudo $0"
    exit 1
fi

# end1 不存在时输出现有接口并停止配置。
if ! ip link show ${INTERFACE} > /dev/null 2>&1; then
    echo -e "${RED}错误: 网络接口 ${INTERFACE} 不存在${NC}"
    echo "可用接口:"
    ip link show | grep -E "^[0-9]+:" | awk '{print $2}' | sed 's/://g'
    exit 1
fi

# 先设置 end0 的管理地址 192.168.137.222/24。
echo -e "${YELLOW}[0/8] 配置end0以太网口固定IP...${NC}"
echo "正在设置end0网口IP为192.168.137.222..."

# end0 可用时才执行启用、清地址、设置静态地址和默认路由。
if ip link show end0 > /dev/null 2>&1; then
    # 启用管理网口。
    ip link set end0 up
    # 清除 end0 的旧地址，以免与静态地址冲突。
    ip addr flush dev end0 2>/dev/null
    # 写入 end0 的静态管理地址。
    ip addr add 192.168.137.222/24 dev end0
    # 尝试设置经由 192.168.137.1 的默认路由，失败不阻断后续配置。
    ip route add default via 192.168.137.1 dev end0 2>/dev/null || true
    echo -e "${GREEN}✓ end0 IP地址配置成功 (192.168.137.222/24)${NC}"
else
    echo -e "${YELLOW}⚠ end0接口不存在，跳过配置${NC}"
fi

# 让 NetworkManager 不再管理视频网口 end1。
echo "配置NetworkManager避免接口冲突..."
# 将 end1 排除规则写入 NetworkManager 的配置目录。
mkdir -p /etc/NetworkManager/conf.d/
cat > /etc/NetworkManager/conf.d/99-unmanaged-devices.conf << EOF
[keyfile]
unmanaged-devices=interface-name:${INTERFACE}
EOF

# 若 NetworkManager 正在运行，则重新加载刚写入的规则。
if systemctl is-active --quiet NetworkManager; then
    systemctl reload NetworkManager 2>/dev/null
    echo -e "${GREEN}✓ NetworkManager配置已更新${NC}"
fi

echo ""
echo -e "${YELLOW}[1/8] 配置end0接口IP地址完成...${NC}"

echo -e "${YELLOW}[2/8] 关闭网络接口...${NC}"
ip link set ${INTERFACE} down
sleep 1

echo -e "${YELLOW}[3/8] 清除现有IP配置...${NC}"
ip addr flush dev ${INTERFACE}

echo -e "${YELLOW}[4/8] 启用网络接口...${NC}"
ip link set ${INTERFACE} up
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ 接口启用成功${NC}"
else
    echo -e "${RED}✗ 接口启用失败${NC}"
    exit 1
fi

# 等待视频网口链路完成启动，再设置本端地址。
sleep 2

echo -e "${YELLOW}[5/8] 配置IP地址...${NC}"
ip addr add ${S_IP}/24 dev ${INTERFACE}
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ IP地址配置成功: ${S_IP}${NC}"
else
    echo -e "${RED}✗ IP地址配置失败${NC}"
    exit 1
fi

echo -e "${YELLOW}[6/8] 验证网络配置...${NC}"
echo ""

# 展示 end1 的链路、地址和路由，便于核对。
echo -e "${BLUE}接口状态:${NC}"
ip link show ${INTERFACE} | head -2

echo ""
echo -e "${BLUE}IP配置:${NC}"
ip addr show ${INTERFACE} | grep inet

echo ""
echo -e "${BLUE}路由表:${NC}"
ip route | grep ${INTERFACE}

echo ""
echo -e "${YELLOW}测试网络连通性...${NC}"
ping -c 3 -W 2 ${M_IP} > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ 与M端 (${M_IP}) 连通正常${NC}"
else
    echo -e "${YELLOW}⚠ 暂时无法连接到M端 (${M_IP})${NC}"
    echo "  这是正常的，请确保M端也已配置完成"
fi

echo ""
echo -e "${YELLOW}[7/8] 显示end0网口状态...${NC}"
echo ""
echo -e "${BLUE}end0网口状态:${NC}"
ip addr show end0 2>/dev/null | grep inet || echo "end0接口未找到或未配置IP"

echo ""
echo -e "${YELLOW}[8/8] 显示所有网络配置摘要...${NC}"
echo ""
echo -e "${GREEN}======================================${NC}"
echo -e "${GREEN}    S端网络配置完成!${NC}"
echo -e "${GREEN}======================================${NC}"
echo -e "配置信息:"
echo -e "  • end0接口: 192.168.137.222/24 (固定IP)"
echo -e "  • ${INTERFACE}接口: ${S_IP}/24 (视频接收)"
echo -e "  • 源IP: ${M_IP}"
echo -e "  • end1状态: $(ip link show ${INTERFACE} | grep -o 'state [A-Z]*' | cut -d' ' -f2)"
echo ""
echo -e "${BLUE}提示:${NC}"
echo "  • end0用于管理网络 (192.168.137.222)"
echo "  • ${INTERFACE}用于视频数据接收 (${M_IP} -> ${S_IP}:8888)"
echo "  • 如需修改IP，请编辑此脚本中的配置参数"
echo ""

# 把本次设置值保存为临时诊断记录。
echo "# S端网络配置信息 - $(date)" > /tmp/s_network_config.txt
echo "END0_IP=192.168.137.222" >> /tmp/s_network_config.txt
echo "INTERFACE=${INTERFACE}" >> /tmp/s_network_config.txt
echo "S_IP=${S_IP}" >> /tmp/s_network_config.txt
echo "M_IP=${M_IP}" >> /tmp/s_network_config.txt
echo "STATUS=CONFIGURED" >> /tmp/s_network_config.txt

echo -e "${GREEN}配置信息已保存到: /tmp/s_network_config.txt${NC}"