// TPGZ features/moveactor: raw GameCube stick units, once per simulation tick.
#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <numbers>
namespace gz {
struct ActorMotion {
 float angle=0;
 void face(int16_t yaw){angle=float(yaw)/65536.f*(2*std::numbers::pi);}
 std::array<float,3> eyeOffset() const {return {-600*std::sin(angle),200,-600*std::cos(angle)};}
 std::array<float,3> update(int16_t& yaw,int16_t& pitch,int sx,int sy,int cx,int cy,uint16_t buttons){
  const bool lock=(buttons&0x40)!=0,fast=(buttons&0x10)!=0,faster=(buttons&0x20)!=0;
  if(!lock)face(yaw);
  const double heading=std::atan2(std::cos(double(angle)),std::sin(double(angle)));
  const double speed=fast?(faster?10.:2.5):1.;
  const int rotation=fast?(faster?800:80):30;
  // L turns the C-stick into pitch/yaw editing without orbiting the camera.
  if(lock){pitch=int16_t(pitch+cy*rotation);yaw=int16_t(yaw-cx*rotation);}
  else yaw=int16_t(yaw+cx*rotation);
  return {float(speed*(sy*std::cos(heading)-sx*std::sin(heading))),
          float(speed*(lock?0:cy)),
          float(speed*(sy*std::sin(heading)+sx*std::cos(heading)))};
 }
};
}
