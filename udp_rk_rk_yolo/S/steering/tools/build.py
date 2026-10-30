"""Build A's S-side actuator, or test S-side logic. No M sources are required."""
from __future__ import annotations

import argparse
import os
import signal
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
    program = compile_program(ROOT / "actuator" / "build" / name, sources, INCLUDES, extra)
    if hardware:
        compile_program(ROOT / "actuator" / "build" / "steering_status",
                        [ROOT / "tools" / "actuator" / "steering_status.c",
                         fspi / "csrc" / "fspi_module.c"], INCLUDES, extra)
    return program


def check_s() -> None:
    tests = ROOT / "build" / "tests"
    actuator = compile_program(tests / "test_actuator", [ROOT / "actuator" / "steering_actuator.c",
        ROOT / "tests" / "actuator" / "test_actuator.c"], INCLUDES)
    controller = compile_program(tests / "test_controller", [ROOT / "controller" / "steering_controller.c",
        ROOT / "feedback" / "steering_feedback.c", ROOT / "tests" / "controller" / "test_controller.c"], INCLUDES)
    print(run(actuator, []).stdout.strip())
    print(run(controller, []).stdout.strip())
    cli = ROOT / "actuator" / "build" / ("steering_step.exe" if os.name == "nt" else "steering_step")
    fractional = run(cli, ["--direction", "left", "--duration-us", "17500"])
    if "requested_ms=17.500" not in fractional.stdout:
        raise AssertionError("Fractional request was truncated")
    run(cli, ["--direction", "left", "--duration-ms", "17", "--duration-us", "500"], 2)
    run(cli, ["--backend", "fspi", "--direction", "left", "--duration-us", "20001",
              "--max-pulse-ms", "20"], 2)
    print("PASS CLI: microsecond input, fractional output, exclusive units and pre-open bound")
    if sys.platform.startswith("linux"):
        fspi = ROOT.parent / "fspi"
        trace = compile_program(tests / "test_trace_cli",
            [*(ROOT / "actuator" / name for name in
               ["steering_actuator.c", "backend_dry_run.c", "backend_fspi.c"]),
             ROOT / "tools" / "actuator" / "steering_step.c",
             ROOT / "tests" / "actuator" / "fspi_trace_stub.c"], INCLUDES,
            ["-DSTEERING_WITH_FSPI", f"-I{fspi / 'include'}", f"-I{ROOT.parent / 'include'}"])
        for case in ["normal", "blocked", "contact-mismatch", "read-fault"]:
            result = subprocess.run([str(trace), "--backend", "fspi", "--direction", "left",
                                     "--duration-ms", "2"], text=True, capture_output=True,
                                    env=dict(os.environ, STEER_TRACE_TEST=case), timeout=10)
            if result.returncode != (0 if case == "normal" else 1) or "STUB_RELEASED" not in result.stdout:
                raise AssertionError(f"Trace fault case {case}: {result.returncode}\n{result.stdout}\n{result.stderr}")
            if case == "normal" and "requested_contact_low=YES" not in result.stdout:
                raise AssertionError("Normal contact was not verified")
        print("PASS native CLI: active contact, STOP block, contact mismatch, read fault; both releases verified")
        result = subprocess.run([str(trace), "--backend", "fspi", "--direction", "right",
                                 "--duration-us", "17500", "--max-pulse-ms", "20"],
                                text=True, capture_output=True,
                                env=dict(os.environ, STEER_TRACE_TEST="normal"), timeout=10)
        if result.returncode or "STUB_RELEASED" not in result.stdout or "requested_ms=17.500" not in result.stdout:
            raise AssertionError(f"Fractional native request failed: {result.stdout}\n{result.stderr}")
        process = subprocess.Popen([str(trace), "--backend", "fspi", "--direction", "left",
                                    "--duration-ms", "100"], text=True,
                                   stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                   env=dict(os.environ, STEER_TRACE_TEST="normal"))
        try:
            first = process.stdout.readline()
            if "requested_contact_low=YES" not in first:
                raise AssertionError(f"No active contact before cancellation: {first}")
            process.send_signal(signal.SIGTERM)
            output, error = process.communicate(timeout=3)
            if process.returncode != 130 or "STUB_RELEASED" not in output or "CANCELLED" not in output:
                raise AssertionError(f"Interrupted deadline failed to release: {process.returncode}\n{output}\n{error}")
        finally:
            if process.poll() is None:
                process.kill(); process.wait()
        print("PASS native CLI: SIGTERM interrupts deadline sleep and releases both contacts")


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
