#ifndef S_VEHICLE_SCENE_H
#define S_VEHICLE_SCENE_H

#include "pedestrian_scene.h"
#include "../../../../common/perception/include/display_vehicles.h"
#include "../../../../common/perception/include/rider_geometry.h"

namespace vehicle_scene {

constexpr display_vehicles::Kind kinds[] = {
    display_vehicles::Bicycle, display_vehicles::Motorcycle, display_vehicles::Car, display_vehicles::Bus, display_vehicles::Truck
};
constexpr unsigned KIND_COUNT = sizeof(kinds)/sizeof(kinds[0]);
constexpr unsigned MAX_CARS_AND_BUSES = 5;

inline bool is_car_or_bus(uint32_t kind)
{
    return display_vehicles::motor_vehicle(kind);
}

struct Geometry {
    unsigned count = 0;
    display_vehicles::Vehicle vehicles[display_vehicles::CAPACITY] = {};
    bool operator==(const Geometry &other) const
    {
        return count == other.count &&
            std::memcmp(vehicles, other.vehicles, count * sizeof(vehicles[0])) == 0;
    }
};

inline void hide_rider_duplicates(pedestrian_scene::Actors &actors, const Geometry &vehicles,
                                  pedestrian_scene::MotionTracker *motion = nullptr,
                                  const display_vehicles::Snapshot *current = nullptr)
{
    unsigned kept = 0;
    for (unsigned i = 0; i < actors.count; ++i) {
        const auto &person = actors.people[i];
        bool riding = false;
        if (motion && person.view == display_people::Front) {
            const uint64_t captured = current ? display_vehicles::deadline(*current) - display_vehicles::TTL : 0;
            riding = motion->confirmed_rider(person, captured, [&](const display_people::Person &raw) {
                if (current) for (unsigned j=0; j<current->count; ++j) {
                    const auto &v=current->vehicles[j];
                    if (!is_car_or_bus(v.kind) && rider_geometry::seated(raw,v.footprint)) return true;
                }
                return false;
            });
        }
        if (person.view == display_people::Front)
            for (unsigned j = 0; j < vehicles.count; ++j) {
                const auto &v = vehicles.vehicles[j];
                if (!is_car_or_bus(v.kind) && rider_geometry::seated(person, v.footprint)) {
                    riding = true;
                    break;
                }
            }
        if (!riding) {
            actors.people[kept] = person;
            actors.motion[kept++] = actors.motion[i];
        }
    }
    actors.count = kept;
}

class Tracker {
    std::array<display_people::History, KIND_COUNT> histories_;
    std::array<uint32_t,display_people::CAPACITY> cycle_kind_ = {};

    static bool same_cycle(const display_people::Person &a,const display_people::Person &b)
    {
        const int shared=std::min(a.x+a.w,b.x+b.w)-int(std::max(a.x,b.x));
        return display_people::overlap(a,b)>.45f ||
            (shared>=.80*std::min(a.w,b.w) && std::max(a.w,b.w)<=1.6*std::min(a.w,b.w) &&
             std::abs(int(a.y+a.h)-int(b.y+b.h))<=.20*std::min(a.h,b.h) &&
             std::abs(int(2*a.x+a.w)-int(2*b.x+b.w))<=.45*std::min(a.w,b.w));
    }

