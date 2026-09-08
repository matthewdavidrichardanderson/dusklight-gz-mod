// GCN GorgeVoidIndicator checker from TPGZ features/gorge/src/gorge.cpp (GPL-3.0).
namespace gorge {
static GorgeTiming timing;
static char buf[80];
void reset(){timing.reset();}
void initState(){
 // This After callback may run after Link's arrival has already started. Preserve
 // the native transition history; warpToPosition/load-start already reset it.
 gorgeTrace("Gorge checker: setup applied");

 dComIfGs_onSwitch(21,3);
 // Keep the native arrival event: console numeric ID 9 is not a valid native composite ID.
 dComIfGs_setSelectItemIndex(on("practice_swap_equips")?0:1,3);
}
void warpToPosition(){
 reset();
 auto& info=g_dComIfG_gameInfo.info;
 info.mMemory.mBit.mSwitch[0]=0;
 dComIfGs_putSave(info.mDan.mStageNo);
 info.mRestart.mRoomParam=0;info.mRestart.mLastMode=0;
 g_dComIfG_gameInfo.play.mNextStage.getStartStage()->set("F_SP121",3,2,0xe);
 g_dComIfG_gameInfo.play.mNextStage.wipe=13;
 info.mRestart.mLastMode|=0x28000000;
 dComIfGs_setLife(12);
 info.mRestart.mStartPoint=2;
 info.mRestart.mRoomPos.set(-11856.857f,-5700.f,56661.5f);
 info.mRestart.mRoomAngleY=24169;
}
bool isAPressed(){
 return !(GZ_getButtonHold(L)&&GZ_getButtonHold(A))&&(GZ_getButtonPressed(A)&&GZ_getButtonPressed(L));
}
bool isBootsPressed(){
 return (!GZ_getButtonHold(X)&&GZ_getButtonPressed(X))||(!GZ_getButtonHold(Y)&&GZ_getButtonPressed(Y));
}
void execute(){
 if(!daAlink_c::checkStageName("F_SP121")){reset();return;}
 const bool wasTracking=timing.tracking(),wasReleased=timing.hasReleased();
 unsigned input=0;
 if(isAPressed())input|=GorgeTiming::Jump;
 if(gorgeChoice()==2&&isBootsPressed())input|=GorgeTiming::Boots;
 timing.tick(g_Counter.mCounter0,gorgeWarpActive(),gorgeControlReady(),input,
  [](GorgeTiming::Input kind,int offset){
   const char* name=kind==GorgeTiming::Jump?"Jump attack":"Boots";
   if(offset==0){
    std::snprintf(buf,sizeof(buf),"%s got it",name);FIFOQueue::push(buf,Queue,0x00cc0000);
   }else{
    std::snprintf(buf,sizeof(buf),"%s %df %s",name,offset<0?-offset:offset,offset<0?"early":"late");
    FIFOQueue::push(buf,Queue,offset<0?0x0000ff00:0x99000000);
   }
  });
 if(!wasTracking&&timing.tracking())gorgeTrace("Gorge checker: native arrival observed");
 if(!wasReleased&&timing.hasReleased())gorgeTrace("Gorge checker: native control returned");
}
}
