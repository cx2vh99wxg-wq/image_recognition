"""CPU-only YOLOPv2 QAT/export pipeline pilot, never a production model.

Requires onnx2torch, torchvision compatible with the installed PyTorch, ONNX
and ONNX Runtime. Inputs are saved RGB PPMs; the formal engine contract is BGR.
Two default steps only exercise the toolchain, not sufficient training.
Stop board capture separately before benchmarking/compiling on a small board.
"""
from pathlib import Path
import argparse,json,sys,traceback

def main():
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('onnx',type=Path)
 p.add_argument('image',type=Path,help='Corrected RGB 320x240 PPM without overlays')
 p.add_argument('directory',type=Path,help='New isolated output directory')
 p.add_argument('--dependency-directory',type=Path)
 p.add_argument('--steps',type=int,default=2)
 a=p.parse_args()
 if not a.onnx.is_file() or not a.image.is_file():p.error('missing ONNX or saved input')
 if not 1<=a.steps<=10000:p.error('steps must be 1..10000')
 if a.directory.exists():p.error('output directory must be new; refusing to overwrite a pilot')
 a.directory.mkdir(parents=True)
 b=a.directory
 if a.dependency_directory:sys.path.insert(0,str(a.dependency_directory))
 result=dict(production_promoted=False,human_labeled_training=False,steps=a.steps)
 try:
  import torch,onnx,numpy as np,onnxruntime as ort
  torch.set_num_threads(2);torch.set_num_interop_threads(1)
  from onnx2torch import convert
  model=convert(str(a.onnx)).eval();result['trainable_parameters']=sum(p.numel() for p in model.parameters() if p.requires_grad)
  from PIL import Image
  image=np.asarray(Image.open(str(a.image)).convert('RGB'))
  if image.shape!=(240,320,3):raise ValueError('expected corrected 320x240 RGB image')
  image=image[:,:,::-1].copy().transpose(2,0,1)[None].astype(np.float32)/255
  options=ort.SessionOptions();options.intra_op_num_threads=2;options.inter_op_num_threads=1
  reference=ort.InferenceSession(str(a.onnx),options,providers=['CPUExecutionProvider']);expected=reference.run(None,{reference.get_inputs()[0].name:image})
  with torch.no_grad():actual=model(torch.from_numpy(image))
  result['fp32_max_errors']=[float(np.abs(a-b.detach().numpy()).max()) for a,b in zip(expected,actual)];assert max(result['fp32_max_errors'])<.002
  from torch.ao.quantization.quantize_fx import prepare_qat_fx
  from torch.ao.quantization import get_default_qat_qconfig_mapping
  from torch.ao.quantization.fx.custom_config import PrepareCustomConfig
  # Preserve ONNX control/shape/resize operators as real leaf modules. Convs
  # remain ordinary trainable nn.Conv2d modules and can receive fake quantizers.
  leaf_types=list({type(m) for m in model.modules() if type(m).__module__.startswith('onnx2torch')})
  custom=PrepareCustomConfig().set_non_traceable_module_classes(leaf_types)
  from torch.ao.quantization.backend_config import get_qnnpack_backend_config,BackendConfig
  native=get_qnnpack_backend_config()
  def includes_transpose(pattern):
   return any(includes_transpose(p) for p in pattern) if isinstance(pattern,tuple) else pattern==torch.nn.ConvTranspose2d
  backend=BackendConfig('qnnpack_without_unsupported_qat_deconv_fusion').set_backend_pattern_configs([p for p in native.configs if not includes_transpose(p.pattern)])
  mapping=get_default_qat_qconfig_mapping('qnnpack').set_object_type(torch.nn.ConvTranspose2d,None)
  candidate=prepare_qat_fx(model.train(),mapping,(torch.from_numpy(image),),prepare_custom_config=custom,backend_config=backend);result['qat_prepare']='accepted';result['fake_quantizers']=sum('FakeQuantize' in type(m).__name__ for m in candidate.modules())
  optimizer=torch.optim.SGD(candidate.parameters(),lr=1e-6)
  for step in range(a.steps):
   optimizer.zero_grad();outputs=candidate(torch.from_numpy(image));loss=sum(torch.nn.functional.mse_loss(o,torch.from_numpy(t)) for o,t in zip(outputs,expected));loss.backward();optimizer.step();result.setdefault('pilot_loss',[]).append(float(loss.detach()))
  candidate.eval();candidate.apply(torch.ao.quantization.disable_observer)
  with torch.no_grad():before_export=candidate(torch.from_numpy(image))
  # PyTorch 2.2's fused observer kernel has no ONNX symbolic. Replace only
  # the frozen fake-quant implementation, preserving all learned qparams.
  from torch.ao.quantization import FakeQuantize,FusedMovingAvgObsFakeQuantize
  replacements={};visited=set()
  def replace_fused(parent):
   if id(parent) in visited:return
   visited.add(id(parent))
   for name,module in list(parent._modules.items()):
    if isinstance(module,FusedMovingAvgObsFakeQuantize):
     if id(module) not in replacements:
      kwargs=dict(observer=type(module.activation_post_process),quant_min=module.quant_min,quant_max=module.quant_max,dtype=module.dtype,qscheme=module.qscheme)
      if module.qscheme in (torch.per_channel_affine,torch.per_channel_symmetric):kwargs['ch_axis']=module.ch_axis
      replacement=FakeQuantize(**kwargs);replacement.load_state_dict(module.state_dict());replacement.disable_observer();replacements[id(module)]=replacement
     parent._modules[name]=replacements[id(module)]
    elif module is not None:replace_fused(module)
  replace_fused(candidate);replaced=len(replacements)
  result['remaining_fused_fake_quant']=sum(isinstance(m,FusedMovingAvgObsFakeQuantize) for m in candidate.modules());assert result['remaining_fused_fake_quant']==0
  with torch.no_grad():after_export=candidate(torch.from_numpy(image))
  result['export_adapter_max_errors']=[float((a-z).abs().max()) for a,z in zip(before_export,after_export)];assert max(result['export_adapter_max_errors'])==0
  result['fused_fake_quant_replaced']=replaced
  torch.onnx.export(candidate,torch.from_numpy(image),str(b/'qat-pilot.onnx'),input_names=['rgb'],output_names=['drivable','lane'],opset_version=13,do_constant_folding=True)
  result['qat_export']='accepted; two-step pipeline check only, not adequately trained'
 except Exception as e:result.update(error_type=type(e).__name__,error=str(e)[:1500]);traceback.print_exc()
 (b/'qat-trainable.json').write_text(json.dumps(result,indent=2));print(json.dumps(result),flush=True)
 return 1 if 'error' in result else 0

if __name__=='__main__':raise SystemExit(main())
