#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
sim_udp_drain.py — "S 端收帧恒 0"根因的定量模型验证（【人员 B】，2026-10-07）

不上板就能回答一个问题：
    在 M 端 5 帧/s（2200 包/s）的产出下，S 端"每轮只 recv 1 包 + 渲染"
    到底会丢多少包？把 socket 缓冲调大（rmem_max）够不够？"每轮收干"能不能救？

模型（按真实参数）：
    产出：一帧 = 440 包（1 帧头 + 439 数据块），5 帧/s → 2200 包/s
    S 端：每轮 poll→取包→渲染 LCD，轮周期 = loop_ms
    内核：socket 接收队列容量 = cap 包（含 skb 记账开销，按 ~2KB/包折算）
          队列满时**丢弃新到的包**（Linux 行为），已入队的包仍会被按序取走
    网卡：ring/softnet 层可在"入队之前"丢包（drop_p 概率，模拟 ring 溢出）

判读：
    单轮只取 1 包时，消费能力 = 1000/loop_ms 包/s。若远小于 2200，队列必然
    长期处于满态并持续丢包 —— 但注意"丢新包"意味着最早那几帧仍是完整的，
    所以理论上慢速取最终仍能收齐第一帧。因此：
      模型算出的收帧数 > 0 而实测为 0  ⇒ 丢包发生在 socket 队列**之前**
                                        （网卡 RX ring / netdev backlog），
                                        此时调 rmem_max 无效，必须查
                                        `ip -s link` 与 /proc/net/softnet_stat。
"""
import sys
from collections import deque

FRAME_PKTS = 440          # 1 帧头 + 439 数据块
FPS        = 5            # M 端桩模式：每 200ms 一帧
DUR_S      = 30.0         # 观测时长（用户日志约 26~30s）


def simulate(loop_ms, cap_pkts, drain_max, drop_p, dur_s=DUR_S, dt=5e-4):
    """返回 (收帧数, 应用取包数, 内核队列丢弃数, 网卡层丢弃数)"""
    q = deque()
    next_pkt = 0            # 全局包序号；第 k 帧占 [k*440, (k+1)*440)
    frames = 0
    taken = 0
    kern_drop = 0
    nic_drop = 0

    cur_fid = -1
    got = set()

    pkt_rate = FRAME_PKTS * FPS
    acc = 0.0
    t = 0.0
    next_loop = 0.0

    while t < dur_s:
        # ---- M 端产出（匀速近似；真实是每 200ms 一次 440 包突发）----
        acc += pkt_rate * dt
        n_new = int(acc)
        acc -= n_new
        for _ in range(n_new):
            p = next_pkt
            next_pkt += 1
            # 网卡 RX ring / softnet backlog 层丢包（入 socket 队列之前）
            # 用 Knuth 乘法散列做确定性伪随机（可复现，不依赖 hash() 随机种子）
            if drop_p > 0.0 and ((p * 2654435761) % 10000) < drop_p * 10000:
                nic_drop += 1
                continue
            if len(q) < cap_pkts:
                q.append(p)
            else:
                kern_drop += 1      # 队列满 → 丢新包

        # ---- S 端主循环（每 loop_ms 跑一轮）----
        while t >= next_loop:
            next_loop += loop_ms / 1000.0
            n = 0
            while q and n < drain_max:
                p = q.popleft()
                n += 1
                taken += 1
                fid, idx = divmod(p, FRAME_PKTS)
                if idx == 0:
                    cur_fid, got = fid, set()          # 帧头：开始新帧（丢残块）
                elif fid == cur_fid:
                    got.add(idx)
                    if len(got) == FRAME_PKTS - 1:     # 块齐（不含帧头）
                        frames += 1
        t += dt

    return frames, taken, kern_drop, nic_drop


def report(title, **kw):
    f, tk, kd, nd = simulate(**kw)
    lim = kw['drain_max'] * 1000.0 / kw['loop_ms']
    print("%-42s 收帧=%-4d 取包=%-6d 内核丢=%-7d 网卡丢=%-6d  消费上限=%.0f 包/s"
          % (title, f, tk, kd, nd, lim))
    return f


print("=" * 108)
print("模型：M 端 5 帧/s × 440 包 = 2200 包/s 产出；观测 %.0f 秒" % DUR_S)
print("=" * 108)

report("修复前 缓冲208KB(默认rmem_max)  每轮取1包",
       loop_ms=30, cap_pkts=148,  drain_max=1,    drop_p=0.0)
report("修复前 缓冲8MB(rmem_max=16MB)   每轮取1包",
       loop_ms=30, cap_pkts=5900, drain_max=1,    drop_p=0.0)
report("修复前 缓冲8MB                每轮取1包(慢轮100ms)",
       loop_ms=100, cap_pkts=5900, drain_max=1,   drop_p=0.0)
print("-" * 108)
report("修复后 缓冲8MB                每轮收干4096",
       loop_ms=30, cap_pkts=5900, drain_max=4096, drop_p=0.0)
report("修复后 缓冲8MB                每轮收干4096 + 网卡丢0.5%",
       loop_ms=30, cap_pkts=5900, drain_max=4096, drop_p=0.005)
report("修复后 缓冲8MB                每轮收干4096 + 网卡丢1%",
       loop_ms=30, cap_pkts=5900, drain_max=4096, drop_p=0.01)
print("=" * 108)
print("结论提示：")
print("  1) 只要「每轮只取 1 包」，消费上限就只有 33 包/s，比产出 2200 低 66 倍；")
print("  2) 但「丢新包」语义下最早几帧仍完整，慢速取最终能收齐 → 模型收帧>0；")
print("  3) 若实测为 0 而模型>0，则网卡 RX ring / netdev backlog 层也在丢包：")
print("     任一层丢 0.5%，收齐一帧的概率 = 0.995^440 ≈ 11%；丢 1% 则 ≈ 1.2%。")
print("     → 这就是「调大 rmem_max 也没用」的原因，必须同时查 ip -s link /")
print("       /proc/net/softnet_stat，并用 ethtool -G 放大 RX ring。")
