from pathlib import Path
from PIL import Image, ImageOps
import subprocess,re,json,sys
root=Path(__file__).resolve().parent
base=root.parent/'line_left_live_20261027'
exe=Path(sys.argv[1]).resolve()
out=root/'transforms';out.mkdir(exist_ok=True)
sources=[]
for n in range(12):
    stem='sample_%02d'%n
    sources.append((stem,base/(stem+'_tile.png'),base/(stem+'_lane.png'),base/(stem+'_green.png'),['SOLID','DASHED']))
for name in ['user0','user2','lab']:
    sources.append((name,root/(name+'_tile.ppm'),root/(name+'_lane.pgm'),root/(name+'_green.pgm'),['UNKNOWN','UNKNOWN'] if name=='lab' else ['DASHED','SOLID']))
results=[];wrong=[]
for name,rgb,lane,green,expected in sources:
    for mirror in [False,True]:
        for scale in [1.,.9]:
            for shift in [-8,0,8]:
                if name=='lab' and (scale!=1 or shift):continue
                ims=[Image.open(p).convert('RGB' if i==0 else 'L') for i,p in enumerate([rgb,lane,green])]
                if mirror:ims=[ImageOps.mirror(im) for im in ims]
                if scale!=1:ims=[im.resize((288,216),Image.Resampling.BILINEAR if i==0 else Image.Resampling.NEAREST) for i,im in enumerate(ims)]
                tag='%s_m%d_s%.1f_y%d'%(name,mirror,scale,shift)
                paths=[]
                for i,im in enumerate(ims):
                    cell=Image.new('RGB' if i==0 else 'L',(320,240),(25,25,25) if i==0 else 0)
                    cell.paste(im,((320-im.width)//2,(240-im.height)//2+shift))
                    p=out/(tag+('_rgb.ppm' if i==0 else '_lane.pgm' if i==1 else '_green.pgm'));cell.save(p);paths.append(p)
                output=subprocess.check_output([str(exe),str(paths[0]),str(paths[1]),str(out/'trace.csv'),'1',str(paths[2])],text=True)
                got=re.findall(r'^(?:LEFT|RIGHT) (\w+) ',output,re.M)
                exp=expected[::-1] if mirror else expected
                ok=all(a==b or a=='UNKNOWN' for a,b in zip(got,exp)) and len(got)==2
                record=dict(case=tag,expected=exp,got=got,ok=ok,detail=output)
                results.append(record)
                if not ok:wrong.append(record)
(root/'saved_results.json').write_text(json.dumps(results,indent=2))
originals=[r for r in results if '_m0_s1.0_y0' in r['case']]
print('Originals:')
for r in originals: print(r['case'],r['got'])
classified=sum(a!='UNKNOWN' for r in results for a in r['got'])
print('Checks',len(results),'classified sides',classified,'/',len(results)*2,'opposite-style failures',len(wrong))
for r in wrong[:20]:print(r['case'],r['expected'],r['got'],r['detail'])
assert not wrong
assert sum(r['got'][0]=='SOLID' for r in originals if r['case'].startswith('sample_'))>=11
assert classified>=len(results)*2*.75
