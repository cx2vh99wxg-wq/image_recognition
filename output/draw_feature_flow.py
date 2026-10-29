from PIL import Image,ImageDraw,ImageFont
from xml.sax.saxutils import escape
W,H=1600,1720
im=Image.new('RGB',(W,H),'white');d=ImageDraw.Draw(im);svg=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}"><rect width="100%" height="100%" fill="white"/>']
f='C:/Windows/Fonts/msyh.ttc';fb='C:/Windows/Fonts/msyhbd.ttc'
def rect(x,y,w,h,fill='#fff',stroke='#555',sw=2):
 d.rectangle((x,y,x+w,y+h),fill=fill,outline=stroke,width=sw);svg.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}"/>')
def text(x,y,s,size=23,b=False):
 d.text((x,y),s,font=ImageFont.truetype(fb if b else f,size),fill='#222',anchor='mm');svg.append(f'<text x="{x}" y="{y}" text-anchor="middle" dominant-baseline="central" font-family="Microsoft YaHei,sans-serif" font-size="{size}" font-weight="{"bold" if b else "normal"}" fill="#222">{escape(s)}</text>')
def box(x,y,w,h,lines,fill='#fff',size=23):
 rect(x,y,w,h,fill)
 for i,s in enumerate(lines):text(x+w/2,y+h/2+(i-(len(lines)-1)/2)*30,s,size if i==0 else size-2,i==0)
def wire(points):
 d.line(points,fill='#444',width=3,joint='curve');svg.append('<polyline fill="none" stroke="#444" stroke-width="3" points="'+' '.join(f'{x},{y}' for x,y in points)+'"/>')
def joint(x,y):
 rect(x-9,y-9,18,18,'#fff','#444',2);wire([(x-8,y-8),(x+8,y+8)]);wire([(x+8,y-8),(x-8,y+8)])
text(800,44,'智能小车功能与特性链路图',34,True)
text(800,86,'多视角采集 · 嵌入式感知 · 交互提示 · 辅助控制',21)
# sampling stage
rect(80,130,1440,222,'#f3f3f3');text(800,162,'图像采集与硬件处理｜FPGA',26,True)
box(110,200,300,112,['六路 OV5640','双板各三路 / 多方向采集'],size=21)
box(470,200,300,112,['视频采集','帧同步 / RGB565 数据'],size=21)
box(830,200,300,112,['单路 CNN 夜视','卷积 / Gamma 校正；其余直通'],size=21)
box(1190,200,300,112,['缓存与图像传输','DDR3 / PCIe / UDP'],size=21)
wire([(410,256),(470,256)]);joint(440,256)
wire([(770,256),(830,256)]);joint(800,256)
wire([(1130,256),(1190,256)]);joint(1160,256)
text(800,334,'夜视增强应用于其中一路；其余画面沿采集与缓存链路传输',17)
# recognition stage
rect(80,410,1440,375,'#f3f3f3');text(800,442,'智能感知与场景判断｜ARM',26,True)
box(115,490,420,116,['车道与道路区域｜YOLOPv2','车道线掩码 / 可行驶区域'],size=21)
box(590,490,420,116,['交通信号灯｜YOLOv5n','灯体定位 / RGB 像素颜色分析'],size=21)
box(1065,490,420,116,['道路目标｜YOLOv5s','80 类目标识别 / 行人检测'],size=21)
box(115,650,420,88,['漆线采样与占空比','实虚线判断 / 弯道信息'],size=21)
box(590,650,420,88,['颜色和连续帧确认','红灯限制前进 / 绿灯解除限制'],size=21)
box(1065,650,420,88,['行人触发条件','生成制动请求'],size=21)
for x in [325,800,1275]:wire([(x,606),(x,650)]);joint(x,628)
# top to three branches
wire([(1340,312),(1340,379),(800,379),(800,410)]);joint(800,379)
wire([(800,463),(800,475),(325,475),(325,490)]);wire([(800,475),(800,490)]);wire([(800,475),(1275,475),(1275,490)]);joint(800,475)
# outcomes
rect(80,850,1440,292,'#f3f3f3');text(800,883,'功能联动与结果呈现',26,True)
box(115,925,420,150,['六路组合显示','画面缩放 / 视角切换','HDMI 输出识别与报警信息'],size=21)
box(590,925,420,150,['实线变道提示','拦截对应人工转向请求','界面显示相应方向红色“×”'],size=21)
box(1065,925,420,150,['辅助驾驶控制','红灯前进限制 / 弯道转向','行人触发制动'],size=21)
wire([(325,738),(325,815),(800,815),(800,850)]);wire([(800,738),(800,815)]);wire([(1275,738),(1275,815),(800,815)]);joint(800,815)
wire([(800,903),(800,912),(325,912),(325,925)]);wire([(800,912),(800,925)]);wire([(800,912),(1275,912),(1275,925)]);joint(800,912)
# user HMI interaction
rect(80,1205,1440,300,'#f3f3f3');text(800,1238,'人机交互与软硬协同',26,True)
box(115,1285,370,135,['HMI 串口屏','手动行驶 / 制动 / 模式选择','视角切换 / 夜视调节'],size=21)
box(615,1285,370,135,['FPGA 控制逻辑','UART 指令解析 / FSPI 寄存器','方向仲裁 / 制动锁存'],size=21)
box(1115,1285,370,135,['车辆执行','FMC 扩展信号 / 射频遥控板','车辆转向与制动'],size=21)
wire([(485,1352),(615,1352)]);joint(550,1352);text(550,1325,'UART',17,True)
wire([(985,1352),(1115,1352)]);joint(1050,1352);text(1050,1325,'控制信号',17,True)
wire([(1275,1142),(1275,1177),(800,1177),(800,1205)]);joint(800,1177);text(1030,1158,'识别结果经共享内存与 FSPI 参与控制',18)
text(800,1555,'注：组合显示为多画面布局；绿灯解除前进限制不等同于自动起步。',19)
text(800,1590,'线框“×”表示功能或接口连接；图中不表示模型串行调用。',19)
svg.append('</svg>');im.save('output/功能与特性链路图.png',dpi=(220,220));open('output/功能与特性链路图.svg','w',encoding='utf-8').write('\n'.join(svg))
