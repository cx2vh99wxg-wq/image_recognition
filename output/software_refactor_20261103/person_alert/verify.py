from pathlib import Path
import subprocess,hashlib
w=Path(__file__).resolve().parent
root=w.parents[2]/'udp_rk_rk_yolo'
inc='-I'+str(root/'S/pcie/include')
results=[]
for phase,src in [('before',w/'before.c'),('after',root/'S/pcie/csrc/person_alert_effects.c')]:
    exe=w/(phase+'.exe')
    subprocess.run(['gcc','-std=gnu11','-O3','-Wall','-Dgettimeofday=fixture_gettimeofday',
                   inc,str(src),str(w/'fixture.c'),'-o',str(exe)],check=True,cwd=w)
    out=w/(phase+'.bin')
    run=subprocess.run([str(exe),str(out)],check=True,cwd=w,capture_output=True)
    results.append((out.read_bytes(),run.stdout,run.stderr))
assert results[0]==results[1]
print('identical pixels, fade information, blink state and logs',len(results[1][0]),hashlib.sha256(results[1][0]).hexdigest())
