"""Compile the production font routines alone and check deterministic pixels.

No model, RKNN library, NPU or board access is involved. --source allows a
preserved implementation to provide a differential baseline.
"""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

EXPECTED = '4d619b0417d4d08c cases=889'  # Preserved pre-refactor pixels.
HARNESS = r'''
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
IMPLEMENTATION
static uint64_t fingerprint = UINT64_C(14695981039346656037);
static void accumulate(const uint8_t* bytes, size_t count) {
    for (size_t i=0;i<count;++i) {
        fingerprint ^= bytes[i];
        fingerprint *= UINT64_C(1099511628211);
    }
}
int main() {
    init_font();
    accumulate(&font_5x7[0][0], sizeof(font_5x7));
    init_font();
    accumulate(&font_5x7[0][0], sizeof(font_5x7));
    const int positions[][2] = {{0,0},{-3,-2},{2,3},{39,2},{2,17},{42,20},{-12,8}};
    int cases=0;
    for (int code=1;code<128;++code) {
        char label[3] = {static_cast<char>(code), '0', 0};
        for (const auto& position : positions) {
            std::vector<uint8_t> storage(16+41*19*3+16, 0xa5);
            for (size_t i=16;i<storage.size()-16;++i) storage[i]=(i*31+code*13)&255;
            draw_text(storage.data()+16,41,19,label,position[0],position[1],251,37,109);
            accumulate(storage.data(),storage.size());
            ++cases;
        }
    }
    std::vector<uint8_t> unchanged(41*19*3,0x73);
    draw_text(nullptr,41,19,"A",0,0,1,2,3);
    draw_text(unchanged.data(),41,19,nullptr,0,0,1,2,3);
    draw_text(unchanged.data(),41,19,"",0,0,1,2,3);
    if (!std::all_of(unchanged.begin(),unchanged.end(),[](uint8_t v){return v==0x73;})) return 2;
    std::printf("%016llx cases=%d\n",static_cast<unsigned long long>(fingerprint),cases);
}
'''

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]/'csrc/yolop_opencv_integration.cpp')
    parser.add_argument('--compiler',default=os.environ.get('CXX','g++'))
    parser.add_argument('--work-dir',type=Path,default=Path(__file__).resolve().parent/'build')
    parser.add_argument('--record',action='store_true')
    args=parser.parse_args()
    text=args.source.read_text(encoding='utf-8')
    start=text.index('static uint8_t font_5x7[128][7];')
    end=text.index('// ===== YOLOP模型参数 =====',start)
    args.work_dir.mkdir(parents=True,exist_ok=True)
    work_dir=args.work_dir.resolve()
    with tempfile.TemporaryDirectory(prefix='font-pixel-test-',dir=str(work_dir)) as folder:
        folder=Path(folder).resolve()
        assert folder.is_relative_to(work_dir)
        source=folder/'font.cpp'
        executable=folder/('font.exe' if os.name=='nt' else 'font')
        source.write_text(HARNESS.replace('IMPLEMENTATION',text[start:end]),encoding='utf-8')
        environment=dict(os.environ,TMP=str(folder),TEMP=str(folder),TMPDIR=str(folder))
        subprocess.run([args.compiler,'-std=c++11','-O2',str(source),'-o',str(executable)],check=True,env=environment)
        output=subprocess.check_output([str(executable)],text=True).strip()
    if not args.record and (EXPECTED is None or output!=EXPECTED):
        raise AssertionError('Font pixels changed: %s, expected %s'%(output,EXPECTED))
    print(('FONT_BASELINE ' if args.record else 'PASS compatibility font ')+output)

if __name__=='__main__':
    main()
