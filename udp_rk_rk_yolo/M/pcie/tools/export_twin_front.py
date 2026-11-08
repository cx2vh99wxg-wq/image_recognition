"""Export an author's TwinLiteNet+ checkpoint for offline RK3568 comparison.

Input is the same corrected RGB 320x240 image in [0,1]. Road has two softmax
channels, lane is foreground softmax probability. Unlike YOLOPv2, lane needs
the author's argmax-equivalent p>0.5 adapter BEFORE the existing IPM pipeline.
This tool never changes the active model or production startup configuration.
"""
import argparse
import hashlib
import inspect
import json
from pathlib import Path
import sys
import types


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source',type=Path)
    parser.add_argument('weights',type=Path)
    parser.add_argument('onnx',type=Path)
    parser.add_argument('--config',choices=('small','medium'),required=True)
    parser.add_argument('--internal-scale',type=int,choices=(1,2),default=1)
    parser.add_argument('--lower-attention',action='store_true',help='Equivalent 2D matmuls and explicit eight-sector mixing for RK3568 compatibility')
    parser.add_argument('--image',type=Path,action='append',default=[])
    args=parser.parse_args()
    if args.onnx.exists():parser.error('refusing to overwrite an exported candidate')
    import torch
    import numpy as np
    import cv2
    import onnx
    import onnxruntime as ort
    torch.set_num_threads(2);torch.set_num_interop_threads(1);torch.manual_seed(271)
    sys.path.insert(0,str(args.source.resolve()))
    source=args.source/'model/model.py'
    # The author's numerical model does not use pyplot. Skip its unused import
    # in this isolated exporter; preserve the downloaded source file unchanged.
    module=types.ModuleType('model.model');module.__file__=str(source)
    sys.modules['model.model']=module
    code=source.read_text().replace('import matplotlib.pyplot as plt','')
    exec(compile(code,str(source),'exec'),module.__dict__)
    model=module.TwinLiteNetPlus(types.SimpleNamespace(config=args.config))
    state=torch.load(str(args.weights),map_location='cpu',weights_only=True)
    model.load_state_dict(state,strict=True);model.eval()
    class Front(torch.nn.Module):
        def __init__(self,network):super().__init__();self.network=network
        def forward(self,image):
            if args.internal_scale==2:
                image=torch.nn.functional.interpolate(image,size=(480,640),mode='bilinear',align_corners=False)
            road,lane=self.network(image)
            if args.internal_scale==2:
                road=torch.nn.functional.interpolate(road,size=(240,320),mode='bilinear',align_corners=False)
                lane=torch.nn.functional.interpolate(lane,size=(240,320),mode='bilinear',align_corners=False)
            return torch.softmax(road,dim=1),torch.softmax(lane,dim=1)[:,1:2]
    net=Front(model).eval();dummy=torch.rand(1,3,240,320)
    samples=[('seeded',dummy)]
    for path in args.image:
        image=cv2.imread(str(path));assert image is not None and image.shape==(240,320,3)
        rgb=cv2.cvtColor(image,cv2.COLOR_BGR2RGB)
        samples.append((str(path),torch.from_numpy(rgb.transpose(2,0,1).copy()[None]).float()/255))
    if args.lower_attention:
        with torch.inference_mode():before=[tuple(x.clone() for x in net(image)) for _,image in samples]
        matmul=torch.matmul
        def two_dimensional(a,b):
            if a.dim()==4 and b.dim()==4 and a.shape[:2]==b.shape[:2]==(1,8):
                return torch.cat([matmul(a[0,i],b[0,i])[None,None] for i in range(8)],dim=1)
            return matmul(a,b)
        class TorchProxy:
            def __getattr__(self,name):return two_dimensional if name=='matmul' else getattr(torch,name)
        module.torch=TorchProxy()
        class SectorMix(torch.nn.Module):
            def __init__(self,conv):super().__init__();self.weight=torch.nn.Parameter(conv.weight.detach().clone())
            def forward(self,x):
                rows=[]
                for i in range(8):
                    value=x[:,0:1]*self.weight[i,0,0,0]
                    for j in range(1,8):value=value+x[:,j:j+1]*self.weight[i,j,0,0]
                    rows.append(value)
                return torch.cat(rows,dim=1)
        model.caam.gcn.conv1=SectorMix(model.caam.gcn.conv1).eval()
        with torch.inference_mode():
            for (name,image),expected in zip(samples,before):
                errors=[]
                for a,b in zip(expected,net(image)):
                    np.testing.assert_allclose(a.numpy(),b.numpy(),atol=4e-4,rtol=1e-3)
                    errors.append(float(torch.max(torch.abs(a-b))))
                print('TWIN_EQUIVALENT_LOWERING_PARITY',name,errors,flush=True)
    with torch.inference_mode():
        reference=net(dummy)
        assert [tuple(p.shape) for p in reference]==[(1,2,240,320),(1,1,240,320)]
        # The board's existing Torch has the legacy exporter. Newer releases
        # accept `dynamo`; choose explicitly only when that argument exists.
        export_options={'dynamo':False} if 'dynamo' in inspect.signature(torch.onnx.export).parameters else {}
        torch.onnx.export(net,dummy,str(args.onnx),opset_version=13,input_names=['rgb'],
                          output_names=['drivable','lane'],do_constant_folding=True,**export_options)
    onnx.checker.check_model(onnx.load(str(args.onnx)))
    opts=ort.SessionOptions();opts.intra_op_num_threads=2;opts.inter_op_num_threads=1
    session=ort.InferenceSession(str(args.onnx),opts,providers=['CPUExecutionProvider'])
    parity=[]
    with torch.inference_mode():
        for name,image in samples:
            expected=net(image);actual=session.run(['drivable','lane'],{'rgb':image.numpy()})
            errors=[]
            for a,b in zip(expected,actual):
                assert np.isfinite(b).all();np.testing.assert_allclose(a.numpy(),b,atol=4e-4,rtol=1e-3)
                errors.append(float(np.max(np.abs(a.numpy()-b))))
            parity.append(dict(image=name,max_abs_error=errors))
            print('TWIN_ONNX_PARITY',args.config,name,errors,flush=True)
    sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest()
    args.onnx.with_suffix('.json').write_text(json.dumps(dict(
        model='TwinLiteNet+',variant=args.config,internal_scale=args.internal_scale,
        equivalent_attention_lowering=args.lower_attention,
        source_url='https://github.com/chequanghuy/TwinLiteNetPlus',source_sha256=sha(source),
        weights_sha256=sha(args.weights),onnx_sha256=sha(args.onnx),
        input='RGB float /255; no ImageNet normalization',
        outputs=['drivable: background/road softmax [1,2,240,320]',
                 'lane: foreground softmax [1,1,240,320]; p>0.5 binary evidence before IPM'],
        parameters=sum(p.numel() for p in model.parameters()),parity=parity,
        production_promoted=False),indent=2))
    print('TWIN_EXPORT_PARITY_PASS',args.onnx,flush=True)


if __name__=='__main__':main()
