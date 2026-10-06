#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
udp_mock.py — M 端 UDP 发送器模拟（【人员 B】，M2 里程碑联调工具）

协议实现与 C 端 common/udp_proto.h 完全一致（帧头 76B + 数据块 16B 头 + payload）。
不依赖 RK3568 / 共享内存，在 PC 上即可向 S 端发送假帧，验证 S 端接收/重组/决策。

用法：
    python3 udp_mock.py [目标IP] [端口] [帧数] [方向]

方向：straight(默认) / left / right / unknown / red / green
示例：
    python3 udp_mock.py 192.168.100.20 8888 100 straight
"""
import socket
import struct
import sys
import time

MAGIC_FRAME = 0x4C414E45          # 'LANE'
MAGIC_DATA  = 0x44415441          # 'DATA'
PROTO_VERSION = 1
BLOCK_SIZE  = 1400

IMG_W, IMG_H = 640, 480
DATA_SIZE = IMG_W * IMG_H * 2

# LaneDirection / ControlCommand 与 driving_types.h 一致
LANE_UNKNOWN, LANE_STRAIGHT, LANE_LEFT, LANE_RIGHT = 0, 1, 2, 3
TL_RED, TL_GREEN = 1, 3


def block_checksum(data: bytes) -> int:
    return sum(data) & 0xFFFFFFFF


def make_frame_hdr(frame_id: int, direction: int, confidence: int,
                   offset: int, now_ns: int) -> bytes:
    ts_s = now_ns // 1_000_000_000
    ts_us = (now_ns // 1000) % 1_000_000
    block_count = (DATA_SIZE + BLOCK_SIZE - 1) // BLOCK_SIZE
    hdr = struct.pack(
        '<19I',
        MAGIC_FRAME, PROTO_VERSION, frame_id,
        IMG_W, IMG_H, DATA_SIZE, BLOCK_SIZE, block_count,
        ts_s, ts_us,
        direction, offset & 0xFFFFFFFF, confidence,
        12345,                         # lane_pixel_cnt
        0x00000001,                    # flags: LANE_VALID
        0, 0, 0, 0,                    # reserved[4]
    )
    return hdr


def make_data_block(frame_id: int, idx: int, payload: bytes) -> bytes:
    return struct.pack('<4I', MAGIC_DATA, frame_id, idx,
                       block_checksum(payload)) + payload


def make_image(direction: int) -> bytes:
    """构造假图像：按方向给不同灰度带，便于肉眼在 LCD 上区分。"""
    img = bytearray(DATA_SIZE)
    base = {LANE_LEFT: 64, LANE_RIGHT: 128, LANE_STRAIGHT: 32}.get(direction, 96)
    for y in range(IMG_H):
        row = bytearray(IMG_W * 2)
        for x in range(IMG_W):
            v = (base + (x * 2 // IMG_W) * 48 + (y * 40 // IMG_H)) & 0xFF
            row[x * 2] = v
            row[x * 2 + 1] = 0
        img[y * IMG_W * 2: (y + 1) * IMG_W * 2] = row
    return bytes(img)


def main():
    ip = sys.argv[1] if len(sys.argv) > 1 else '192.168.100.20'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 8888
    frames = int(sys.argv[3]) if len(sys.argv) > 3 else 100
    direction_name = sys.argv[4] if len(sys.argv) > 4 else 'straight'
    direction = {
        'straight': LANE_STRAIGHT, 'left': LANE_LEFT,
        'right': LANE_RIGHT, 'unknown': LANE_UNKNOWN,
    }.get(direction_name, LANE_STRAIGHT)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 2 * 1024 * 1024)
    sock.settimeout(2.0)
    addr = (ip, port)

    print(f"[mock] -> {ip}:{port}  帧数={frames}  方向={direction_name}")

    img = make_image(direction)
    for fid in range(1, frames + 1):
        hdr = make_frame_hdr(fid, direction, 95, 0, time.time_ns())
        sock.sendto(hdr, addr)
        for idx in range(0, len(img), BLOCK_SIZE):
            chunk = img[idx: idx + BLOCK_SIZE]
            sock.sendto(make_data_block(fid, idx // BLOCK_SIZE, chunk), addr)
        if fid % 10 == 0:
            print(f"[mock] 帧 {fid}/{frames} 已发送 ({len(img)} B)")
        time.sleep(0.03)   # ~33fps

    print("[mock] 发送完成")
    sock.close()


if __name__ == '__main__':
    main()
