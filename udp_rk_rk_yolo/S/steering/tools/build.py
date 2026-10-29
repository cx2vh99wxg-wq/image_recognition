"""Build A's S-side actuator, or test S-side logic. No M sources are required."""
from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
COMMON = ROOT.parents[1] / "common" / "steering"
sys.path.insert(0, str(COMMON))
from build_support import compile_program, run

INCLUDES = [COMMON / "contracts", ROOT / "actuator", ROOT / "feedback", ROOT / "controller"]


def build_actuator(hardware: bool = False) -> Path:
    sources = [ROOT / "actuator" / name for name in
               ["steering_actuator.c", "backend_dry_run.c", "backend_fspi.c"]]
    sources.append(ROOT / "tools" / "actuator" / "steering_step.c")
    extra = []
    if hardware:
        if not sys.platform.startswith("linux"):
            raise RuntimeError("Hardware build requires Linux/RK3568; dry-run builds on Windows.")
        fspi = ROOT.parent / "fspi"
        sources.append(fspi / "csrc" / "fspi_module.c")
        extra = ["-DSTEERING_WITH_FSPI", f"-I{fspi / 'include'}",
                 f"-I{ROOT.parent / 'include'}", "-Wno-error"]
    name = "steering_step_fspi" if hardware else "steering_step"
    return compile_program(ROOT / "actuator" / "build" / name, sources, INCLUDES, extra)


def check_s() -> None:
    tests = ROOT / "build" / "tests"
    actuator = compile_program(tests / "test_actuator", [ROOT / "actuator" / "steering_actuator.c",
        ROOT / "tests" / "actuator" / "test_actuator.c"], INCLUDES)
    controller = compile_program(tests / "test_controller", [ROOT / "controller" / "steering_controller.c",
        ROOT / "feedback" / "steering_feedback.c", ROOT / "tests" / "controller" / "test_controller.c"], INCLUDES)
    print(run(actuator, []).stdout.strip())
    print(run(controller, []).stdout.strip())


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--hardware", action="store_true", help="Compile Linux FSPI tool; never run it")
    parser.add_argument("--test", action="store_true", help="Build mock tool and test S-side logic")
    args = parser.parse_args()
    if args.hardware and args.test:
        parser.error("Compile hardware separately from software tests.")
    try:
        build_actuator(args.hardware)
        if args.test:
            check_s()
        return 0
    except (subprocess.CalledProcessError, RuntimeError, AssertionError, OSError) as error:
        print(f"FAILED: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
