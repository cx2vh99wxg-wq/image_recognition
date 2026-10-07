#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
run_s_board.py -- S 板（决策 + 通信 + 显示）一键验证脚本【人员 B】

把"配网 + 编译 + 启动 planning_main + 模拟 M 板发帧自测"做成单文件脚本，
避免从聊天里逐条复制命令。纯 Python3 标准库，无第三方依赖。

用法（在 S 板 RK3568 上，仓库根或 scripts/ 目录下）：
    sudo python3 run_s_board.py            # = all：配网 + 编译 + 启动
    sudo python3 run_s_board.py setup      # 只配静态 IP（临时，重启丢）
    sudo python3 run_s_board.py persist    # 写 systemd 开机自启配网（持久）
    sudo python3 run_s_board.py build      # 只编译 planning_main
    sudo python3 run_s_board.py start      # 只启动（--no-lcd，后台）
    sudo python3 run_s_board.py stop       # 停止 planning_main
    sudo python3 run_s_board.py status     # 看进程 / 网口 / IP
    sudo python3 run_s_board.py log        # 看最新日志
    sudo python3 run_s_board.py mock left  # 模拟 M 板发"左转"帧做决策自测
    sudo python3 run_s_board.py mock right # 发"右转"
    sudo python3 run_s_board.py mock       # 默认发"直道"

