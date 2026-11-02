#!/usr/bin/env python3
"""Rebuild the grayscale dashboard atlas from locally installed DejaVu fonts.

Requires Pillow; run in Ubuntu with fonts-dejavu-core installed. The PNG has
three 16x6 ASCII grids (32..126), 32-pixel cells, and 20/28/54-pixel rows.
The unused final cell in each grid stores PEUI version 1 and measured font
advance (1/64 px), cap top and cap height. Rendering shares one baseline per
grid; the original font license is copied beside the generated resource.
"""

import argparse
from pathlib import Path
import shutil

from PIL import Image, ImageDraw, ImageFont, PngImagePlugin


def generate(output_dir, font_dir, copyright_file):
    output_dir.mkdir(parents=True, exist_ok=True)
    image = Image.new("L", (512, 612), 0)
    draw = ImageDraw.Draw(image)
    styles = ((13, 20, 0, "DejaVuSansMono.ttf"),
              (20, 28, 120, "DejaVuSansMono.ttf"),
              (42, 54, 288, "DejaVuSansMono-Bold.ttf"))
    for size, cell_height, band_y, filename in styles:
        font = ImageFont.truetype(str(font_dir / filename), size)
        baseline = cell_height - font.getmetrics()[1] - 2
        for index, code in enumerate(range(32, 127)):
            x = (index % 16) * 32 + 2
            y = band_y + (index // 16) * cell_height + baseline
            draw.text((x, y), chr(code), font=font, fill=255, anchor="ls")

        h_index = ord("H") - 32
        hx, hy = (h_index % 16) * 32, band_y + (h_index // 16) * cell_height
        ink = image.crop((hx, hy, hx + 32, hy + cell_height)).getbbox()
        advance = round(font.getlength("M") * 64)
        if not ink or not 64 <= advance <= 32 * 64:
            raise RuntimeError("Unexpected DejaVu font metrics")
        metadata = b"PEUI" + bytes((1, advance & 255, advance >> 8,
                                    ink[1], ink[3] - ink[1]))
        for offset, value in enumerate(metadata):
            image.putpixel((15 * 32 + offset, band_y + 5 * cell_height), value)
        print(f"{filename} {size}px: advance={advance}/64, "
              f"cap_top={ink[1]}, cap_height={ink[3] - ink[1]}")

    info = PngImagePlugin.PngInfo()
    info.add_text("Description", "DejaVu Sans Mono ASCII opacity atlas; PEUI v1; 13/20/42px")
    destination = output_dir / "dashboard_font.png"
    image.save(destination, pnginfo=info, optimize=True, compress_level=9)
    shutil.copyfile(copyright_file, output_dir / "dashboard_font.LICENSE.txt")
    print(f"Wrote {destination} ({destination.stat().st_size} bytes)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--font-dir", type=Path, default=Path("/usr/share/fonts/truetype/dejavu"))
    parser.add_argument("--copyright-file", type=Path,
                        default=Path("/usr/share/doc/fonts-dejavu-core/copyright"))
    args = parser.parse_args()
    generate(args.output_dir, args.font_dir, args.copyright_file)
