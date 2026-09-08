#include "presentation_layout.hpp"
#include <cmath>
#include <array>
int main(){
 using namespace gz;
 auto a=overlayViewport(1920,1080),b=overlayViewport(2560,1080),c=overlayViewport(640,480);
 if(std::abs(a.width/a.canvasWidth-b.width/b.canvasWidth)>.001f)return 1;
 if(a.left||b.left||c.canvasWidth!=608)return 2;
 for(float extent:{32.f,158.f,320.f,500.f})for(float fraction:{0.f,.25f,.5f,1.f}){
  float x=fraction*(b.canvasWidth-extent);
  float saved=positionFromWide(x,b.canvasWidth,extent);
  if(std::abs(positionToWide(saved,b.canvasWidth,extent)-x)>.001f)return 3;
  float narrow=positionToWide(saved,608,extent);
  if(narrow<0||narrow+extent>608.001f)return 4;
  // Resize is read-only: widening again returns to the same placement.
  if(std::abs(positionToWide(saved,b.canvasWidth,extent)-x)>.001f)return 5;
 }
 if(positionToWide(10000,608,158)!=450)return 6;
 auto portrait=overlayViewport(720,1280);
 if(portrait.canvasWidth!=608||portrait.top<=0)return 7;
 if(overlayViewport(0,1080).width)return 8;
}
