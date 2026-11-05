"""Export the official TorchScript's two segmentation heads for a single front tile."""
import argparse
from pathlib import Path

import numpy as np
import onnx
import onnxruntime as ort
import torch


class Segmentation(torch.nn.Module):
    def __init__(self, model, pad):
        super().__init__()
        self.model = model
        self.pad = pad
        if pad:
            self.register_buffer('border', torch.full((1, 3, pad, 320), 114.0 / 255))

    def forward(self, image):
        if self.pad:
            image = torch.cat((self.border, image, self.border), dim=2)
        _, road, lane = self.model(image)
        if self.pad:
            road = road[:, :, self.pad:-self.pad, :]
            lane = lane[:, :, self.pad:-self.pad, :]
        return road, lane


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('weights', type=Path)
    parser.add_argument('onnx', type=Path)
    parser.add_argument('--image', type=Path)
    args = parser.parse_args()
    torch.set_num_threads(4)
    torch.set_num_interop_threads(1)
    torch.manual_seed(904)
    model = torch.jit.load(str(args.weights), map_location='cpu').eval()
    samples = [torch.zeros(1, 3, 240, 320), torch.rand(1, 3, 240, 320)]
    if args.image:
        from PIL import Image
        pixels = np.asarray(Image.open(args.image).convert('RGB'))
        if pixels.shape != (240, 320, 3):
            raise ValueError('saved image must be the 320×240 front tile')
        samples.append(torch.from_numpy(pixels.transpose(2, 0, 1).copy()[None]).float() / 255)
    with torch.inference_mode():
        for height, width in ((480, 640), (240, 320), (256, 320)):
            try:
                _, road, lane = model(torch.zeros(1, 3, height, width))
                print('shape', height, width, list(road.shape), list(lane.shape), flush=True)
            except RuntimeError as e:
                print('shape', height, width, 'rejected', str(e).splitlines()[-1], flush=True)
        for pad in (0, 8):
            net = Segmentation(model, pad).eval()
            try:
                outputs = net(samples[0])
                expected = ((1, 2, 240, 320), (1, 1, 240, 320))
                if tuple(tuple(o.shape) for o in outputs) != expected:
                    raise ValueError('unexpected segmentation shape')
            except (RuntimeError, ValueError) as e:
                print('pad', pad, 'rejected', str(e).splitlines()[-1], flush=True)
                continue
            print('export pad', pad, 'pixels per side', flush=True)
            break
        else:
            raise RuntimeError('No configuration preserves the intended output contract')
    with torch.no_grad():
        traced = torch.jit.trace(net, samples[0], strict=False, check_trace=False)
        torch.onnx.export(traced, samples[0], str(args.onnx), opset_version=13,
                          input_names=['rgb'], output_names=['drivable', 'lane'],
                          do_constant_folding=True)
        graph = onnx.load(str(args.onnx))
        for output, shape in zip(graph.graph.output, expected):
            for dim, value in zip(output.type.tensor_type.shape.dim, shape):
                dim.dim_value = value
        onnx.save(graph, str(args.onnx))
        onnx.checker.check_model(str(args.onnx))
        opts = ort.SessionOptions()
        opts.intra_op_num_threads = 4
        opts.inter_op_num_threads = 1
        session = ort.InferenceSession(str(args.onnx), opts, providers=['CPUExecutionProvider'])
        for idx, image in enumerate(samples):
            road, lane = net(image)
            r, l = session.run(['drivable', 'lane'], {'rgb': image.numpy()})
            for name, expected, actual in (('road', road.numpy(), r), ('lane', lane.numpy(), l)):
                if not np.isfinite(actual).all():
                    raise RuntimeError(name + ' non-finite output')
                print('onnx_check', idx, name, actual.shape, 'range', float(actual.min()),
                      float(actual.max()), 'max_abs_diff', float(np.max(np.abs(expected - actual))), flush=True)
                np.testing.assert_allclose(actual, expected, atol=3e-4, rtol=1e-3)
    print('EXPORT_AND_ONNX_PARITY_PASS', args.onnx)


if __name__ == '__main__':
    main()
