"""Decode saved YOLOv5/v8 probe tensors only; no live control interface.

Uses each model's own head contract. YOLOv8 class scores do not have a YOLOv5
objectness multiplier. Scores/boxes here are detector evidence, before the
production traffic-housing/color and pedestrian-road association gates.
"""
import argparse
import json
from pathlib import Path
import numpy as np


def decode(prefix,family,threshold=.25):
    meta=json.loads(Path(str(prefix)+'.json').read_text());outputs=[]
    for a in meta['outputs']:
        path=str(prefix)+'.'+str(a['index'])+('.f32' if meta['float_outputs'] else '.raw')
        if meta['float_outputs']:value=np.fromfile(path,dtype='<f4')
        else:
            if a['type']!=2:raise ValueError('native probe requires INT8 tensors')
            value=(np.fromfile(path,dtype='i1').astype(np.float32)-a['zero_point'])*a['scale']
        if value.size!=int(np.prod(a['shape'])) or len(a['shape'])!=4:raise ValueError('tensor size')
        value=value.reshape(a['shape'])
        if a['format']==1:value=value.transpose(0,3,1,2)
        elif a['format']!=0:raise ValueError('tensor layout')
        if not np.isfinite(value).all():raise ValueError('nonfinite tensor')
        outputs.append(value)
    boxes=[]
    def append(xyxy,cls,score):
        if cls in (0,9) and score>=threshold:
            b=np.clip(np.asarray(xyxy,dtype=np.float32),0,320)
            if b[2]>b[0] and b[3]>b[1]:boxes.append(dict(class_id=int(cls),score=float(score),xyxy=b.tolist()))
    if family=='v5':
        anchors=np.asarray([[[10,13],[16,30],[33,23]],[[30,61],[62,45],[59,119]],[[116,90],[156,198],[373,326]]])
        if len(outputs)!=3:raise ValueError('expected three YOLOv5 heads')
        for out in outputs:
            _,c,h,w=out.shape;stride=320//w
            if c!=255 or h!=w or stride not in (8,16,32):raise ValueError('YOLOv5 layout')
            if out.min()<-.01 or out.max()>1.02:raise ValueError('YOLOv5 probabilities')
            a=anchors[(8,16,32).index(stride)];v=out.reshape(3,85,h,w)
            ids=v[:,5:].argmax(axis=1);scores=v[:,4]*v[:,5:].max(axis=1)
            for k,y,x in zip(*np.where((scores>=threshold)&np.isin(ids,[0,9]))):
                cx=(2*v[k,0,y,x]-.5+x)*stride;cy=(2*v[k,1,y,x]-.5+y)*stride
                bw=4*v[k,2,y,x]**2*a[k,0];bh=4*v[k,3,y,x]**2*a[k,1]
                append([cx-bw/2,cy-bh/2,cx+bw/2,cy+bh/2],ids[k,y,x],scores[k,y,x])
    else:
        if len(outputs)!=9:raise ValueError('expected official nine-output YOLOv8 export')
        for i in range(0,9,3):
            reg,cls=outputs[i:i+2];_,c,h,w=reg.shape;stride=320//w
            if c!=64 or cls.shape!=(1,80,h,w) or stride not in (8,16,32):raise ValueError('YOLOv8 layout')
            if cls.min()<-.01 or cls.max()>1.02:raise ValueError('YOLOv8 probabilities')
            d=reg.reshape(4,16,h,w);d=np.exp(d-d.max(axis=1,keepdims=True));d/=d.sum(axis=1,keepdims=True)
            d=(d*np.arange(16,dtype=np.float32)[None,:,None,None]).sum(axis=1)
            ids=cls[0].argmax(axis=0);scores=cls[0].max(axis=0)
            for y,x in zip(*np.where((scores>=threshold)&np.isin(ids,[0,9]))):
                l,t,r,b=d[:,y,x]
                append([(x+.5-l)*stride,(y+.5-t)*stride,(x+.5+r)*stride,(y+.5+b)*stride],ids[y,x],scores[y,x])
    selected=[]
    for box in sorted(boxes,key=lambda b:b['score'],reverse=True):
        a=np.asarray(box['xyxy']);area=(a[2]-a[0])*(a[3]-a[1]);keep=True
        for old in selected:
            if old['class_id']!=box['class_id']:continue
            b=np.asarray(old['xyxy']);wh=np.maximum(0,np.minimum(a[2:],b[2:])-np.maximum(a[:2],b[:2]));inter=float(np.prod(wh))
            if inter/(area+(b[2]-b[0])*(b[3]-b[1])-inter)>.45:keep=False;break
        if keep:selected.append(box)
    return dict(family=family,threshold=threshold,candidates=len(boxes),detections=selected,
                production_promoted=False,note='Saved detector evidence only; not labeled accuracy or brake validation')


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('prefix',type=Path);p.add_argument('--family',choices=('v5','v8'),required=True)
    args=p.parse_args();print(json.dumps(decode(args.prefix,args.family),indent=2))


if __name__=='__main__':main()
