from pathlib import Path
from itertools import permutations
import re, subprocess,hashlib

work=Path(__file__).resolve().parent
root=work.parents[2]/'udp_rk_rk_yolo'
shim='-I'+str(work/'shim')
headers={k:(work/(k+'_abi.h'),root/p) for k,p in (
    ('M','M/pcie/include/pcie_dma_read_test.h'),
    ('S','S/pcie/include/pcie_dma_read_test.h'),
    ('old_udp','S/udp/include/pcie_dma_read_test.h'))}

def output(args, data=None):
    return subprocess.run(args,input=data,capture_output=True,check=True,cwd=work).stdout
def inclusion(paths):
    return ''.join(f'#include "{p.as_posix()}"\n' for p in paths)

for lang, compiler, standard in [('c','gcc','c11'),('c++','g++','c++11')]:
    for order in [*[(k,) for k in headers],*permutations(headers,3)]:
        variants=[inclusion([headers[key][variant] for key in order]).encode() for variant in (0,1)]
        args=[compiler,'-std='+standard,shim,'-E','-x',lang]
        for flag in ('-P','-dM'):
            a,b=[output(args+[flag,'-'],data) for data in variants]
            assert a==b,(lang,order,flag)
print('ABI: 18 C/C++ include combinations, declarations and macros identical')

def extract(source,name):
    return re.search(rf'^\w+ {name}\([^;]*?\)\n\{{.*?^\}}',source,re.M|re.S)[0]
main=r'''
static int scenario, calls;
int ioctl(int fd,unsigned long command,void *opaque)
{
    assert(fd==37);
    DMA_OPERATION *op=(DMA_OPERATION*)opaque;
    assert(op->offset_addr==0 && op->current_len==17);
    assert(command==(calls==0 ? PCI_MAP_ADDR_CMD : PCI_DMA_WRITE_CMD));
    for (unsigned char byte : op->data.write_buf) assert(byte==0);
    printf("ioctl %lu\n",command);
    ++calls;
    op->offset_addr=128; // The second request must reset this.
    if (scenario==calls || scenario==calls+2) {
        errno=EACCES;
        return scenario>2 ? 3 : -1;
    }
    return 0;
}

int main() {
    for(scenario=0;scenario<5;++scenario) {
        PCIEManager m={};
        memset(&m.dma_operation,0xAA,sizeof m.dma_operation);
        m.pci_driver_fd=37;
        calls=0;
        int rc=pcie_manager_init_streaming(&m);
        assert(rc==(scenario ? -1 : 0));
        assert(calls==((scenario==1 || scenario==3) ? 1 : 2));
        printf("final offset=%d, rc=%d, calls=%d\n",m.dma_operation.offset_addr,rc,calls);
    }
    PCIEManager invalid={}; invalid.pci_driver_fd=-1;
    assert(pcie_manager_init_streaming(nullptr)==-1);
    assert(pcie_manager_init_streaming(&invalid)==-1);
#ifdef TEST_M
    _putenv("VISION_DMA_ROWS=2");
    invalid.pci_driver_fd=37;
    assert(pcie_manager_init_streaming(&invalid)==-1);
#endif
    unsigned char pixels[DISPLAY_WIDTH*DISPLAY_HEIGHT*3];
    for(size_t i=0;i<sizeof pixels;++i) pixels[i]=(i*73+29)%256;
    assert(save_frame_to_ppm(pixels,"fixture.ppm")==0);
    assert(save_frame_to_ppm(pixels,"missing-dir/frame.ppm")==-1);
    assert(save_frame_to_ppm(nullptr,"fixture.ppm")==-1);
    assert(save_frame_to_ppm(pixels,nullptr)==-1);
    emergency_stop("fixture","PCIe");
    emergency_stop(nullptr,nullptr);
}
'''
fixture=r'''
#include <cassert>
#include <cerrno>
#include <cstdint>
static time_t fixture_time(time_t *out)
{
    const time_t t=0;
    if(out) *out=t;
    return t;
}
#define time fixture_time
#define LEADING_PIXELS 8
#define IMAGE_WIDTH 26
#define DISPLAY_WIDTH 4
#define DISPLAY_HEIGHT 3
struct PCIEManager {
    int pci_driver_fd;
    DMA_OPERATION dma_operation;
    int dma_rows;
};
'''
for board in 'MS':
    outputs=[]
    for phase in ('before','after'):
        old=phase=='before'
        header=headers[board][0 if old else 1]
        source=(work/f'{board}_main.cpp') if old else (root/board/'pcie/csrc/main.cpp')
        src=source.read_text(encoding='utf-8')
        support='' if old else inclusion([root/'common/pcie/capture_support.h'])
        body='\n'.join(extract(src,n) for n in ('pcie_manager_init_streaming','save_frame_to_ppm','emergency_stop'))
        file=work/f'{board}_{phase}.cpp'
        file.write_text(inclusion([header])+fixture+support+body+main,encoding='utf-8')
        exe=file.with_suffix('.exe')
        args=['g++','-std=c++11','-O2','-Wall',shim]
        if board=='M': args+=['-DTEST_M']
        subprocess.run(args+[str(file),'-o',str(exe)],check=True,cwd=work)
        logs=subprocess.run([str(exe)],capture_output=True,check=True,cwd=work)
        outputs.append((logs.stdout,logs.stderr,(work/'fixture.ppm').read_bytes()))
    assert outputs[0]==outputs[1],board
    digest=hashlib.sha256(b''.join(outputs[1])).hexdigest()
    print(board,'DMA trace, diagnostics, failure handling, and PPM bytes identical',digest)
