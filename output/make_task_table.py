from PIL import Image, ImageDraw, ImageFont
from html import escape
rows=[
('图像采集与显示','完成采集、处理、显示全流程。','六路 OV5640 采集周边画面，经 HDMI 展示图像与识别结果。'),
('软硬协同与传输','实现板内高速通信和不少于四路视频传输。','双板各采集三路；板内经 PCIe 传图，板间以 UDP 交换视频及感知信息，进程间使用共享内存。'),
('图像处理与夜视','实现基础图像处理，适应复杂光照。','FPGA 完成格式转换；一路采用 CNN 卷积及 Gamma 校正进行夜视增强，效果待量化。'),
('道路与目标感知','识别车道、可行驶区域及道路目标。','M 端 YOLOPv2 识别车道线和可行驶区域；S 端 YOLOv5s 支持 80 类通用目标识别，准确率待实测。'),
('交通信号控制','识别红绿灯并实现行驶约束。','M 端 YOLOv5n 识别信号灯；代码包含红灯限制前进、绿灯解除限制，场景联动待验证。'),
('变道与弯道控制','识别车道线型，辅助变道及弯道行驶。','实现实线转向请求拦截、提示及弯道转向控制；自动完整变道尚待验证。'),
('危险场景与交互','识别行人等危险目标并进行控制。','行人检测可触发制动请求，经 FSPI 控制 FPGA；HMI 串口屏支持操作和模式选择。'),
('实时性与规划','测试帧率、延迟，并完成道路规划验证。','10 月 25 日测试：M 端发布约 30 fps，S 端接收约 30 fps，显示渲染约 12 fps；最新性能和导航规划待验证。')]

html=['<!doctype html><html lang="zh-CN"><meta charset="utf-8"><title>赛题任务完成情况</title><style>body{font-family:"Microsoft YaHei",sans-serif;color:#222;margin:12px auto;max-width:980px}h1{font-size:18px;text-align:center;margin:8px 0}table{border-collapse:collapse;width:100%;table-layout:fixed;font-size:12px;line-height:1.35}th,td{border:1px solid #aaa;padding:5px 8px;vertical-align:middle}th{background:#dedede}tbody tr:nth-child(even){background:#f6f6f6}th:nth-child(1){width:17%}th:nth-child(2){width:32%}th:nth-child(3){width:51%}td:first-child{font-weight:bold}p{font-size:11px;color:#555}@page{size:A4 portrait;margin:12mm}@media print{body{margin:0}table{font-size:10pt}th,td{padding:3pt 5pt}tr{break-inside:avoid}}</style><h1>赛题任务完成情况</h1><table><thead><tr><th>项目</th><th>得分要求</th><th>完成情况</th></tr></thead><tbody>']
for row in rows:html.append('<tr>'+''.join('<td>'+escape(v)+'</td>' for v in row)+'</tr>')
html.append('</tbody></table><p>注：“待验证”表示已有实现或设计依据，但仍需结合最终版本及现场测试结果确认；赛题要求经概括，具体考核以赛题指南为准。</p></html>')
open('output/赛题任务完成情况表.html','w',encoding='utf-8').write('\n'.join(html))
font='C:/Windows/Fonts/msyh.ttc'; bold='C:/Windows/Fonts/msyhbd.ttc'; f=ImageFont.truetype(font,21); fb=ImageFont.truetype(bold,21); title=ImageFont.truetype(bold,30)
xs=[50,350,910,1900]; pad=15
im=Image.new('RGB',(1950,4000),'white');d=ImageDraw.Draw(im)
def wrap(s,width,ft):
 lines=[];line=''
 for c in s:
  if d.textlength(line+c,font=ft)>width and line:
   if c in '，。、；：！？）' and len(line)>1: lines.append(line[:-1]);line=line[-1]+c
   else: lines.append(line);line=c
  else:line+=c
 if line:lines.append(line)
 return lines
d.text((975,45),'赛题任务完成情况',font=title,fill='#222',anchor='mm');y=85
for j,s in enumerate(['项目','得分要求','完成情况']):
 d.rectangle((xs[j],y,xs[j+1],y+52),fill='#dedede',outline='#999',width=2);d.text(((xs[j]+xs[j+1])/2,y+26),s,font=fb,fill='#222',anchor='mm')
y+=52
for idx,row in enumerate(rows):
 parts=[wrap(v,xs[i+1]-xs[i]-30,fb if i==0 else f) for i,v in enumerate(row)];h=max(len(a) for a in parts)*31+20
 for i,lines in enumerate(parts):
  d.rectangle((xs[i],y,xs[i+1],y+h),fill='#f6f6f6' if idx%2 else 'white',outline='#aaa',width=2)
  for k,s in enumerate(lines):d.text((xs[i]+pad,y+8+k*31),s,font=fb if i==0 else f,fill='#222')
 y+=h
note='注：待验证项目需结合最终版本及现场测试确认；赛题要求为概括表述。';d.text((50,y+25),note,font=ImageFont.truetype(font,20),fill='#555')
im.crop((0,0,1950,y+85)).save('output/赛题任务完成情况表.png',dpi=(220,220))
