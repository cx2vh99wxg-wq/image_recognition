"""Check M and S independently, then their shared feedback contract. No hardware access."""
from __future__ import annotations

import argparse
import csv
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import zlib

COMMON = Path(__file__).resolve().parents[1]
PROJECT = COMMON.parents[1]
S_ROOT = PROJECT / "S" / "steering"
M_ROOT = PROJECT / "M" / "imu"
sys.path.insert(0, str(COMMON))
from build_support import compile_program, run, EXE


def run_python(script: Path, *args: str, quiet: bool = False) -> None:
    result = subprocess.run([sys.executable, str(script), *args], text=True,
                            capture_output=quiet, timeout=120)
    if result.returncode:
        raise RuntimeError(f"{script}: exit {result.returncode}\n"
                           f"{result.stdout or ''}\n{result.stderr or ''}")


def check_isolated_builds(tests: Path) -> None:
    # Temporary copies deliberately omit the opposite board's source directory.
    with tempfile.TemporaryDirectory(prefix="isolation_", dir=tests) as directory:
        isolated = Path(directory).resolve()
        if not isolated.is_relative_to(tests.resolve()):
            raise RuntimeError("Isolation directory outside the intended test workspace")
        ignore = shutil.ignore_patterns("build", "logs", "__pycache__")
        for role, source, suffix in [("M", M_ROOT, "imu"), ("S", S_ROOT, "steering")]:
            project = isolated / role / "udp_rk_rk_yolo"
            shutil.copytree(COMMON, project / "common" / "steering", ignore=ignore)
            destination = project / role / suffix
            shutil.copytree(source, destination, ignore=ignore)
            opposite = "S" if role == "M" else "M"
            assert not (project / opposite).exists()
            run_python(destination / "tools" / "build.py", "--test", quiet=True)
            print(f"PASS isolation: {role} builds/tests without any {opposite} directory", flush=True)


def check_cli(tests: Path, contract_test: Path) -> None:
    actuator = S_ROOT / "actuator" / "build" / f"steering_step{EXE}"
    imu = {name: M_ROOT / "build" / f"{name}{EXE}"
           for name in ["imu_dump", "angle_monitor", "feedback_inspect"]}
    with tempfile.TemporaryDirectory(prefix="cli_", dir=tests) as directory:
        work = Path(directory)
        fixture = work / "feedback.bin"
        print(run(contract_test, [str(fixture)]).stdout.strip())
        prefix = struct.pack(">4sHHIIIiiIQI", b"STF1", 1, 48, 0x01020304, 7, 7,
                             -12345, -5000, 12, 123456789, 0)
        assert fixture.read_bytes() == prefix + struct.pack(">I", zlib.crc32(prefix))
        assert "angle_deg=-12.345" in run(imu["feedback_inspect"], ["--file", str(fixture)]).stdout
        report = work / "actuator.csv"
        result = run(actuator, ["--direction", "left", "--duration-ms", "3", "--repeat", "2",
                               "--gap-ms", "1", "--csv", str(report)])
        assert "DRY RUN" in result.stdout and result.stdout.count("COMMAND_COMPLETED") == 2
        with report.open(newline="") as stream:
            rows = list(csv.DictReader(stream))
        assert len(rows) == 2 and all(r["backend"] == "dry-run" for r in rows)
        assert all(float(r["software_elapsed_ms"]) >= 3 for r in rows)
        saved = report.read_bytes()
        run(actuator, ["--direction", "left", "--duration-ms", "3", "--csv", str(report)], 1)
        assert report.read_bytes() == saved
        for bad in [["--direction", "left", "--duration-ms", "0"],
                    ["--direction", "left", "--duration-ms", "201"],
                    ["--direction", "right", "--duration-ms", "nan"],
                    ["--direction", "release", "--duration-ms", "1"],
                    ["--direction", "left", "--duration-ms", "3", "--repeat", "0"]]:
            run(actuator, bad, 2)
        run(actuator, ["--backend", "fspi", "--direction", "left", "--duration-ms", "3"], 1)
        run(actuator, ["--direction", "release"])
        assert "SIMULATED" in run(imu["imu_dump"], ["--samples", "3", "--interval-ms", "1"]).stdout
        angle_csv = work / "imu.csv"
        run(imu["angle_monitor"], ["--samples", "4", "--interval-ms", "10",
            "--mock-rate-dps", "30", "--csv", str(angle_csv)])
        with angle_csv.open(newline="") as stream:
            rows = list(csv.DictReader(stream))
        assert len(rows) == 4 and all(r["zero_valid"] == "0" for r in rows)
        dt = (int(rows[-1]["sample_time_ns"]) - int(rows[0]["sample_time_ns"])) / 1e9
        assert abs(float(rows[-1]["relative_deg"]) - 30 * dt) < 1e-4
        assert "NOT_IMPLEMENTED_OR_NOT_READY" in run(
            imu["imu_dump"], ["--backend", "fpga", "--samples", "1"], 1).stderr
        run(imu["angle_monitor"], ["--mock-rate-dps", "nan"], 2)
    print("PASS CLI: simulation, pulse logs, bounds, no overwrite, sample timestamps and hardware stubs")


def main() -> int:
    argparse.ArgumentParser(description=__doc__).parse_args()
    try:
        run_python(S_ROOT / "tools" / "build.py", "--test")
        run_python(M_ROOT / "tools" / "build.py", "--test")
        tests = COMMON / "build" / "tests"
        contract_test = compile_program(tests / "test_feedback_wire",
            [COMMON / "contracts" / "steering_feedback_wire.c", COMMON / "tests" / "test_feedback_wire.c"],
            [COMMON / "contracts"])
        check_cli(tests, contract_test)
        check_isolated_builds(tests)
        print("ALL CHECKS PASSED. No device access, FPGA programming or live UDP feedback performed.")
        return 0
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired, RuntimeError, AssertionError, OSError) as error:
        print(f"FAILED: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
