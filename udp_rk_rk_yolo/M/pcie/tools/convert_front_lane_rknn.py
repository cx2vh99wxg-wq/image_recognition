"""Compile the two-head 320x240 front camera ONNX for RK3568."""

import argparse
from pathlib import Path

from rknn.api import RKNN


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('onnx', type=Path)
    parser.add_argument('rknn', type=Path)
    args = parser.parse_args()

    model = RKNN()
    try:
        steps = (
            ('config', lambda: model.config(target_platform='rk3568', mean_values=[[0, 0, 0]],
                                            std_values=[[255, 255, 255]])),
            ('load_onnx', lambda: model.load_onnx(model=str(args.onnx))),
            ('build', lambda: model.build(do_quantization=False)),
            ('export_rknn', lambda: model.export_rknn(str(args.rknn))),
        )
        for name, step in steps:
            status = step()
            if status != 0:
                raise SystemExit(f'{name} failed: {status}')
    finally:
        model.release()


if __name__ == '__main__':
    main()
