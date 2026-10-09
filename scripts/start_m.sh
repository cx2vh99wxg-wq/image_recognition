#!/bin/bash
# start_m.sh — M 端板卡启动脚本（【人员 B】集成）
#
# 流程：配置网卡 → 内核 UDP 调优 → 启动感知进程 → 启动 UDP 发送器
#   · 真实模式（检测到 /dev/pango_pci_driver）：perception_main 加载
#     yolopv2.rknn 走 PCIe 采集真实图像；udp_m_send_main 读共享内存发帧。
#   · 桩模式（无 FPGA / 无摄像头）：只起 udp_m_send_main --stub，发送
#     2×2 拼接模拟图（3 路渐变 + 1 预留格），供 S 端联调。★ 桩模式不加载 yolopv2.rknn —— RKNN 的
#     加载与 PCIe 采集在 perception 里是同一分支（perception_pipeline.c），
#     没有设备就无法初始化模型，这是硬件约束不是脚本限制。
#
# 用法（在 M 端 RK3568 上，需 root）：
#   sudo ./scripts/start_m.sh [--stub|--real] [--no-build] [模型绝对路径] [驱动ko绝对路径]
#   sudo bash ./scripts/start_m.sh        # 也可以（本脚本不依赖可执行位）
#
# 无参数时自动判定模式：有 /dev/pango_pci_driver → 真实，否则 → 桩。
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BIN_DIR="${BIN_DIR:-$REPO_ROOT/bin}"

# ---- 参数 ----
NO_BUILD=0
FORCE_MODE=""                 # "" | stub | real
POS=()
for a in "$@"; do
    case "$a" in
        --stub)     FORCE_MODE=stub ;;
        --real)     FORCE_MODE=real ;;
        --no-build) NO_BUILD=1 ;;
        *)          POS+=("$a") ;;
    esac
done
MODEL_PATH="${POS[0]:-$REPO_ROOT/model/yolopv2_Nx3x480x640_rk3568.rknn}"
KO_PATH="${POS[1]:-$REPO_ROOT/drivers/pango_pci_driver.ko}"

# ---- 可执行文件定位 ----
# 优先 $BIN_DIR（宿主交叉编译后 make board 的部署布局），其次模块目录
# （板端原生 make -C <模块> bin 的默认产物位置）；两处都没有则就地编译。
#
# ★ 防呆（2026-10-09 加）：光"文件存在"不够——git pull 后忘了重编译时，旧二进制
#   会被静默沿用，症状是"改动明明提交了，屏幕/行为却没变"。故先做"新鲜度"判定：
#   模块自身 + common 的 .c/.h/Makefile 只要有比二进制新的，就视为过期 → 自动
#   重新编译；编译失败或 --no-build 时回退旧二进制，但打印"使用可能过期的二进制"
#   警告（绝不再静默）。
build_module() {              # $1=模块目录名
    [ -d "$REPO_ROOT/$1" ] || return 0
    echo "  · 编译 $1： make -C $1 bin" >&2
    ( cd "$REPO_ROOT" && make -C "$1" bin ) 1>&2 \
        || echo "    ⚠ $1 编译失败（改用已有二进制）" >&2
}
bin_fresh() {                 # $1=二进制路径 $2=模块目录名；0=存在且不比源码旧
    local bin="$1" mod="$2" newer=""
    [ -f "$bin" ] || return 1
    [ -n "$mod" ] || return 0
    newer="$(find "$REPO_ROOT/$mod/csrc" "$REPO_ROOT/$mod/include" \
                  "$REPO_ROOT/common/csrc" "$REPO_ROOT/common/include" \
                  -type f \( -name '*.c' -o -name '*.h' -o -name 'Makefile' \) \
                  -newer "$bin" 2>/dev/null | head -n 1)"
    [ -z "$newer" ]
}
resolve_bin() {               # $1=文件名  $2=模块目录名
    local name="$1" mod="$2" cand have_old=0
    # 1) 存在且不比源码旧 → 直接用（宿主部署 / 刚编译过的正常路径）
    for cand in "$BIN_DIR/$name" "$REPO_ROOT/$mod/$name"; do
        [ -f "$cand" ] || continue
        have_old=1
        if bin_fresh "$cand" "$mod"; then
            printf '%s\n' "$cand"; return 0
        fi
    done
    # 2) 缺失或已过期 → 重新编译（--no-build 可跳过）
    if [ "$NO_BUILD" = 0 ] && [ -n "$mod" ]; then
        if [ "$have_old" = 1 ]; then
            echo "  ⚠ $name 比源码旧（git pull 后忘了重编译？）→ 自动重新编译 $mod" >&2
        fi
        build_module "$mod"
        for cand in "$REPO_ROOT/$mod/$name" "$BIN_DIR/$name"; do
            [ -f "$cand" ] || continue
            if bin_fresh "$cand" "$mod"; then
                printf '%s\n' "$cand"; return 0
            fi
        done
    fi
    # 3) 兜底：仍用过期的（编译失败或 --no-build），但把风险讲清楚
    for cand in "$BIN_DIR/$name" "$REPO_ROOT/$mod/$name"; do
        [ -f "$cand" ] || continue
        echo "  ⚠ 警告：使用可能过期的二进制（编译未成功或已 --no-build）： $cand" >&2
        printf '%s\n' "$cand"; return 0
    done
    return 1
}

