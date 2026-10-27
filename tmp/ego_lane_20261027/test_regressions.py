from pathlib import Path
from PIL import Image
import subprocess,re,json
root=Path(__file__).resolve().parent
old=root.parent/'line_style_fix_20261026'
exe=root.parent/'line_left_live_20261027/green_replay'
results=[]
for name,raw,mask,expected in [('user0','video_0.ppm','user0_mask.pgm',['DASHED','SOLID']),('user2','video_2.ppm','user2_mask.pgm',['DASHED','SOLID']),('lab','lab.ppm','lab_mask.pgm',['UNKNOWN','UNKNOWN'])]:
    for suffix,path in [('tile.ppm',old/raw),('lane.pgm',old/mask),('green.pgm',root/(name+'_green.png'))]:
        Image.open(path).crop((0,0,320,240)).save(root/(name+'_'+suffix))
    args=[str(root/(name+'_tile.ppm')),str(root/(name+'_lane.pgm')),str(root/(name+'_trace.csv')),'1',str(root/(name+'_green.pgm'))]
    output=subprocess.check_output([str(exe),*args],text=True)
    types=re.findall(r'^(?:LEFT|RIGHT) (\w+) ',output,re.M)
    results.append(dict(name=name,expected=expected,got=types,output=output))
    print(name,types,'expected',expected)
    print(output)
(root/'regression_results.json').write_text(json.dumps(results,indent=2))