  public:
    Geometry update(const display_vehicles::Snapshot *snapshot, uint64_t now)
    {
        std::array<display_people::Snapshot, KIND_COUNT> by_kind = {};
        std::array<uint32_t,display_people::CAPACITY> labels = {};
        const bool fresh = snapshot && display_vehicles::fresh(*snapshot, now) && display_vehicles::valid(*snapshot);
        if (fresh) {
            for (auto &batch : by_kind) {
                batch.session = snapshot->session;
                batch.source = snapshot->source;
                batch.ready = 1;
                display_people::expires(batch, display_vehicles::deadline(*snapshot));
            }
            for (unsigned i = 0; i < snapshot->count; ++i) {
                const auto &vehicle = snapshot->vehicles[i];
                if (!is_car_or_bus(vehicle.kind)) {
                    // One physical two-wheeler can alternate between the two COCO classes.
                    auto &batch=by_kind[0];unsigned j=0;
                    while(j<batch.count && !same_cycle(batch.people[j],vehicle.footprint)) ++j;
                    if(j==batch.count || vehicle.footprint.quality>batch.people[j].quality) {
                        batch.people[j]=vehicle.footprint;labels[j]=vehicle.kind;
                        if(j==batch.count) ++batch.count;
                    }
                    continue;
                }
                for (unsigned kind = 0; kind < by_kind.size(); ++kind) {
                    if (vehicle.kind == kinds[kind]) {
                        auto &batch = by_kind[kind];
                        batch.people[batch.count++] = vehicle.footprint;
                    }
                }
            }
        }
        struct VisibleTrack { uint32_t kind; const display_people::History::Track *track; };
        std::array<VisibleTrack, 3 * display_vehicles::CAPACITY> cars_and_buses, two_wheelers;
        unsigned car_bus_count = 0, two_wheeler_count = 0;
        for (unsigned i = 0; i < histories_.size(); ++i) {
            histories_[i].update(fresh ? &by_kind[i] : nullptr, now);
            const bool car_or_bus = is_car_or_bus(kinds[i]);
            auto &visible = car_or_bus ? cars_and_buses : two_wheelers;
            auto &count = car_or_bus ? car_bus_count : two_wheeler_count;
            for (unsigned slot=0;slot<histories_[i].tracks.size();++slot) {
                const auto &track=histories_[i].tracks[slot];
                if (!track.active) continue;
                if(car_or_bus && fresh && track.seen!=now) {
                    bool replaced=false;
                    for(unsigned j=0;j<snapshot->count;++j) {
                        const auto &newer=snapshot->vehicles[j];
                        if(is_car_or_bus(newer.kind) && newer.kind!=kinds[i] &&
                           display_people::overlap(newer.footprint,track.raw)>.68f) {replaced=true;break;}
                    }
                    if(replaced)continue;
                }
                if(i==0 && track.seen==now) {
                    for(unsigned j=0;j<by_kind[0].count;++j)
                        if(!std::memcmp(&track.raw,&by_kind[0].people[j],sizeof(track.raw)))cycle_kind_[slot]=labels[j];
                }
                visible[count++] = {i==0 ? cycle_kind_[slot] : uint32_t(kinds[i]), &track};
            }
        }
        const auto by_identity = [](const VisibleTrack &a, const VisibleTrack &b) {
            if (a.kind != b.kind)
                return a.kind < b.kind;
            return a.track->generation < b.track->generation;
        };
        const auto by_recency = [&](const VisibleTrack &a, const VisibleTrack &b) {
            if (a.track->seen != b.track->seen)
                return a.track->seen > b.track->seen;
            return by_identity(a, b);
        };
        const unsigned shown_car_buses = std::min(car_bus_count, MAX_CARS_AND_BUSES);
        if (car_bus_count > shown_car_buses) {
            std::nth_element(cars_and_buses.begin(), cars_and_buses.begin() + shown_car_buses,
                             cars_and_buses.begin() + car_bus_count,
                             [&](const VisibleTrack &a, const VisibleTrack &b) {
                                 const auto near=[](const display_people::Person &p){
                                     return p.w>=70 && p.h>=42 && p.y+p.h>=182;
                                 };
                                 if(near(a.track->raw)!=near(b.track->raw))return near(a.track->raw);
                                 if (a.track->raw.quality != b.track->raw.quality)
                                     return a.track->raw.quality > b.track->raw.quality;
                                 return by_recency(a, b);
                             });
        }
        // Keep the same draw order while confidence changes within the selected set.
        std::sort(cars_and_buses.begin(), cars_and_buses.begin() + shown_car_buses, by_identity);
        std::stable_sort(two_wheelers.begin(), two_wheelers.begin() + two_wheeler_count, by_recency);
        Geometry result;
        const auto append = [&](const VisibleTrack &entry) {
            auto &vehicle = result.vehicles[result.count++];
            vehicle = {entry.kind, entry.track->shown};
            if (!is_car_or_bus(entry.kind)) {
                // A nearby cyclist crosses many pixels between detections.
                // Keep the current wheel position when smoothing falls behind.
                const auto &raw=entry.track->raw;
                const int dx=int(2*raw.x+raw.w)-int(2*vehicle.footprint.x+vehicle.footprint.w);
                if(std::abs(dx)>10)vehicle.footprint=raw;
            }
            vehicle.footprint.quality = 0; // Confidence can change membership, never the pixels of a selected model.
        };
        for (unsigned i = 0; i < two_wheeler_count && result.count < display_vehicles::CAPACITY - shown_car_buses; ++i) {
            const auto &entry = two_wheelers[i];
            const bool duplicate = std::any_of(result.vehicles, result.vehicles + result.count,
                [&](const display_vehicles::Vehicle &v) {
                    return v.kind != entry.kind && display_people::overlap(v.footprint, entry.track->shown) > .45f;
                });
            if (!duplicate) append(entry);
        }
        for (unsigned i = 0; i < shown_car_buses; ++i)
            append(cars_and_buses[i]);
        return result;
    }
};

// Meshes use x across the road, positive y towards the viewer, and z up.
// Rasterize each shape once and reuse its shaded image as the object moves.
class Sprite {
  public:
    static constexpr int SIDE = 72;
    std::array<uint8_t, SIDE * SIDE> pixels = {};
    int left = SIDE, top = SIDE, right = 0, bottom = 0;

