#pragma once
#include <cstdint>
namespace gz {
struct SceneLoadState{
 bool active=false;
 void requested(uint32_t result,int16_t fade){if(result!=UINT32_MAX&&fade!=0x7fff)active=true;}
 void completed(){active=false;}
};
}
