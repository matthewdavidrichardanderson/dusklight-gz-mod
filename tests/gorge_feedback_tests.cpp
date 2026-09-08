#include "gorge_timing.hpp"
#include <vector>
#include <utility>
using gz::GorgeTiming;
int main(){
 // Arrival can run entirely through the event controller without PROC_WARP.
 {
  GorgeTiming timer;std::vector<std::pair<int,int>> results;
  auto emit=[&](GorgeTiming::Input input,int offset){results.emplace_back(input,offset);};
  timer.tick(10,GorgeTiming::arrivalActive(1,false),false,0,emit);
  timer.tick(140,GorgeTiming::arrivalActive(1,false),false,GorgeTiming::Jump,emit);
  timer.tick(141,GorgeTiming::arrivalActive(0,false),true,0,emit);
  timer.tick(142,GorgeTiming::arrivalActive(0,false),true,GorgeTiming::Boots,emit);
  if(results!=std::vector<std::pair<int,int>>{{1,-1},{2,1}})return 3;
 }
 for(unsigned release:{132u,160u,201u}){
  GorgeTiming timer;std::vector<std::pair<int,int>> results;
  auto emit=[&](GorgeTiming::Input input,int offset){results.emplace_back(input,offset);};
  timer.tick(0,true,false,0,emit);
  timer.tick(release-9,false,false,GorgeTiming::Jump,emit);
  timer.tick(release-8,false,false,GorgeTiming::Jump,emit);
  timer.tick(release-1,false,false,GorgeTiming::Boots,emit);
  timer.tick(release,false,true,GorgeTiming::Jump,emit);
  timer.tick(release+1,false,true,GorgeTiming::Jump|GorgeTiming::Boots,emit);
  timer.tick(release+10,false,true,GorgeTiming::Boots,emit);
  const std::vector<std::pair<int,int>> expected={{1,-8},{2,-1},{1,0},{2,1}};
  if(results!=expected)return 1;
  // A later talk/event signal must not restart this attempt or repeat success.
  timer.tick(release+20,true,true,GorgeTiming::Jump,emit);
  if(results!=expected)return 4;
  timer.reset();results.clear();
  timer.tick(300,true,false,0,emit);
  timer.tick(460,false,true,0,emit);
  timer.tick(469,false,true,GorgeTiming::Jump,emit);
  if(results!=std::vector<std::pair<int,int>>{{1,9}})return 2;
 }
 return 0;
}
