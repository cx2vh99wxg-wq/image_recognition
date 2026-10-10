#!/usr/bin/env python3
"""Read literal metadata embedded by RKNN Toolkit (never eval/pickle).
Not an inference test; anchor grids are absent from these exported model outputs.
"""
from pathlib import Path
import ast,argparse,hashlib,json,struct
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--output',type=Path);args=p.parse_args()
result={}
for name in ('yolov5s-640-640.rknn','yolopv2_Nx3x480x640_rk3568.rknn'):
    b=(ROOT/'model'/name).read_bytes()
    offset=b.find(b"{'attrs':")
    if offset<4:raise SystemExit(f'No supported RKNN metadata: {name}')
    size=struct.unpack_from('<I',b,offset-4)[0]
    if size>1024*1024 or offset+size>len(b):raise SystemExit('Invalid metadata length')
    d=ast.literal_eval(b[offset:offset+size].decode('utf-8').rstrip('\0'))
    entry={'sha256':hashlib.sha256(b).hexdigest(),'size':len(b),'metadata':d,
           'anchor_source':'reference profile in yolo_decode.c; verify against original PT/ONNX exporter, not stored here'}
    result[name]=entry
    print(name,entry['sha256'])
    for tensor,a in d['attrs'].items():
        q=d.get('quant_tab',{}).get(tensor,{})
        print(' ',tensor,a.get('shape'),a.get('layout'),q.get('min'),q.get('max'))
if args.output:args.output.write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
