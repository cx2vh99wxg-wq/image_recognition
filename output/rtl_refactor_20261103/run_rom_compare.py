"""Compare each refactored Gamma curve and the OV5640 register stream."""
import os
from pathlib import Path
import re
import subprocess
import tempfile

from build_pds import FPGA, HERE


def main():
    source = FPGA / 'source'
    baseline = HERE / 'deep_before'
    curves = [Path(f'cnn/Curve_Gamma_{number}.v') for number in range(18, 91)]
    paths = [*curves, Path('ov5640/rtl/reg_config.v')]
    reference = []
    for path in paths:
        name = path.stem.encode()
        raw = (baseline / path).read_bytes()
        raw, count = re.subn(rb'(\bmodule\s+)' + name + rb'\b', rb'\g<1>' + name + b'_old', raw)
        assert count == 1, path
        reference.append(raw)
    log_dir = Path(os.environ.get('SIM_RESULTS_DIR', HERE / 'results_deep_rom'))
    log_dir.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='pineapple-rom-') as tmp:
        work = Path(tmp)
        old = work / 'original.v'
        old.write_bytes(b'\n'.join(reference))
        bench = work / 'compare_rom.sv'
        inst = []
        for number, path in enumerate(curves):
            inst += [
                f'{path.stem}_old old_{number}(clk, pixel, old_curves[{number}]);',
                f'{path.stem} new_{number}(clk, pixel, new_curves[{number}]);',
            ]
        text = (HERE / 'tests' / 'rom_compare.sv.in').read_text(encoding='utf-8')
        bench.write_text(text.replace('@CURVE_INSTANCES@', '\n'.join(inst)), encoding='utf-8')
        with (log_dir / 'rom_tables.build.log').open('wb') as log:
            subprocess.run(['verilator', '--binary', '--timing', '-Wno-fatal', '-j', '3',
                            '--top-module', 'compare_rom', '--Mdir', str(work / 'obj'),
                            str(bench), str(old), str(source / 'ov5640/rtl/i2c_com.v'),
                            *(str(source / path) for path in paths)],
                           stdout=log, stderr=subprocess.STDOUT, check=True)
        result = subprocess.run([str(work / 'obj/Vcompare_rom')], capture_output=True, timeout=120)
        (log_dir / 'rom_tables.run.log').write_bytes(result.stdout + result.stderr)
        print(result.stdout.decode(errors='replace'), end='')
        result.check_returncode()


if __name__ == '__main__':
    main()