    explicit Sprite(display_vehicles::Kind kind)
    {
        depth_.fill(-1000.f);
        if (kind == display_vehicles::Bicycle || kind == display_vehicles::Motorcycle) {
            scale_ = 11.f;
            two_wheeler(kind == display_vehicles::Motorcycle);
        } else if (kind == display_vehicles::Car) {
            scale_ = 8.f;
            four_wheeler(false);
        } else if (kind == display_vehicles::Truck) {
            scale_ = 6.f;
            four_wheeler(false);
            block(0,.55f,1.04f,1.66f,.83f,2.74f,1.f,218,249);
            block(0,-1.64f,.93f,.55f,.89f,1.85f,.82f,162,239);
        } else {
            scale_ = 6.f;
            four_wheeler(true);
        }
        for (int y = 0; y < SIDE; ++y)
            for (int x = 0; x < SIDE; ++x)
                if (pixels[y * SIDE + x]) {
                    left = std::min(left, x);
                    top = std::min(top, y);
                    right = std::max(right, x + 1);
                    bottom = std::max(bottom, y + 1);
                }
    }

  private:
    struct Point { float x, y, z; };
    std::array<float, SIDE * SIDE> depth_;
    float scale_ = 1.f;

    Point project(Point p) const
    {
        return {36.f + scale_ * p.x,
                43.f + scale_ * (.58f * p.y - .81f * p.z),
                .81f * p.y + .58f * p.z};
    }

    static float edge(Point a, Point b, float x, float y)
    {
        return (b.x - a.x) * (y - a.y) - (b.y - a.y) * (x - a.x);
    }

    void triangle(Point a, Point b, Point c, int material)
    {
        const Point u{b.x - a.x, b.y - a.y, b.z - a.z}, v{c.x - a.x, c.y - a.y, c.z - a.z};
        const Point n{u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
        const float length = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        if (length < .0001f)
            return;
        const float diffuse = std::abs((-.37f * n.x - .20f * n.y + .88f * n.z) / length);
        const uint8_t gray = uint8_t(std::min(255.f, material * (.55f + .45f * diffuse)));
        a = project(a); b = project(b); c = project(c);
        const float area = edge(a, b, c.x, c.y);
        if (std::abs(area) < .001f)
            return;
        const int x0 = std::max(0, int(std::floor(std::min({a.x, b.x, c.x}))));
        const int y0 = std::max(0, int(std::floor(std::min({a.y, b.y, c.y}))));
        const int x1 = std::min(SIDE - 1, int(std::ceil(std::max({a.x, b.x, c.x}))));
        const int y1 = std::min(SIDE - 1, int(std::ceil(std::max({a.y, b.y, c.y}))));
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                const float wa = edge(b, c, x + .5f, y + .5f) / area;
                const float wb = edge(c, a, x + .5f, y + .5f) / area;
                const float wc = edge(a, b, x + .5f, y + .5f) / area;
                const float z = wa * a.z + wb * b.z + wc * c.z;
                const int index = y * SIDE + x;
                if (wa >= 0 && wb >= 0 && wc >= 0 && z >= depth_[index]) {
                    depth_[index] = z;
                    pixels[index] = gray;
                }
            }
        }
    }

    void quad(Point a, Point b, Point c, Point d, int material)
    {
        triangle(a, b, c, material);
        triangle(a, c, d, material);
    }

    void block(float x, float y, float width, float length, float base, float top,
               float taper, int side, int roof)
    {
        const Point low[] = {{x-width,y-length,base},{x+width,y-length,base},
                             {x+width,y+length,base},{x-width,y+length,base}};
        const float w = width * taper, l = length * taper;
        const Point high[] = {{x-w,y-l,top},{x+w,y-l,top},{x+w,y+l,top},{x-w,y+l,top}};
        for (unsigned i = 0; i < 4; ++i) {
            const unsigned j = (i + 1) % 4;
            quad(low[i], low[j], high[j], high[i], side);
        }
        quad(high[0], high[1], high[2], high[3], roof);
    }

