#!/bin/bash

# M 端视频发送网络初始化。脚本会修改网口地址、NetworkManager 状态和路由。
# 目标网口 end1 使用 192.168.100.10/24，发送到 S 端 192.168.100.20。
# 适用于项目使用的 Debian 10 / RK3568 主机。
# 作者：科研轮转。
# 原始配置日期：2026 年 10 月 19 日。

# 以下常量定义本端、对端及网络段；命令顺序维持现有设备配置流程。
INTERFACE="end1"
M_IP="192.168.100.10"      # 发送端在 end1 上使用的地址。
NETMASK="255.255.255.0"    # 地址规划所用子网掩码。
NETWORK="192.168.100.0/24" # 发送与接收设备共用的网络段。
S_IP="192.168.100.20"      # 接收端目标地址，用于连通性检查。

# 仅供终端状态信息着色，不影响网络配置命令。
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # 重置终端颜色。

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}    M端网络配置脚本 (发送端)${NC}"
echo -e "${BLUE}======================================${NC}"
echo -e "接口: ${YELLOW}${INTERFACE}${NC}"
echo -e "本机IP: ${YELLOW}${M_IP}${NC}"
echo -e "目标IP: ${YELLOW}${S_IP}${NC}"
echo -e "网络段: ${YELLOW}${NETWORK}${NC}"
echo ""

# 修改网口和服务需要管理员权限；权限不足时在执行命令前退出。
if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}错误: 请使用root权限运行此脚本${NC}"
    echo "使用方法: sudo $0"
    exit 1
fi

# 先确认 end1 可见，缺失时列出当前接口供排查。
if ! ip link show ${INTERFACE} > /dev/null 2>&1; then
    echo -e "${RED}错误: 网络接口 ${INTERFACE} 不存在${NC}"
    echo "可用接口:"
    ip link show | grep -E "^[0-9]+:" | awk '{print $2}' | sed 's/://g'
    exit 1
fi

echo -e "${YELLOW}[1/8] 配置end0接口IP地址...${NC}"
# 先按原流程尝试设置 end0 管理地址 192.168.137.111/24。
echo "正在配置end0接口..."
nmcli con mod end0 ipv4.addresses 192.168.137.111/24 ipv4.method manual 2>/dev/null || echo "end0连接可能不存在，将跳过配置"
nmcli con up end0 2>/dev/null || echo "end0连接激活失败，将继续其他配置"
if ip addr show end0 | grep -q "192.168.137.111"; then
    echo -e "${GREEN}✓ end0接口配置成功: 192.168.137.111${NC}"
else
    echo -e "${YELLOW}⚠ end0接口配置可能失败，继续配置end1接口${NC}"
fi
sleep 1

echo -e "${YELLOW}[2/8] 停止NetworkManager服务...${NC}"
systemctl stop NetworkManager 2>/dev/null || echo "NetworkManager未运行"

echo -e "${YELLOW}[3/8] 关闭网络接口...${NC}"
ip link set ${INTERFACE} down
sleep 1

echo -e "${YELLOW}[4/8] 清除现有IP配置...${NC}"
ip addr flush dev ${INTERFACE}

echo -e "${YELLOW}[5/8] 启用网络接口...${NC}"
ip link set ${INTERFACE} up
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ 接口启用成功${NC}"
else
    echo -e "${RED}✗ 接口启用失败${NC}"
    exit 1
fi

# 等待 end1 完成链路启动，再写入本端静态地址。
sleep 2

echo -e "${YELLOW}[6/8] 配置IP地址...${NC}"
ip addr add ${M_IP}/24 dev ${INTERFACE}
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ IP地址配置成功: ${M_IP}${NC}"
else
    echo -e "${RED}✗ IP地址配置失败${NC}"
    exit 1
fi

echo -e "${YELLOW}[7/8] 验证网络配置...${NC}"
echo ""

# 展示链路、地址和路由，供人工核对配置结果。
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
ping -c 3 -W 2 ${S_IP} > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ 与S端 (${S_IP}) 连通正常${NC}"
else
    echo -e "${YELLOW}⚠ 暂时无法连接到S端 (${S_IP})${NC}"
    echo "  这是正常的，请确保S端也已配置完成"
fi

echo ""
echo -e "${YELLOW}[8/8] 显示所有网络配置摘要...${NC}"
echo ""
echo -e "${BLUE}end0接口状态:${NC}"
ip addr show end0 2>/dev/null | grep inet || echo "end0接口未配置或不存在"

echo ""
echo -e "${GREEN}======================================${NC}"
echo -e "${GREEN}    M端网络配置完成!${NC}"
echo -e "${GREEN}======================================${NC}"
echo -e "配置信息:"
echo -e "  • end0接口: 192.168.137.111/24 (固定IP)"
echo -e "  • ${INTERFACE}接口: ${M_IP}/24 (视频传输)"
echo -e "  • 目标IP: ${S_IP}"
echo -e "  • end1状态: $(ip link show ${INTERFACE} | grep -o 'state [A-Z]*' | cut -d' ' -f2)"
echo ""
echo -e "${BLUE}提示:${NC}"
echo "  • end0用于管理网络 (192.168.137.111)"
echo "  • ${INTERFACE}用于视频数据发送 (${M_IP} -> ${S_IP}:8888)"
echo "  • 如需修改IP，请编辑此脚本中的配置参数"
echo ""

# 把本次配置值写到临时记录文件，供后续诊断使用。
echo "# M端网络配置信息 - $(date)" > /tmp/m_network_config.txt
echo "END0_IP=192.168.137.111" >> /tmp/m_network_config.txt
echo "INTERFACE=${INTERFACE}" >> /tmp/m_network_config.txt
echo "M_IP=${M_IP}" >> /tmp/m_network_config.txt
echo "S_IP=${S_IP}" >> /tmp/m_network_config.txt
echo "STATUS=CONFIGURED" >> /tmp/m_network_config.txt

echo -e "${GREEN}配置信息已保存到: /tmp/m_network_config.txt${NC}"