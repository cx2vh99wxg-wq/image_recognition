"""Build S actuator/UDP monitor or test S logic. No M sources are required."""
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
    goal_sources=[ROOT/'controller/steering_response.c',ROOT/'tools/actuator/steering_goal.c']
    if hardware:
        goal_sources += [ROOT/'controller'/n for n in ['steering_controller.c','steering_keepalive.c','steering_runtime.c']]
        goal_sources += [ROOT/'actuator/steering_actuator.c',ROOT/'feedback/steering_feedback.c',ROOT/'feedback/feedback_tracker.c',COMMON/'contracts/steering_feedback_wire.c',ROOT.parent/'fspi/csrc/fspi_module.c']
    compile_program(ROOT/'actuator/build'/('steering_goal_fspi' if hardware else 'steering_goal'),goal_sources,INCLUDES,extra)
    lane_sources=[s for s in goal_sources if s.name!='steering_goal.c']+[ROOT/'tools/actuator/steering_lane_goal.c']
    compile_program(ROOT/'actuator/build'/('steering_lane_goal_fspi' if hardware else 'steering_lane_goal'),lane_sources,INCLUDES,extra)
    action_sources=[ROOT/'actuator/steering_sequence.c',ROOT/'actuator/steering_actuator.c',ROOT/'tools/actuator/steering_action.c']
    if hardware:action_sources += [ROOT/'feedback/steering_feedback.c',ROOT/'feedback/feedback_tracker.c',COMMON/'contracts/steering_feedback_wire.c',fspi/'csrc/fspi_module.c']
    compile_program(ROOT/'actuator/build'/('steering_action_fspi' if hardware else 'steering_action'),action_sources,INCLUDES,extra)
    return program


