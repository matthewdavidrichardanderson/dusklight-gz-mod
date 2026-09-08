#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace gz {
class GorgeTiming {
public:
 enum Input { Jump=1, Boots=2 };
 // GZ mHalt is the decomp event-status field, not Link's warp action.
 static bool arrivalActive(unsigned eventStatus,bool warpArrival){return eventStatus==1||warpArrival;}
 struct Press {uint32_t frame=0;unsigned input=0;};
 void reset(){*this=GorgeTiming{};}
 template<class Emit> void tick(uint32_t frame,bool warp,bool ready,unsigned input,Emit emit){
  // Original GZ arms once per load, not on every subsequent event transition.
  if(warp&&!active)active=true;
  if(!active)return;
  if(!released){
   if(!ready){
    if(input){history[write]={frame,input};write=(write+1)%history.size();}
    return;
   }
   released=true;releaseFrame=frame;
   // Early inputs cannot be classified until the native transition is known.
   for(std::size_t n=0;n<history.size();++n){
    const auto& press=history[(write+n)%history.size()];
    const auto offset=int32_t(press.frame-frame);
    if(press.input&&offset>=-8&&offset<0)report(press.input,offset,emit);
   }
  }
  const auto offset=int32_t(frame-releaseFrame);
  if(offset>=0&&offset<10&&input)report(input,offset,emit);
 }
 bool tracking()const{return active;}
 bool hasReleased()const{return released;}
private:
 template<class Emit> void report(unsigned input,int offset,Emit emit){
  if((input&Jump)&&(!gotJump||offset<0)){
   emit(Jump,offset);if(offset==0)gotJump=true;
  }
  if(input&Boots)emit(Boots,offset);
 }
 std::array<Press,18> history{};
 std::size_t write=0;
 uint32_t releaseFrame=0;
 bool active=false,released=false,gotJump=false;
};
}
