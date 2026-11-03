from pathlib import Path
import json,re
w=Path(__file__).resolve().parent
root=w.parents[1]/'udp_rk_rk_yolo'
paths=json.loads((w/'manifest.json').read_text())
headers={
 'M/imu/Makefile':'构建 IMU 工具；测试入口由同一 Python 驱动执行。',
 'S/steering/Makefile':'默认生成转向工具，实板和测试构建各有独立入口。',
 'S/steering/actuator/Makefile':'从执行器目录调用转向构建驱动。',
 'common/steering/Makefile':'验证两端共享的转向契约。',
}
for rel,note in headers.items():
 p=root/rel; data=p.read_bytes(); b=w/'before'/rel;b.parent.mkdir(parents=True,exist_ok=True);b.write_bytes(data)
 nl=b'\r\n' if b'\r\n' in data else b'\n'
 p.write_bytes(('# '+note).encode()+nl+data);paths.append(rel)
rel='S/udp/csrc/main.cpp'
p=root/rel; data=p.read_bytes(); old=b'UDP image publication failed';new=b'Could not publish the UDP video frame'
assert data.count(old)==1
b=w/'before'/rel;b.parent.mkdir(parents=True,exist_ok=True);b.write_bytes(data)
p.write_bytes(data.replace(old,new));paths.append(rel)
for rel in paths:
 p=root/rel; data=p.read_bytes()
 clean=data.decode().replace('未已完成','未完成').replace('说明：- ','- ')
 p.write_bytes(clean.encode())
(w/'manifest.json').write_text(json.dumps(sorted(paths),indent=2),encoding='utf-8')
