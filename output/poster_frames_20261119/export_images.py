"""Select verified model outputs and preserve their source-frame provenance."""

import json
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parent
OUT = ROOT / 'deliverables'
FONT = ImageFont.truetype('C:/Windows/Fonts/msyh.ttc', 22)
SMALL_FONT = ImageFont.truetype('C:/Windows/Fonts/msyh.ttc', 17)

GROUPS = {
    '左弯': {
        1: '中弯', 2: '中弯', 3: '中弯', 8: '急弯',
        9: '急弯', 10: '急弯', 16: '急弯', 17: '中弯',
    },
    '右弯': {
        6: '缓弯', 7: '中弯', 12: '中弯', 13: '急弯', 14: '急弯',
        18: '中弯', 19: '急弯', 20: '中弯', 21: '缓弯', 28: '急弯',
    },
    '过渡弯道': {
        4: '山路', 5: '山路', 11: '山路', 15: '山路', 22: '山路',
        23: '山路', 24: '山路', 25: '山路', 27: '山路', 29: '山路',
    },
}
CITIES = {
    '112507_01': '城市直道_多车道',
    '113343_01': '城市直道_前方车辆',
    '113343_03': '城市直道_两侧车辆',
    '113452_02': '城市直道_车辆骑行者',
    '112507_05': '城市路口_绿灯',
}


def main():
    sources = {r['name']: r for r in json.loads((ROOT/'lane_inputs.json').read_text(encoding='utf-8'))}
    selections = []
    for group, frames in GROUPS.items():
        for number, strength in frames.items():
            selections.append((group, f'mountain_{number:03d}', strength))
    for stem, label in CITIES.items():
        selections.append(('直道与路口', stem, label))

    records = []
    for index, (group, stem, label) in enumerate(selections, 1):
        folder = OUT/'车道线识别'/group
        folder.mkdir(parents=True, exist_ok=True)
        originals = OUT/'车道线原始尺寸'
        originals.mkdir(exist_ok=True)
        row = sources[stem]
        name = f'{index:02d}_{group}_{label}_{row["seconds"]:.2f}秒.png'
        with Image.open(ROOT/'lane_outputs'/(stem+'.png')) as image:
            # Remove only the 30-pixel black letterbox used by model input.
            image = image.crop((0, 30, 320, 210)).convert('RGB')
            image.save(originals/name)
            image.resize((1280, 720), Image.Resampling.LANCZOS).save(folder/name)
        records.append(dict(file=str((folder/name).relative_to(OUT)).replace('\\','/'),
                            original=str((originals/name).relative_to(OUT)).replace('\\','/'),
                            group=group, description=label, **row))

    # Numbered contact sheet leaves each standalone image free of added labels.
    cell_w, cell_h = 400, 266
    sheet = Image.new('RGB', (cell_w*4, cell_h*((len(records)+3)//4)), '#101722')
    draw = ImageDraw.Draw(sheet)
    for i, row in enumerate(records):
        x, y = (i % 4)*cell_w, (i//4)*cell_h
        with Image.open(OUT/row['file']) as image:
            image.thumbnail((384, 216))
            sheet.paste(image, (x+8, y+6))
        draw.text((x+8,y+226), f'{i+1:02d} {row["group"]} · {row["description"]}',
                  font=SMALL_FONT, fill='white')
    sheet.save(OUT/'车道线选图总览.jpg', quality=94)
    environment = sorted((OUT/'环境感知').glob('*.png'))
    sheet = Image.new('RGB', (1600, 910), '#101722')
    draw = ImageDraw.Draw(sheet)
    for i, path in enumerate(environment):
        with Image.open(path) as image:
            image.thumbnail((380, 820))
            sheet.paste(image, (i*400+(400-image.width)//2, 12))
        draw.text((i*400+10,850), path.stem[:17], font=SMALL_FONT, fill='white')
    sheet.save(OUT/'环境感知选图总览.jpg', quality=94)
    (OUT/'图片来源.json').write_text(json.dumps(records,ensure_ascii=False,indent=2),encoding='utf-8')
    (OUT/'图片说明.txt').write_text(
        '海报功能展示图片\n\n'
        f'车道线识别共 {len(records)} 帧。左弯 8 帧，右弯 10 帧，过渡弯道 10 帧，城市直道与路口 5 帧。\n'
        '图像来自现有道路录像，2026-11-19 在 M 板运行当前 RKNN 模型得到。'
        '复用了生产版 yolo_engine_detect 与 yolo_overlay_lane_and_drivable。\n'
        '模型文件为 yolopv2_front_320x240_mixed_error_guided_rk3568.rknn。\n'
        '红色为模型车道线输出，绿色为可行驶区。保留原录像中的水印。\n'
        '车道线识别目录为 1280×720 放大版，车道线原始尺寸目录为 320×180 原图。'
        '放大版仅使用常规插值，模型原始推理输入为 320×240，已裁去上下黑边。'
        '轻、中、急弯按画面形态归类，不表示标定曲率或方向盘转角。\n\n'
        '环境感知目录共 4 张，来自板上保存的 X11 截图裁切。'
        '01、02 是车辆、路边行人、骑行者及信号灯的实际渲染截图。'
        '03、04 为历史红灯/绿灯识别状态回放时保存的 X11 画面。\n'
        'X11完整截图目录保留上述 4 张完整画面，供核对对应关系。\n'
        '本次实时前摄对着室内，弯道图采用已有道路录像回放输入。\n\n'
        '每张车道图对应的录像文件名、时间点见图片来源.json。\n',
        encoding='utf-8')
    print('LANE', len(records), 'ENVIRONMENT', len(environment), 'FULL', len(list((OUT/'X11完整截图').glob('*.png'))))
    print('PNG', len(list(OUT.rglob('*.png'))), 'TOTAL_BYTES', sum(p.stat().st_size for p in OUT.rglob('*') if p.is_file()))


if __name__ == '__main__':
    main()
