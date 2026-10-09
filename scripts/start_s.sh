#!/bin/bash
# start_s.sh — S 端板卡启动脚本（【人员 B】集成）
#
# 流程：配置网卡 → 内核 UDP 调优 → 以「桌面用户」身份启动决策+显示进程
#       （planning_main 内含 UDP 接收 / 行人检测 / 决策 / LCD）
#
# 用法（在 S 端 RK3568 上，需 root）：
#   sudo ./scripts/start_s.sh [--model 行人模型绝对路径] [--no-lcd] [--with-stub] [--no-build]
#   sudo bash ./scripts/start_s.sh        # 也可以（本脚本不依赖可执行位）
#
# 本地图（屏幕左半屏）来源自动判定 —— 判据是「shm_pcie_img 段有没有写者」：
#   · ipcs 里存在 0x12345679 段 → 有采集进程在写，用真实图；
#   · 不存在（S 板当前尚无本地采集进程）→ 自动加 --local-stub，用合成拼接图案顶上，
#     保证「每板 3 路 × 2 板 = 6 路拼接 + 渲染」这条通路始终可见、可验证。
#   加 --no-local-stub 可强制走真实采集（不兜底）。
#
# --with-stub：单板自测模式。除 S 端决策+显示外，本机再起一个
#   udp_m_send_main --stub（B 交付的 M 端发送器桩），把 2×2 拼接模拟图发往
#   UDP_IP_S（=本机 192.168.100.20），接收端 bind INADDR_ANY:8888 直接收到。
#   于是无需 M 板 / 摄像头，仅凭本脚本即可复现完整链路。
#
# 两个反复踩过的坑，本脚本已内置规避：
#   1) 脚本可执行位：Windows 上克隆/提交时 git 容易丢 +x，板端 git pull 后
#      直接 exec 子脚本会 "Permission denied"。故内部一律用 bash 调子脚本。
#   2) X11 授权：网络/驱动要 root，但画面要「桌面会话用户的授权 cookie」。
#      整个进程用 root 跑时，root 的 XAUTHORITY 指向 /root/.Xauthority
#      （通常不存在）→ XOpenDisplay 失败 → 进程照常收帧/决策，屏幕上却什么都
#      没有（"脚本说启动完成、就是没图像"的根因）。故这里自动降权到桌面用户。
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BIN_DIR="${BIN_DIR:-$REPO_ROOT/bin}"

# 内部统一用 bash 调子脚本，不依赖文件的可执行位
SH() { bash "$@"; }

# ---- 可执行文件定位 ----
# 优先 $BIN_DIR（宿主交叉编译后 make board 的部署布局），其次模块目录
# （板端原生 make -C <模块> bin 的默认产物位置）；两处都没有则就地编译。
build_module() {              # $1=模块目录名
    [ -d "$REPO_ROOT/$1" ] || return 0
    echo "  · 编译 $1： make -C $1 bin" >&2
    ( cd "$REPO_ROOT" && make -C "$1" bin ) 1>&2 \
        || echo "    ⚠ $1 编译失败（改用已有二进制）" >&2
}
resolve_bin() {               # $1=文件名  $2=模块目录名
    local name="$1" mod="$2"
    [ -f "$BIN_DIR/$name" ] && { printf '%s\n' "$BIN_DIR/$name"; return 0; }
    [ -n "$mod" ] && [ -f "$REPO_ROOT/$mod/$name" ] && { printf '%s\n' "$REPO_ROOT/$mod/$name"; return 0; }
    if [ "$NO_BUILD" = 0 ] && [ -n "$mod" ]; then
        build_module "$mod"
        [ -f "$REPO_ROOT/$mod/$name" ] && { printf '%s\n' "$REPO_ROOT/$mod/$name"; return 0; }
        [ -f "$BIN_DIR/$name" ] && { printf '%s\n' "$BIN_DIR/$name"; return 0; }
    fi
    return 1
}

# 行人检测模型（仓库实际文件为 model/yolov5s-640-640.rknn）
MODEL="$REPO_ROOT/model/yolov5s-640-640.rknn"
NO_LCD=0
WITH_STUB=0
NO_BUILD=0
NO_LOCAL_STUB=0        # 1=强制用真实本地采集（不自动兜底合成图案）

while [ $# -gt 0 ]; do
    case "$1" in
        --model)         MODEL="$2"; shift 2 ;;
        --no-lcd)        NO_LCD=1; shift ;;
        --with-stub)     WITH_STUB=1; shift ;;
        --no-build)      NO_BUILD=1; shift ;;
        --no-local-stub) NO_LOCAL_STUB=1; shift ;;
        *)               shift ;;
    esac
