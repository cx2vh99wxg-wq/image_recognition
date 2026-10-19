#!/bin/bash
# 逐项查看 M 端构建产物、进程、共享内存和日志；脚本只读取状态。
# 最后的建议依据 PCIe 与 UDP 两个进程是否同时运行给出。

echo "==================================="
echo "M端系统诊断工具"
echo "==================================="
echo ""

# 检查两个预期构建产物是否存在，并在存在时显示文件大小。
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

# 用进程名称查找采集端和发送端，保留原有的 PID 输出。
echo "=== 检查运行进程 ==="
PCIE_PIDS=$(pgrep -f "pcie_capture" 2>/dev/null || true)
if [ -n "$PCIE_PIDS" ]; then
    echo "✓ PCIe程序运行中 (PIDs: $PCIE_PIDS)"
else
    echo "✗ PCIe程序未运行"
fi

UDP_PIDS=$(pgrep -f "udp_sender" 2>/dev/null || true)
if [ -n "$UDP_PIDS" ]; then
    echo "✓ UDP程序运行中 (PIDs: $UDP_PIDS)"
else
    echo "✗ UDP程序未运行"
fi
echo ""

# 若系统提供 ipcs，则以固定键 0x12345679 查找 PCIe 图像段。
echo "=== 检查共享内存 ==="
if command -v ipcs >/dev/null 2>&1; then
    SHARED_MEM=$(ipcs -m | grep 0x12345679 2>/dev/null || true)
    if [ -n "$SHARED_MEM" ]; then
        echo "✓ PCIe共享内存存在"
        echo "$SHARED_MEM"
    else
        echo "✗ PCIe共享内存不存在"
    fi
else
    echo "⚠ ipcs命令不可用，无法检查共享内存"
fi
echo ""

# 分别查看 PCIe 与 UDP 日志，存在时打印末尾十行。
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

# 按两个进程的四种组合输出已有的运行状态和处理建议。
echo "=== 系统建议 ==="
if [ -n "$PCIE_PIDS" ] && [ -n "$UDP_PIDS" ]; then
    echo "✓ 系统正常运行"
elif [ -n "$PCIE_PIDS" ] && [ -z "$UDP_PIDS" ]; then
    echo "⚠ PCIe程序运行中，但UDP程序未运行"
    echo "  建议: cd udp && ./build/udp_sender"
elif [ -z "$PCIE_PIDS" ] && [ -n "$UDP_PIDS" ]; then
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
