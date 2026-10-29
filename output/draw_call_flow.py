from PIL import Image,ImageDraw,ImageFont
from xml.sax.saxutils import escape
W,H=2200,1650
im=Image.new('RGB',(W,H),'white');d=ImageDraw.Draw(im);svg=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}"><rect width="100%" height="100%" fill="white"/>']
f='C:/Windows/Fonts/msyh.ttc';fb='C:/Windows/Fonts/msyhbd.ttc'
def rect(x,y,w,h,fill='#fff',stroke='#555',sw=2):
 d.rectangle((x,y,x+w,y+h),fill=fill,outline=stroke,width=sw);svg.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}"/>')
def text(x,y,s,size=22,b=False):
 d.text((x,y),s,font=ImageFont.truetype(fb if b else f,size),fill='#222',anchor='mm');svg.append(f'<text x="{x}" y="{y}" text-anchor="middle" dominant-baseline="central" font-family="Microsoft YaHei,sans-serif" font-size="{size}" font-weight="{"bold" if b else "normal"}" fill="#222">{escape(s)}</text>')
def box(x,y,w,h,lines,fill='#fff',fs=22):
 rect(x,y,w,h,fill)
 for i,s in enumerate(lines):text(x+w/2,y+h/2+(i-(len(lines)-1)/2)*30,s,fs if i==0 else fs-2,i==0)
def wire(pts):
 d.line(pts,fill='#444',width=3,joint='curve');svg.append('<polyline fill="none" stroke="#444" stroke-width="3" points="'+' '.join(f'{x},{y}' for x,y in pts)+'"/>')
def joint(x,y):
 rect(x-9,y-9,18,18,'#fff','#444',2);wire([(x-8,y-8),(x+8,y+8)]);wire([(x+8,y-8),(x-8,y+8)])
text(1100,43,'智能小车系统调用与软硬协同链路',35,True)
text(1100,84,'FPGA 图像采集及控制  ·  ARM 智能识别  ·  双板数据协同',21)
# parallel board regions
for x,tag in [(35,'M'),(1135,'S')]:
 rect(x,125,1030,1040,'#f6f6f6','#888',2);text(x+515,156,tag+' 端｜RK3568_MES2L100H',27,True)
 box(x+35,190,400,80,['OV5640 × 3','DVP 图像数据 / I²C 参数配置'],'#fff',22)
 rect(x+35,305,400,423,'#e7e7e7');text(x+235,336,tag+'-FPGA｜Logos-2',24,True)
 box(x+60,365,350,86,['OV5640 驱动与采集','帧同步 / RGB565 视频数据'],'#fff',21)
 box(x+60,485,350,86,['DDR3 控制与帧缓存','三路写入 / 帧数据整理'],'#fff',21)
 box(x+60,605,350,86,['PCIe DMA / 发送','从缓存读取并传输帧数据'],'#fff',21)
 for a,b in [(270,305),(451,485),(571,605)]:wire([(x+235,a),(x+235,b)]);joint(x+235,(a+b)//2)
 rect(x+510,190,485,852,'#e7e7e7');text(x+752,221,tag+'-RK3568J｜ARM / Linux',24,True)
 box(x+535,255,435,86,['PCIe 驱动与采集进程','接收图像帧 / 转换与发布'],'#fff',21)
 box(x+535,377,435,86,['本板共享内存','图像快照 / 状态 / 感知结果'],'#fff',21)
 wire([(x+435,648),(x+475,648),(x+475,298),(x+510,298)]);joint(x+475,298);text(x+473,610,'PCIe',17,True)
 wire([(x+752,341),(x+752,377)]);joint(x+752,359)
 if tag=='M':
  box(x+535,505,435,121,['YOLOPv2 + 车道后处理','车道掩码 / 可行驶区域','漆线采样 → 实虚线 / 弯道'],'#fff',20)
  box(x+535,670,435,121,['YOLOv5n + 颜色判断','定位信号灯 / RGB 像素分析','多帧确认灯色'],'#fff',20)
  box(x+535,847,435,112,['UDP 发送进程','图像帧 / 车道及灯色元数据'],'#fff',21)
 else:
  box(x+535,505,435,121,['YOLOv5s 目标识别','通用目标 / person 触发判断','发布制动请求'],'#fff',20)
  box(x+535,670,435,121,['控制进程 / FSPI','车道与灯色规则 / 行人制动','读取共享内存并写控制寄存器'],'#fff',20)
  box(x+535,847,435,112,['X11 图像合成与显示','六路画面 / 识别叠加 / 方向报警'],'#fff',20)
 wire([(x+752,463),(x+752,505)]);joint(x+752,484)
 if tag=='M':
  wire([(x+535,420),(x+520,420),(x+520,903),(x+535,903)])
  wire([(x+520,565),(x+535,565)]);joint(x+520,565)
  wire([(x+520,730),(x+535,730)]);joint(x+520,730)
 else:
  wire([(x+752,626),(x+752,670)]);joint(x+752,648)
  wire([(x+752,791),(x+752,847)]);joint(x+752,819)
# board to board communication lower gap (separate block)
box(615,1210,970,75,['以太网 / UDP：M → S 视频帧 + 车道线型 / 弯道 / 信号灯信息'],'#ededed',21)
wire([(1005,1084),(1005,1187),(775,1187),(775,1210)]);joint(1005,1187)
wire([(1425,1210),(1425,1187),(1610,1187),(1610,1028),(1670,1028)]);joint(1610,1187)
# External and FPGA control section
box(35,1340,340,91,['CNN 单路夜视处理','RGB 转换 / 卷积 / Gamma 校正'],'#ededed',20)
box(415,1340,290,91,['HMI 串口屏','操作 / 模式 / 夜视调节'],'#ededed',20)
box(745,1340,390,91,['S-FPGA 控制模块','UART 解析 / FSPI 寄存器','方向仲裁 / 制动锁存'],'#ededed',20)
box(1175,1340,275,91,['FMC 扩展','控制信号引脚'],'#ededed',20)
box(1490,1340,330,91,['射频遥控信号板','方向与停止控制'],'#ededed',20)
box(1860,1340,300,91,['小车执行机构','前进 / 转向 / 制动'],'#ededed',20)
wire([(705,1385),(745,1385)]);joint(725,1385);text(725,1364,'UART',15)
wire([(1135,1385),(1175,1385)]);joint(1155,1385);wire([(1450,1385),(1490,1385)]);joint(1470,1385);wire([(1820,1385),(1860,1385)]);joint(1840,1385)
# link control from ARM S to FPGA block via right outside board
wire([(2105,855),(2180,855),(2180,1305),(940,1305),(940,1340)]);joint(2180,1305);text(1750,1280,'FSPI：S 端控制进程写寄存器',19,True)
box(1710,1100,360,56,['HDMI：六路画面与识别结果'],'#ededed',19);wire([(1890,1084),(1890,1100)]);joint(1890,1092)
text(1100,1495,'说明：各板共享内存用于本板进程通信；两板间使用 UDP；方框内“×”表示接口连接。',19)
text(1100,1531,'CNN 接入的板卡以实际烧录固件为准；本图展示功能调用关系，不代表每个模块均经过逐项实车验证。',19)
svg.append('</svg>');im.save('output/智能小车系统调用链路图.png',dpi=(220,220));open('output/智能小车系统调用链路图.svg','w',encoding='utf-8').write('\n'.join(svg))
