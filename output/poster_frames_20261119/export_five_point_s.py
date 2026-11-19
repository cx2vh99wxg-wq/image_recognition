import csv
import io
import json
from pathlib import Path
import shutil
import tarfile

import numpy as np
from PIL import Image, ImageDraw, ImageFont

root = Path(__file__).resolve().parent
out = root / '前方中央_五点车道线'
source = {item['name']: item for item in json.loads((root / 'lane_inputs.json').read_text('utf-8'))}
details = {}
for line in (root / 'five_point_m' / 'centers.txt').read_text().splitlines():
    _, token, direction, confidence, angle, curvature, *points = line.split()
    details[Path(token).stem] = (int(direction), int(confidence), int(angle), float(curvature),
                                [tuple(map(int, point.split(','))) for point in points])

xi = np.arange(728) * 320 // 728
yi = np.arange(546) * 240 // 546
archived = root.parent / 'hdmi_live_1119b'
screen = out / '00_S板X11实截_前方中央.png'
with Image.open(archived / 'dashboard-s.jpg') as full_screen:
    full_screen.crop((175, 90, 320, 199)).save(screen)
original = out / '00_S板前视存档_原始画幅.jpg'
shutil.copyfile(archived / 'front-s.jpg', original)
paths = [(screen, '00  S 板 X11 实截'), (original, '00  S 板前视存档')]
order = ('直道', '左弯', '右弯')
counts = [0, 0, 0]
index = [['文件', '来源', '时间（秒）', '转向方向', '拟合航向角幅值（度）', '五点坐标（320×240）']]
index.append([screen.name, 'S 板 X11 屏幕存档裁图', '', '', '', ''])
index.append([original.name, 'S 板前视画面存档', '', '', '', ''])
with tarfile.open(root / 'five_point_s_screens.tar.gz') as archive:
    members = archive.getmembers()
    assert len(members) == 24
    for member in sorted(members, key=lambda item: (details[Path(item.name).stem][0], item.name)):
        name = Path(member.name).stem
        direction, confidence, angle, curvature, points = details[name]
        data = archive.extractfile(member).read()
        with Image.open(io.BytesIO(data)) as pic:
            image = np.array(pic.convert('RGB'))
        with Image.open(root / 'five_point_m' / f'{name}.png') as pic:
            tile = np.array(pic)[:, :, ::-1] & np.array([248,252,248], dtype=np.uint8)
        assert image.shape == (546, 728, 3), name
        assert np.array_equal(image, tile[yi[:, None], xi[None, :]]), name
        assert len(points) == 5 and len({y for _, y in points}) == 5, name
        for x, y in points:
            assert 2 <= x < 318 and 2 <= y < 238, (name, x, y)
            assert np.all(tile[y-2:y+3, x-2:x+3] == [248,252,248]), (name, x, y)
        for boundary in ([48,240,248], [248,220,56]):
            assert np.count_nonzero(np.all(image == boundary, axis=2)) >= 40, (name, boundary)

        item = source[name]
        timestamp = item['seconds']
        region = '山路' if name.startswith('mountain_') else '城市道路'
        counts[direction] += 1
        number = len(paths) - 1
        direction_name = order[direction]
        dest = out / f'{number:02d}_{direction_name}_{region}_{timestamp:.2f}秒.png'
        dest.write_bytes(data)
        paths.append((dest, f'{number:02d}  {direction_name} · {region} · {timestamp:.2f}s'))
        index.append([dest.name, Path(item['source']).name, timestamp, direction_name, angle,
                      ' '.join(f'{x},{y}' for x, y in points)])

with (out / '图片来源.csv').open('w', encoding='utf-8-sig', newline='') as file:
    csv.writer(file).writerows(index)
(out / '图片说明.txt').write_text(
    '00 号含两张此前记录。X11 实截从屏幕存档裁出，尺寸 145×109。前视存档尺寸 320×240。\n'
    '01 至 24 号图来自现有道路录像。M 板用部署的 YOLOPv2 模型和 IPM 五点叠加处理，'
    'S 板用同一显示程序的离线预览模式渲染「前方中央」，再裁出单路视频区，尺寸 728×546。\n'
    '逐张比对了裁图与前方中央输入的像素映射，并核验了五个完整白点及青、黄侧边标线。'
    '夹角字段是单帧拟合的航向角幅值。\n', encoding='utf-8-sig')

columns = 5
rows = (len(paths) + columns - 1) // columns
sheet = Image.new('RGB', (columns * 244 + 12, rows * 202 + 56), '#101922')
draw = ImageDraw.Draw(sheet)
regular = ImageFont.truetype('C:/Windows/Fonts/msyh.ttc', 16)
heading = ImageFont.truetype('C:/Windows/Fonts/msyhbd.ttc', 25)
draw.text((18, 14), 'S 板前方中央  ·  五点车道线识别', fill='#ebf4fb', font=heading)
for i, (path, label) in enumerate(paths):
    x, y = 12 + i % columns * 244, 58 + i // columns * 202
    with Image.open(path) as pic:
        sheet.paste(pic.resize((232,174), Image.Resampling.LANCZOS), (x,y))
    draw.text((x+2,y+176), label, fill='#dbebf6', font=regular)
sheet.save(out / '选图总览.jpg', quality=92)
print('verified', sum(counts), dict(zip(order, counts)), 'plus two archived S-board frames')