done

# RKNN 运行时库路径（librknnrt.so）
export LD_LIBRARY_PATH="$REPO_ROOT/lib:${LD_LIBRARY_PATH:-}"

if [ "$(id -u)" -ne 0 ]; then
    echo "提示：网络配置与内核调优需要 root；图形进程会自动降权到桌面用户。"
    echo "      建议：sudo ./scripts/start_s.sh"
fi

echo "==== S 端启动 ===="

# 0) 定位可执行文件：bin/ → planning/ → 现场编译（--no-build 可跳过编译）
PLANNING_BIN="$(resolve_bin planning_main planning)" || {
    echo "错误：找不到也无法编译 planning_main"
    echo "      板端构建： cd $REPO_ROOT && make -C planning bin"
    echo "      交叉编译： 宿主机 make board 后把 bin/ 拷到板卡"
    exit 1
}
echo "  可执行文件: $PLANNING_BIN"
if [ ! -f "$MODEL" ]; then
    echo "警告：找不到行人模型 $MODEL（将退化为无行人模式，决策仍可用）"
fi

# 1) 配置网卡（自动探测 end0/end1 + ip 命令 + NetworkManager 规避）
echo "[1] 配置网络"
SH "$SCRIPT_DIR/setup_network_s.sh" || echo "警告：网络配置未成功（请检查网线是否插在 end0）"

# 2) 内核 UDP 缓冲调优：一帧 440 包突发到达，rmem_max 默认仅 ~208KB，会静默丢包
#    注意用 -f 而不是 -x：脚本若丢了可执行位，-x 会让调优被"静默跳过"。
echo "[2] 内核 UDP 缓冲调优"
if [ -f "$SCRIPT_DIR/tune_net.sh" ]; then
    SH "$SCRIPT_DIR/tune_net.sh" || echo "警告：tune_net.sh 执行失败，接收缓冲偏小可能丢包"
else
    echo "警告：未找到 tune_net.sh，已跳过（接收缓冲偏小会静默丢包）"
fi

