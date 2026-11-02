#!/usr/bin/env python3
"""Read-only Linux publication/network/CPU sampler; no camera/control commands.

These counters measure shared-memory publications, not sensor exposures or
displayed frames. Use capture raw_changed_fps and DISPLAY shown_fps alongside it.
"""
import argparse
import ctypes
import json
import os
import time
from pathlib import Path


class Control(ctypes.Structure):
    _fields_ = [("frame_id", ctypes.c_uint32), ("ready", ctypes.c_uint32),
                ("active", ctypes.c_uint32), ("readers", ctypes.c_uint32),
                ("timestamp", ctypes.c_uint64), ("width", ctypes.c_uint32),
                ("height", ctypes.c_uint32), ("checksum", ctypes.c_uint32),
                ("reserved", ctypes.c_ubyte * 64)]


def network(interface):
    result = {}
    for field in ("rx_bytes", "tx_bytes", "rx_dropped", "tx_dropped", "rx_errors", "tx_errors"):
        try:
            result[field] = int(Path("/sys/class/net", interface, "statistics", field).read_text())
        except OSError:
            pass
    lines = Path("/proc/net/snmp").read_text().splitlines()
    for i, line in enumerate(lines[:-1]):
        if line.startswith("Udp:"):
            result.update({"udp_" + k: int(v) for k, v in
                           zip(line.split()[1:], lines[i + 1].split()[1:])})
            break
    return result


def cpu():
    result = {}
    names = {"app", "pcie_capture", "udp_sender", "video_receiver",
             "eight_camera_re", "vehicle_control"}
    for entry in Path("/proc").iterdir():
        if not entry.name.isdigit():
            continue
        try:
            raw = (entry / "stat").read_text()
            name = raw[raw.index("(") + 1:raw.rindex(")")]
            if name in names:
                fields = raw[raw.rindex(")") + 2:].split()
                result[entry.name] = (name, int(fields[11]) + int(fields[12]), fields[19])
        except (OSError, ValueError, IndexError):
            pass
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--seconds", type=float, default=10)
    parser.add_argument("--interface", default="end1")
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    if args.seconds <= 0:
        parser.error("--seconds must be positive")
    assert ctypes.sizeof(Control) == 104
    lib = ctypes.CDLL(None, use_errno=True)
    lib.shmget.argtypes = [ctypes.c_int, ctypes.c_size_t, ctypes.c_int]
    lib.shmget.restype = ctypes.c_int
    lib.shmat.argtypes = [ctypes.c_int, ctypes.c_void_p, ctypes.c_int]
    lib.shmat.restype = ctypes.c_void_p
    lib.shmdt.argtypes = [ctypes.c_void_p]
    lib.shmdt.restype = ctypes.c_int
    streams = {}
    try:
        for name, key in (("pcie", 0x12345679), ("udp", 0x1234567A)):
            sid = lib.shmget(key, 1, 0)  # Never create or alter a segment.
            if sid < 0:
                continue
            ptr = lib.shmat(sid, None, 0o10000)  # SHM_RDONLY
            if ptr and ptr != ctypes.c_void_p(-1).value:
                streams[name] = (ptr, Control.from_address(ptr))
        start = time.monotonic()
        previous = {k: int(v[1].frame_id) for k, v in streams.items()}
        counts = {k: 0 for k in streams}
        interrupted = {k: False for k in streams}
        n0, c0 = network(args.interface), cpu()
        while time.monotonic() - start < args.seconds:
            time.sleep(min(1, max(0, args.seconds - (time.monotonic() - start))))
            for name, (_, control) in streams.items():
                current = int(control.frame_id)
                delta = (current - previous[name]) & 0xFFFFFFFF
                if delta > 100000 or not control.active:
                    interrupted[name] = True
                else:
                    counts[name] += delta
                previous[name] = current
        elapsed = time.monotonic() - start
        n1, c1 = network(args.interface), cpu()
        result = {"elapsed_s": elapsed,
                  "publication_fps": {k: count / elapsed for k, count in counts.items()},
                  "interrupted": interrupted,
                  "network_delta": {k: v - n0[k] for k, v in n1.items() if k in n0},
                  "cpu_pct_one_core": {
                      pid: {"name": value[0], "cpu": (value[1] - c0[pid][1]) * 100 /
                            os.sysconf("SC_CLK_TCK") / elapsed}
                      for pid, value in c1.items()
                      if pid in c0 and value[2] == c0[pid][2]}}
        output = json.dumps(result, ensure_ascii=False, indent=2) + "\n"
        if args.output:
            args.output.write_text(output, encoding="utf-8")
        print(output, end="")
    finally:
        for ptr, _ in streams.values():
            lib.shmdt(ptr)


if __name__ == "__main__":
    main()
