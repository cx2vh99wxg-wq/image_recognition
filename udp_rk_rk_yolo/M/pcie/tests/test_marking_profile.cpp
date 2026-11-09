#include "lane_marking_profile.h"
#include "ego_lane_selector.h"
#include "line_style_analyzer.h"
#include <cassert>
#include <chrono>
#include <cstdio>

static bool same_points(const std::vector<m_lane::LaneRowPoint>&a,const std::vector<m_lane::LaneRowPoint>&b){
    if(a.size()!=b.size())return false;
    for(size_t i=0;i<a.size();++i)if(a[i].x!=b[i].x||a[i].t!=b[i].t||a[i].y!=b[i].y||a[i].width!=b[i].width||a[i].nn!=b[i].nn)return false;
    return true;
}
static void same_pair(const m_lane::PointLanePair&a,const m_lane::PointLanePair&b){
    assert(a.width==b.width&&a.height==b.height&&a.candidates==b.candidates&&a.first==b.first&&a.last==b.last&&a.sampled==b.sampled);
    assert(a.left_x==b.left_x&&a.right_x==b.right_x&&a.left_y==b.left_y&&a.right_y==b.right_y);
    assert(same_points(a.left,b.left)&&same_points(a.right,b.right));
}

int main()
{
    const int w = 320, h = 240;
    std::vector<float> mask(w * h);
    std::vector<uint8_t> rgb(w * h * 3), coverage(w * h, 1);
    unsigned before = 0, after = 0, total = 0;
    for (int bend : {-1, 0, 1})
        for (int filled : {0, 1})
            for (unsigned left : {LINE_SOLID, LINE_DASHED})
                for (unsigned right : {LINE_SOLID, LINE_DASHED})
                {
                    std::fill(mask.begin(), mask.end(), 0.f);
                    std::fill(rgb.begin(), rgb.end(), 35);
                    for (int y = 30; y <= 225; ++y)
                        for (int side = 0; side < 2; ++side)
                        {
                            int x = (side ? 220 : 90) + bend * (y - 125) * (y - 125) / 550;
                            bool paint = (side ? right : left) == LINE_SOLID || (y - 30) % 48 < 27;
                            for (int dx = -2; dx <= 2; ++dx)
                            {
                                if (filled || paint) mask[y * w + x + dx] = 1.f;
                                if (paint) for (int c = 0; c < 3; ++c)
                                    rgb[(y * w + x + dx) * 3 + c] = 180;
                            }
                        }
                    const auto steering = m_lane::select_point_lanes(mask.data(), w, h);
                    const auto old = m_lane::EgoLaneSelector::describe_points(steering, rgb.data(), true);
                    const auto points = m_lane::marking_profile(mask.data(), steering);
                    const auto rows=m_lane::lane_mask_rows(mask.data(),w,h,0,h);
                    const auto reused=m_lane::select_point_lanes(mask.data(),w,h,&rows);
                    same_pair(steering,reused);
                    same_pair(points,m_lane::marking_profile(mask.data(),reused,&rows));
                    const auto actual = m_lane::EgoLaneSelector::describe_points(points, rgb.data(), true, coverage.data());
                    before += old.left.type == left; before += old.right.type == right;
                    after += actual.left.type == left; after += actual.right.type == right; total += 2;
                    assert(actual.left.type == left && actual.right.type == right);
                    // Style grouping must not change the steering-selected observations.
                    const auto unchanged = m_lane::select_point_lanes(mask.data(), w, h);
                    assert(unchanged.sampled == steering.sampled && unchanged.left_x == steering.left_x);
                }
    std::printf("controlled fragment/curve tests: old=%u/%u new=%u/%u (synthetic, not road accuracy)\n", before, total, after, total);
    // Genuine black asphalt is usable; unmapped black pixels are evidence barriers.
    std::fill(rgb.begin(), rgb.end(), 0);
    std::fill(mask.begin(), mask.end(), 0.f);
    for (int y = 30; y <= 225; ++y)
        for (int dx = -2; dx <= 2; ++dx)
        {
            mask[y * w + 100 + dx] = 1;
            if ((y - 30) % 48 < 27)
                for (int c = 0; c < 3; ++c) rgb[(y * w + 100 + dx) * 3 + c] = 170;
        }
    auto steering = m_lane::select_point_lanes(mask.data(), w, h);
    auto points = m_lane::marking_profile(mask.data(), steering);
    auto raw = m_lane::EgoLaneSelector::describe_points(points, rgb.data(), true, coverage.data());
    assert(raw.left.type == LINE_DASHED && raw.right.type == LINE_UNKNOWN);
    std::fill(coverage.begin(), coverage.end(), 0);
    assert(!m_lane::EgoLaneSelector::describe_points(points, rgb.data(), true, coverage.data()).left.type);
    m_lane::LineStyleAnalyzer confirm;
    assert(confirm.confirm(raw, 1, 1000000000ULL, &points).left.type == LINE_DASHED);
    assert(!confirm.confirm(raw, 1, 1000000000ULL, &points).left.type);
    assert(confirm.confirm(raw, 2, 1200000000ULL, &points).left.type == LINE_DASHED);
    for (int phase = 0; phase <= 35; phase += 5)
    {
        std::fill(mask.begin(), mask.end(), 0.f);
        std::fill(rgb.begin(), rgb.end(), 35);
        std::fill(coverage.begin(), coverage.end(), 1);
        for (int y=20;y<=225;++y) if ((y+phase)%48<27)
            for (int dx=-2;dx<=2;++dx)
            {
                mask[y*w+100+dx]=1.f;
                for (int c=0;c<3;++c) rgb[(y*w+100+dx)*3+c]=180;
            }
        points=m_lane::marking_profile(mask.data(), steering);
        raw=m_lane::EgoLaneSelector::describe_points(points,rgb.data(),true,coverage.data());
        assert(raw.left.type==LINE_DASHED);
        assert(confirm.confirm(raw,3+phase/5,1400000000ULL+phase/5*200000000ULL,&points).left.type==LINE_DASHED);
    }
    std::fill(mask.begin(), mask.end(), 0.f);
    auto none = m_lane::marking_profile(mask.data(), steering);
    assert(none.left.empty() && none.right.empty());
    // Work is bounded even with high-frequency false-positive mask speckles.
    for (int y=0;y<h;++y) for (int x=0;x<w;++x) mask[y*w+x] = (x % 12 < 2) ? 1.f : 0.f;
    auto start = std::chrono::steady_clock::now();
    for (int i=0;i<200;++i) (void)m_lane::marking_profile(mask.data(), steering);
    std::printf("marking_profile noisy-mask mean_ms=%.3f\n", std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/200);
    std::puts("PASS: gaps, curves, one-sided evidence, map coverage, immediate fresh result, duplicate rejection, bounded noise");
}
