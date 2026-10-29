"""Build/test B's M-side acquisition and angle tools. No S sources are required."""
from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
COMMON = ROOT.parents[1] / "common" / "steering"
sys.path.insert(0, str(COMMON))
from build_support import compile_program, run

INCLUDES = [COMMON / "contracts", ROOT / "include", ROOT / "tools"]
SOURCES = [ROOT / "csrc" / name for name in
           ["imu_source.c", "steering_angle.c", "mpu9250_transport.c"]]


def build_imu() -> dict[str, Path]:
    outputs = {}
    for name in ["imu_dump", "angle_monitor"]:
        outputs[name] = compile_program(ROOT / "build" / name,
            [*SOURCES, ROOT / "tools" / "imu_tool.c", ROOT / "tools" / f"{name}.c"], INCLUDES)
    outputs["feedback_inspect"] = compile_program(ROOT / "build" / "feedback_inspect",
        [COMMON / "contracts" / "steering_feedback_wire.c", ROOT / "tools" / "feedback_inspect.c"], INCLUDES)
    return outputs


def check_m() -> None:
    test = compile_program(ROOT / "build" / "test_imu", [*SOURCES, ROOT / "tests" / "test_imu.c"], INCLUDES)
    print(run(test, []).stdout.strip())


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--test", action="store_true", help="Build mock tools and test M-side logic")
    args = parser.parse_args()
    try:
        build_imu()
        if args.test:
            check_m()
        return 0
    except (subprocess.CalledProcessError, RuntimeError, AssertionError, OSError) as error:
        print(f"FAILED: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
