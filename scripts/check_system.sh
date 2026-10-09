#!/bin/bash
# check_system.sh — 板端系统诊断（【人员 B】集成）
#
# 自动识别当前板卡角色（M/S）并检查：
#   1. PCIe 驱动 / 设备节点
#   2. RKNN 运行时库
#   3. 可执行文件
#   4. 运行进程
#   5. 共享内存
#
# 用法：   ./check_system.sh
# 角色判断：目录下存在 perception_main + udp_m_send_main 视为 M 端，
#          存在 planning_main 视为 S 端（可用 ROLE=m|s 强制指定）。

ROLE="${ROLE:-}"
if [ -z "$ROLE" ]; then
    if [ -f "bin/perception_main" ] && [ -f "bin/udp_m_send_main" ]; then
        ROLE="m"
    elif [ -f "bin/planning_main" ]; then
        ROLE="s"
    fi
fi

ok()   { printf "  \033[0;32m✓\033[0m %s\n" "$1"; }
bad()  { printf "  \033[0;31m✗\033[0m %s\n" "$1"; }
warn() { printf "  \033[1;33m⚠\033[0m %s\n" "$1"; }

echo "======================================"
echo " 板端系统诊断（角色: ${ROLE:-未知}）"
echo "======================================"

# 1) 驱动
echo "[1] PCIe 驱动"
if lsmod 2>/dev/null | grep -q pango_pci_driver; then
    ok "pango_pci_driver 已加载"
else
    bad "pango_pci_driver 未加载（insmod drivers/pango_pci_driver.ko）"
fi
if [ -e /dev/pango_pci_driver ]; then
    ok "设备节点 /dev/pango_pci_driver 存在"
else
    warn "设备节点不存在（驱动未加载或 FPGA 未上电）"
fi

# 2) RKNN 运行时
echo "[2] RKNN 运行时"
if [ -f lib/librknnrt.so ]; then
    ok "lib/librknnrt.so 存在"
else
    bad "lib/librknnrt.so 缺失（放入 lib/ 并 export LD_LIBRARY_PATH）"
fi

# 3) 可执行文件
echo "[3] 可执行文件"
for b in bin/perception_main bin/udp_m_send_main bin/planning_main; do
    if [ -f "$b" ]; then ok "$b"; else warn "$b 不存在（make board 后拷贝 bin/）"; fi
done

# 4) 运行进程
echo "[4] 运行进程"
for p in perception_main udp_m_send_main planning_main; do
    if pgrep -f "$p" >/dev/null 2>&1; then ok "$p 运行中 (pid $(pgrep -f "$p" | tr '\n' ' '))"; else warn "$p 未运行"; fi
done

# 5) 共享内存
echo "[5] 共享内存（key 见 driving_config.h）"
if command -v ipcs >/dev/null 2>&1; then
    ipcs -m 2>/dev/null | grep -E '1234567|key' || warn "无本系统共享内存段"
else
    warn "ipcs 不可用"
fi

echo "======================================"
echo " 诊断完成"
echo "======================================"
