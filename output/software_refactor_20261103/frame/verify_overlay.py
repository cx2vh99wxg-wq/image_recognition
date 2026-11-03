from pathlib import Path
import hashlib,subprocess
w=Path('output/software_refactor_20261103/frame').resolve()
root=w.parents[2]/'udp_rk_rk_yolo'
inc=root/'M/pcie/include'
test=(w.parent/'deep/m_test.cpp').read_text(encoding='utf-8')
test=test.replace('if (fail_stage) results.count=0; engine.drawResults','if (fail_stage) results.count=0;\n          engine.drawResults')
test=test.replace('    fclose(fp);','''
    for (int width=1;width<=41;width+=4)
      for(int height=1;height<=39;height+=3)
        for(int i=0;i<100;++i) {
          detect_result_group_t group={};
          group.count=2;
          for(int j=0;j<2;++j) {
            auto &obj=group.results[j];
            obj.box.left=(i*7+j*3)%(width+8)-4;
            obj.box.top=(i*3+j*5)%(height+8)-4;
            obj.box.right=(i*11+j*13)%(width+8)-4;
            obj.box.bottom=(i*13+j*11)%(height+8)-4;
            obj.prop=0.9f;
            strcpy(obj.name,j ? "person" : "car");
          }
          std::vector<uint8_t> rgb(width*height*3,43);
          draw_detection_results(rgb.data(),width,height,&group);
          fwrite(rgb.data(),1,rgb.size(),fp);
          std::fill(rgb.begin(),rgb.end(),43);
          engine.drawBoundingBoxes(rgb.data(),width,height,&group);
          fwrite(rgb.data(),1,rgb.size(),fp);
        }
    fclose(fp);''')
fixture=w/'overlay_test.cpp'
fixture.write_text('#include <algorithm>\n'+test,encoding='utf-8')
for phase,src in [('before',w/'yolop_opencv_integration.cpp'),('after',root/'M/pcie/csrc/yolop_opencv_integration.cpp')]:
    exe=w/(phase+'.exe')
    includes=['-I'+str(w)] if phase=='before' else []
    subprocess.run(['g++','-std=c++11','-O3','-Wall',*includes,'-I'+str(inc),str(src),str(fixture),'-o',str(exe)],check=True)
    with (w/(phase+'.log')).open('wb') as log:
        subprocess.run([str(exe),phase+'.bin'],cwd=w,stdout=log,stderr=subprocess.STDOUT,check=True)
lines=[]
for suffix in ('bin','log'):
    old,new=[(w/(phase+'.'+suffix)).read_bytes() for phase in ('before','after')]
    assert old==new,suffix
    lines.append(f'{suffix}: identical {len(old)} bytes, SHA256 {hashlib.sha256(old).hexdigest()}')
(w/'overlay_comparison.txt').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print('\n'.join(lines))
