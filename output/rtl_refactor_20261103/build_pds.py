"""Run the two vendor flows in isolated copies of the selected projects."""
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
FPGA = ROOT / 'fpga/fpga_pcie_ov5640'
WORK = HERE / os.environ.get('PINEAPPLE_PDS_OUTPUT', 'pds')
SEED = int(os.environ.get('PINEAPPLE_PNR_SEED', '3'))
PDS = Path('D:/PDS/Installation/PDS_2022.2-SP6.4/bin/pds_shell.exe')
TOPS = {'S': 'image_pcie_capture', 'M': 'image_pcie_capture_m_imu'}
OUTPUT_DIRS = {'compile', 'synthesize', 'device_map', 'place_route', 'report_timing',
               'report_power', 'generate_bitstream', 'generate_netlist',
               'constraint_check', 'log', 'logbackup'}


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def stage(board):
    name = f'project_{board.lower()}_application_refactor_20261102'
    entry = FPGA / name / f'project_{board.lower()}_refactor.pds'
    text = entry.read_text(encoding='utf-8-sig')
    inputs = []
    for ref in re.findall(r'\(_(?:file|ip|ip_source_item)\s+"([^"]+)"', text):
        if Path(ref).parts[0] in OUTPUT_DIRS:
            continue
        path = (entry.parent / ref).resolve()
        assert path.is_relative_to(FPGA.resolve()), path
        assert path.is_file(), path
        inputs.append({'path': path.relative_to(FPGA).as_posix(), 'sha256': digest(path)})
    assert any(Path(r['path']).name == 'gamma_judge.v' for r in inputs)
    directory = WORK / name
    if not directory.exists():
        directory.mkdir()
        shutil.copytree(entry.parent/'ipcore', directory/'ipcore')
        for path in entry.parent.iterdir():
            if path.is_file() and path.suffix.lower() in {'.fdc','.pcf','.sdc','.pds','.wf'}:
                shutil.copy2(path, directory/path.name)
    for row in inputs:
        assert digest(WORK/row['path']) == row['sha256'], row
    return directory/entry.name, inputs


def build(board, entry):
    top = TOPS[board]
    commands = [
        ('compile', f'compile -force_to_run -top_module {top}'),
        ('synthesize', 'synthesize -ads -force_to_run -selected_syn_tool_opt 2'),
        ('device_map', 'dev_map -force_to_run'),
        ('place_route', f'pnr -force_to_run -gplace_seed {SEED} -groute_seed {SEED} '
                        '-mode normal -optimize_multi_corner_timing -fix_hold_violation -slack_weight 90'),
        ('timing', 'report_timing -force_to_run')
    ]
    tcl = WORK/f'build_{board}.tcl'
    tcl.write_text(f'open_project "{entry.as_posix()}"\n' + ''.join(
        f'puts "PINEAPPLE_BEGIN {name}"\n{command}\nputs "PINEAPPLE_END {name}"\n'
        for name, command in commands), encoding='utf-8')
    log = WORK/f'build_{board}.log'
    try:
        with log.open('wb') as out:
            proc = subprocess.run([str(PDS), '-file', str(tcl)], cwd=entry.parent,
                                  stdout=out, stderr=subprocess.STDOUT,
                                  creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    except OSError as exc:
        return {'board': board, 'completed': [], 'launch_error': str(exc)}
    data = log.read_text(encoding='utf-8', errors='replace')
    return {'board': board, 'seed': SEED, 'exit_code': proc.returncode,
            'completed': [name for name, _ in commands if f'PINEAPPLE_END {name}' in data],
            'log': str(log)}


if __name__ == '__main__':
    WORK.mkdir(exist_ok=True)
    if not (WORK/'source').exists():
        shutil.copytree(FPGA/'source', WORK/'source')
    staged = {board: stage(board) for board in TOPS}
    (HERE/'pds_inputs.json').write_text(json.dumps(
        {board: paths for board, (_, paths) in staged.items()}, indent=2), encoding='utf-8')
    print('PDS staged', {b: len(p) for b, (_,p) in staged.items()}, flush=True)
    with ThreadPoolExecutor(max_workers=2) as pool:
        futures = [pool.submit(build, board, entry) for board, (entry, _) in staged.items()]
        results = [f.result() for f in futures]
    (WORK/'results.json').write_text(json.dumps(results, indent=2), encoding='utf-8')
    print(results, flush=True)
    if any(r.get('exit_code') != 0 or len(r['completed']) != 5 for r in results):
        raise SystemExit(1)
