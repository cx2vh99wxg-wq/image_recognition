from bisect import bisect_left
import csv
import gzip
import json
from collections import defaultdict
from pathlib import Path

base = Path('tmp/clip_review')
ticks = defaultdict(list)
for wall, video, milliseconds, event in csv.reader((base/'verified-player.csv').open()):
    if event == 'tick':
        ticks[video].append((int(wall)/1000, int(milliseconds)/1000))
delta = float(Path('tmp/actor_retest/nearverify-COM7.offset').read_text())
best = defaultdict(list)
for line in gzip.open(base/'verified.jsonl.gz','rt'):
    stamp, key, *v = json.loads(line)
    if key != 16 or not v[2]: continue
    for short, series in ticks.items():
        slot = bisect_left(series,(stamp-delta,))
        if not 0 < slot < len(series): continue
        t = series[slot][1]
        for i in range(v[2]):
            camera, x, y, w, h, score, flags, gx, gy = v[6+i*9:15+i*9]
            if score >= 22 and h >= 15:
                best[short].append((round(score*h,2),round(t,2),x,y,w,h,score,flags))
        break
for short, results in best.items():
    chosen=[]
    for row in sorted(results,reverse=True):
        if len(chosen)<7 and all(abs(r[1]-row[1])>.45 for r in chosen):chosen.append(row)
    print(short,chosen)
