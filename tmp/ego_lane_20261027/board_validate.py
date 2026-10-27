import pathlib,subprocess,json,re,struct
root=pathlib.Path('/home/linaro/M/ego_lane_20261027')
exe=root/'candidate/pcie/build/line_style_replay'
samples=pathlib.Path('/tmp/line_left_samples_20261027')
out=root/'validation';out.mkdir(exist_ok=True)
records=[]
for n in range(12):
    name='sample_%02d'%n;paths=[]
    for suffix,ext,source,channels in [('tile','ppm',name+'.ppm',3),('lane','pgm',name+'_mask.pgm',1)]:
        with (samples/source).open('rb') as f:
            magic=f.readline();w,h=map(int,f.readline().split());assert f.readline().strip()==b'255';pixels=f.read()
        assert w==640 and h==480
        tile=b''.join(pixels[y*w*channels:y*w*channels+320*channels] for y in range(240))
        p=out/(name+'_'+suffix+'.'+ext);p.write_bytes(magic+b'320 240\n255\n'+tile);paths.append(p)
    floats=struct.unpack('<%df'%(640*480*2),(samples/(name+'_probe_drivable.f32')).read_bytes())
    green=bytes(255 if floats[640*480+y*640+x]>floats[y*640+x] else 0 for y in range(240) for x in range(320))
    p=out/(name+'_green.pgm');p.write_bytes(b'P5\n320 240\n255\n'+green);paths.append(p)
    text=subprocess.check_output([str(exe),str(paths[0]),str(paths[1]),str(out/(name+'.csv')),'20',str(paths[2])],text=True)
    got=re.findall(r'^(?:LEFT|RIGHT) (\w+) ',text,re.M)
    records.append(dict(name=name,got=got,detail=text))
    print(name,got,re.findall(r'PERF.*',text)[0],flush=True)
(out/'arm_saved_results.json').write_text(json.dumps(records,indent=2))
assert sum(r['got'][0]=='SOLID' for r in records)==11
assert all(r['got'][0]!='DASHED' and r['got'][1]!='SOLID' for r in records)
model=pathlib.Path('/home/linaro/M/pcie/model/yolopv2_Nx3x480x640_rk3568.rknn')
assert model.is_file()
probe=root/'candidate/pcie/build/line_style_model_probe'
with (out/'model_probe.log').open('w') as f:
    subprocess.run([str(probe),str(model),str(samples/'sample_00.ppm'),str(out/'actual_model')],stdout=f,stderr=subprocess.STDOUT,check=True)
text=(out/'model_probe.log').read_text()
assert 'PROBE LEFT SOLID' in text and 'PROBE RIGHT DASHED' in text,text[-2000:]
print('ACTUAL_MODEL_LEFT_SOLID_RIGHT_DASHED',flush=True)
print('ARM_SAVED_VALIDATION_PASS',flush=True)
