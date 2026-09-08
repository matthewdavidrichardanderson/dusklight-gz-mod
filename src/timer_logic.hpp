#pragma once
#include <cstdint>
namespace gz {
struct PracticeTimers {
 bool running=false;
 double rta=0,igt=0,loads=0;
 uint64_t frames=0;
 bool rtaStarted=false,igtStarted=false;
 double rtaElapsed=0,igtElapsed=0,igtExcluded=0,pendingLoad=0;
 void advance(double seconds,uint32_t logicalFrames,bool loading,bool trackLoads,bool trackRta=true,bool trackIgt=true){
  if(seconds<0)return;
  if(!trackRta)rtaStarted=false;
  if(!trackIgt)igtStarted=false;
  // TPGZ preserves its clock origin when stopped: resuming includes that wall-clock gap.
  if(running&&trackRta&&!rtaStarted){rtaStarted=true;rtaElapsed=0;}
  if(running&&trackIgt&&!igtStarted){igtStarted=true;igtElapsed=igtExcluded=0;}
  if(rtaStarted)rtaElapsed+=seconds;
  if(igtStarted)igtElapsed+=seconds;
  if(running&&trackRta){rta=rtaElapsed;frames+=logicalFrames;}
  if(running&&trackIgt){
   if(loading)igtExcluded+=seconds;
   else igt=igtElapsed-igtExcluded;
  }
  // The original load timer commits/displays an interval once loading ends.
  if(trackLoads){
   if(loading)pendingLoad+=seconds;
   else {loads+=pendingLoad;pendingLoad=0;}
  }
 }
 void reset(){*this=PracticeTimers{};}
};
}
