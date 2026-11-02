from collections import defaultdict
import json
from pathlib import Path

out = Path(__file__).resolve().parent
inventory = json.loads((out / 'inventory_before.json').read_text(encoding='utf-8'))
groups = defaultdict(lambda: [0, 0])
for item in inventory['files']:
    path = Path(item['path'])
    if item['path'].startswith('.git/'):
        continue
    key = (path.parts[0], path.suffix.lower() or '(none)')
    groups[key][0] += item['size']
    groups[key][1] += 1
for key, value in sorted(groups.items(), key=lambda p: -p[1][0])[:35]:
    print('%.2f MiB %u files %s %s' % (value[0]/2**20, value[1], *key))
print('FPGA build directory totals')
for item in inventory['directories']:
    if item['path'].startswith('fpga/') and item['path'].count('/') == 3:
        print('%.2f MiB %s' % (item['size']/2**20, item['path']))
print('PDS input files and includes before first compile task')
import re
root = Path(inventory['root'])
for name in ['project/project.pds','project_m_imu/project_m_imu.pds','project_m_imu_nack_20261101/project_m_imu.pds']:
    path = root / 'fpga/fpga_pcie_ov5640' / name
    text = path.read_text(encoding='utf-8', errors='replace').split('(_task tsk_compile')[0]
    references = re.findall(r'\(_file\s+"([^"]+)"', text)
    print(name, len(references))
    for ref in references:
        if '/source/' not in ref:
            print('  ', ref)
