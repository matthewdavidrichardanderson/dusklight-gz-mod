#pragma once
#include <cstdint>
namespace gz {
// Four clawshot endpoints, interpolated with the host presentation fraction.
// Discontinuous simulation sequences must not blend with an old scene/shot.
template<class Vec> struct ChainHistory {
 Vec previous[4]{},current[4]{};
 uint64_t sequence=0;
 bool valid=false;
 void reset(){valid=false;}
 void capture(const Vec* anchors,uint64_t tick){
  if(valid&&tick==sequence)return;
  for(int i=0;i<4;++i){previous[i]=valid&&tick==sequence+1?current[i]:anchors[i];current[i]=anchors[i];}
  sequence=tick;valid=true;
 }
 void interpolate(Vec* anchors,float fraction)const{
  if(!valid)return;
  for(int i=0;i<4;++i)anchors[i]=previous[i]+(current[i]-previous[i])*fraction;
 }
};
}
