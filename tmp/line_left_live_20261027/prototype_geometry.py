from pathlib import Path
root=Path(__file__).resolve().parents[2]
out=Path(__file__).resolve().parent
text=(out/'analyzer_before.h').read_text()
reference=r'''
    static double road_reference(const uint8_t* green,int w,int h) {
        if(!green)return -1;
        std::vector<uint8_t> visited((size_t)w*h,0);
        std::vector<int> queue,best;
        const int lo=h*12/100,hi=h*96/100;
        for(int y=lo;y<hi;++y)for(int x=0;x<w;++x) {
            int seed=y*w+x;if(!green[seed]||visited[seed])continue;
            queue.clear();queue.push_back(seed);visited[seed]=1;
            for(size_t i=0;i<queue.size();++i) {
                int p=queue[i],px=p%w,py=p/w;
                const int dx[4]={-1,1,0,0},dy[4]={0,0,-1,1};
                for(int d=0;d<4;++d) {
                    int nx=px+dx[d],ny=py+dy[d];
                    if(nx<0||nx>=w||ny<lo||ny>=hi)continue;
                    int q=ny*w+nx;if(green[q]&&!visited[q]){visited[q]=1;queue.push_back(q);}
                }
            }
            if(queue.size()>best.size())best=queue;
        }
        if(best.size()<(size_t)(w*h/160))return -1;
        int first=h,last=0;
        for(int p:best){first=std::min(first,p/w);last=std::max(last,p/w);}
        if(last-first<h*.12)return -1;
        int band=std::max(5,(last-first)/8);
        std::vector<int> mins(h,w),maxs(h,-1),counts(h,0);
        for(int p:best){int y=p/w,x=p%w;if(y<=first+band){mins[y]=std::min(mins[y],x);maxs[y]=std::max(maxs[y],x);++counts[y];}}
        std::vector<double> centers;
        for(int y=first;y<=std::min(last,first+band);++y)if(counts[y]>=3)centers.push_back((mins[y]+maxs[y])*.5);
        if(centers.size()<3)return -1;
        std::sort(centers.begin(),centers.end());return centers[centers.size()/2];
    }
'''
text=text.replace('    static int light(',reference+'    static int light(',1)
text=text.replace('rows,int side,std::vector<PaintSample>* trace)', 'rows,int side,double reference,bool guided,std::vector<PaintSample>* trace)')
text=text.replace('const double bottom=h*.95, center=w*.5;','const double bottom=h*.95, center=reference;')
text=text.replace('score/=1+.25*std::abs(hypothesis.at(0)-center)/w;',
                  'score/=guided?1+std::abs(hypothesis.at(0)-center)/(w*.18):1+.25*std::abs(hypothesis.at(0)-center)/w;')
text=text.replace('if(last-first<h*.12){out.reason=LINE_SHORT_TRACK;return out;}',r'''
        double visible_length=0;
        for(int y=first;y<last;++y) {
            double t=(y-bottom)/(h*.52),next=(y+1-bottom)/(h*.52);
            visible_length+=std::hypot(1.0,f.at(next)-f.at(t));
        }
        if(visible_length<h*.12){out.reason=LINE_SHORT_TRACK;return out;}
''')
text=text.replace('(last-first)*100/(int)(h*.52)', '(int)(visible_length*100)/(int)(h*.52)')
text=text.replace('last-first>=h*.22','visible_length>=h*.22')
text=text.replace('if(prev==0&&run>=2) {longest_gap=std::max(longest_gap,run);if(i>(size_t)run&&i<cleaned.size())++gaps;}',
    'if(prev==0&&run>=2&&i>(size_t)run&&i<cleaned.size()&&cleaned[i-run-1]==1&&v==1) {longest_gap=std::max(longest_gap,run);++gaps;}')
old='YOLOLineStyleResult analyze(const float* mask,int w,int h,const uint8_t* image,int iw,int ih,LineStyleTrace* trace=nullptr) const'
assert old in text
text=text.replace(old,old.replace('trace=nullptr)', 'trace=nullptr,const uint8_t* green=nullptr)'))
text=text.replace('result.left=boundary(mask,image,iw,ih,w,h,rows,0,',
                  'double reference=road_reference(green,w,h);bool guided=reference>=0;if(!guided)reference=w*.5;\n        result.left=boundary(mask,image,iw,ih,w,h,rows,0,reference,guided,')
text=text.replace('result.right=boundary(mask,image,iw,ih,w,h,rows,1,',
                  'result.right=boundary(mask,image,iw,ih,w,h,rows,1,reference,guided,')
(out/'include').mkdir(exist_ok=True)
(out/'include/line_style_analyzer.h').write_text(text)

replay=(root/'udp_rk_rk_yolo/M/pcie/tools/line_style_replay.cpp').read_text()
replay=replay.replace('argc<3||argc>5','argc!=6')
replay=replay.replace('m_lane::LineStyleAnalyzer analyzer;',r'''
    std::vector<uint8_t> green;int gw,gh;
    if(!load(argv[5],green,gw,gh,"P5",1)||gw!=mw||gh!=mh)return 3;
    m_lane::LineStyleAnalyzer analyzer;
''')
replay=replay.replace('analyzer.analyze_front_camera(lane.data(),mw,mh,image.data(),iw,ih,argc>=4?&trace:nullptr)',
                      'analyzer.analyze(lane.data(),mw,mh,image.data(),iw,ih,&trace,green.data())')
(out/'green_replay.cpp').write_text(replay)
