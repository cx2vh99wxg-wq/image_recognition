#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/../.."
root="$PWD"
work="$root/output/rtl_refactor_20261103"
source="${RTL_SOURCE_DIR:-$root/fpga/fpga_pcie_ov5640/source}"
phase="${1:?baseline, revised or compare}"
case "$phase" in baseline|revised|compare) ;; *) exit 2 ;; esac
results="${SIM_RESULTS_DIR:-$work/results}/$phase"
mkdir -p "$results"
if [[ -z "${ONLY_TEST:-}" ]]; then
    : >"$results/summary.log"
fi
inputs=("$source"/fspi/*.v "$source"/uart_lcd/*.v "$source/remote_vehicle_control.v")
if [[ "$phase" == baseline ]]; then
    inputs=("$work/original/fspi/spi_ram_crtl.v" "$work/original/fspi/fspi_slave.v"
            "$work/original/fspi/fspi_slave_top.v" "$work/original/uart_lcd/uart_lcd.v"
            "$source/uart_lcd/uart_loopback.v" "$work/original/uart_lcd/uart_rx.v" "$work/original/uart_lcd/uart_tx.v"
            "$work/original/remote_vehicle_control.v")
fi
tests=("$root"/fpga/fpga_pcie_ov5640/sim/test_*.v)
if [[ "$phase" == compare ]]; then
    tests=("$work"/tests/compare_*.sv)
    inputs+=("$source/ov5640/rtl/cmos_capture_data.v" "$source/cnn/gamma_judge.v"
             "$source/mpu9250/fspi_slave_top_m_imu.v" "$source/mpu9250/fspi_slave_m_imu.v"
             "$source"/cnn/Curve_Gamma_*.v)
    for reference in "$work"/reference/*.v; do
        if [[ "$reference" != "$work/reference/gamma_judge.v" ]]; then
            inputs+=("$reference")
        fi
    done
fi
failed=0
ran=0
for test in "${tests[@]}"; do
    name="$(basename "$test")"
    name="${name%.*}"
    if [[ -n "${ONLY_TEST:-}" && "$name" != "$ONLY_TEST" ]]; then continue; fi
    ran=1
    # Build on the Linux filesystem to avoid slow tool output on the Windows mount.
    build="$(mktemp -d "/tmp/pineapple-${phase}-${name}-XXXXXX")"
    test_inputs=("${inputs[@]}")
    if [[ "$name" == compare_gamma ]]; then
        gamma_reference="$build/gamma_reference.v"
        for original in "$work/reference/gamma_judge.v" "$work"/original/cnn/Curve_Gamma_*.v; do
            sed -E 's/\b(Curve_Gamma_[0-9]+)\b/\1_reference/g' "$original"
            printf '\n'
        done >"$gamma_reference"
        test_inputs+=("$gamma_reference")
    fi
    if verilator --binary --timing -Wno-fatal -j 3 --Mdir "$build" --top-module "$name" \
        "$test" "${test_inputs[@]}" >"$results/$name.build.log" 2>&1; then
        if timeout "${SIM_TIMEOUT:-180}" "$build/V$name" >"$results/$name.run.log" 2>&1; then
            if grep -q 'FAIL' "$results/$name.run.log" || ! grep -q 'PASS' "$results/$name.run.log"; then
                status=RUN_FAIL
            else
                status=PASS
            fi
        else
            code=$?
            if [[ "$code" == 124 ]]; then status=TIMEOUT; else status="RUN_FAIL_$code"; fi
        fi
    else
        status=BUILD_FAIL
    fi
    printf '%s %s\n' "$name" "$status" | tee -a "$results/summary.log"
    if [[ "$status" != PASS ]]; then failed=1; fi
done
if [[ "$ran" == 0 ]]; then exit 2; fi
exit "$failed"