    void wheel(float x, float y, float radius, float width, bool spoked)
    {
        const float near = x + width, far = x - width;
        constexpr int segments = 16;
        for (int i = 0; i < segments; ++i) {
            const float a = i * .39269908f, b = (i+1) * .39269908f;
            const float ay = std::cos(a) * radius, az = std::sin(a) * radius;
            const float by = std::cos(b) * radius, bz = std::sin(b) * radius;
            const Point na{near,y+ay,radius+az}, nb{near,y+by,radius+bz};
            const Point fa{far,y+ay,radius+az}, fb{far,y+by,radius+bz};
            quad(na, nb, fb, fa, 84);
            if (spoked) {
                const Point ia{near,y+ay*.75f,radius+az*.75f}, ib{near,y+by*.75f,radius+bz*.75f};
                quad(na, nb, ib, ia, 224);
                if (i % 2 == 0)
                    triangle({near,y,radius}, ia, {near,y+ay*.70f+by*.10f,radius+az*.70f+bz*.10f}, 202);
            } else {
                triangle({near,y,radius}, na, nb, 148);
                triangle({far,y,radius}, fa, fb, 120);
            }
        }
    }

    void bar(float y1, float z1, float y2, float z2, float radius)
    {
        const float d = std::hypot(y2-y1, z2-z1);
        const float y = (z2-z1)*radius/d, z = (y1-y2)*radius/d;
        quad({radius,y1-y,z1-z}, {radius,y2-y,z2-z}, {radius,y2+y,z2+z}, {radius,y1+y,z1+z}, 237);
        quad({-radius,y1-y,z1-z}, {-radius,y2-y,z2-z}, {-radius,y2+y,z2+z}, {-radius,y1+y,z1+z}, 237);
        quad({-radius,y1+y,z1+z}, {-radius,y2+y,z2+z}, {radius,y2+y,z2+z}, {radius,y1+y,z1+z}, 252);
    }

    void two_wheeler(bool motor)
    {
        const float tire = motor ? .13f : .05f;
        for (float y : {-1.18f, 1.15f})
            wheel(0, y, .47f, tire, !motor);
        bar(-1.18f,.47f,-.75f,1.47f,.08f);
        bar(-.76f,1.26f,.18f,.51f,.08f);
        bar(.18f,.51f,1.15f,.47f,.08f);
        bar(.18f,.51f,.49f,1.19f,.08f);
        bar(-.76f,1.26f,.49f,1.19f,.08f);
        bar(.49f,1.19f,1.15f,.47f,.07f);
        block(0,-.75f,.42f,.08f,1.44f,1.58f,.8f,238,249);
        block(0,.49f,.16f,.25f,1.14f,1.30f,.87f,112,151);
        if (motor) {
            block(0,-.32f,.32f,.55f,.76f,1.32f,.70f,242,255);
            block(0,.20f,.30f,.46f,.37f,.78f,.82f,163,231);
            block(0,.64f,.23f,.45f,.88f,1.18f,.83f,231,249);
        }
    }

    void four_wheeler(bool bus)
    {
        const float width = bus ? 1.08f : .91f;
        const float length = bus ? 3.4f : 2.12f;
        for (float side : {-1.f, 1.f})
            for (float axle : {-.67f, .67f})
                wheel(side * width * .92f, axle * length, bus ? .52f : .39f, .13f, false);
        block(0,0,width,length,.30f,bus?1.45f:1.00f,.96f,246,255);
        if (bus) {
            block(0,0,width*.96f,length*.97f,1.43f,2.70f,.96f,247,255);
            quad({-.77f,3.20f,1.65f},{.77f,3.20f,1.65f},{.77f,3.26f,2.45f},{-.77f,3.26f,2.45f},128);
        } else {
            block(0,.05f,.79f,1.21f,.99f,1.64f,.73f,137,253);
            block(0,.03f,.58f,.89f,1.61f,1.70f,.90f,226,255);
        }
        for (float x : {-.65f, .65f}) {
            block(x,-length*.97f,.14f,.065f,.55f,.72f,.92f,210,255);
            block(x,length*.99f,.15f,.065f,.55f,.72f,.92f,142,190);
        }
    }
};

