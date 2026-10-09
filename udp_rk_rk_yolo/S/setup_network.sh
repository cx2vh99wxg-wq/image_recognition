#!/bin/bash

#=======================================================================
# S端 (接收端) 网络配置脚本
# 功能: 配置end1网口用于视频数据接收
# 系统: Debian 10 / RK3568
# 作者: 辉哥大盗
# 日期: 2025年10月13日
#=======================================================================

# 配置参数
INTERFACE="end1"
S_IP="192.168.100.20"      # S端IP地址
NETMASK="255.255.255.0"    # 子网掩码  
NETWORK="192.168.100.0/24" # 网络段
M_IP="192.168.100.10"      # M端IP地址 (源)

# 颜色输出定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}    S端网络配置脚本 (接收端)${NC}"
echo -e "${BLUE}======================================${NC}"
echo -e "接口: ${YELLOW}${INTERFACE}${NC}"
echo -e "本机IP: ${YELLOW}${S_IP}${NC}"
echo -e "源IP: ${YELLOW}${M_IP}${NC}"
echo -e "网络段: ${YELLOW}${NETWORK}${NC}"
echo ""

# 检查是否为root用户
if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}错误: 请使用root权限运行此脚本${NC}"
    echo "使用方法: sudo $0"
    exit 1
fi

# 检查网络接口是否存在
if ! ip link show ${INTERFACE} > /dev/null 2>&1; then
    echo -e "${RED}错误: 网络接口 ${INTERFACE} 不存在${NC}"
    echo "可用接口:"
    ip link show | grep -E "^[0-9]+:" | awk '{print $2}' | sed 's/://g'
    exit 1
fi

# 配置end0以太网口固定IP (确保管理网络始终可用)
echo -e "${YELLOW}[0/8] 配置end0以太网口固定IP...${NC}"
echo "正在设置end0网口IP为192.168.137.222..."

# 确保end0接口存在且启用
if ip link show end0 > /dev/null 2>&1; then
    # 启用end0接口
    ip link set end0 up
    # 清除现有配置
    ip addr flush dev end0 2>/dev/null
    # 配置IP地址
    ip addr add 192.168.137.222/24 dev end0
    # 设置默认路由（确保网络栈正常初始化）
    ip route add default via 192.168.137.1 dev end0 2>/dev/null || true
    echo -e "${GREEN}✓ end0 IP地址配置成功 (192.168.137.222/24)${NC}"
else
    echo -e "${YELLOW}⚠ end0接口不存在，跳过配置${NC}"
fi

# 配置NetworkManager避免干扰
echo "配置NetworkManager避免接口冲突..."
# 创建配置文件让NetworkManager不管理end1
mkdir -p /etc/NetworkManager/conf.d/
cat > /etc/NetworkManager/conf.d/99-unmanaged-devices.conf << EOF
[keyfile]
unmanaged-devices=interface-name:${INTERFACE}
EOF

# 重新加载NetworkManager配置
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

# 等待接口完全启动
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

# 显示接口状态
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

# 保存配置信息到文件
echo "# S端网络配置信息 - $(date)" > /tmp/s_network_config.txt
echo "END0_IP=192.168.137.222" >> /tmp/s_network_config.txt
echo "INTERFACE=${INTERFACE}" >> /tmp/s_network_config.txt
echo "S_IP=${S_IP}" >> /tmp/s_network_config.txt
echo "M_IP=${M_IP}" >> /tmp/s_network_config.txt
echo "STATUS=CONFIGURED" >> /tmp/s_network_config.txt

echo -e "${GREEN}配置信息已保存到: /tmp/s_network_config.txt${NC}"