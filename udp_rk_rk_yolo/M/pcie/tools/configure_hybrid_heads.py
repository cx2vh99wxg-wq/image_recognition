"""Build an explicit FP16 dual-segmentation-head profile from a Toolkit config.

This is a measured candidate, not an automatic accuracy recommendation. The
shared backbone remains quantized. Layer names are derived from ONNX ancestry
and intersected with the names actually exported by Toolkit (no YAML dependency).
"""
import argparse
import json
import re
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('onnx', type=Path)
    parser.add_argument('config', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    if args.output.exists():
        parser.error('refusing to overwrite a candidate config')
    import onnx
    graph = onnx.load(str(args.onnx)).graph
    producers = {name: node for node in graph.node for name in node.output}

    def ancestors(name):
        visited = set()
        pending = [name]
        while pending:
            tensor = pending.pop()
            if tensor in visited or tensor not in producers:
                continue
            visited.add(tensor)
            pending.extend(producers[tensor].input)
        return visited

    outputs = [p.name for p in graph.output]
    if outputs != ['drivable', 'lane']:
        parser.error('expected the existing named two-head YOLOPv2 ONNX')
    road, lane = map(ancestors, outputs)
    exclusive = road ^ lane
    text = args.config.read_text()
    if not text.startswith('custom_quantize_layers: {}\nquantize_parameters:'):
        parser.error('expected a new, empty editable Toolkit hybrid configuration')
    names = re.findall(r'^    (\S.*):$', text, re.M)
    floating_ops = {'Conv', 'ConvTranspose', 'BatchNormalization', 'Sigmoid', 'Mul', 'Add',
                    'Relu', 'LeakyRelu', 'Concat', 'Resize', 'Slice'}
    selected = [name for name in names if name in exclusive and name in producers and
                producers[name].op_type in floating_ops]
    if len(selected) < 4 or not any(name in road for name in selected) or not any(name in lane for name in selected):
        parser.error('did not locate both independent floating-point segmentation heads')
    header = 'custom_quantize_layers:\n'+''.join('    '+json.dumps(name)+': float16\n' for name in selected)
    args.output.write_text(header+text.split('\n', 1)[1])
    args.output.with_suffix('.profile.json').write_text(json.dumps(dict(
        fp16_layers=selected, shared_backbone_outputs=len(road & lane),
        policy='explicit dual-head FP16 trial; accuracy must be measured, not assumed'), indent=2))
    print('HYBRID_HEAD_PROFILE', len(selected), args.output)


if __name__ == '__main__':
    main()
