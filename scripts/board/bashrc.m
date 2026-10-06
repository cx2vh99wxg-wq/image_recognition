# =====================================================================
# bashrc.m — M 端（发送端）~/.bashrc 追加模板（【人员 B】集成）
#
# 由 scripts/board/deploy_board.sh 安装：读取本文件，把 @REPO_ROOT@
# 替换为仓库在板卡上的绝对路径后，追加到 ~/.bashrc 末尾。
# 如需手动追加：复制以下内容到 ~/.bashrc，并把 @REPO_ROOT@ 改成实际路径。
#
# 作用：登录即自动（1）加载 RKNN 运行时库路径（2）insmod PCIe 驱动
#       （3）配置直连网络。进程启动仍建议用 scripts/start_m.sh 手动/后台启动。
# =====================================================================

# ===== B 端 M 板自动配置（开始）=====
REPO_ROOT="@REPO_ROOT@"

# 1) RKNN 运行时库（librknnrt.so）
export LD_LIBRARY_PATH="${REPO_ROOT}/lib:${LD_LIBRARY_PATH}"

# 2) 自动加载 PCIe 驱动（若未加载）
if [ -f "${REPO_ROOT}/drivers/pango_pci_driver.ko" ] && ! lsmod 2>/dev/null | grep -q pango_pci_driver; then
    sudo insmod "${REPO_ROOT}/drivers/pango_pci_driver.ko" 2>/dev/null || true
fi

# 3) 自动配置直连网络
if [ -f "${REPO_ROOT}/scripts/setup_network_m.sh" ]; then
    sudo "${REPO_ROOT}/scripts/setup_network_m.sh" >/dev/null 2>&1 || true
fi

# 4) 提示启动命令
echo "[M 端就绪] 启动感知+发送: cd ${REPO_ROOT} && sudo ./scripts/start_m.sh"
# ===== B 端 M 板自动配置（结束）=====
