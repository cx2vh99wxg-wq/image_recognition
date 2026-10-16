#!/bin/bash
# M FPGA OV5640 -> PCIe -> RK3568 -> local X11 windows only.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
DEVICE="${PCIE_DEVICE:-/dev/pango_pci_driver}"
KO="$REPO_ROOT/drivers/pango_pci_driver.ko"

if [ "$(id -u)" -eq 0 ]; then
    echo "请从 M 板图形桌面的普通用户终端运行本脚本；脚本会按需用 sudo 加载驱动。" >&2
    exit 1
fi
if [ -z "${DISPLAY:-}" ]; then
    echo "DISPLAY 未设置：请从 M 板 HDMI 图形桌面终端运行。" >&2
    exit 1
fi
if pgrep -f '(^|/)(udp_m_send_main|perception_main|adas_main|planning_main|stereo_local_preview)([[:space:]]|$)' >/dev/null; then
    echo "已有采集或显示进程；先在 M 板运行 sudo bash scripts/stop_all.sh。" >&2
    exit 1
fi

if command -v lspci >/dev/null && ! lspci -nn | grep -qi '\[0755:0755\]'; then
    echo "lspci 未显示 FPGA PCIe 端点 [0755:0755]；先核对当前 FPGA 位流与 PCIe 枚举。" >&2
    exit 1
fi
if ! lsmod | grep -q '^pango_pci_driver '; then
    if command -v modinfo >/dev/null; then
        KO_KERNEL="$(modinfo -F vermagic "$KO" | awk '{print $1}')"
        if [ "$KO_KERNEL" != "$(uname -r)" ]; then
            echo "驱动内核版本 $KO_KERNEL 与运行内核 $(uname -r) 不同；先使用匹配内核的 .ko。" >&2
            exit 1
        fi
    fi
    sudo insmod "$KO"
fi
if [ ! -c "$DEVICE" ]; then
    echo "没有设备节点 $DEVICE；查看 lspci -nnk 和 sudo dmesg | tail -80。" >&2
    exit 1
fi
if [ ! -r "$DEVICE" ] || [ ! -w "$DEVICE" ]; then
    sudo chown "$(id -u):$(id -g)" "$DEVICE"
    sudo chmod u+rw "$DEVICE"
fi
make -C "$REPO_ROOT/planning" local-preview
exec "$REPO_ROOT/planning/stereo_local_preview" "$@"
