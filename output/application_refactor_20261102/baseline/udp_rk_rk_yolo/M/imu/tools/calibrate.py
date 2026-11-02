"""Make a bias/axis calibration from real, independently stationary raw data."""
import argparse
import csv
import json
import math
from pathlib import Path
import statistics


def calibrate(rows, axis, *, allow_simulated=False):
    if len(rows) < 200:
        raise ValueError("Need at least 200 fresh stationary samples")
    t = [int(r["sample_time_ns"]) for r in rows]
    if (t[-1] - t[0]) < 1_900_000_000 or any(b <= a or b - a > 50_000_000 for a, b in zip(t, t[1:])):
        raise ValueError("Need >=1.9s with monotonic timestamps and gaps <=50ms")
    sessions = {r["session"] for r in rows}
    if len(sessions) != 1:
        raise ValueError("Session changed during calibration")
    for row in rows:
        flags = int(row["flags"])
        if not flags & 1 or (flags & 4 and not allow_simulated):
            raise ValueError("Invalid or simulated data cannot calibrate real steering")
    gyro = [[float(r[f"g{k}_dps"]) for r in rows] for k in "xyz"]
    accel = [[float(r[f"a{k}_mps2"]) for k in "xyz"] for r in rows]
    bias = [statistics.mean(v) for v in gyro]
    std = [statistics.stdev(v) for v in gyro]
    norms = [math.sqrt(sum(v*v for v in a)) for a in accel]
    if not all(math.isfinite(x) for x in bias + std + norms):
        raise ValueError("Non-finite sample")
    if max(std) > .2 or max(abs(x) for x in bias) > 10:
        raise ValueError("Gyro is moving/vibrating or bias is unreasonable")
    if abs(statistics.mean(norms) - 9.80665) > .5 or statistics.stdev(norms) > .15:
        raise ValueError("Acceleration is inconsistent with a stationary fixture")
    norm = math.sqrt(sum(x*x for x in axis))
    if not math.isfinite(norm) or norm < 1e-9:
        raise ValueError("Invalid mounting axis")
    return dict(axis=[x / norm for x in axis], bias=bias, gyro_std=std,
                accel_norm_mean=statistics.mean(norms), samples=len(rows),
                duration_s=(t[-1]-t[0])/1e9,
                temperature_c=statistics.mean(float(r["temp_c"]) for r in rows),
                physical_center_established=False, chassis_compensated=False,
                note="Stationary declaration and mounting direction need independent physical verification")


def axis_from_left_sweep(rows, bias):
    """Only use a physically confirmed left sweep; net gyro vector gives sign."""
    integral=[0.,0.,0.]
    path=0.
    for a,b in zip(rows,rows[1:]):
        dt=(int(b["sample_time_ns"])-int(a["sample_time_ns"]))/1e9
        if not 0 < dt <= .05:
            raise ValueError("Axis recording contains a gap")
        w=[.5*(float(a[f"g{k}_dps"])+float(b[f"g{k}_dps"]))-bias[i] for i,k in enumerate("xyz")]
        for i in range(3):integral[i]+=w[i]*dt
        path+=math.sqrt(sum(x*x for x in w))*dt
    norm=math.sqrt(sum(x*x for x in integral))
    if norm<15 or path<=0 or norm/path<.85:
        raise ValueError("Use a single-axis, >=15deg physically positive-left sweep")
    return [x/norm for x in integral]


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--input",type=Path,required=True)
    p.add_argument("--output",type=Path,required=True)
    p.add_argument("--stationary",action="store_true",required=True,
                   help="Declare that chassis and sensor were physically stationary")
    p.add_argument("--axis",choices=list("xyz"),default="z")
    p.add_argument("--sign",type=int,choices=[-1,1],default=1)
    p.add_argument("--positive-left-csv",type=Path,
                   help="Separate physically confirmed left sweep, no chassis motion")
    a=p.parse_args()
    with a.input.open(newline="",encoding="utf-8-sig") as f:rows=list(csv.DictReader(f))
    axis=[float(a.sign) if k==a.axis else 0. for k in "xyz"]
    report=calibrate(rows,axis)
    if a.positive_left_csv:
        with a.positive_left_csv.open(newline="",encoding="utf-8-sig") as f:sweep=list(csv.DictReader(f))
        if any(int(r["flags"])&4 or not int(r["flags"])&1 for r in sweep):
            raise ValueError("Axis sweep must contain valid real data")
        report["axis"]=axis_from_left_sweep(sweep,report["bias"])
    with a.output.open("x",encoding="ascii") as f:
        f.write("IMUCAL1\naxis "+" ".join(f"{x:.9g}" for x in report["axis"])+"\nbias "+
                " ".join(f"{x:.9g}" for x in report["bias"])+"\n")
    a.output.with_suffix(a.output.suffix+".json").write_text(json.dumps(report,indent=2),encoding="utf-8")
    print(json.dumps(report,indent=2))


if __name__=="__main__":
    try:main()
    except (ValueError,OSError,KeyError) as e:raise SystemExit("Calibration rejected: "+str(e))
