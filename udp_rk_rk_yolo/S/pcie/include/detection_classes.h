#ifndef S_DETECTION_CLASSES_H
#define S_DETECTION_CLASSES_H
#include <array>
// COCO has no traffic-cone class. Do not relabel bottles as traffic cones.
// Keep people and supported physical road obstacles; all other results are
// discarded before box construction/display. Class competition stays intact.
inline const unsigned char *s_detection_classes() {
 static const std::array<unsigned char,80> allowed=[] {
  std::array<unsigned char,80> a{};
  for(unsigned id:{0u,1u,2u,3u,5u,6u,7u,10u,13u})a[id]=1;
  return a;
 }();
 return allowed.data();
}
#endif