# 3) 确定图形进程的运行身份与 X11 环境
#    探测顺序：RENDER_USER 环境变量 → sudo 调用者 → 登录会话 → who → /home 下首个用户
detect_desktop_user() {
    local u
    if [ -n "${RENDER_USER:-}" ]; then echo "$RENDER_USER"; return; fi
    if [ -n "${SUDO_USER:-}" ] && [ "$SUDO_USER" != "root" ]; then echo "$SUDO_USER"; return; fi
    u="$(loginctl list-sessions --no-legend 2>/dev/null | awk '$3!="" && $3!="root" {print $3; exit}')"
    if [ -n "$u" ]; then echo "$u"; return; fi
    u="$(who 2>/dev/null | awk 'NR==1{print $1}')"
    if [ -n "$u" ]; then echo "$u"; return; fi
    for d in /home/*; do
        [ -d "$d" ] || continue
        echo "$(basename "$d")"; return
    done
    id -un
}

RUSER="$(detect_desktop_user)"

# DISPLAY：优先沿用当前环境，否则按 /tmp/.X11-unix/X<n> 现存套接字推断
if [ -z "${DISPLAY:-}" ]; then
    for n in 0 1 2; do
        if [ -e "/tmp/.X11-unix/X$n" ]; then DISPLAY=":$n"; break; fi
    done
fi
DISPLAY="${DISPLAY:-:0}"

# XAUTHORITY：桌面会话的 X 授权 cookie，缺了它 root 也画不出图
XAUTH="${XAUTHORITY:-}"
if [ -z "$XAUTH" ]; then
    for p in "/home/$RUSER/.Xauthority" "${HOME:-/root}/.Xauthority"; do
        [ -f "$p" ] && { XAUTH="$p"; break; }
    done
fi
XAUTH="${XAUTH:-/home/$RUSER/.Xauthority}"

if [ "$NO_LCD" = 1 ]; then
    echo "[3] --no-lcd：跳过 X11（不开窗口）"
else
    echo "[3] 图形身份：用户=$RUSER  DISPLAY=$DISPLAY  XAUTHORITY=$XAUTH"
fi

# 4) 启动决策+显示进程
#    关键：当以 root 运行时，用 sudo -u 把图形进程降权到桌面用户，
#    否则 XOpenDisplay 会因缺少授权而失败（黑屏但进程正常）。
#
#    本地图来源判定：S 板的本地图由「本板 FPGA 经 PCIe 送图 → 采集进程写 shm_pcie_img」
#    提供。注意**判据是"共享段有没有写者"而不是"设备节点在不在"** —— 设备节点在只说明
#    PCIe 链路通了，不代表有进程在写图（当前代码库里 S 板还没有本地采集进程）。
#    故这里查 ipcs 里有没有 0x12345679 段：
#      · 有   → 有采集进程在写，用真实图；
#      · 没有 → 自动加 --local-stub，用合成拼接图案顶上，保证 6 路拼接通路可见可验证。
#    ipcs 不存在或查询失败时按"没有"处理（保守兜底）。--no-local-stub 可强制走真实。
ARGS=(--model "$MODEL")
[ "$NO_LCD" = 1 ] && ARGS+=(--no-lcd)

LOCAL_STUB=0
if [ "$NO_LOCAL_STUB" = 1 ]; then
    LOCAL_SRC="真实采集（--no-local-stub 强制）"
elif ipcs -m 2>/dev/null | grep -qi '0x12345679'; then
    LOCAL_SRC="真实采集（检测到 shm_pcie_img 段，已有采集进程在写）"
else
    LOCAL_STUB=1
    LOCAL_SRC="合成拼接图案（未检测到 shm_pcie_img 段 → S 板暂无本地采集进程）"
    ARGS+=(--local-stub)
fi
echo "[3b] 本地图来源：$LOCAL_SRC"

LOG="$REPO_ROOT/planning.log"
: > "$LOG"

if [ "$NO_LCD" = 1 ]; then
    RUN_ENV=(env LD_LIBRARY_PATH="$LD_LIBRARY_PATH")
elif [ "$(id -u)" -eq 0 ] && [ "$RUSER" != "root" ]; then
    RUN_ENV=(sudo -u "$RUSER" env HOME="/home/$RUSER" DISPLAY="$DISPLAY" \
             XAUTHORITY="$XAUTH" LD_LIBRARY_PATH="$LD_LIBRARY_PATH")
else
    RUN_ENV=(env DISPLAY="$DISPLAY" XAUTHORITY="$XAUTH" LD_LIBRARY_PATH="$LD_LIBRARY_PATH")
fi

echo "[4] 启动 planning_main ${ARGS[*]}"
nohup "${RUN_ENV[@]}" "$PLANNING_BIN" "${ARGS[@]}" > "$LOG" 2>&1 &
PID=$!
sleep 2

# 4b) 启动后自检：把"启动成功但其实没画面"变成看得见的告警
if ! kill -0 "$PID" 2>/dev/null; then
    echo "⚠ planning_main 启动后立即退出，日志末尾："
    tail -8 "$LOG" | sed 's/^/    /'
    echo "  常见原因：模型路径错、共享内存段损坏（./scripts/stop_all.sh 后重试）、端口 8888 被占用。"
    exit 1
fi

if [ "$NO_LCD" = 0 ] && grep -q "XOpenDisplay 失败" "$LOG" 2>/dev/null; then
    echo
    echo "⚠ 图形上屏失败：进程在跑（收帧/决策正常），但屏幕上不会有画面。"
    grep -m1 -A6 "XOpenDisplay 失败" "$LOG" | sed 's/^/    /'
    echo "    临时规避： 加 --no-lcd 只跑数据链路"
    echo "    彻底解决： RENDER_USER=<桌面用户> sudo -E ./scripts/start_s.sh"
fi

# 5) 可选：本机自环桩发帧（--with-stub）
#    无 M 板 / 无 FPGA / 无摄像头时，单板即可验证全链路：在本机再起一个
#    udp_m_send_main --stub，把灰色渐变图发往 UDP_IP_S（=本机 192.168.100.20），
#    接收端 bind INADDR_ANY:8888 直接收到，等价于「M 板在持续发帧」。
#    屏幕右半出现灰度渐变、左半为 S 板本地摄像头（未接则黑），与双板联调观感一致。
if [ "$WITH_STUB" = 1 ]; then
    SEND_BIN="$(resolve_bin udp_m_send_main planning)" || SEND_BIN=""
    if [ -z "$SEND_BIN" ]; then
        echo "⚠ --with-stub 需要 udp_m_send_main（先 cd $REPO_ROOT && make -C planning bin）"
    else
        echo "[5] 本机桩发帧：$SEND_BIN --stub（目标 192.168.100.20:8888，本机回环）"
        nohup "$SEND_BIN" --stub \
            > "$REPO_ROOT/udp_send.log" 2>&1 &
        echo "    日志: $REPO_ROOT/udp_send.log"
    fi
fi

echo "==== S 端启动完成 ===="
echo "日志: $LOG"
[ "$WITH_STUB" = 1 ] && echo "桩发帧日志: $REPO_ROOT/udp_send.log"
echo "全部停止请用: ./scripts/stop_all.sh"
