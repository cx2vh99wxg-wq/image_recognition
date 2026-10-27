from pathlib import Path
from PIL import Image
import numpy as np,json,re
r=Path(__file__).resolve().parent.parent/'line_left_live_20261027'
a=json.loads((r/'left_dataset_results.json').read_text())[9]
v=re.search(r'LEFT .*?fit=([0-9.,-]+) y=(\d+)\.\.(\d+)',a['after_detail'])
aa,bb,cc=map(float,v[1].split(','));start,end=map(int,(v[2],v[3]))
image=np.array(Image.open(r/'sample_09_tile.png')).astype(int)
def light(p):return int((p[0]+2*p[1]+p[2])/4)
for y in range(start,end+1):
    t=(y-228)/124.8;x=round(aa+bb*t+cc*t*t)
    candidates=[]
    for px in range(max(0,x-3),min(319,x+3)+1):
        p=image[y,px];white=p.min()>=75 and p.max()-p.min()<85
        yellow=p[1]>=95 and max(p[0],p[2])>=105 and min(p[0],p[2])<p[1]*.8
        bg=[]
        for side in (-1,1):
            vals=[light(image[y,sx]) for d in (6,8,10) if 0<=(sx:=px+side*d)<320]
            if vals:bg.append(sum(vals)//len(vals))
        background=max(bg);contrast=light(p)-background
        candidates.append((contrast-(abs(px-x)*.65),list(p),white or yellow,background,contrast))
    best=max(candidates)
    if not any(c[2] and c[4]>max(12,c[3]//8) for c in candidates):print(y,x,best)
