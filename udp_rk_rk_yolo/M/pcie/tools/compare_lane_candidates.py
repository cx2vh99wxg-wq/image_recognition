"""Compare saved RK3568 candidate outputs against FP16, without changing a live model.

Agreement with FP16 is a regression check, not labeled accuracy. Whole held-out
scenes must be excluded from calibration. No automatic model promotion is performed.
"""
import argparse
import json
from pathlib import Path
import statistics
import numpy as np


def overlap(a, b):
    union = np.count_nonzero(a | b)
    return float(np.count_nonzero(a & b) / union) if union else 1.0


def foot_classes(road):
    # Same complete-ground-patch and uncertainty margin as PersonRoadAssociation
    # for a representative 24px-wide person. No image-boundary boxes prove OUTSIDE.
    result = []
    if np.count_nonzero(road) < 64:
        return np.ones(len(range(40,230,5))*len(range(12,308,5)), dtype=np.uint8)
    for y in range(40, 230, 5):
        for x in range(12, 308, 5):
            hits = int(road[y:y+6, x-4:x+5].sum())
            if hits*5 >= 9*6:
                result.append(2)  # inside
            else:
                result.append(1 if road[y-2:y+9, x-7:x+8].any() else 0)
    return np.asarray(result)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory', type=Path)
    parser.add_argument('--candidate', action='append', help='Additional saved candidate tag; default: int8 and hybrid2')
    args = parser.parse_args()
    base = args.directory
    report = json.loads((base/'quantization.json').read_text())
    results = {}
    for candidate in args.candidate or ('int8', 'hybrid2'):
        if not report.get('conversions', {}).get(candidate, {}).get('success'):
            results[candidate] = dict(available=False)
            continue
        rows = []
        for i, case in enumerate(report['cases']):
            def load(tag, kind, channels=1):
                p = np.fromfile(base/'outputs'/f'{tag}-{i}.{kind}.f32', dtype='<f4')
                if p.size != 320*240*channels or not np.isfinite(p).all():
                    raise ValueError('invalid tensor '+str((tag, i, kind)))
                return p.reshape(channels, 240, 320)
            a, b = load('fp16', 'lane')[0] > .3, load(candidate, 'lane')[0] > .3
            ar, br = load('fp16', 'road', 2), load(candidate, 'road', 2)
            ar, br = ar[1] > ar[0], br[1] > br[0]
            af, bf = foot_classes(ar), foot_classes(br)
            before, after = report['fp16'][i], report[candidate][i]
            valid_before=bool(before['sampled'] and before['confidence'])
            valid_after=bool(after['sampled'] and after['confidence'])
            signed=lambda r: -r['angle'] if r['curve']==1 else r['angle'] if r['curve']==2 else 0
            rows.append(dict(scene=case['scene'], held_out=case['scene'] in ('scene2', 'scene3'),
                lane_iou=overlap(a, b), road_iou=overlap(ar, br),
                bird_lane_iou=overlap(load('fp16', 'bird_lane')[0] > .3, load(candidate, 'bird_lane')[0] > .3),
                geometry_equal=all(before[k] == after[k] for k in ('curve', 'confidence', 'angle', 'sampled')),
                validity_equal=valid_before==valid_after,
                lost_validity=valid_before and not valid_after,
                both_valid=valid_before and valid_after,
                direction_equal=(before['curve']==after['curve']) if valid_before and valid_after else None,
                angle_error_deg=abs(signed(before)-signed(after)) if valid_before and valid_after else None,
                marking_equal=all(before[k] == after[k] for k in ('left', 'right')),
                protected_to_outside=int(np.count_nonzero((af != 0) & (bf == 0))),
                foot_relation_changes=int(np.count_nonzero(af != bf)),
                fp16_ms=before['median_ms'], candidate_ms=after['median_ms']))
        held = [r for r in rows if r['held_out']]
        results[candidate] = dict(available=True,
            median_fp16_ms=statistics.median(r['fp16_ms'] for r in rows),
            median_candidate_ms=statistics.median(r['candidate_ms'] for r in rows),
            speedup=statistics.median(r['fp16_ms']/r['candidate_ms'] for r in rows),
            held_out_frames=len(held), geometry_matches=sum(r['geometry_equal'] for r in held),
            marking_matches=sum(r['marking_equal'] for r in held),
            lost_validity=sum(r['lost_validity'] for r in held),
            both_valid=sum(r['both_valid'] for r in held),
            direction_mismatches=sum(r['direction_equal'] is False for r in held),
            maximum_angle_error_deg=max((r['angle_error_deg'] for r in held if r['both_valid']),default=None),
            minimum_lane_iou=min(r['lane_iou'] for r in held),
            minimum_road_iou=min(r['road_iou'] for r in held),
            protected_to_outside=sum(r['protected_to_outside'] for r in held), rows=rows,
            production_promoted=False,
            limitation='Small pilot set and baseline agreement; labeled independent accuracy still required')
    target=base/'comparison.json'
    target.write_text(json.dumps(results, indent=2))
    print(json.dumps({k:{a:b for a,b in v.items() if a!='rows'} for k,v in results.items()}, indent=2))


if __name__ == '__main__':
    main()
