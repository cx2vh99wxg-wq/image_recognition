"""Collapse repeated output assignments in the fixed Gamma ROMs."""
from collections import defaultdict
from pathlib import Path
import re

from build_pds import FPGA, HERE

ENTRY = re.compile(rb"\s*8'h([\da-f]{2})\s*:\s*Post_Data\s*<=\s*8'h([\da-f]{2});\s*", re.I)
GROUP = re.compile(rb"\s*((?:8'h[\da-f]{2}\s*,\s*)*8'h[\da-f]{2})\s*:\s*Post_Data\s*<=\s*8'h([\da-f]{2});", re.I)
CASE = re.compile(rb"(\tcase\(Pre_Data\)\r?\n)(.*?)(\tendcase)", re.S)


def group_table(data, name):
    case = CASE.search(data)
    if case is None:
        raise ValueError(f'{name}: unexpected ROM structure')
    rows = ENTRY.findall(case[2])
    if ENTRY.sub(b'', case[2]).strip() or [int(key, 16) for key, _ in rows] != list(range(256)):
        raise ValueError(f'{name}: expected one entry for each input byte')
    groups = defaultdict(list)
    for key, value in rows:
        groups[value.upper()].append(b"8'h" + key.upper())
    newline = b'\r\n' if b'\r\n' in case[1] else b'\n'
    lines = []
    for value, keys in groups.items():
        chunks = [b", ".join(keys[start:start + 8]) for start in range(0, len(keys), 8)]
        labels = (b"," + newline + b"\t    ").join(chunks)
        lines.append(b"\t    " + labels + b" : Post_Data <= 8'h" + value + b";")
    body = newline.join(lines) + newline
    parsed = GROUP.findall(body)
    expanded = [(int(key, 16), int(value, 16)) for labels, value in parsed
                for key in re.findall(rb"8'h([\da-f]{2})", labels, re.I)]
    if sorted(expanded) != [(int(key, 16), int(value, 16)) for key, value in rows]:
        raise ValueError(f'{name}: converted ROM differs from the original')
    comment = (b"\t// 2026-11-03: group inputs with identical curve outputs." + newline +
               b"\t// An unmatched X/Z input leaves the registered output unchanged." + newline)
    return data[:case.start()] + comment + case[1] + body + data[case.start(3):]


def main():
    source = FPGA / 'source/cnn'
    baseline = HERE / 'original/cnn'
    changes = []
    for number in range(18, 91):
        name = f'Curve_Gamma_{number}.v'
        path = source / name
        snapshot = baseline / name
        current = path.read_bytes()
        original = snapshot.read_bytes() if snapshot.exists() else current
        replacement = group_table(original, name)
        if current not in (original, replacement):
            raise ValueError(f'{name}: source differs from snapshot and generated replacement')
        changes.append((path, snapshot, original, replacement))
    for path, snapshot, original, replacement in changes:
        if not snapshot.exists():
            snapshot.write_bytes(original)
        if path.read_bytes() != replacement:
            path.write_bytes(replacement)
    print(f'Verified {len(changes) * 256:,} Gamma table entries in {len(changes)} files')


if __name__ == '__main__':
    main()
