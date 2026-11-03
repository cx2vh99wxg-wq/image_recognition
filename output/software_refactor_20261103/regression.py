from pathlib import Path
import subprocess
w=Path(__file__).resolve().parent
root=w.parents[1]/'udp_rk_rk_yolo'
tests=[
 ('M/udp/tests/test_video_transport_v2.cpp','M/udp/include'),
 ('S/udp/tests/test_video_transport_v2.cpp','S/udp/include'),
 ('S/x11/csrc/tests/test_six_camera.cpp','S/x11/csrc/include'),
 ('S/x11/csrc/tests/test_video_input_fps.cpp','S/x11/csrc/include'),
 ('S/test_brake_release_policy.c','S/include'),
]
for i,(source,inc) in enumerate(tests):
    print(source,flush=True)
    cpp=source.endswith('.cpp')
    exe=w/f'regression_{i}.exe'
    subprocess.run(['g++' if cpp else 'gcc','-std=c++14' if cpp else '-std=c11','-O2','-I'+str(root/inc),
                    '-I'+str(root/'S/include'),'-I'+str(w/'shim'),str(root/source),'-o',str(exe),'-lws2_32'],check=True,cwd=w)
    subprocess.run([str(exe)],check=True,cwd=w)


