"""Report routed clock-pair timing against the exact PDS release baseline."""
import json
from pathlib import Path
import re
import sys

from build_pds import FPGA, HERE, TOPS, WORK

SECTIONS = [
    ('Setup_Slow', 'Setup Summary(Slow Corner):'),
    ('Hold_Slow', 'Hold Summary(Slow Corner):'),
    ('Setup_Fast', 'Setup Summary(Fast Corner):'),
    ('Hold_Fast', 'Hold Summary(Fast Corner):')
]
ROW = re.compile(r'(\S+)\s+(\S+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+(\d+)\s+(\d+)')


def read_report(project, top):
    text = (project/'report_timing'/f'{top}.rtr').read_text(errors='replace')
    result = {}
    for name, label in SECTIONS:
        table = text.split(label)[1].split('===')[0]
        result[name] = {(a,b): float(slack) for a,b,slack,*_ in ROW.findall(table)}
        assert result[name], (project, name)
    return result


result = {}
for board, top in TOPS.items():
    project = f'project_{board.lower()}_application_refactor_20261102'
    old = read_report(FPGA/project, top)
    new = read_report(WORK/project, top)
    result[board] = {}
    for name, _ in SECTIONS:
        negative = [[*pair, slack] for pair, slack in new[name].items() if slack < 0]
        introduced = [row for row in negative if old[name].get(tuple(row[:2]), 0) >= 0]
        fspi = [pair for pair in new[name] if pair[0] == pair[1] and
                'u_fspi_slave_top/clk_gen_inst' in pair[0]]
        assert len(fspi) == 1, (board, name, fspi)
        result[board][name] = {
            'clock_pairs': len(new[name]), 'worst_slack_ns': min(new[name].values()),
            'baseline_worst_slack_ns': min(old[name].values()),
            'fspi_self_slack_ns': new[name][fspi[0]],
            'baseline_fspi_self_slack_ns': old[name].get(fspi[0]),
            'negative_pairs': negative, 'new_negative_pairs': introduced
        }
output = Path(sys.argv[1]) if len(sys.argv) > 1 else HERE/'timing_comparison.json'
output.write_text(json.dumps(result, indent=2), encoding='utf-8')
print(json.dumps(result, indent=2))
