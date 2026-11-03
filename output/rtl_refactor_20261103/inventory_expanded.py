"""Inventory the expanded refactor by actual M/S PDS source paths."""
from collections import Counter
from hashlib import sha256
import json
from pathlib import Path

from build_pds import FPGA, HERE

UPSTREAM = {
    'isp/ISP/color_display.v': 'Copyright header, 正点原子',
    'isp/ISP/color_projection.v': 'Copyright header, 正点原子',
    'isp/image_gaussian.v': 'External author header',
    'isp/shape/shape_display.v': 'Copyright header, 正点原子',
    'isp/shape/shape_projection.v': 'Copyright header, 正点原子',
    'isp/vip/binarization.v': 'Copyright header, 正点原子',
    'isp/vip/digital_recognition.v': 'Copyright header, 正点原子',
    'isp/vip/myram.v': 'Copyright header, 正点原子',
    'isp/vip/projection.v': 'Copyright header, 正点原子',
    'isp/vip/vip.v': 'Copyright header, 正点原子',
    'ov5640/rtl/i2c_com.v': 'Meyesemi, Will',
    'ov5640/rtl/power_on_delay.v': 'Meyesemi, Will',
    'uart_lcd/uart_loopback.v': 'Copyright header, 正点原子',
    'vga_ctrl.v': 'Author header, EmbedFire',
}


def digest(path):
    return sha256(path.read_bytes()).hexdigest()


def main():
    manifest = json.loads((HERE / 'pds_inputs.json').read_text(encoding='utf-8'))
    first_batch = json.loads((HERE / 'replacement_hashes.json').read_text(encoding='utf-8'))
    rows = {}
    for board, inputs in manifest.items():
        for entry in inputs:
            path = entry['path']
            if path.startswith('source/'):
                rows.setdefault(path, {'boards': []})['boards'].append(board)
    for path, row in rows.items():
        relative = Path(path).relative_to('source').as_posix()
        snapshot = HERE / 'original' / relative
        row['current_sha256'] = digest(FPGA / path)
        if snapshot.exists():
            row['original_sha256'] = digest(snapshot)
            if row['current_sha256'] == first_batch.get(relative, {}).get('replacement'):
                row['state'] = 'phase_one_validated'
            elif row['current_sha256'] != row['original_sha256']:
                row['state'] = 'expanded_refactored'
            else:
                row['state'] = 'pending_structure_review'
        elif path.startswith('source/pcie/'):
            row['state'] = 'retained_vendor_pcie_support'
        elif relative in UPSTREAM:
            row['state'] = 'retained_attributed_upstream_rtl'
            row['provenance'] = UPSTREAM[relative]
        elif relative == 'HDMI/lcd_para.v':
            row['state'] = 'retained_display_configuration'
            row['provenance'] = 'Display timing and color macro definitions'
        elif relative == 'isp/fps/fps_display.v':
            row['state'] = 'preexisting_workspace_refactor_pending_review'
        else:
            row['state'] = 'pending_ownership_and_structure_review'
    counts = Counter(row['state'] for row in rows.values())
    report = {'counts': counts, 'files': dict(sorted(rows.items()))}
    (HERE / 'expanded_source_inventory.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(counts, indent=2))


if __name__ == '__main__':
    main()
