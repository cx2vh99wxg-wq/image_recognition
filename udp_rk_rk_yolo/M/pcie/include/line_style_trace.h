#ifndef LINE_STYLE_TRACE_H
#define LINE_STYLE_TRACE_H
#include <vector>
namespace m_lane {
struct PaintSample { int y,x,raw,filtered; };
struct ObjectTrace { double a,b,c; int first,last; unsigned nn_points; };
struct LineStyleTrace {
    std::vector<PaintSample> left,right;
    std::vector<ObjectTrace> objects;
    double reference_x=-1,near_y=-1;
    unsigned road_area=0;
    int selected_left=-1,selected_right=-1;
};
}
#endif
