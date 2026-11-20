"""Fit a new V2 table from repeated, measured single-pulse bench trials."""

import argparse
import csv
import hashlib
import json
import math
import statistics
from pathlib import Path

from export_runtime import encode_partition_cells


WIDTHS_US = (19000, 22000, 25000, 30000, 40000, 50000, 60000, 70000, 80000, 90000)
MAX_FINE_PULSE_US = 60000
DIRECTIONS = {"left": 1, "right": 2}
MOTION_THRESHOLD_DEG = 0.15


def read_trial(row, widths):
    direction = DIRECTIONS[row["direction"]]
    width = int(row["request_us"])
    session = int(row["reference_session"])
    zero, actual, before, after, delta, peak_min, peak_max = (
        float(row[name]) for name in (
            "zero_raw_deg", "actual_us", "before_deg", "after_deg", "delta_deg",
            "peak_min_deg", "peak_max_deg"
        )
    )
    if not 0 < session <= 0xFFFFFFFF or not all(
        math.isfinite(v) for v in (zero, actual, before, after, delta, peak_min, peak_max)
    ):
        raise ValueError("invalid reference or non-finite angle")
    if width not in widths or not width - 50 <= actual <= width + 2000:
        raise ValueError("pulse timing outside the accepted range")
    if not -2 <= before < 2 or abs(after - before - delta) > 0.002:
        raise ValueError("inconsistent angles or start outside the measured partition")
    if not (
        -18 <= peak_min <= min(before, after) + 0.002
        and max(before, after) - 0.002 <= peak_max <= 18
    ):
        raise ValueError("invalid transient angle envelope")
    signed = delta if direction == 1 else -delta
    if signed < -0.4:
        raise ValueError("motion opposite the commanded direction")
    return (session, zero), (direction, width), (before, delta, signed, peak_min, peak_max)


def response_is_outlier(signed, moving):
    if signed <= MOTION_THRESHOLD_DEG or len(moving) < 5:
        return False
    median = statistics.median(moving)
    mad = statistics.median(abs(value - median) for value in moving)
    return abs(signed - median) > max(0.25, 3.5 * 1.4826 * mad)


def fit(paths, minimum_samples, widths=WIDTHS_US):
    expected = [(direction, width) for width in widths for direction in DIRECTIONS.values()]
    groups = {key: [] for key in expected}
    rejected, sources, seen = [], [], set()
    reference = None
    for path in paths:
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        if digest in seen:
            continue
        seen.add(digest)
        sources.append({"file": str(path), "sha256": digest})
        with path.open(newline="", encoding="utf-8-sig") as stream:
            for number, row in enumerate(csv.DictReader(stream), start=2):
                if row.get("type") != "TEST":
                    continue
                sample_id = {"file": str(path), "row": number}
                try:
                    trial_reference, key, trial = read_trial(row, widths)
                    if reference is None:
                        reference = trial_reference
                    if trial_reference != reference:
                        raise ValueError("different IMU session or mechanical zero")
                    groups[key].append((sample_id, trial))
                except (KeyError, TypeError, ValueError) as error:
                    rejected.append({**sample_id, "reason": str(error)})

    summaries, cells = [], []
    for (direction, width), samples in groups.items():
        moving = [trial[2] for _, trial in samples if trial[2] > MOTION_THRESHOLD_DEG]
        kept = []
        for sample_id, trial in samples:
            if response_is_outlier(trial[2], moving):
                rejected.append({**sample_id, "reason": "response outlier by median absolute deviation"})
            else:
                kept.append(trial)
        n = len(kept)
        moved = sum(trial[2] > MOTION_THRESHOLD_DEG for trial in kept)
        summaries.append({
            "direction": direction,
            "request_us": width,
            "retained_samples": n,
            "moved_samples": moved,
            "mean_turn_deg": statistics.mean(trial[2] for trial in kept) if kept else None,
        })
        if minimum_samples <= n <= 64:
            cells.append({
                "zone_low_deg": -2,
                "zone_high_deg": 2,
                "direction": direction,
                "first_us": width,
                "brake_us": 0,
                "priority": 10,
                "start_min_deg": min(trial[0] for trial in kept),
                "start_max_deg": max(trial[0] for trial in kept),
                "responses_deg": [trial[1] for trial in kept],
                # Preserve transient extrema even for statistically excluded responses.
                "peak_low_deg": min(trial[3] - trial[0] for _, trial in samples),
                "peak_high_deg": max(trial[4] - trial[0] for _, trial in samples),
            })
    coverage_ready = len(cells) == len(expected) and all(
        any(group["direction"] == direction and group["moved_samples"] for group in summaries)
        for direction in DIRECTIONS.values()
    )
    return {
        "reference": reference,
        "sources": sources,
        "groups": summaries,
        "rejected": rejected,
        "coverage_ready": coverage_ready,
    }, cells


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv_files", type=Path, nargs="+")
    parser.add_argument("--output", type=Path, required=True, help="New output directory")
    parser.add_argument("--minimum-samples", type=int, default=8)
    parser.add_argument("--width-ms", type=int, nargs="+", help="Pulse widths measured in this calibration")
    args = parser.parse_args()
    if not 5 <= args.minimum_samples <= 64:
        parser.error("--minimum-samples must be between 5 and 64")
    widths = tuple(sorted(set(ms * 1000 for ms in args.width_ms))) if args.width_ms else WIDTHS_US
    if not widths or widths[0] <= 0 or widths[-1] > 90000:
        parser.error("pulse widths must be within 1–90 ms")
    report, cells = fit(args.csv_files, args.minimum_samples, widths)
    args.output.mkdir()
    (args.output / "pulse_sweep_report.json").write_text(
        json.dumps(report, indent=2) + "\n", encoding="utf-8"
    )
    if not report["coverage_ready"]:
        print("No candidate table: insufficient repeated measurements or no usable response.")
        return 2
    fine_cells = [cell for cell in cells if cell["first_us"] <= MAX_FINE_PULSE_US]
    if {cell["direction"] for cell in fine_cells
        if any(abs(value) > MOTION_THRESHOLD_DEG for value in cell["responses_deg"])} == set(DIRECTIONS.values()):
        (args.output / "response_model.pulse_sweep.table").write_text(
            encode_partition_cells(fine_cells), encoding="ascii", newline="\n"
        )
        print("Fine-pulse candidate written. Independent closed-loop validation is still required.")
    else:
        print("Report written. The tested fine pulses have no usable response in at least one direction.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