def check_s() -> None:
    tests = ROOT / "build" / "tests"
    actuator = compile_program(tests / "test_actuator", [ROOT / "actuator" / "steering_actuator.c",
        ROOT / "tests" / "actuator" / "test_actuator.c"], INCLUDES)
    controller = compile_program(tests / "test_controller", [ROOT / "controller" / "steering_controller.c",
        ROOT / "controller" / "steering_response.c", ROOT / "controller" / "steering_keepalive.c",
        ROOT / "actuator" / "steering_actuator.c", ROOT / "feedback" / "steering_feedback.c",
        ROOT / "feedback" / "feedback_tracker.c", COMMON / "contracts" / "steering_feedback_wire.c",
        ROOT / "tests" / "controller" / "test_controller.c"], INCLUDES)
    print(run(actuator, []).stdout.strip())
    sequence=compile_program(tests/'test_sequence',[ROOT/'actuator/steering_sequence.c',ROOT/'actuator/steering_actuator.c',ROOT/'tests/actuator/test_sequence.c'],INCLUDES)
    print(run(sequence,[]).stdout.strip())
    predictive=compile_program(tests/'test_predictive',[ROOT/'tests/actuator/test_predictive.c'],INCLUDES)
    print(run(predictive,[]).stdout.strip())
    action=ROOT/'actuator/build'/('steering_action.exe' if os.name=='nt' else 'steering_action')
    assert 'no_device_access=1' in run(action,['--direction','left','--first-us','30000','--brake-us','17500','--gap-us','2000']).stdout
    run(action,['--direction','right','--first-us','60001'],2)
    run(action,['--direction','right','--brake-us','10000','--gap-us','1999'],2)
    assert 'no_device_access=1' in run(action,['--direction','left','--max-phase-ms','120','--first-us','100000']).stdout
    run(action,['--direction','left','--max-phase-ms','251','--first-us','100000'],2)
    run(action,['--direction','left','--max-phase-ms','120','--first-us','100000','--brake-us','10000'],2)
    print(run(controller, []).stdout.strip())
    partition=compile_program(tests/'test_response_partition',[ROOT/'controller/steering_response.c',ROOT/'tests/controller/test_response_partition.c'],INCLUDES)
    print(run(partition,[str(tests/'partition_fixture.table')]).stdout.strip())
    receiver=compile_program(tests/'test_live_receiver',[ROOT/'feedback/steering_feedback.c',ROOT/'feedback/feedback_tracker.c',
        COMMON/'contracts/steering_feedback_wire.c',ROOT/'tests/feedback/test_live_receiver.c'],INCLUDES)
    print(run(receiver,[]).stdout.strip())
    if sys.platform.startswith('linux'):
        compile_program(tests/'test_runtime_integration',[ROOT/'controller'/n for n in
            ['steering_controller.c','steering_response.c','steering_keepalive.c','steering_runtime.c']]+
            [ROOT/'actuator/steering_actuator.c',ROOT/'feedback/steering_feedback.c',ROOT/'feedback/feedback_tracker.c',
             COMMON/'contracts/steering_feedback_wire.c',ROOT/'tests/controller/test_runtime_integration.c'],INCLUDES,
             [f'-I{ROOT.parent / "fspi/include"}',f'-I{ROOT.parent / "include"}'])
    tracker = compile_program(tests / "test_feedback_tracker",
        [ROOT / "feedback" / "feedback_tracker.c", ROOT / "tests" / "feedback" / "test_tracker.c",
         COMMON / "contracts" / "steering_feedback_wire.c"], INCLUDES)
    print(run(tracker, []).stdout.strip())
    cli = ROOT / "actuator" / "build" / ("steering_step.exe" if os.name == "nt" else "steering_step")
    fractional = run(cli, ["--direction", "left", "--duration-us", "17500"])
    if "requested_ms=17.500" not in fractional.stdout:
        raise AssertionError("Fractional request was truncated")
    run(cli, ["--direction", "left", "--duration-ms", "17", "--duration-us", "500"], 2)
    run(cli, ["--backend", "fspi", "--direction", "left", "--duration-us", "20001",
              "--max-pulse-ms", "20"], 2)
    print("PASS CLI: microsecond input, fractional output, exclusive units and pre-open bound")
    for name in ["test_bench.py", "test_response_model.py", "test_partition_response.py", "test_target.py", "test_auto_calibration.py", "test_confirm_center.py", "test_terminal_center.py", "test_homing.py"]:
        result = subprocess.run([sys.executable, str(ROOT / "tests" / "feedback" / name)],
                                text=True, capture_output=True, timeout=30)
        if result.returncode:
            raise AssertionError(f"{name} failed:\n{result.stdout}\n{result.stderr}")
    print("PASS bench feedback/reference guards, empirical response limits and target orchestration")
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
        # 注入迟醒时，旧的完整截止休眠会超出原有 +2ms 验收窗；
        # 受保护的休眠方式仍须按时释放。
        # 两套实现都使用模拟触点，不打开真实设备。
        import csv
        import tempfile
        with tempfile.TemporaryDirectory(prefix='release_wake_',dir=tests) as folder:
            folder=Path(folder)
            source=(ROOT/'tools/actuator/steering_step.c').read_text()
            old=folder/'baseline_step.c'
            marker='const uint64_t guard = UINT64_C(3000000);'
            if source.count(marker)!=1:raise AssertionError('Wake guard fixture missing')
            old.write_text(source.replace(marker,'const uint64_t guard = UINT64_C(0);'))
            support=[ROOT/'actuator'/n for n in ['steering_actuator.c','backend_dry_run.c','backend_fspi.c']]
            support += [ROOT/'tests/actuator/fspi_trace_stub.c',ROOT/'tests/actuator/wakeup_delay_stub.c']
            extra=['-DSTEERING_WITH_FSPI',f'-I{ROOT.parent / "fspi/include"}',f'-I{ROOT.parent / "include"}']
            for label,step in [('baseline',old),('guarded',ROOT/'tools/actuator/steering_step.c')]:
                binary=compile_program(folder/label,[*support,step],INCLUDES,extra)
                for trial in range(3):
                    path=folder/('%s_%d.csv'%(label,trial))
                    run(binary,['--backend','fspi','--direction','left','--duration-us','30000','--csv',str(path)])
                    with path.open() as f:row=next(csv.DictReader(f))
                    elapsed=(int(row['release_end_ns'])-int(row['enable_end_ns']))/1e6
                    if (label=='baseline' and elapsed<=32) or (label=='guarded' and not 29.95<=elapsed<=32):
                        raise AssertionError('Unexpected wake regression %s %.6fms'%(label,elapsed))
        print('PASS native CLI: injected 2ms late wake reproduced; original timing tolerance retained')


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--hardware", action="store_true", help="Compile Linux FSPI tool; never run it")
    parser.add_argument("--test", action="store_true", help="Build mock tool and test S-side logic")
    parser.add_argument("--monitor", action="store_true", help="Build Linux UDP angle monitor; no SPI access")
    args = parser.parse_args()
    if sum([args.hardware, args.test, args.monitor]) > 1:
        parser.error("Choose hardware, software tests, or the UDP monitor separately.")
    try:
        if args.monitor:
            if not sys.platform.startswith("linux"):
                raise RuntimeError("UDP monitor requires Linux/POSIX; tracker tests also run on Windows.")
            compile_program(ROOT / "feedback" / "build" / "steering_feedback_monitor",
                [ROOT / "tools" / "feedback" / "steering_feedback_monitor.c",
                 ROOT / "feedback" / "feedback_tracker.c", COMMON / "contracts" / "steering_feedback_wire.c"],
                INCLUDES)
            return 0
        build_actuator(args.hardware)
        if args.test:
            check_s()
        return 0
    except (subprocess.CalledProcessError, RuntimeError, AssertionError, OSError) as error:
        print(f"FAILED: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
