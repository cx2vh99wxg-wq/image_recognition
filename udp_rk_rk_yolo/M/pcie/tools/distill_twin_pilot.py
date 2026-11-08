"""Offline, CPU-only TwinLiteNet+ distillation pilot; never promotes a model.

Teacher is the deployed two-head YOLOPv2 ONNX, not human ground truth. Train
images must exclude entire held-out scenes. This small pilot verifies training
and candidate export; its loss cannot establish driving accuracy or safety.
"""
import argparse
import hashlib
import json
from pathlib import Path
import sys
import types


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('source',type=Path)
    p.add_argument('weights',type=Path)
    p.add_argument('teacher',type=Path)
    p.add_argument('dataset',type=Path)
    p.add_argument('output',type=Path)
    p.add_argument('--steps',type=int,default=84)
    p.add_argument('--learning-rate',type=float,default=1e-4)
    args=p.parse_args()
    if args.output.exists():p.error('refusing to overwrite a checkpoint')
    if not 1<=args.steps<=10000:p.error('steps must be 1..10000')
    import torch
    import numpy as np
    import cv2
    import onnxruntime as ort
    torch.set_num_threads(2);torch.set_num_interop_threads(1);torch.manual_seed(431)
    sys.path.insert(0,str(args.source.resolve()))
    source=args.source/'model/model.py'
    module=types.ModuleType('model.model');module.__file__=str(source)
    sys.modules['model.model']=module
    exec(compile(source.read_text().replace('import matplotlib.pyplot as plt',''),str(source),'exec'),module.__dict__)
    student=module.TwinLiteNetPlus(types.SimpleNamespace(config='small'))
    student.load_state_dict(torch.load(str(args.weights),map_location='cpu',weights_only=True),strict=True)
    # One image per batch: keep pretrained BatchNorm statistics fixed. The
    # convolution/PReLU parameters remain trainable; no live model is touched.
    student.eval()
    opts=ort.SessionOptions();opts.intra_op_num_threads=2;opts.inter_op_num_threads=1
    teacher=ort.InferenceSession(str(args.teacher),opts,providers=['CPUExecutionProvider'])
    images=[]
    for line in args.dataset.read_text().splitlines():
        path=Path(line.strip())
        if not line.strip():continue
        image=cv2.imread(str(path))
        if image is None or image.shape!=(240,320,3):raise ValueError('expected corrected RGB 320x240 source: '+str(path))
        rgb=cv2.cvtColor(image,cv2.COLOR_BGR2RGB)
        x=rgb.transpose(2,0,1).copy()[None].astype(np.float32)/255
        # The formal legacy engine feeds BGR bytes. Preserve its decisions
        # as the teacher while retaining the student's required RGB input.
        teacher_input=image.transpose(2,0,1).copy()[None].astype(np.float32)/255
        road,lane=teacher.run(['drivable','lane'],{'rgb':teacher_input})
        if road.shape!=(1,2,240,320) or lane.shape!=(1,1,240,320):raise ValueError('teacher contract')
        if not all(np.isfinite(v).all() and v.min()>=0 and v.max()<=1.001 for v in (road,lane)):raise ValueError('teacher probabilities')
        # Preserve road argmax, and preserve the existing lane p>0.3 decision
        # boundary when adapting it to a two-class student's p>0.5 output.
        road=road/np.maximum(road.sum(axis=1,keepdims=True),1e-7)
        lane=np.clip(lane,1e-5,1-1e-5)
        odds=lane/(1-lane)*(.7/.3);lane=odds/(1+odds)
        lane=np.concatenate((1-lane,lane),axis=1)
        images.append((torch.from_numpy(x),torch.from_numpy(road),torch.from_numpy(lane),
                       str(path),hashlib.sha256(path.read_bytes()).hexdigest()))
    if not images:raise ValueError('empty pilot dataset')
    optimizer=torch.optim.Adam(student.parameters(),lr=args.learning_rate)
    def loss(x,road,lane):
        r,l=student(x)
        rloss=-(road*torch.log_softmax(r,dim=1)).sum(dim=1).mean()
        # Thin foreground should contribute without counting blank pixels as
        # an accuracy gain. These weights are recorded, not tuned on holdout.
        weight=1+7*lane[:,1]
        lloss=(-(lane*torch.log_softmax(l,dim=1)).sum(dim=1)*weight).sum()/weight.sum()
        return rloss+lloss
    with torch.no_grad():before=[float(loss(*row[:3])) for row in images]
    history=[]
    for step in range(args.steps):
        x,r,l,*_=images[step%len(images)]
        optimizer.zero_grad(set_to_none=True);value=loss(x,r,l)
        if not torch.isfinite(value):raise ValueError('non-finite training loss')
        value.backward();torch.nn.utils.clip_grad_norm_(student.parameters(),1.)
        optimizer.step();history.append(float(value.detach()))
        if step%7==0:print('DISTILL_STEP',step,history[-1],flush=True)
    with torch.no_grad():after=[float(loss(*row[:3])) for row in images]
    torch.save(student.state_dict(),str(args.output))
    result=dict(teacher_sha256=hashlib.sha256(args.teacher.read_bytes()).hexdigest(),
                initial_weights_sha256=hashlib.sha256(args.weights.read_bytes()).hexdigest(),
                output_sha256=hashlib.sha256(args.output.read_bytes()).hexdigest(),
                student='TwinLiteNet+ Small 320x240',steps=args.steps,learning_rate=args.learning_rate,
                teacher_input='BGR /255 matching deployed legacy engine',student_input='RGB /255 matching author',
                sources=[dict(path=row[3],sha256=row[4]) for row in images],
                batch_norm='frozen statistics',lane_foreground_weight=8,
                before_teacher_loss=float(np.mean(before)),after_teacher_loss=float(np.mean(after)),history=history,
                human_labels=False,production_promoted=False,
                limitation='Small teacher-labeled pilot; evaluate independent scenes and pedestrian road relation before promotion')
    args.output.with_suffix('.json').write_text(json.dumps(result,indent=2))
    print('DISTILL_PILOT_FINISHED',result['before_teacher_loss'],result['after_teacher_loss'],flush=True)


if __name__=='__main__':main()
