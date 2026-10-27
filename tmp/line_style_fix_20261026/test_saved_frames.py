from pathlib import Path
from PIL import Image, ImageDraw
import subprocess, re, json, csv, sys

root=Path(__file__).resolve().parent
exe=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else root/'replay'
fixtures=root/'transforms';fixtures.mkdir(exist_ok=True)
cases=[]
def run(name,image,mask,expected,allow_unknown=False):
    csv_path=root/(name+'_evidence.csv')
    output=subprocess.check_output([str(exe),str(image),str(mask),str(csv_path)],text=True)
    matches=re.findall(r'^(LEFT|RIGHT) (\w+) quality=(\d+) reason=(.*?) duty=(\d+) spans=(\d+) gaps=(\d+) coverage=(\d+)',output,re.M)
    got=[m[1] for m in matches]
    assert got==expected or (allow_unknown and all(a==b or a=='UNKNOWN' for a,b in zip(got,expected))),(name,output,expected)
    cases.append({'case':name,'expected':expected,'result':got,'detail':output})
    return matches,csv_path

for n in (0,2):
    raw=root/f'video_{n}.ppm';mask=root/f'user{n}_mask.pgm'
    matches,csv_path=run(f'user{n}',raw,mask,['DASHED','SOLID'])
    im=Image.open(raw).crop((0,0,320,240)).resize((960,720));draw=ImageDraw.Draw(im)
    for r in csv.DictReader(csv_path.open()):
        x,y=int(r['x'])*3,int(r['y'])*3
        col=(0,255,180) if r['side']=='LEFT' else (255,190,40)
        if int(r['filtered'])==0:col=(250,70,120)
        draw.ellipse((x-3,y-3,x+3,y+3),fill=col)
    draw.rectangle((0,0,960,28),fill=(8,12,28))
    draw.text((12,8),f'LEFT DASHED Q{matches[0][2]}   RIGHT SOLID Q{matches[1][2]} / actual saved RGB + RKNN mask',fill='white')
    im.save(root/f'user{n}_verified.png')
    # These are geometry/photometry regressions of a saved NN result, not
    # additional independent real-world accuracy samples.
    for mirror in (False,True):
        for scale in (.85,1.0):
            for shift_y in (-8,0,8):
                rgb=Image.open(raw);mm=Image.open(mask)
                a=rgb.crop((0,0,320,240));b=mm.crop((0,0,320,240))
                if mirror:a=a.transpose(Image.Transpose.FLIP_LEFT_RIGHT);b=b.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
                if scale!=1:
                    a=a.resize((272,204),Image.Resampling.BILINEAR);b=b.resize((272,204),Image.Resampling.NEAREST)
                x=(320-a.width)//2;y=(240-a.height)//2+shift_y
                cell=Image.new('RGB',(320,240),(25,25,25));cell.paste(a,(x,y))
                cm=Image.new('L',(320,240),0);cm.paste(b,(x,y))
                rgb.paste(cell,(0,0));mm.paste(cm,(0,0))
                tag=f'user{n}_mirror{int(mirror)}_scale{scale}_y{shift_y}'
                rp=fixtures/(tag+'.ppm');mp=fixtures/(tag+'.pgm');rgb.save(rp);mm.save(mp)
                run(tag,rp,mp,['SOLID','DASHED'] if mirror else ['DASHED','SOLID'],allow_unknown=True)
run('lab',root/'lab.ppm',root/'lab_mask.pgm',['UNKNOWN','UNKNOWN'])
(root/'saved_frame_results.json').write_text(json.dumps(cases,indent=2),encoding='utf-8')
transformed=[c for c in cases if '_mirror' in c['case']]
confirmed=sum(a==b for c in transformed for a,b in zip(c['result'],c['expected']))
assert confirmed>=len(transformed)*2*.75,('too many inconclusive transformed tracks',confirmed)
print('Saved-frame and transformed-frame checks PASS',len(cases),'(2 independent road frames + 1 negative; transforms counted separately)',
      'transformed confirmed',confirmed,'/',len(transformed)*2,'remaining UNKNOWN; no opposite style allowed')
