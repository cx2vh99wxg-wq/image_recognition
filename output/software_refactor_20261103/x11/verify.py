from pathlib import Path
import re, subprocess, hashlib
w=Path(__file__).resolve().parent
root=w.parents[2]/'udp_rk_rk_yolo'
main=root/'S/x11/csrc/main.cpp'
def find(src, pattern):
    return re.search(pattern,src,re.M|re.S)[0]
funcs=['pcie_display_source_row','rgb565_to_rgb888','render_camera_block','render_scaled_camera_block',
       'calculate_car_edge_position','draw_lane_lines','render_car_png_with_state']
digests=[]
for phase,path in [('before',w/'before.cpp'),('after',main)]:
    src=path.read_text(encoding='utf-8')
    head='\n'.join(m[0] for m in re.finditer(r'^#define (?:PCIE_IMAGE|UDP_IMAGE|COMBINED_IMAGE|CAMERA_BLOCK)_[^\n]+',src,re.M))
    types=find(src,r'^struct VehicleLayerState\n\{.*?^\};')
    methods=[find(src,rf'^[^\n;]*\b{name}\([^;]*?\)\n\{{.*?^\}}') for name in funcs]
    if phase=='after':
        methods.insert(2,find(src,r'^static int first_visible_pixel\([^;]*?\)\n\{.*?^\}'))
        methods.insert(3,find(src,r'^template <typename Sample>\nstatic void paint_camera_tile\([^;]*?\)\n\{.*?^\}'))
    code=w/f'{phase}.fixture.cpp'
    code.write_text('#include "six_camera_render.h"\n#include "frame_pixels.h"\n#include <cmath>\n'
                    'enum CurveType {CURVE_NONE, CURVE_LEFT, CURVE_RIGHT, CURVE_STRAIGHT};\n'+head+'\n'+types+'\n'+
                    '\n'.join(methods)+'\n'+(w/'fixture.cpp').read_text(encoding='utf-8-sig'),encoding='utf-8')
    exe=code.with_suffix('.exe')
    subprocess.run(['g++','-std=c++14','-O3','-I'+str(root/'S/include'),'-I'+str(root/'S/x11/csrc/include'),
                    '-I'+str(root/'common/image'),str(code),'-o',str(exe)],check=True,cwd=w)
    out=w/(phase+'.bin')
    subprocess.run([str(exe),str(out)],check=True,cwd=w)
    with out.open('rb') as file:
        checksum=hashlib.file_digest(file,'sha256').hexdigest()
    digests.append((out.stat().st_size,checksum))
    out.unlink()
assert digests[0]==digests[1],digests
print('identical output',*digests[1])
