import io
from pathlib import Path
import tarfile

from PIL import Image, ImageDraw

root = Path(__file__).resolve().parent
out = root / 'five_point_m'
out.mkdir(exist_ok=True)
with tarfile.open(root / 'five_point_m.tar.gz') as archive:
    for member in archive.getmembers():
        assert member.isfile() and Path(member.name).name == member.name
        (out / member.name).write_bytes(archive.extractfile(member).read())
lines = (out / 'centers.txt').read_text().splitlines()
directions = {0: 'straight', 1: 'left', 2: 'right'}
grid = Image.new('RGB', (1000, 160 * ((len(lines) + 4) // 5)), (17, 20, 26))
draw = ImageDraw.Draw(grid)
for i, line in enumerate(lines):
    _, token, kind, _, angle, _, *points = line.split()
    name = Path(token).stem
    x, y = i % 5 * 200, i // 5 * 160
    bgr = Image.open(out / f'{name}.png')
    b, g, r = bgr.split()
    rgb = Image.merge('RGB', (r, g, b))
    grid.paste(rgb.resize((192,144), Image.Resampling.NEAREST), (x+4,y+15))
    draw.text((x+4,y+1), f'{name}  {directions[int(kind)]}  {angle}', fill='white')
grid.save(root / 'five_point_candidates.jpg', quality=90)