# RKNN 运行时库路径（librknnrt.so）
export LD_LIBRARY_PATH="$REPO_ROOT/lib:${LD_LIBRARY_PATH:-}"

if [ "$(id -u)" -ne 0 ]; then
    echo "提示：驱动加载/网络配置需要 root 权限，建议用 sudo 运行"
fi

echo "==== M 端启动 ===="

# ---- 运行模式判定：显式 --stub/--real 优先，否则看 PCIe 设备节点 ----
if [ "$FORCE_MODE" = stub ]; then
    MODE=stub;  WHY="（--stub 指定）"
elif [ "$FORCE_MODE" = real ]; then
    MODE=real;  WHY="（--real 指定）"
elif [ -e /dev/pango_pci_driver ]; then
    MODE=real;  WHY="（检测到 /dev/pango_pci_driver）"
else
    MODE=stub;  WHY="（未检测到 /dev/pango_pci_driver，无 FPGA/摄像头）"
fi
echo "  运行模式：$MODE $WHY"
if [ "$MODE" = stub ]; then
    echo "  注：桩模式只发 2×2 拼接模拟图，不加载 yolopv2.rknn（无 PCIe 采集则无法初始化模型）"
fi

# 1) 加载 PCIe 驱动（仅真实模式；若已加载则跳过）
if [ "$MODE" = real ]; then
    if lsmod 2>/dev/null | grep -q pango_pci_driver; then
        echo "[1] pango_pci_driver 已加载"
    else
        echo "[1] 加载驱动: $KO_PATH"
        insmod "$KO_PATH"
    fi
else
    echo "[1] 桩模式：跳过 PCIe 驱动加载"
fi

# 2) 配置网卡（独立脚本：自动探测 end0/end1 + ip 命令 + NetworkManager 规避）
#    用 bash 显式调起：脚本若丢了可执行位（Windows 克隆/提交时常见），
#    直接 exec 会报 "Permission denied"。
echo "[2] 配置网络"
bash "$SCRIPT_DIR/setup_network_m.sh" || echo "警告：网络配置未成功（请检查网线是否插在 end0）"

# 3) 内核 UDP 缓冲调优（发送端突发写入；接收端 S 板更要调，见 start_s.sh）
#    注意用 -f 而非 -x：可执行位缺失时 -x 会让调优被"静默跳过"
echo "[3] 内核 UDP 缓冲调优"
if [ -f "$SCRIPT_DIR/tune_net.sh" ]; then
    bash "$SCRIPT_DIR/tune_net.sh" || echo "警告：tune_net.sh 执行失败"
fi

# 4) 启动感知进程（A 交付）——仅真实模式。桩模式下 perception 无 PCIe 采集，
#    且其主循环是无 sleep 的紧循环（会空转吃满一核），故桩模式不起它。
if [ "$MODE" = real ]; then
    PERC_BIN="$(resolve_bin perception_main perception)" || PERC_BIN=""
    if [ -z "$PERC_BIN" ]; then
        echo "[4] 警告：未找到 perception_main，跳过感知进程"
    else
        echo "[4] 启动 perception_main（真实模式，模型 $MODEL_PATH）"
        nohup "$PERC_BIN" --model "$MODEL_PATH" \
            > "$REPO_ROOT/perception.log" 2>&1 &
    fi
else
    echo "[4] 桩模式：跳过 perception_main（不发真实图像，改由发送器造图）"
fi

# 5) 启动 UDP 发送器（B 交付）；桩模式发 2×2 拼接模拟图，无需摄像头
SEND_BIN="$(resolve_bin udp_m_send_main planning)" || {
    echo "错误：找不到也无法编译 udp_m_send_main"
    exit 1
}
SEND_ARGS=()
[ "$MODE" = stub ] && SEND_ARGS+=(--stub)
echo "[5] 启动 udp_m_send_main ${SEND_ARGS[*]}"
nohup "$SEND_BIN" "${SEND_ARGS[@]}" \
    > "$REPO_ROOT/udp_send.log" 2>&1 &

sleep 1
echo "==== M 端启动完成（模式=$MODE）===="
echo "日志: perception.log / udp_send.log"
echo "全部停止请用: ./scripts/stop_all.sh"
