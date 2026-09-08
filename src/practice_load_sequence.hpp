#pragma once
namespace gz {
// TPGZ triggerLoad publishes enabled only after the During override has run.
template<class Next,class During>
void composePracticeDestination(Next& next,const char* stage,int room,int point,int layer,During during){
 next.getStartStage()->set(stage,room,point,layer);
 next.wipe=13;next.wipe_speed=0;
 during();
 next.onEnable();
}
}
