from pathlib import Path
from PIL import Image,ImageDraw
import collections,json,re,subprocess
root=Path(__file__).resolve().parent
before=root.parent/'line_style_fix_20261026/replay'
after=root/'green_replay'
rows=[];sheet=Image.new('RGB',(960,4*260))
for n in range(12):
    prefix='sample_%02d'%n
    for kind,extension in [('tile','ppm'),('lane','pgm'),('green','pgm')]:
        Image.open(root/(prefix+'_'+kind+'.png')).save(root/(prefix+'_'+kind+'.'+extension))
    args=[str(root/(prefix+'_tile.ppm')),str(root/(prefix+'_lane.pgm'))]
    old=subprocess.check_output([str(before),*args],text=True)
    new=subprocess.check_output([str(after),*args,str(root/(prefix+'_after.csv')),'1',str(root/(prefix+'_green.pgm'))],text=True)
    def parse(text):return re.findall(r'^(?:LEFT|RIGHT) (\w+) quality=(\d+) reason=(.*?) duty=',text,re.M)
    a,b=parse(old),parse(new)
    rows.append(dict(sample=prefix,old=a,new=b,before_detail=old,after_detail=new))
    tile=Image.open(root/(prefix+'_tile.png'));panel=Image.new('RGB',(320,260));panel.paste(tile,(0,20))
    ImageDraw.Draw(panel).text((3,4),prefix+'  '+a[0][0]+' -> '+b[0][0],fill='white')
    sheet.paste(panel,((n%3)*320,(n//3)*260))
    print(prefix,a[0][0],'=>',b[0][0],b[0][2], 'R='+b[1][0])
old_count=collections.Counter(r['old'][0][0] for r in rows)
new_count=collections.Counter(r['new'][0][0] for r in rows)
print('LEFT_GT_SOLID_12',dict(old_count),'=>',dict(new_count))
(root/'left_dataset_results.json').write_text(json.dumps(rows,indent=2))
sheet.save(root/'left_samples_comparison.png')
