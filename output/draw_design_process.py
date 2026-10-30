from PIL import Image, ImageDraw, ImageFont
from xml.sax.saxutils import escape
W,H=1600,1250
im=Image.new('RGB',(W,H),'white'); d=ImageDraw.Draw(im)
font='C:/Windows/Fonts/msyh.ttc';bold='C:/Windows/Fonts/msyhbd.ttc'
svg=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">','<rect width="100%" height="100%" fill="white"/>']
def rect(x,y,w,h,fill,stroke='#777',sw=2):
 d.rounded_rectangle((x,y,x+w,y+h),radius=9,fill=fill,outline=stroke,width=sw);svg.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="9" fill="{fill}" stroke="{stroke}" stroke-width="{sw}"/>')
def text(x,y,s,size=24,b=False,col='#222'):
 d.text((x,y),s,font=ImageFont.truetype(bold if b else font,size),fill=col,anchor='mm');svg.append(f'<text x="{x}" y="{y}" text-anchor="middle" dominant-baseline="central" font-family="Microsoft YaHei,sans-serif" font-size="{size}" font-weight="{"bold" if b else "normal"}" fill="{col}">{escape(s)}</text>')
def line(points,arrow=True):
 d.line(points,fill='#555',width=3,joint='curve');svg.append('<polyline fill="none" stroke="#555" stroke-width="3" points="'+' '.join(f'{x},{y}' for x,y in points)+'"/>')
 if arrow:
  (x,y),(px,py)=points[-1],points[-2];import math;a=math.atan2(y-py,x-px);p=[(x,y),(x-13*math.cos(a-.48),y-13*math.sin(a-.48)),(x-13*math.cos(a+.48),y-13*math.sin(a+.48))];d.polygon(p,fill='#555');svg.append('<polygon fill="#555" points="'+' '.join(f'{a},{b}' for a,b in p)+'"/>')
text(800,47,'系统设计流程图',35,True)
text(800,91,'软件链路验证  →  硬件通路构建  →  智能感知与交互集成',21,col='#555')
# orderly two-column reading order, explicit continuation at bottom
rect(60,125,705,920,'#f6f6f6','#bbb');rect(835,125,705,920,'#f6f6f6','#bbb')
text(412,158,'链路建立与图像采集',23,True);text(1188,158,'智能功能与系统交互',23,True)
left=[('01','验证 UDP 通信','确认 M / S 板间通信协议与数据格式'),('02','建立 PCIe 通道','跑通驱动，连接 FPGA 与 ARM'),('03','打通摄像头数据流','接入 OV5640，验证图像采集'),('04','组合六路摄像头画面','合成多视角画面，形成统一显示')]
right=[('05','部署 YOLO 系列模型','识别车道线、道路区域与目标'),('06','实现 FPGA 夜视增强','CNN 判定 + Gamma 查表处理'),('07','打通射频无线链路','驱动射频信号板并联动车辆'),('08','接入 HMI 串口屏','实现操作交互与车辆控制')]
def panel(x,y,entry):
 num,title,desc=entry;rect(x,y,620,145,'#fff','#777');rect(x+18,y+20,66,53,'#dedede','#aaa');text(x+51,y+47,num,23,True);text(x+350,y+52,title,26,True);text(x+350,y+99,desc,20)
for i,e in enumerate(left):panel(100,190+i*202,e)
for i,e in enumerate(right):panel(880,190+i*202,e)
for x in (410,1190):
 for i in range(3):line([(x,335+i*202),(x,392+i*202)])
line([(720,868),(790,868),(790,178),(1190,178),(1190,190)])
rect(320,1090,960,90,'#e8e8e8','#888');text(800,1120,'系统联调与功能验证',24,True);text(800,1154,'感知 → 处理 → 传输 → 控制',22)
line([(1190,941),(1190,1065),(1000,1065),(1000,1090)])
svg.append('</svg>');im.save('output/系统设计流程图.png',dpi=(220,220));open('output/系统设计流程图.svg','w',encoding='utf-8').write('\n'.join(svg))
