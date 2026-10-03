#!/bin/bash
# 系统检查和诊断脚本
# 用于验证M端系统的运行状态

echo "==================================="
echo "M端系统诊断工具"
echo "==================================="
echo ""

# 1. 检查可执行文件
echo "=== 检查可执行文件 ==="
if [ -f "pcie/build/pcie_capture" ]; then
    echo "✓ PCIe程序存在: pcie/build/pcie_capture"
    ls -lh pcie/build/pcie_capture
else
    echo "✗ PCIe程序不存在，请先运行 make pcie"
fi

if [ -f "udp/build/udp_sender" ]; then
    echo "✓ UDP程序存在: udp/build/udp_sender"
    ls -lh udp/build/udp_sender
else
    echo "✗ UDP程序不存在，请先运行 make udp"
fi
echo ""

# 2. 检查运行进程
echo "=== 检查运行进程 ==="
PCIE_PIDS=$(pgrep -f "pcie_capture" 2>/dev/null || true)
if [ ! -z "$PCIE_PIDS" ]; then
    echo "✓ PCIe程序运行中 (PIDs: $PCIE_PIDS)"
else
    echo "✗ PCIe程序未运行"
fi

UDP_PIDS=$(pgrep -f "udp_sender" 2>/dev/null || true)
if [ ! -z "$UDP_PIDS" ]; then
    echo "✓ UDP程序运行中 (PIDs: $UDP_PIDS)"
else
    echo "✗ UDP程序未运行"
fi
echo ""

# 3. 检查共享内存
echo "=== 检查共享内存 ==="
if command -v ipcs >/dev/null 2>&1; then
    SHARED_MEM=$(ipcs -m | grep 0x12345679 2>/dev/null || true)
    if [ ! -z "$SHARED_MEM" ]; then
        echo "✓ PCIe共享内存存在"
        echo "$SHARED_MEM"
    else
        echo "✗ PCIe共享内存不存在"
    fi
else
    echo "⚠ ipcs命令不可用，无法检查共享内存"
fi
echo ""

# 4. 检查日志文件
echo "=== 检查日志文件 ==="
if [ -f "pcie/pcie.log" ]; then
    echo "✓ PCIe日志存在: pcie/pcie.log"
    echo "最后10行:"
    tail -n 10 pcie/pcie.log
else
    echo "✗ PCIe日志不存在"
fi
echo ""

if [ -f "udp/udp.log" ]; then
    echo "✓ UDP日志存在: udp/udp.log"
    echo "最后10行:"
    tail -n 10 udp/udp.log
else
    echo "✗ UDP日志不存在"
fi
echo ""

# 5. 系统建议
echo "=== 系统建议 ==="
if [ ! -z "$PCIE_PIDS" ] && [ ! -z "$UDP_PIDS" ]; then
    echo "✓ 系统正常运行"
elif [ ! -z "$PCIE_PIDS" ] && [ -z "$UDP_PIDS" ]; then
    echo "⚠ PCIe程序运行中，但UDP程序未运行"
    echo "  建议: cd udp && ./build/udp_sender"
elif [ -z "$PCIE_PIDS" ] && [ ! -z "$UDP_PIDS" ]; then
    echo "⚠ UDP程序运行中，但PCIe程序未运行（异常状态）"
    echo "  建议: make stop && make run"
else
    echo "✗ 所有程序都未运行"
    echo "  建议: make run 启动系统"
fi

echo ""
echo "==================================="
echo "诊断完成"
echo "==================================="