inline const Sprite &sprite(uint32_t kind)
{
    static const Sprite bicycle(display_vehicles::Bicycle), motorcycle(display_vehicles::Motorcycle),
                        car(display_vehicles::Car), bus(display_vehicles::Bus), truck(display_vehicles::Truck);
    if (kind == display_vehicles::Bicycle) return bicycle;
    if (kind == display_vehicles::Motorcycle) return motorcycle;
    if (kind == display_vehicles::Bus) return bus;
    if (kind == display_vehicles::Truck) return truck;
    return car;
}

inline double vehicle_x(const display_people::Person &p, const pedestrian_scene::Placement &placed,
                        int width, int height, int angle)
{
    if (!(p.flags & display_vehicles::LaneRelative)) return placed.x;
    const auto section=lane_cue::road_section(lane_cue::picture_rect(width,height),width,angle,placed.y);
    return section.center + section.half*(2.*p.gx/1000.-1.);
}

// When a close lead car hides the markings, its disjoint neighbour should
// still appear beside it. Measured lane coordinates always take precedence.
inline double place_vehicle(const Geometry &scene, unsigned index, const pedestrian_scene::Placement &position,
                            int width, int height, int angle)
{
    const auto &v = scene.vehicles[index];
    const auto &p = v.footprint;
    double x = vehicle_x(p, position, width, height, angle);
    if (!is_car_or_bus(v.kind) || (p.flags & display_vehicles::LaneRelative) ||
        p.w < 50 || p.y + p.h < 177 || (p.x <= 160 && p.x+p.w >= 160)) return x;
    const auto road = lane_cue::road_section(lane_cue::picture_rect(width,height),width,angle,position.y);
    for (unsigned i = 0; i < scene.count; ++i) {
        const auto &lead = scene.vehicles[i].footprint;
        if (!is_car_or_bus(scene.vehicles[i].kind) || lead.w < 70 || lead.h < 45 ||
            lead.x > 160 || lead.x + lead.w < 160 ||
            std::abs(int(lead.y+lead.h)-int(p.y+p.h)) > 24) continue;
        const int margin = int(std::min(lead.w,p.w) / 8);
        if (int(p.x) >= int(lead.x+lead.w)-margin)
            return std::max(x,road.center+1.5*road.half);
        if (int(p.x+p.w) <= int(lead.x)+margin)
            return std::min(x,road.center-1.5*road.half);
    }
    return x;
}

inline void draw(uint32_t *pixels, int stride, int width, int height, const Geometry &scene, int angle)
{
    if (!pixels || stride < width || width < 8 || height < 8)
        return;
    std::array<pedestrian_scene::Placement, display_vehicles::CAPACITY> positions;
    std::array<unsigned, display_vehicles::CAPACITY> order;
    for (unsigned i = 0; i < scene.count; ++i) {
        positions[i] = pedestrian_scene::locate(scene.vehicles[i].footprint, angle, width, height);
        order[i] = i;
    }
    std::sort(order.begin(), order.begin() + scene.count, [&](unsigned a, unsigned b) {
        return positions[a].y < positions[b].y;
    });
    for (unsigned i = 0; i < scene.count; ++i) {
        const unsigned index = order[i];
        const auto &vehicle = scene.vehicles[index];
        const auto &p = positions[index];
        const auto &s = sprite(vehicle.kind);
        const bool wider_spacing = is_car_or_bus(vehicle.kind);
        const double size = (vehicle.kind == display_vehicles::Bus || vehicle.kind == display_vehicles::Truck) ? 1.18 :
                            vehicle.kind == display_vehicles::Car ? .98 : .80;
        const int h = std::max(14, int(std::lround(p.height * size * 1.44)));
        const int w = std::max(1, h * (s.right - s.left) / (s.bottom - s.top));
        const double x = place_vehicle(scene,index,p,width,height,angle);
        int left = int(std::lround(x)) - w / 2;
        if (wider_spacing && w <= width)
            left = std::max(0, std::min(width - w, left));
        const int top = int(std::lround(p.y)) - h;
        for (int y = std::max(0, -top); y < std::min(h, height-top); ++y) {
            const int sy = s.top + y * (s.bottom - s.top) / h;
            for (int x = std::max(0, -left); x < std::min(w, width-left); ++x) {
                const int sx = s.left + x * (s.right - s.left) / w;
                const uint32_t gray = s.pixels[sy * Sprite::SIDE + sx];
                if (gray)
                    pixels[(top+y)*stride+left+x] = 0xff000000u | gray * 0x010101u;
            }
        }
    }
}
}
#endif
