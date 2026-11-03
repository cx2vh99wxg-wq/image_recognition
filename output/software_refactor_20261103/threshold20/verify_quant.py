from pathlib import Path
import hashlib,subprocess
w=Path(__file__).resolve().parent
root=w.parents[2]/'udp_rk_rk_yolo'
for b in 'MS':
    results=[]
    for phase,source in [('before',w/f'{b}_postprocess.cpp'),('after',root/b/'pcie/csrc/postprocess.cpp')]:
        exe=w/f'{b}_quant_{phase}.exe'
        subprocess.run(['g++','-std=c++11','-O3','-I'+str(root/b/'pcie/include'),'-I'+str(root/b/'include'),
                        str(source),str(w/'quant_test.cpp'),'-o',str(exe)],check=True)
        target=w/f'{b}_quant_{phase}.bin'
        log=subprocess.run([str(exe),str(target)],check=True,cwd=w,capture_output=True)
        results.append((target.read_bytes(),log.stdout,log.stderr))
    assert results[0]==results[1],b
    print(b, '320 identical result groups and diagnostics',hashlib.sha256(results[1][0]).hexdigest())
