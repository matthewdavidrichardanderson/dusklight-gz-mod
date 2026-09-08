#pragma once
#include "d/actor/d_a_alink.h"
#include "d/d_camera.h"
namespace gz {
// Use the native relocation path so movement, collision and the visible pose agree.
// GZ's teleport keeps velocity; the native event setter ordinarily clears vertical speed.
inline void placeNativeLink(const cXyz& position,s16 angle){
 if(auto* link=daAlink_getAlinkActorClass()){
  const float verticalSpeed=link->speed.y;
  link->setPlayerPosAndAngle(&position,angle,TRUE);
  link->speed.y=verticalSpeed;
 }
}
inline void placeNativeCamera(const cXyz& center,const cXyz& eye){
 if(auto* camera=dCam_getBody())camera->Reset(center,eye);
}
}
