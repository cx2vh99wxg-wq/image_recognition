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
# 默认 --local-stub：S 端使用确定的灰度模拟图；不以共享内存存在推断摄像头在线。
# --local-pcie：直接读取 S 板自己的 PCIe；--no-local-stub：兼容外部共享内存采集。
# 模拟模式自动关闭行人推理与彩色叠加；真实采集验证可加 --no-person --no-overlay。
#
# --with-stub：单板自测模式。除 S 端决策+显示外，本机再起一个
#   udp_m_send_main --stub（B 交付的 M 端发送器桩），把 2×2 拼接模拟图发往
#   127.0.0.1，接收端 bind INADDR_ANY:8888 直接收到。
#   于是无需 M 板 / 摄像头，仅凭本脚本即可复现完整链路。
#
# 三个反复踩过的坑，本脚本已内置规避：
#   1) 脚本可执行位：Windows 上克隆/提交时 git 容易丢 +x，板端 git pull 后
#      直接 exec 子脚本会 "Permission denied"。故内部一律用 bash 调子脚本。
#   2) X11 授权：网络/驱动要 root，但画面要「桌面会话用户的授权 cookie」。
#      整个进程用 root 跑时，root 的 XAUTHORITY 指向 /root/.Xauthority
#      （通常不存在）→ XOpenDisplay 失败 → 进程照常收帧/决策，屏幕上却什么都
#      没有（"脚本说启动完成、就是没图像"的根因）。故这里自动降权到桌面用户。
#   3) pull 后忘重编译：二进制还在但比源码旧，画面/行为仍是旧版
#      （"改动明明提交了，屏幕上却没变"）。resolve_bin 会检测源码是否比
#      二进制新，过期就自动重新编译；失败则停止，不回退旧程序。
set -e
if [ "${1:-}" = --adas ]; then shift; exec bash "$(dirname "$0")/start_adas.sh" s "$@"; fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BIN_DIR="${BIN_DIR:-$REPO_ROOT/bin}"

# 内部统一用 bash 调子脚本，不依赖文件的可执行位
SH() { bash "$@"; }

source "$SCRIPT_DIR/resolve_bin.sh"

# 行人检测模型（仓库实际文件为 model/yolov5s-640-640.rknn）
MODEL="$REPO_ROOT/model/yolov5s-640-640.rknn"
NO_LCD=0
WITH_STUB=0
NO_BUILD=0
LOCAL_SOURCE=stub
NO_PERSON=0
NO_OVERLAY=0

while [ $# -gt 0 ]; do
    case "$1" in
        --model)         [ $# -ge 2 ] || { echo "--model 需要路径"; exit 2; }; MODEL="$2"; shift 2 ;;
        --no-lcd)        NO_LCD=1; shift ;;
        --with-stub)     WITH_STUB=1; shift ;;
        --no-build)      NO_BUILD=1; shift ;;
        --local-stub)    LOCAL_SOURCE=stub; shift ;;
        --local-pcie)    LOCAL_SOURCE=pcie; shift ;;
        --no-local-stub) LOCAL_SOURCE=shm; shift ;;
        --no-person)     NO_PERSON=1; shift ;;
        --no-overlay)    NO_OVERLAY=1; shift ;;
        --help|-h) echo "Usage: $0 [--local-stub|--local-pcie|--no-local-stub] [--no-person] [--no-overlay] [--no-lcd] [--with-stub] [--no-build] [--model path]"; exit 0 ;;
        *)               echo "未知参数：$1"; exit 2 ;;
    esac
done

# Prevent an old window/process from hiding the newly built result.
if pgrep -f '(^|/)planning_main([[:space:]]|$)' >/dev/null; then
    echo "planning_main 已在运行；请先 bash scripts/stop_all.sh，再重新启动。"
    exit 1
fi

# RKNN 运行时库路径（librknnrt.so）
export LD_LIBRARY_PATH="$REPO_ROOT/lib:${LD_LIBRARY_PATH:-}"

if [ "$(id -u)" -ne 0 ]; then
    echo "提示：网络配置与内核调优需要 root；图形进程会自动降权到桌面用户。"
    echo "      建议：sudo ./scripts/start_s.sh"
fi

echo "==== S 端启动 ===="

# 0) 定位可执行文件：planning/ → bin/ → 现场编译（校验六路版本标识）
PLANNING_BIN="$(resolve_bin planning_main planning)" || {
    echo "错误：找不到也无法编译 planning_main"
    echo "      板端构建： cd $REPO_ROOT && make -C planning bin"
    echo "      交叉编译： 宿主机 make board 后把 bin/ 拷到板卡"
    exit 1
}
echo "  可执行文件: $PLANNING_BIN"
"$PLANNING_BIN" --build-info
if [ "$LOCAL_SOURCE" != stub ] && [ "$NO_PERSON" = 0 ] && [ ! -f "$MODEL" ]; then
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

# 4) Explicit S-board source. No silent fallback in PCIe mode.
ARGS=(--model "$MODEL")
[ "$NO_LCD" = 1 ] && ARGS+=(--no-lcd)
case "$LOCAL_SOURCE" in
    stub) ARGS+=(--local-stub --no-person --no-overlay) ;;
    pcie)
        if [ ! -e /dev/pango_pci_driver ]; then
            insmod "$REPO_ROOT/drivers/pango_pci_driver.ko"
        fi
        # planning_main runs as the desktop user. Grant only this user access
        # to the capture device for this boot; do not make /dev/mem world-writable.
        if [ "$(id -u)" -eq 0 ] && [ "$RUSER" != root ]; then
            chown "$RUSER" /dev/pango_pci_driver
            chmod u+rw /dev/pango_pci_driver
        fi
        ARGS+=(--local-pcie) ;;
    shm) echo "外部共享内存模式：必须另有 S 本地采集进程写图，段存在不代表实时帧。" ;;
esac
[ "$NO_PERSON" = 1 ] && ARGS+=(--no-person)
[ "$NO_OVERLAY" = 1 ] && ARGS+=(--no-overlay)
echo "[3b] S 本地图来源：$LOCAL_SOURCE；布局 960x480（M 上三路 / S 下三路）"

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
#    udp_m_send_main --stub，把每板拼接模拟图发往 127.0.0.1，
#    接收端 bind INADDR_ANY:8888 直接收到，等价于「M 板在持续发帧」。
#    上排 M 三路、下排 S 三路，与双板联调布局一致。
if [ "$WITH_STUB" = 1 ]; then
    SEND_BIN="$(resolve_bin udp_m_send_main planning)" || SEND_BIN=""
    if [ -z "$SEND_BIN" ]; then
        echo "⚠ --with-stub 需要 udp_m_send_main（先 cd $REPO_ROOT && make -C planning bin）"
    else
        echo "[5] 本机桩发帧：$SEND_BIN --stub（目标 127.0.0.1:8888，本机回环）"
        nohup "$SEND_BIN" --stub --ip 127.0.0.1 \
            > "$REPO_ROOT/udp_send.log" 2>&1 &
        echo "    日志: $REPO_ROOT/udp_send.log"
    fi
fi

echo "==== S 端启动完成 ===="
echo "日志: $LOG"
[ "$WITH_STUB" = 1 ] && echo "桩发帧日志: $REPO_ROOT/udp_send.log"
echo "全部停止请用: ./scripts/stop_all.sh"
