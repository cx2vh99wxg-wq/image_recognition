#ifndef LINE_STYLE_WIRE_H
#define LINE_STYLE_WIRE_H
#include "line_style_ipc.h"
#include <arpa/inet.h>
#ifdef __cplusplus
#include <algorithm>
namespace line_wire {
constexpr size_t SIZE=60;
inline uint32_t checksum(const uint8_t* p,size_t n) {uint32_t s=2166136261u;for(size_t i=0;i<n;++i)s=(s^p[i])*16777619u;return s;}
inline void put(uint8_t* p,uint32_t x){x=htonl(x);memcpy(p,&x,4);}
inline uint32_t get(const uint8_t* p){uint32_t x;memcpy(&x,p,4);return ntohl(x);}
inline bool is_packet(const uint8_t* p,size_t n){return n>=4&&get(p)==0x4c535431u;}
inline bool encode(uint8_t* p,const LineStyleData& d,uint64_t now) {
    if(!line_fresh(&d,now)||d.left>2||d.right>2)return false;
    put(p,0x4c535431u);put(p+4,0x0001003cu);put(p+8,d.session);put(p+12,d.source_id);
    uint64_t remaining=line_deadline(&d)-now;
    put(p+16,(uint32_t)((LINE_STYLE_TTL_NS-std::min<uint64_t>(remaining,LINE_STYLE_TTL_NS)+999999)/1000000));
    const uint32_t values[]={d.left,d.right,d.left_quality,d.right_quality,d.left_duty,d.right_duty,d.left_spans,d.right_spans};
    for(int i=0;i<8;++i)put(p+20+i*4,values[i]);
    put(p+52,0);put(p+56,checksum(p,56));return true;
}
class Receiver {
    uint32_t session_=0,id_=0,retired_[16]={};unsigned retired_count_=0;
    uint64_t deadline_=0,last_peer_=0;
public:
    bool ingest(const uint8_t* p,size_t n,uint64_t received,uint64_t now,LineStyleData& d) {
        if(n!=SIZE||get(p)!=0x4c535431u||get(p+4)!=0x0001003cu||get(p+56)!=checksum(p,56))return false;
        uint32_t session=get(p+8),id=get(p+12),age=get(p+16);
        if(!session||age>=1000||get(p+20)>2||get(p+24)>2||get(p+28)>100||get(p+32)>100||get(p+36)>100||get(p+40)>100)return false;
        if(session!=session_) {
            for(unsigned i=0;i<std::min(16u,retired_count_);++i)if(session==retired_[i])return false;
            // Reject arbitrary competing sessions while the current sender is live.
            if(session_&&now-last_peer_<500000000ULL&&id>8)return false;
            if(session_)retired_[retired_count_++%16]=session_;
            session_=session;id_=id;deadline_=0;
        } else if(id!=id_&&(int32_t)(id-id_)<=0)return false;
        uint64_t deadline=received+(1000-age)*1000000ULL;
        if(id==id_&&deadline_)deadline=std::min(deadline,deadline_);
        if(deadline<=now)return false;
        id_=id;deadline_=deadline;last_peer_=now;d={};d.session=session;d.source_id=id;
        d.left=get(p+20);d.right=get(p+24);d.left_quality=get(p+28);d.right_quality=get(p+32);
        d.left_duty=get(p+36);d.right_duty=get(p+40);d.left_spans=get(p+44);d.right_spans=get(p+48);
        line_set_deadline(&d,deadline);return true;
    }
};
}
#endif
#endif
