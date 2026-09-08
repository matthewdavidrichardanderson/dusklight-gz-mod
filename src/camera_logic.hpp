#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
namespace gz {
struct FreeCamera {
 std::array<double,3> eye{},center{};
 double pitch=0,yaw=0;
 void initialize(std::array<double,3> e,std::array<double,3> c){
  eye=e;center=c;
  const double dx=c[0]-e[0],dy=c[1]-e[1],dz=c[2]-e[2];
  yaw=std::atan2(dz,dx);pitch=std::atan2(dy,std::hypot(dx,dz));
 }
 void update(double forward,double side,double vertical,double pitchInput,double yawInput,bool fast){
  const double speed=fast?2.0:0.2;
  eye[0]+=speed*(forward*std::cos(yaw)*std::cos(pitch)-side*std::sin(yaw));
  eye[1]+=speed*(forward*std::sin(pitch)+vertical);
  eye[2]+=speed*(forward*std::sin(yaw)*std::cos(pitch)+side*std::cos(yaw));
  // GZ applies rotation on the following frame.
  center={eye[0]+std::cos(yaw)*std::cos(pitch),eye[1]+std::sin(pitch),eye[2]+std::sin(yaw)*std::cos(pitch)};
  constexpr double pi=std::numbers::pi;
  yaw=std::fmod(yaw+yawInput*0.002+2*pi,2*pi);
  pitch=std::clamp(pitch+pitchInput*0.002,-pi/2+0.1,pi/2-0.1);
 }
};
}
