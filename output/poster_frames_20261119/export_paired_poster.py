import csv
import io
import json
from pathlib import Path
import tarfile

import numpy as np
from PIL import Image, ImageDraw, ImageFont, ImageOps

root = Path(__file__).resolve().parent
target = root / '当前版本_海报对照图'
originals = target / 'X11完整截图'
originals.mkdir(parents=True, exist_ok=True)
font_dir = Path('C:/Windows/Fonts')

def font(size, bold=False):
    return ImageFont.truetype(str(font_dir / ('msyhbd.ttc' if bold else 'msyh.ttc')), size)

def crop(image, rect):
    sx, sy = image.width / 1600, image.height / 900
    return image.crop(tuple(round(x*(sx if i%2==0 else sy)) for i,x in enumerate(rect)))

def set_into(image, source, rect):
    left, top, width, height = rect
    adjusted = ImageOps.contain(source, (width,height), Image.Resampling.LANCZOS)
    image.paste(adjusted,(left+(width-adjusted.width)//2,top+(height-adjusted.height)//2))

def provenance(entry):
    name=entry['name']
    if name.startswith('mountain_'):
        return '20261119-0716-09.8896139.mp4'
    short = '113539' if entry['video']=='sidewalk_people' else entry['video'].split('_')[0]
    return f'video_20260929_{short}.mp4'

def verify_left_image(frame, data):
    source = np.frombuffer(data,dtype='<u2').reshape(240,320)
    bgr565 = np.stack(((source & 31) << 3, ((source >> 5) & 63) << 2, (source >> 11) << 3), axis=-1)
    dx = np.arange(frame.width)*1600//frame.width - 254
    dy = np.arange(frame.height)*900//frame.height - 224
    columns = np.flatnonzero((dx>=30) & (dx<698))[::3]
    rows = np.flatnonzero((dy>=85) & (dy<461))[::3]
    expected = bgr565[(dy[rows]*240//546)[:,None], (dx[columns]*320//728)[None,:]]
    actual = np.asarray(frame)[rows[:,None],columns[None,:]]
    return np.mean(np.all(expected==actual,axis=2))

exported = {}
for file in sorted(root.glob('current_*_screens.tar.gz')):
    input_archive = file.with_name(file.name.replace('_screens','_selection'))
    with tarfile.open(file) as archive, tarfile.open(input_archive) as input_frames:
        selection = json.loads(archive.extractfile('selection.json').read())
        for row in selection:
            screenshot = archive.extractfile(f'{row["shot"]}_{row["view"]}.png').read()
            with Image.open(io.BytesIO(screenshot)) as loaded:
                frame = loaded.convert('RGB')
            assert frame.size in ((1440,810),(1920,1080)), (row['shot'],frame.size)
            original_tile = input_frames.extractfile(f'{row["shot"]}.{row["view"]}').read()
            ratio = verify_left_image(frame,original_tile)
            assert ratio > .94, (row['shot'],ratio)
            front = crop(frame, (254, 293 if row['view']=='raw' else 224,
                                 982, 701 if row['view']=='raw' else 770))
            assert np.asarray(front).std() > 19, row['shot']
            panel = crop(frame, (1220,186,1552,798))
            assert np.asarray(panel).std() > 15, row['shot']
            light = int(row['observations'][7])
            if light in (1,3):
                x, y = 1144 + 26*(light-1), 78
                red,green,blue = frame.getpixel((round(x*frame.width/1600),round(y*frame.height/900)))
                assert (red>125 and red>1.4*green) if light==1 else (green>110 and green>1.35*red), (row['shot'],(red,green,blue))

            canvas = Image.new('RGB',(1352,742),'#091421')
            draw = ImageDraw.Draw(canvas)
            draw.text((35,19),row['title'],fill='#eef7fc',font=font(33,True))
            view = '前视鸟瞰（IPM）' if row['view']=='ipm' else '前视原画面'
            draw.text((36,75),view,fill='#a7c9dc',font=font(21))
            draw.text((974,75),'S 板 X11 感知结果',fill='#a7c9dc',font=font(21))
            draw.rectangle((32,112,944,689),fill='#070f19',outline='#17314a',width=1)
            set_into(canvas,front,(34,114,909,574))
            set_into(canvas,panel,(969,151 if light else 111,338,537 if light else 577))
            if light:
                banner = crop(frame,(1120,52,1540,106))
                set_into(canvas,banner,(980,108,318,35))
            draw.text((36,704),f'{provenance(row)}  ·  {row["time"]:.2f} 秒',
                      fill='#91abb9',font=font(17))
            filename = row['file'] + '.png'
            canvas.save(target/filename,optimize=True)
            (originals/filename).write_bytes(screenshot)
            exported[filename] = row
            print(row['shot'], f'left match {ratio:.3%}')

if not exported:
    raise SystemExit('No new paired screenshots found')
exported = [(row,filename) for filename,row in sorted(exported.items())]
with (target/'截图来源.csv').open('w',encoding='utf-8-sig',newline='') as handle:
    writer=csv.writer(handle)
    writer.writerow(['文件','原视频','秒','左图','车道方向','角度幅值','线型（左/右）','灯色','行人','车辆'])
    for row,filename in exported:
        o=row['observations']
        writer.writerow([filename,provenance(row),f'{row["time"]:.3f}',row['view'],
                         {'0':'直道或未确认','1':'左弯','2':'右弯'}.get(o[1],o[1]),o[2],
                         '/'.join({'0':'未知','1':'实线','2':'虚线'}[x] for x in o[5:7]),
                         {'0':'未知','1':'红','2':'黄','3':'绿'}[o[7]],o[11],o[12]])

text = (root/'poster_text_zh.md').read_text(encoding='utf-8').replace('**','').replace('# ','',1)
(target/'功能介绍.txt').write_text(text,encoding='utf-8-sig')
tiles = len(exported)
sheet = Image.new('RGB',(1152,222*((tiles+2)//3)+64),'#0e1b28')
d = ImageDraw.Draw(sheet)
d.text((19,13),'当前版本｜前视与环境感知',font=font(26,True),fill='#eef7fc')
for i, (_,filename) in enumerate(exported):
    x, y = 13+(i%3)*380, 55+(i//3)*222
    with Image.open(target/filename) as image:
        sheet.paste(image.resize((368,202), Image.Resampling.LANCZOS),(x,y))
sheet.save(target/'选图总览.jpg',quality=93)
print('VERIFIED_AND_EXPORTED',len(exported))
