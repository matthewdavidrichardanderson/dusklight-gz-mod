// GCN GZ practice saves using the decomp's big-endian savedata representation.
#include "core.hpp"
#include "loading.hpp"
#include "practice_data.hpp"
#include "mods/svc/resource.h"
#include "d/d_com_inf_game.h"
#include "d/d_camera.h"
#include "d/d_s_play.h"
#include "f_pc/f_pc_manager.h"
#include "d/actor/d_a_player.h"
#include "SSystem/SComponent/c_phase.h"
#include <optional>
#include <cstring>
#include <type_traits>
#include <cstddef>
#include "practice_specials.hpp"
#include "practice_load_sequence.hpp"
namespace gz {
// These are serialized save fields, not console RAM addresses. Fail on layout drift.
static_assert(std::is_trivially_copyable_v<dSv_save_c>);
static_assert(sizeof(dSv_save_c)==0x958);
static_assert(offsetof(dSv_save_c,mPlayer)==0);
static_assert(offsetof(dSv_save_c,mSave)==0x1f0);
static_assert(offsetof(dSv_save_c,mSave2)==0x5f0);
static_assert(offsetof(dSv_save_c,mEvent)==0x7f0);
static_assert(offsetof(dSv_save_c,mMiniGame)==0x940);
static_assert(offsetof(dSv_player_c,mPlayerReturnPlace)==0x58);
// phase_1 is ambiguous in release manifests. Its first stage application has a unique native target.
DEFINE_HOOK(&dComIfG_play_c::setStartStage,PracticeStartStage);
DEFINE_HOOK_SYMBOL("dScnPly_Create",int(scene_class*),PracticeCreated);
DEFINE_HOOK(&fpcM_Management,PracticePostLoop);
static std::optional<dSv_save_c> pendingSave;
static std::optional<u8> pendingVibration;
static const PracticeEntry* pendingEntry=nullptr;
static const PracticeEntry* lastEntry=nullptr;
static bool injected=false,arrived=false;
using SetupCallback=void(*)();
static SetupCallback pendingAfter=nullptr,lastDuring=nullptr,lastAfter=nullptr;
static unsigned actorWaitFrames=0;
static std::string lastLoadLabel;
static bool noSpecial(const char* name){return std::strcmp(name,"nullptr")==0;}
static bool supported(const PracticeEntry& e){return e.present&&supportsSpecial(e.during)&&supportsSpecial(e.after);}
static void loadPractice(const PracticeEntry& e,SetupCallback during=nullptr,SetupCallback after=nullptr,const char* label=nullptr){
 if(!playable()||pendingEntry||!supported(e))return;
 ResourceBuffer buffer=RESOURCE_BUFFER_INIT;
 if(svc_resource->load(mod_ctx,e.path,&buffer)!=MOD_OK){notify("Practice save resource unavailable");return;}
 if(buffer.size!=2700||!buffer.data){svc_resource->free(mod_ctx,&buffer);notify("Invalid practice save size");return;}
 const auto* bytes=static_cast<const unsigned char*>(buffer.data);
 bool valid=false;
 for(size_t i=0;i<8;i++){
  const auto c=bytes[0x58+i];
  if(c==0){valid=i>0;break;}
  if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'))break;
 }
 if(!valid){svc_resource->free(mod_ctx,&buffer);notify("Invalid practice destination");return;}
 // Construct through a native copy (no initialization calls at DLL load).
 pendingSave.emplace(g_dComIfG_gameInfo.info.getSavedata());
 std::memcpy(&*pendingSave,buffer.data,sizeof(dSv_save_c));
 svc_resource->free(mod_ctx,&buffer);
 // Retain game configuration; practice setup owns gameplay state.
 pendingSave->getPlayer().getConfig()=g_dComIfG_gameInfo.info.getPlayer().getConfig();
 // Snapshot the saved vibration preference, not the title
 // scene's inactive live vibration flag.
 pendingVibration=dComIfGs_getOptVibration();
 pendingSave->getPlayer().getConfig().setVibration(*pendingVibration);
 if(on("practice_swap_equips")){
  auto& status=pendingSave->getPlayer().getPlayerStatusA();
  const auto x=status.getSelectItemIndex(SELECT_ITEM_X),mix=status.getMixItemIndex(SELECT_ITEM_X);
  status.setSelectItemIndex(SELECT_ITEM_X,status.getSelectItemIndex(SELECT_ITEM_Y));
  status.setMixItemIndex(SELECT_ITEM_X,status.getMixItemIndex(SELECT_ITEM_Y));
  status.setSelectItemIndex(SELECT_ITEM_Y,x);status.setMixItemIndex(SELECT_ITEM_Y,mix);
 }
 auto& ret=pendingSave->getPlayer().getPlayerReturnPlace();
 g_dComIfG_gameInfo.info.getRestart().mLastSpeedF=0;
 g_dComIfG_gameInfo.info.getRestart().mLastMode=0;
 g_dComIfG_gameInfo.info.getRestart().setStartPoint(ret.getPlayerStatus());
 pendingAfter=after;lastDuring=during;lastAfter=after;
 lastLoadLabel=label?label:e.label;
 pendingEntry=&e;lastEntry=&e;injected=false;arrived=false;actorWaitFrames=0;
 const int layer=dComIfG_play_c::getLayerNo_common_common(ret.getName(),ret.getRoomNo(),0xff);
 cDmr_SkipInfo=0;
 // Compose the complete destination before publishing the request. The native
 // next-stage setter refuses replacements once enabled (unlike GZ's field writes).
 composePracticeDestination(g_dComIfG_gameInfo.play.mNextStage,ret.getName(),ret.getRoomNo(),ret.getPlayerStatus(),layer,[&](){
  if(during)during();else runSpecial(e.during,e);
 });
 closeMenu();notify(std::string("Loading practice: ")+lastLoadLabel);
}
void loadCheckerPractice(const char* resource,const char* label,SetupCallback during,SetupCallback after){
 for(const auto& entry:practiceEntries)if(std::strcmp(entry.path,resource)==0){
  loadPractice(entry,during,after,label);return;
 }
 notify("Checker practice resource missing");
}
void cancelPracticeForSpeedrun(){
 pendingEntry=nullptr;pendingSave.reset();pendingVibration.reset();
 injected=false;arrived=false;pendingAfter=nullptr;
}
bool practiceLoadPending(){return pendingEntry!=nullptr;}
void reloadPractice(){if(lastEntry){const auto label=lastLoadLabel;loadPractice(*lastEntry,lastDuring,lastAfter,label.c_str());}else notify("No practice save loaded yet");}
void practiceTick(){
 if(!pendingEntry||!arrived||!playable()||sceneLoading())return;
 // Raw practice injection bypasses regular save loading's setNowVibration.
 // Restore once after arrival, independently of later actor-setup waits.
 if(pendingVibration){
  dComIfGs_setOptVibration(*pendingVibration);
  dComIfGp_setNowVibration(*pendingVibration);
  pendingVibration.reset();
 }
 const auto& e=*pendingEntry;
 const bool needsCamera=!pendingAfter&&((noSpecial(e.after)&&(e.requirements&2))||
  std::strcmp(e.after,"SaveMngSpecial_CenterCamera")==0||std::strcmp(e.after,"SaveMngSpecial_SolBacktrackCamera")==0||
  std::strcmp(e.after,"SaveMngSpecial_KB4")==0||std::strcmp(e.after,"SaveMngSpecial_GorgeVoid")==0||
  std::strcmp(e.after,"SaveMngSpecial_StallordCad")==0);
 if((!pendingAfter&&!specialActorsReady(e.after))||(needsCamera&&!dCam_getBody())){
  if(++actorWaitFrames<300)return;
  notify(std::string("Practice setup FAILED: required actor or camera missing for ")+e.label);
  pendingEntry=nullptr;pendingSave.reset();pendingVibration.reset();injected=false;arrived=false;return;
 }
 if(!pendingAfter&&noSpecial(e.after)&&(e.requirements&3)){
  placeNativeLink(cXyz(e.position[0],e.position[1],e.position[2]),static_cast<s16>(e.angle));
 }
 if(!pendingAfter&&noSpecial(e.after)&&(e.requirements&2))if(auto* camera=dCam_getBody()){
  cXyz eye(e.eye[0],e.eye[1],e.eye[2]),center(e.center[0],e.center[1],e.center[2]);
  camera->Reset(center,eye);
 }
 if(pendingAfter)pendingAfter();else runSpecial(e.after,e);
 svc_log->info(mod_ctx,(std::string("Practice setup applied: ")+e.category+" / "+e.label+"; during="+(lastDuring?"checker override":e.during)+"; after="+(pendingAfter?"checker override":e.after)).c_str());
 pendingEntry=nullptr;pendingSave.reset();pendingVibration.reset();injected=false;arrived=false;
}
void initPractice(){
 toggle("practice_swap_equips","Practice","Swap X / Y equipment","Swap selected and mixed items when loading a practice save.");
 action("reload_practice","Practice","Reload last practice save",[](){reloadPractice();}).available=[](){return lastEntry&&!pendingEntry;};
 for(const auto& e:practiceEntries){
  const auto id=std::string("practice_")+e.category+"_"+std::to_string(e.index);
  auto& control=action(id.c_str(),"Practice",(std::string(e.categoryLabel)+" / "+e.label).c_str(),[&e](){loadPractice(e);});
  control.help=e.description;control.available=[](){return !pendingEntry;};
  if(!supported(e))control.reason=std::string("Setup callback not ported yet: ")+e.during+" / "+e.after;
 }
}
ModResult installPractice(){
 auto r=guardedPre<PracticeStartStage>([](ModContext*,void* args,void*,void*){
  if(pendingSave&&!injected&&mods::arg<dComIfG_play_c*>(args,0)==&g_dComIfG_gameInfo.play
     &&mods::arg<dStage_startStage_c*>(args,1)==dComIfGp_getNextStartStage()){
   g_dComIfG_gameInfo.info.setSavedata(*pendingSave);
   const int stage=g_dComIfG_gameInfo.info.getDan().mStageNo;
   if(stage>=0&&stage<dSv_save_c::STAGE_MAX)g_dComIfG_gameInfo.info.getSave(stage);
   // GZ overwrites the scratch-buffer tail including dungeon transient state.
   // Use native initialization instead of carrying live flags across same-dungeon saves.
   g_dComIfG_gameInfo.info.resetDan();
   injected=true;
  }
  return HOOK_CONTINUE;
 });
 if(r!=MOD_OK)return r;
 r=guardedPost<PracticeCreated>([](ModContext*,void*,void* ret,void*){
  if(!pendingEntry)return;
  const auto result=*static_cast<int*>(ret);
  if(injected&&result==cPhs_COMPLEATE_e)arrived=true;
  else if(result==cPhs_ERROR_e){
   notify(std::string("Practice load FAILED: scene creation failed for ")+pendingEntry->label);
   pendingEntry=nullptr;pendingSave.reset();pendingVibration.reset();injected=false;arrived=false;
  }
 });
 if(r!=MOD_OK)return r;
 // GZ checks completion in both loop phases. The native scene request can finish
 // inside Management; apply before the next actor execution consumes event orders.
 return guardedPost<PracticePostLoop>([](ModContext*,void*,void*,void*){practiceTick();});
}
}
