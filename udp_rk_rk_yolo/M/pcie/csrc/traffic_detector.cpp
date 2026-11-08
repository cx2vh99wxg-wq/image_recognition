#include "traffic_detector.h"
#include "front_person.h"
#include "rknn_api.h"
#include <chrono>
#include <cstring>
#include <fstream>
#include <sstream>
namespace traffic
{
namespace
{
const int anchors[3][6] = {
    {10, 13, 16, 30, 33, 23}, {30, 61, 62, 45, 59, 119}, {116, 90, 156, 198, 373, 326}};
bool fail(std::string *error, const std::string &text)
{
    if (error)
        *error = text;
    return false;
}
}
struct Detector::Impl
{
    rknn_context context = 0;
    int width = 0, height = 0;
    bool quantized = false;
    unsigned passes = 0, view_cursor = 0, locked_view = 0, hits = 0;
    std::vector<rknn_tensor_attr> attrs;
    std::vector<uint8_t> pixels;
    ~Impl()
    {
        if (context)
            rknn_destroy(context);
    }
};
Detector::Detector() : impl_(new Impl)
{
}
Detector::~Detector() = default;
int Detector::input_size() const
{
    return impl_->width;
}
unsigned Detector::last_passes() const
{
    return impl_->passes;
}
bool Detector::init(const char *path, std::string *error)
{
    impl_.reset(new Impl);
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
        return fail(error, "traffic model missing: " + std::string(path));
    auto size = file.tellg();
    if (size <= 0 || size > 100000000)
        return fail(error, "invalid traffic model size");
    file.seekg(0);
    std::vector<char> model((size_t)size);
    file.read(model.data(), size);
    if (!file ||
        rknn_init(&impl_->context, model.data(), model.size(), RKNN_FLAG_PRIOR_LOW, nullptr))
        return fail(error, "traffic rknn_init failed");
    rknn_input_output_num io = {};
    if (rknn_query(impl_->context, RKNN_QUERY_IN_OUT_NUM, &io, sizeof(io)) || io.n_input != 1 ||
        io.n_output != 3)
        return fail(error, "traffic model requires 1 input, 3 YOLOv5 heads");
    rknn_tensor_attr input = {};
    if (rknn_query(impl_->context, RKNN_QUERY_INPUT_ATTR, &input, sizeof(input)))
        return fail(error, "traffic input query failed");
    if (input.n_dims != 4)
        return fail(error, "traffic input rank is not 4");
    impl_->height = input.dims[input.fmt == RKNN_TENSOR_NCHW ? 2 : 1];
    impl_->width = input.dims[input.fmt == RKNN_TENSOR_NCHW ? 3 : 2];
    if (impl_->width != impl_->height || impl_->width < 256 || impl_->width > 640 ||
        impl_->width % 32)
        return fail(error, "traffic input must be square, 256..640 and divisible by 32");
    impl_->attrs.resize(3);
    for (unsigned i = 0; i < 3; ++i)
    {
        auto &a = impl_->attrs[i];
        a = {};
        a.index = i;
        if (rknn_query(impl_->context, RKNN_QUERY_OUTPUT_ATTR, &a, sizeof(a)))
            return fail(error, "traffic output query failed");
        int channels = a.fmt == RKNN_TENSOR_NCHW ? a.dims[1] : a.dims[3];
        if (a.n_dims != 4 || channels != 255)
            return fail(error, "traffic head must have 3*(5+80) channels");
        bool quant = a.qnt_type == RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC && a.type == RKNN_TENSOR_INT8;
        if (i && quant != impl_->quantized)
            return fail(error, "mixed traffic output quantization");
        impl_->quantized = quant;
    }
    impl_->pixels.assign((size_t)impl_->width * impl_->height * 3, 114);
    // 在车道工作线程启动前完成模型装载和首次缓冲区分配，
    // 冷启动分配不计入运行期间的车道空闲预算。
    rknn_input warm = {};
    warm.index = 0;
    warm.buf = impl_->pixels.data();
    warm.size = impl_->pixels.size();
    warm.type = RKNN_TENSOR_UINT8;
    warm.fmt = RKNN_TENSOR_NHWC;
    if (rknn_inputs_set(impl_->context, 1, &warm) || rknn_run(impl_->context, nullptr))
        return fail(error, "traffic startup warmup failed");
    rknn_output outputs[3] = {};
    for (unsigned i = 0; i < 3; ++i)
    {
        outputs[i].index = i;
        outputs[i].want_float = !impl_->quantized;
    }
    if (rknn_outputs_get(impl_->context, 3, outputs, nullptr))
        return fail(error, "traffic warmup outputs failed");
    rknn_outputs_release(impl_->context, 3, outputs);
    return true;
}
bool Detector::detect(const uint8_t *rgb, int w, int h, int stride, std::vector<Box> &boxes,
                      std::string *error, unsigned budget_ms, PersonResult *persons)
{
    boxes.clear();
    if (persons) *persons = PersonResult{};
    auto &p = *impl_;
    if (!p.context || !rgb || w < 1 || h < 1 || stride < w * 3)
        return fail(error, "invalid traffic input");
    p.passes = 0;
    // Round-robin views on fresh frames; one NPU pass never delays the next
    // lane job by a three-scale burst. Keep a successful view for tracking.
    const float views[] = {1.f, .5f, .25f};
    // Shared person+signal service always covers the full corrected front image.
    // Tiny padded views are retained only for the traffic-only offline probes.
    const unsigned view_id = persons ? 0 : p.hits ? p.locked_view : p.view_cursor++ % 3;
    (void)budget_ms; // Admission still measures the whole call and backs off overruns.
    {
        const float view = views[view_id];
        const float scale = std::min(float(p.width) / w, float(p.height) / h) * view;
        int rw = std::max(1, (int)std::lround(w * scale)),
            rh = std::max(1, (int)std::lround(h * scale));
        int px = (p.width - rw) / 2, py = (p.height - rh) / 2;
        std::fill(p.pixels.begin(), p.pixels.end(), 114);
        for (int y = 0; y < rh; ++y)
        {
            float fy = std::max(0.f, (y + .5f) / scale - .5f);
            int y0 = std::min(h - 1, (int)fy), y1 = std::min(h - 1, y0 + 1);
            float ay = fy - y0;
            for (int x = 0; x < rw; ++x)
            {
                float fx = std::max(0.f, (x + .5f) / scale - .5f);
                int x0 = std::min(w - 1, (int)fx), x1 = std::min(w - 1, x0 + 1);
                float ax = fx - x0;
                for (int c = 0; c < 3; ++c)
                {
                    float top = rgb[y0 * stride + x0 * 3 + c] * (1 - ax) +
                                rgb[y0 * stride + x1 * 3 + c] * ax,
                          bot = rgb[y1 * stride + x0 * 3 + c] * (1 - ax) +
                                rgb[y1 * stride + x1 * 3 + c] * ax;
                    p.pixels[((y + py) * p.width + x + px) * 3 + c] =
                        (uint8_t)std::lround(top * (1 - ay) + bot * ay);
                }
            }
        }
        rknn_input input = {};
        input.index = 0;
        input.buf = p.pixels.data();
        input.size = p.pixels.size();
        input.type = RKNN_TENSOR_UINT8;
        input.fmt = RKNN_TENSOR_NHWC;
        if (rknn_inputs_set(p.context, 1, &input) || rknn_run(p.context, nullptr))
            return fail(error, "traffic RKNN execution failed");
        ++p.passes;
        rknn_output outputs[3] = {};
        for (unsigned i = 0; i < 3; ++i)
        {
            outputs[i].index = i;
            outputs[i].want_float = !p.quantized;
        }
        if (rknn_outputs_get(p.context, 3, outputs, nullptr))
            return fail(error, "traffic outputs_get failed");
        std::vector<Box> pass_candidates;
        bool valid = true;
        unsigned levels = 0;
        for (unsigned k = 0; k < 3 && valid; ++k)
        {
            auto &a = p.attrs[k];
            bool nchw = a.fmt == RKNN_TENSOR_NCHW;
            int gh = a.dims[nchw ? 2 : 1], gw = a.dims[nchw ? 3 : 2], n = gw * gh,
                stride_model = p.width / gw;
            int level = stride_model == 8    ? 0
                        : stride_model == 16 ? 1
                        : stride_model == 32 ? 2
                                             : -1;
            if (level < 0 || (levels & (1u << level)) || p.height / gh != stride_model)
            {
                valid = false;
                break;
            }
            levels |= 1u << level;
            if (!outputs[k].buf || outputs[k].size < (size_t)n * 255 * (p.quantized ? 1 : 4))
            {
                valid = false;
                break;
            }
            auto at = [&](int channel, int pos)
            {
                int index = nchw ? channel * n + pos : pos * 255 + channel;
                return p.quantized ? (int(((int8_t *)outputs[k].buf)[index]) - a.zp) * a.scale
                                   : ((float *)outputs[k].buf)[index];
            };
            for (int anchor = 0; anchor < 3 && valid; ++anchor)
                for (int y = 0; y < gh && valid; ++y)
                    for (int x = 0; x < gw; ++x)
                    {
                        int pos = y * gw + x, base = anchor * 85;
                        float obj = at(base + 4, pos), cls = at(base + 5 + 9, pos);
                        const float person_cls = persons ? at(base + 5, pos) : 0.f;
                        if (!std::isfinite(obj) || !std::isfinite(cls) || obj < -.01f ||
                            obj > 1.02f || cls < -.01f || cls > 1.02f ||
                            !std::isfinite(person_cls) || person_cls < -.01f || person_cls > 1.02f)
                        {
                            valid = false;
                            break;
                        }
                        float score = obj * cls;
                        const float person_score = obj * person_cls;
                        if (score < locator_score_gate &&
                            (!persons || person_score < FRONT_PERSON_CONFIDENCE_THRESHOLD))
                            continue;
                        float dx = at(base, pos), dy = at(base + 1, pos), dw = at(base + 2, pos),
                              dh = at(base + 3, pos);
                        if (!std::isfinite(dx + dy + dw + dh))
                        {
                            valid = false;
                            break;
                        }
                        float bw = 4 * dw * dw * anchors[level][anchor * 2],
                              bh = 4 * dh * dh * anchors[level][anchor * 2 + 1];
                        Box b =
                            clipped(Box(((2 * dx - .5f + x) * stride_model - bw / 2 - px) / scale,
                                        ((2 * dy - .5f + y) * stride_model - bh / 2 - py) / scale,
                                        bw / scale, bh / scale, score),
                                    w, h);
                        bool person_top = persons && person_score >= FRONT_PERSON_CONFIDENCE_THRESHOLD;
                        // Match S: class 0 must be the highest-scoring COCO class,
                        // rather than a low secondary score on another object.
                        for (int id=1;person_top && id<80;++id)
                        {
                            const float other=at(base+5+id,pos);
                            if (!std::isfinite(other)) { valid=false; person_top=false; break; }
                            if (other>person_cls) person_top=false;
                        }
                        // Person safety examines every threshold-passing class-0 anchor,
                        // independent of signal housing/color/ROI and display limits.
                        if (person_top && b.w > 0 && b.h > 0)
                        {
                            ++persons->count;
                            if (person_score > persons->best.score)
                            { persons->best = b; persons->best.score = person_score; }
                        }
                        if (score >= locator_score_gate && b.w >= 3 && b.h >= 4 && b.y + b.h * .5 < h * .88)
                            pass_candidates.push_back(b);
                    }
        }
        rknn_outputs_release(p.context, 3, outputs);
        if (!valid || levels != 7)
            return fail(error, "unexpected YOLOv5 head layout/activation; refusing to decode");
        // Locator uses only the model score and color-independent housing evidence.
        std::sort(pass_candidates.begin(), pass_candidates.end(),
                  [](const Box &a, const Box &b) { return a.score > b.score; });
        if (pass_candidates.size() > 64)
            pass_candidates.resize(64);
        std::vector<Box> selected;
        for (const auto &b : pass_candidates)
        {
            if (!signal_housing(rgb, w, h, stride, b))
                continue;
            bool keep = true;
            for (const auto &old : selected)
                if (overlap(b, old) > .45f)
                {
                    keep = false;
                    break;
                }
            if (keep)
                selected.push_back(b);
            if (selected.size() >= 16)
                break;
        }
        if (!selected.empty())
        {
            boxes.swap(selected);
            p.locked_view = view_id;
            p.hits = 1;
            return true;
        }
        p.hits = 0;
    }
    return true;
}
}
