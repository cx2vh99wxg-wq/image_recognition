from PIL import Image, ImageDraw, ImageFont
from xml.sax.saxutils import escape
W,H=2200,1450
im=Image.new('RGB',(W,H),'white');d=ImageDraw.Draw(im)
font='C:/Windows/Fonts/msyh.ttc'; bold='C:/Windows/Fonts/msyhbd.ttc'
svg=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">','<rect width="100%" height="100%" fill="white"/>']
def rect(x,y,w,h,fill='#fff',stroke='#444',sw=2):
 d.rectangle((x,y,x+w,y+h),fill=fill,outline=stroke,width=sw);svg.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}"/>')
def text(x,y,s,size=23,b=False):
 d.text((x,y),s,font=ImageFont.truetype(bold if b else font,size),fill='#222',anchor='mm');svg.append(f'<text x="{x}" y="{y}" text-anchor="middle" dominant-baseline="central" font-family="Microsoft YaHei,sans-serif" font-size="{size}" font-weight="{"bold" if b else "normal"}" fill="#222">{escape(s)}</text>')
def wire(points,w=3):
 d.line(points,fill='#444',width=w,joint='curve');svg.append('<polyline points="'+' '.join(f'{x},{y}' for x,y in points)+f'" fill="none" stroke="#444" stroke-width="{w}"/>')
def connector(x,y):
 rect(x-10,y-10,20,20,'#fff','#444',2);wire([(x-9,y-9),(x+9,y+9)],1);wire([(x+9,y-9),(x-9,y+9)],1)
def block(x,y,w,h,lines,fill='#fff',sizes=None):
 rect(x,y,w,h,fill); sizes=sizes or [22]*len(lines)
 for i,s in enumerate(lines):text(x+w/2,y+h/2+(i-(len(lines)-1)/2)*31,s,sizes[i],i==0)
text(1100,48,'双板异构协同智能小车系统总体架构',35,True)
text(1100,91,'多路视觉采集 · FPGA 硬件处理 · ARM 智能感知 · 车辆控制',22)
# parallel board panels
for y,tag in [(140,'M'),(670,'S')]:
 rect(35,y,1625,450,'#f6f6f6','#777',2);text(325,y+29,f'{tag} 端  RK3568_MES2L100H 开发板',25,True)
 rect(65,y+67,230,345,'#ededed');text(180,y+93,'摄像头输入',23,True)
 for i in range(3): block(90,y+125+i*78,180,55,[f'OV5640  {i+1 if tag=="M" else i+4}'],sizes=[21])
 rect(350,y+67,560,345,'#ededed');text(630,y+95,f'{tag}-FPGA  ·  Logos-2',25,True)
 block(375,y+130,210,90,['三路采集','DVP / I²C'],sizes=[21,19]);block(635,y+130,245,90,['图像帧缓存','DDR3 控制器'],sizes=[21,19])
 wire([(585,y+175),(635,y+175)]);connector(610,y+175)
 block(375,y+260,505,84,['帧数据整理与 PCIe DMA','板载 FPGA DDR3 提供帧缓存'],sizes=[21,19])
 wire([(757,y+220),(757,y+260)]);connector(757,y+240)
 wire([(295,y+245),(350,y+245)]);connector(322,y+245);text(322,y+220,'DVP',16);text(322,y+269,'I²C',16)
 rect(975,y+67,650,345,'#ededed');text(1300,y+95,f'{tag}-RK3568J  ·  ARM / Linux',25,True)
 block(1000,y+130,185,90,['PCIe 接收','驱动与帧数据'],sizes=[20,18]);block(1235,y+130,180,90,['共享内存','图像 / 状态'],sizes=[20,18]);block(1465,y+130,135,90,['进程','协同'],sizes=[20,19])
 wire([(1185,y+175),(1235,y+175)]);connector(1210,y+175);wire([(1415,y+175),(1465,y+175)]);connector(1440,y+175)
 if tag=='M': lines=['YOLOPv2：车道线 / 可行驶区域','YOLOv5n：交通信号灯识别']
 else: lines=['YOLOv5s：80 类通用目标识别','行人制动 / 车道信息处理']
 block(1000,y+260,600,84,lines,sizes=[21,20]);wire([(1325,y+220),(1325,y+260)]);connector(1325,y+240)
 wire([(910,y+305),(940,y+305),(940,y+175),(975,y+175)]);connector(940,y+175);text(947,y+270,'PCIe',18,True)
# UDP joins
block(1760,340,360,100,['以太网 / UDP','M 端视频及感知信息传至 S 端'],'#eeeeee',[23,19])
wire([(1625,340),(1695,340),(1695,390),(1760,390)]);connector(1695,390)
wire([(1940,440),(2140,440),(2140,870),(1625,870)]);connector(1695,870)
# display
block(1760,690,360,105,['HDMI 显示设备','六路画面与识别信息'],'#eeeeee',[23,19]);wire([(1625,737),(1760,737)]);connector(1695,737);text(1695,716,'HDMI',17)
# control interconnects laid out under board
block(55,1190,390,112,['单路 CNN 夜视模块','RGB 转换 / 卷积 / Gamma 校正'],'#eeeeee',[23,19]);text(250,1323,'接入板卡以实际烧录版本为准',17)
block(515,1190,290,112,['HMI 串口屏','模式选择 / 人工操作'],'#eeeeee',[23,19])
block(855,1190,290,112,['FMC 扩展板','控制信号引脚扩展'],'#eeeeee',[23,19])
block(1195,1190,320,112,['射频遥控信号板','方向 / 停止触点控制'],'#eeeeee',[23,19])
block(1565,1190,300,112,['智能小车','行驶 / 转向 / 刹停'],'#eeeeee',[23,19])
wire([(1145,1246),(1195,1246)]);connector(1170,1246);text(1170,1224,'GPIO',16)
wire([(1515,1246),(1565,1246)]);connector(1540,1246);text(1540,1224,'射频',16)
# S FPGA / RK control, separate tracks
wire([(550,1082),(550,1150),(660,1150),(660,1190)]);connector(550,1150);text(610,1130,'UART',17,True)
wire([(750,1082),(750,1170),(1000,1170),(1000,1190)]);connector(1000,1170);text(872,1150,'控制引脚',17)
wire([(800,1082),(800,1120),(1325,1120),(1325,1082)]);connector(800,1120);connector(1325,1120);text(1065,1100,'FSPI：控制寄存器 / 仲裁 / 制动锁存',18,True)
# CNN association to generic FPGA, no uncertain physical endpoint
text(1100,1384,'注：共享内存为各 RK 端本地进程间通信；DDR3 为 FPGA 视频缓存；接口方框表示信号连接。',19)
svg.append('</svg>')
im.save('output/智能小车总体架构图.png',dpi=(220,220))
open('output/智能小车总体架构图.svg','w',encoding='utf-8').write('\n'.join(svg))
