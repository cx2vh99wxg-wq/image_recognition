#!/bin/bash
# deploy_board.sh — 板卡一键部署（【人员 B】集成）
#
# 把「开机自启配置 + 驱动 + 运行时库 + 网络」一次性部署到当前板卡。
# 在板卡上（把仓库拷贝到板卡任意目录后）运行本脚本即可，无需手动改文件。
#
# 用法（板卡上，需 sudo）：
#   sudo ./scripts/board/deploy_board.sh [m|s]
#   角色省略时按 bin/ 下二进制自动判断：有 perception_main+udp_m_send_main 视为 M，
#   有 planning_main 视为 S。
#
# 本脚本会：
#   1. 把对应角色的 bashrc 追加块（含 LD_LIBRARY_PATH/insmod/网络自启）写入 ~/.bashrc
#   2. 立即 insmod drivers/pango_pci_driver.ko（若未加载）
#   3. 立即执行 setup_network_m/s.sh 配置直连网络
#   4. 打印后续启动命令
set -e

# 仓库根目录 = 本脚本上两级（scripts/board/ -> 仓库根）
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

ROLE="${1:-}"
if [ -z "$ROLE" ]; then
    if [ -f "$REPO_ROOT/bin/perception_main" ] && [ -f "$REPO_ROOT/bin/udp_m_send_main" ]; then
        ROLE="m"
    elif [ -f "$REPO_ROOT/bin/planning_main" ]; then
        ROLE="s"
    fi
fi

case "$ROLE" in
    m|M) ROLE="m"; TPL="$SCRIPT_DIR/bashrc.m"; NET="setup_network_m.sh" ;;
    s|S) ROLE="s"; TPL="$SCRIPT_DIR/bashrc.s"; NET="setup_network_s.sh" ;;
    *) echo "错误：无法确定板卡角色，请显式指定：sudo $0 m|s"; exit 1 ;;
esac

echo "==== 部署角色：$( [ "$ROLE" = m ] && echo 'M 端（发送）' || echo 'S 端（接收）' ) ===="
echo "仓库根：$REPO_ROOT"

# 1) 追加 bashrc 自启配置（幂等：先清除旧块再追加）
MARK_START="# ===== B 端 $( [ "$ROLE" = m ] && echo M || echo S ) 板自动配置（开始）====="
MARK_END="# ===== B 端 $( [ "$ROLE" = m ] && echo M || echo S ) 板自动配置（结束）====="
if [ -f "$HOME/.bashrc" ]; then
    sed -i "/${MARK_START//\//\\/}/,/${MARK_END//\//\\/}/d" "$HOME/.bashrc"
fi
sed "s|@REPO_ROOT@|${REPO_ROOT}|g" "$TPL" >> "$HOME/.bashrc"
echo "  ✓ 已写入 ~/.bashrc 自启配置（LD_LIBRARY_PATH / insmod / 网络）"

# 2) 立即加载驱动
if lsmod 2>/dev/null | grep -q pango_pci_driver; then
    echo "  ✓ pango_pci_driver 已加载"
else
    if [ -f "$REPO_ROOT/drivers/pango_pci_driver.ko" ]; then
        insmod "$REPO_ROOT/drivers/pango_pci_driver.ko" && echo "  ✓ 已加载 pango_pci_driver.ko" \
            || echo "  ⚠ insmod 失败（内核版本不匹配？）"
    else
        echo "  ⚠ 未找到 drivers/pango_pci_driver.ko"
    fi
fi

# 3) 配置网络（用 bash 调起，不依赖脚本可执行位）
if [ -f "$REPO_ROOT/scripts/$NET" ]; then
    bash "$REPO_ROOT/scripts/$NET" || true
fi

echo ""
echo "==== 部署完成 ===="
echo "后续启动："
if [ "$ROLE" = m ]; then
    echo "  cd $REPO_ROOT && sudo ./scripts/start_m.sh"
else
    echo "  cd $REPO_ROOT && sudo ./scripts/start_s.sh"
fi
echo "诊断：cd $REPO_ROOT && ./scripts/check_system.sh"
