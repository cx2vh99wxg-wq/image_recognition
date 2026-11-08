"""Compile the two-head 320x240 front camera ONNX for RK3568."""

import argparse
from pathlib import Path

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('onnx', type=Path)
    parser.add_argument('rknn', type=Path)
    parser.add_argument('--precision', choices=('fp16', 'int8', 'hybrid1', 'hybrid2'), default='fp16')
    parser.add_argument('--dataset', type=Path, help='Calibration image list; decoded RGB values must match runtime input order (legacy BGR requires channel-swapped files)')
    parser.add_argument('--hybrid-prefix', type=Path, help='Prefix of Toolkit-generated .model/.data/.quantization.cfg')
    parser.add_argument('--proposal-images', type=int, default=3)
    parser.add_argument('--no-proposal', action='store_true', help='Generate editable hybrid config without automatic precision proposals')
    args = parser.parse_args()
    if args.precision in ('int8', 'hybrid1') and (not args.dataset or not args.dataset.is_file()):
        parser.error('INT8/hybrid1 requires an existing --dataset')
    if args.precision == 'hybrid2' and not args.hybrid_prefix:
        parser.error('hybrid2 requires --hybrid-prefix')
    if args.precision != 'hybrid1' and args.rknn.exists():
        parser.error('refusing to overwrite an existing RKNN; use a separate candidate path')
    from rknn.api import RKNN

    model = RKNN()
    try:
        steps = [
            ('config', lambda: model.config(target_platform='rk3568', mean_values=[[0, 0, 0]],
                                            std_values=[[255, 255, 255]])),
        ]
        if args.precision == 'hybrid2':
            prefix = str(args.hybrid_prefix)
            steps += [('hybrid2', lambda: model.hybrid_quantization_step2(
                model_input=prefix+'.model', data_input=prefix+'.data',
                model_quantization_cfg=prefix+'.quantization.cfg'))]
        else:
            steps += [('load_onnx', lambda: model.load_onnx(model=str(args.onnx)))]
            if args.precision == 'hybrid1':
                steps += [('hybrid1', lambda: model.hybrid_quantization_step1(
                    dataset=str(args.dataset), proposal=not args.no_proposal,
                    proposal_dataset_size=max(1, args.proposal_images)))]
            else:
                options = dict(do_quantization=args.precision == 'int8')
                if args.precision == 'int8':
                    options['dataset'] = str(args.dataset)
                steps += [('build', lambda: model.build(**options))]
        if args.precision != 'hybrid1':
            steps += [('export_rknn', lambda: model.export_rknn(str(args.rknn)))]
        for name, step in steps:
            print('STEP', name, args.precision, flush=True)
            status = step()
            if status != 0:
                raise SystemExit(f'{name} failed: {status}')
    finally:
        model.release()


if __name__ == '__main__':
    main()