网口约定：优先 end0（你的板子只有 end0 能用），其次 end1/eth0。
IP 约定：S 板 192.168.100.20/24（与 driving_config.h 的 UDP_IP_S 一致）。
"""

import argparse
import os
import socket
import struct
import subprocess
import sys
import time

# ---- 仓库根目录（脚本放仓库根或 scripts/ 都能用）----
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
if os.path.isdir(os.path.join(SCRIPT_DIR, "planning")):
    REPO_ROOT = SCRIPT_DIR
else:
    REPO_ROOT = os.path.dirname(SCRIPT_DIR)

S_IP = "192.168.100.20"
M_IP = "192.168.100.10"
PORT = 8888
NETIF_PREF = ["end0", "end1", "eth0"]
LOG_FILE = os.path.join(REPO_ROOT, "planning.log")

# ---- UDP 协议常量（与 planning/test/udp_mock.py 一致）----
MAGIC_FRAME = 0x4C414E45   # 'LANE'
MAGIC_DATA = 0x44415441    # 'DATA'
BLOCK_SIZE = 1400
IMG_W, IMG_H = 640, 480
DATA_SIZE = IMG_W * IMG_H * 2
LANE_UNKNOWN, LANE_STRAIGHT, LANE_LEFT, LANE_RIGHT = 0, 1, 2, 3


def sh(cmd, capture=True, check=False):
    """执行 shell 命令，返回 CompletedProcess。"""
    return subprocess.run(cmd, shell=True, capture_output=capture, text=True)


def need_root():
    if os.geteuid() != 0:
        print("错误：配网/进程管理需要 root，请用 sudo 运行，例如：")
        print("    sudo python3 run_s_board.py " + " ".join(sys.argv[1:]))
        sys.exit(1)


def find_netif():
    for n in NETIF_PREF:
        if sh(f"ip link show {n}").returncode == 0:
            return n
    return None


def find_binary():
    for rel in ("bin/planning_main", "planning/planning_main"):
        p = os.path.join(REPO_ROOT, rel)
        if os.path.exists(p):
            return p
    return None


def _stage_bin():
    """把编译产物统一拷到 bin/，让 start_s.sh 等也能用。"""
    src = os.path.join(REPO_ROOT, "planning", "planning_main")
    dst_dir = os.path.join(REPO_ROOT, "bin")
    if os.path.exists(src):
        os.makedirs(dst_dir, exist_ok=True)
        try:
            import shutil
            shutil.copy(src, os.path.join(dst_dir, "planning_main"))
        except Exception:
            pass


def cmd_setup(persist=False):
    need_root()
    n = find_netif()
    if not n:
        print("错误：未找到可用有线网口（试过 end0/end1/eth0）。")
        print(sh("ip link show").stdout)
        sys.exit(1)
    sh(f"ip link set {n} up", check=False)
    sh(f"ip addr flush dev {n}", check=False)
    r = sh(f"ip addr add {S_IP}/24 dev {n}")
    if r.returncode != 0:
        print("配网失败：", r.stderr)
        sys.exit(1)
    print(f"[OK] 网口 {n} -> {S_IP}/24（临时，重启会丢；用 persist 子命令可持久化）")
    if persist:
        write_systemd(n)


def write_systemd(n):
    unit = "/etc/systemd/system/s-board-net.service"
    content = (
        "[Unit]\n"
        "Description=S board static IP (B part)\n"
        "After=network.target\n\n"
        "[Service]\n"
        "Type=oneshot\n"
        "RemainAfterExit=yes\n"
        "ExecStart=/sbin/ip link set %s up\n"
        "ExecStart=/sbin/ip addr add %s/24 dev %s\n\n"
        "[Install]\n"
        "WantedBy=multi-user.target\n"
    ) % (n, S_IP, n)
    try:
        with open(unit, "w") as f:
            f.write(content)
        sh("systemctl daemon-reload", check=False)
        sh("systemctl enable s-board-net", check=False)
        print(f"[OK] 已写 {unit} 并 enable（开机自动配 {n}->{S_IP}）")
    except Exception as e:
        print("写 systemd 失败：", e)
        print("可手动把下面两行加进 /etc/rc.local：")
        print(f"  ip link set {n} up; ip addr add {S_IP}/24 dev {n}")


def cmd_build():
    libso = os.path.join(REPO_ROOT, "lib", "librknnrt.so")
    if not os.path.exists(libso):
        print("警告：lib/librknnrt.so 缺失！板上将无法链接 -lrknnrt。")
        print("请确认 git clone 包含了 lib/ 目录；否则从参考工程拷 librknnrt.so 到 lib/。")
    print(">>> make -C planning bin")
    r = sh(f"make -C planning bin", cwd=REPO_ROOT)
    if r.returncode != 0:
        print("编译失败：\n", r.stderr)
        sys.exit(1)
    _stage_bin()
    b = find_binary()
    print(f"[OK] 已编译：{b}")


def cmd_start(nolcd=True):
    b = find_binary()
    if not b:
        print("未找到 planning_main，请先运行 build。")
        sys.exit(1)
    # 先停掉旧进程，避免端口/共享内存冲突
    sh("pkill -f planning_main", check=False)
    time.sleep(0.5)
    env = os.environ.copy()
    env["LD_LIBRARY_PATH"] = os.path.join(REPO_ROOT, "lib") + ":" + env.get("LD_LIBRARY_PATH", "")
    args = [b, "--no-lcd"] if nolcd else [b]
    with open(LOG_FILE, "wb") as f:
        subprocess.Popen(args, stdout=f, stderr=subprocess.STDOUT, env=env, cwd=REPO_ROOT)
    time.sleep(1.0)
    print(f"[OK] 已启动 planning_main（{'无屏' if nolcd else '带屏'}），日志：{LOG_FILE}")
    print("      自测：sudo python3 run_s_board.py mock left")


def cmd_stop():
    r = sh("pkill -f planning_main", check=False)
    if r.returncode == 0:
        print("[OK] 已停止 planning_main")
    else:
        print("没有正在运行的 planning_main 进程")


def cmd_status():
    n = find_netif()
    print("=== 进程 ===")
    p = sh("pgrep -af planning_main")
    print(p.stdout.strip() or "（未运行）")
    print("=== 网口 / IP ===")
    if n:
        out = sh(f"ip addr show {n}").stdout
        for line in out.splitlines():
            if "inet " in line or "state " in line or line.strip().startswith(n + ":"):
                print(line)
    print("=== 最新日志 ===")
    cmd_log()


def cmd_log(lines=40):
    if not os.path.exists(LOG_FILE):
        print("（无日志文件）")
        return
    with open(LOG_FILE, "r", errors="replace") as f:
        data = f.read().splitlines()
    for line in data[-lines:]:
        print(line)


def _block_checksum(data):
    return sum(data) & 0xFFFFFFFF


def _make_frame_hdr(frame_id, direction, conf, offset, now_ns):
    ts_s = now_ns // 1_000_000_000
    ts_us = (now_ns // 1000) % 1_000_000
    block_count = (DATA_SIZE + BLOCK_SIZE - 1) // BLOCK_SIZE
    return struct.pack(
        "<19I", MAGIC_FRAME, 1, frame_id, IMG_W, IMG_H, DATA_SIZE, BLOCK_SIZE,
        block_count, ts_s, ts_us, direction, offset & 0xFFFFFFFF, conf,
        12345, 0x1, 0, 0, 0, 0,
    )


def _make_data_block(frame_id, idx, payload):
    return struct.pack("<4I", MAGIC_DATA, frame_id, idx, _block_checksum(payload)) + payload


def _make_image(direction):
    img = bytearray(DATA_SIZE)
    base = {LANE_LEFT: 64, LANE_RIGHT: 128, LANE_STRAIGHT: 32}.get(direction, 96)
    for y in range(IMG_H):
        for x in range(IMG_W):
            v = (base + (x * 2 // IMG_W) * 48 + (y * 40 // IMG_H)) & 0xFF
            img[y * IMG_W * 2 + x * 2] = v
    return bytes(img)


def cmd_mock(direction_name="straight", frames=100):
    direction = {
        "straight": LANE_STRAIGHT, "left": LANE_LEFT,
        "right": LANE_RIGHT, "unknown": LANE_UNKNOWN,
    }.get(direction_name, LANE_STRAIGHT)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 2 * 1024 * 1024)
    addr = (S_IP, PORT)
    print(f"[mock] -> {S_IP}:{PORT}  帧数={frames}  方向={direction_name}")
    print("（同时另开一个终端运行：sudo python3 run_s_board.py status 看 S 板决策）")
    img = _make_image(direction)
    for fid in range(1, frames + 1):
        hdr = _make_frame_hdr(fid, direction, 95, 0, time.time_ns())
        sock.sendto(hdr, addr)
        for idx in range(0, len(img), BLOCK_SIZE):
            chunk = img[idx: idx + BLOCK_SIZE]
            sock.sendto(_make_data_block(fid, idx // BLOCK_SIZE, chunk), addr)
        if fid % 10 == 0:
            print(f"[mock] 帧 {fid}/{frames} 已发送")
        time.sleep(0.03)
    print("[mock] 发送完成")
    sock.close()


def cmd_all():
    cmd_setup(persist=False)
    cmd_build()
    cmd_start(nolcd=True)
    print("")
    print("下一步：sudo python3 run_s_board.py mock left   # 看 S 板决策是否变左转")


def main():
    ap = argparse.ArgumentParser(description="S 板一键验证脚本（人员 B）")
    sub = ap.add_subparsers(dest="cmd")
    sub.add_parser("setup", help="配临时静态 IP")
    p_persist = sub.add_parser("persist", help="配网并写 systemd 开机自启")
    sub.add_parser("build", help="编译 planning_main")
    sub.add_parser("start", help="后台启动 planning_main(--no-lcd)")
    sub.add_parser("stop", help="停止 planning_main")
    sub.add_parser("status", help="进程/网口/IP/日志")
    sub.add_parser("log", help="看最新日志")
    p_mock = sub.add_parser("mock", help="模拟 M 板发帧自测")
    p_mock.add_argument("direction", nargs="?", default="straight",
                        choices=["left", "right", "straight", "unknown"])
    p_mock.add_argument("frames", nargs="?", type=int, default=100)
    sub.add_parser("all", help="setup + build + start")
    args = ap.parse_args()

    if args.cmd == "setup":
        cmd_setup(persist=False)
    elif args.cmd == "persist":
        cmd_setup(persist=True)
    elif args.cmd == "build":
        cmd_build()
    elif args.cmd == "start":
        cmd_start(nolcd=True)
    elif args.cmd == "stop":
        cmd_stop()
    elif args.cmd == "status":
        cmd_status()
    elif args.cmd == "log":
        cmd_log()
    elif args.cmd == "mock":
        cmd_mock(args.direction, args.frames)
    elif args.cmd == "all":
        cmd_all()
    else:
        cmd_all()


if __name__ == "__main__":
    main()
