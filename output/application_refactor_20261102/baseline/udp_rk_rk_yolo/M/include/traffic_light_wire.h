#ifndef TRAFFIC_LIGHT_WIRE_H
#define TRAFFIC_LIGHT_WIRE_H
#include "traffic_light_ipc.h"
#include <arpa/inet.h>
#include <algorithm>
namespace traffic_wire {
constexpr size_t SIZE=72;
inline uint32_t checksum(const uint8_t* p,size_t n){uint32_t v=2166136261u;for(size_t i=0;i<n;++i)v=(v^p[i])*16777619u;return v;}
inline void put(uint8_t* p,uint32_t v){v=htonl(v);memcpy(p,&v,4);}
inline uint32_t get(const uint8_t* p){uint32_t v;memcpy(&v,p,4);return ntohl(v);}
inline bool is_packet(const uint8_t* p,size_t n){return n>=4&&get(p)==0x54464c31u;}
inline bool valid(const TrafficLightData& d){return d.session&&d.state<=3&&d.raw_state<=3&&d.color_quality<=100&&d.detector_quality<=100&&d.x<=320&&d.y<=240&&d.width<=320-d.x&&d.height<=240-d.y&&(d.flags&~127u)==0&&d.reason<=TRAFFIC_DEFERRED&&d.detection_age_ms<=60000;}
inline bool encode(uint8_t* p,const TrafficLightData& d,uint64_t now){
    if(!traffic_fresh(&d,now)||!valid(d))return false;
    const uint32_t age=(uint32_t)((TRAFFIC_TTL_NS-(traffic_deadline(&d)-now)+999999ULL)/1000000ULL);
    const uint32_t v[]={0x54464c31u,0x00010048u,d.session,d.source_id,d.detector_id,age,d.state,d.raw_state,d.color_quality,d.detector_quality,d.flags,d.x,d.y,d.width,d.height,d.detection_age_ms,d.reason};
    for(int i=0;i<17;++i)put(p+i*4,v[i]);
    put(p+68,checksum(p,68));return true;
}
class Receiver {
    uint32_t session_=0,id_=0,retired_[16]={};unsigned retired_count_=0;uint64_t deadline_=0,last_peer_=0;
public:
    bool ingest(const uint8_t* p,size_t n,uint64_t received,uint64_t now,TrafficLightData& d){
        if(n!=SIZE||get(p)!=0x54464c31u||get(p+4)!=0x00010048u||get(p+68)!=checksum(p,68))return false;
        uint32_t session=get(p+8),id=get(p+12),age=get(p+20);if(!session||age>=750||received>now)return false;
        TrafficLightData q={};q.session=session;q.source_id=id;q.detector_id=get(p+16);q.state=get(p+24);q.raw_state=get(p+28);q.color_quality=get(p+32);q.detector_quality=get(p+36);q.flags=get(p+40);q.x=get(p+44);q.y=get(p+48);q.width=get(p+52);q.height=get(p+56);q.detection_age_ms=get(p+60);q.reason=get(p+64);
        if(!valid(q))return false;
        bool changed=session!=session_;
        if(changed){for(unsigned i=0;i<std::min(16u,retired_count_);++i)if(session==retired_[i])return false;if(session_&&now-last_peer_<750000000ULL&&id>8)return false;}
        else if(id!=id_&&(int32_t)(id-id_)<=0)return false;
        uint64_t expiry=received+(750-age)*1000000ULL;if(!changed&&id==id_&&deadline_)expiry=std::min(expiry,deadline_);if(expiry<=now)return false;
        if(changed){if(session_)retired_[retired_count_++%16]=session_;session_=session;}
        id_=id;deadline_=expiry;last_peer_=now;traffic_set_deadline(&q,expiry);d=q;return true;
    }
};
}
#endif
