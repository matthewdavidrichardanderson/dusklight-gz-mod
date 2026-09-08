#pragma once
#include <cstdint>
namespace gz {
struct PracticeTimers {
 bool running=false;
 double rta=0,rtaElapsed=0;
 int64_t igt=0,loads=0,igtElapsed=0,igtExcluded=0,pendingLoad=0;
 uint64_t frames=0;
 bool rtaStarted=false,igtStarted=false;
 // Wall time belongs to RTA only. IGT/load values stay in OSGetTime ticks,
 // using Dusk's game clock and overlap-process load boundaries.
 void advance(double seconds,uint32_t logicalFrames,int64_t gameTicks,bool loading,bool trackLoads,bool trackRta=true,bool trackIgt=true){
  if(seconds<0||gameTicks<0)return;
  if(!trackRta)rtaStarted=false;
  if(!trackIgt)igtStarted=false;
  // Keep GZ's independent controls and retained clock origin on resume.
  if(running&&trackRta&&!rtaStarted){rtaStarted=true;rtaElapsed=0;}
  if(running&&trackIgt&&!igtStarted){igtStarted=true;igtElapsed=igtExcluded=0;}
  if(rtaStarted)rtaElapsed+=seconds;
  if(igtStarted){igtElapsed+=gameTicks;if(loading)igtExcluded+=gameTicks;}
  if(running&&trackRta){rta=rtaElapsed;frames+=logicalFrames;}
  if(running&&trackIgt&&!loading)igt=igtElapsed-igtExcluded;
  if(trackLoads){
   if(loading)pendingLoad+=gameTicks;
   else {loads+=pendingLoad;pendingLoad=0;}
  }
 }
 void reset(){*this=PracticeTimers{};}
};
}
