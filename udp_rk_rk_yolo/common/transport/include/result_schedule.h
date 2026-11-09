#ifndef RESULT_SCHEDULE_H
#define RESULT_SCHEDULE_H
#include <cstdint>
#include <cstddef>
#include <cstring>
namespace result_transport {
struct Schedule {
 uint64_t last=0;uint8_t previous[512]={};size_t length=0;bool have=false;
 // Age/deadline bytes and the trailing checksum change on retransmit; they
 // are not a new observation. Semantic changes are sent at the next poll.
 bool due(const uint8_t*p,size_t n,size_t age_offset,uint64_t now,uint64_t heartbeat){
  if(n<8||n>sizeof(previous)||age_offset>n-8)return false;
  const bool same=have&&length==n&&!std::memcmp(p,previous,age_offset)&&
      !std::memcmp(p+age_offset+4,previous+age_offset+4,n-age_offset-8);
  if(same&&now>=last&&now-last<heartbeat)return false;
  std::memcpy(previous,p,n);length=n;have=true;last=now;return true;
 }
};
}
#endif
