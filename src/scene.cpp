// Native equivalents of TPGZ scene.cpp and utils/audio.cpp.
#include "core.hpp"
#include "m_Do/m_Do_audio.h"
#include "link_tools.hpp"
#include "loading.hpp"
#include "d/d_com_inf_actor.h"
#include "d/d_com_inf_game.h"
#include "d/d_meter_HIO.h"
#include "f_op/f_op_actor.h"
#include "Z2AudioLib/Z2SoundMgr.h"
#include <array>
#include <cmath>
namespace gz {
namespace {
template<class T> struct Override {
 bool owned=false;T saved{},written{};
 void apply(T& value,bool enabled,T target){
  if(enabled){if(!owned){saved=value;owned=true;}value=written=target;}
  else if(owned){if(value==written)value=saved;owned=false;}
 }
};
Override<bool> actors;
Override<float> hud;
Override<int> camera;
bool hidden=false,wasHidden=false,timeOwned=false;
int timeSaved=0;
std::array<float,19> audioSaved{};
bool audioActive=false;
Z2SoundMgr* audioOwner=nullptr;
float& audioVolume(Z2SoundMgr* sound,int i){
 if(i==0)return sound->getSeqMgr()->getParams()->params_.mVolume;
 if(i==1)return sound->getStreamMgr()->getParams()->params_.mVolume;
 if(i==2)return sound->getSeMgr()->getParams()->params_.mVolume;
 return sound->getSeMgr()->getCategory(i-3)->getParams()->params_.mVolume;
}
float normalizedTime(){float t=std::fmod(dComIfGs_getTime(),360.f);return t<0?t+360:t;}
void setClock(int hours,int minutes){dComIfGs_setTime(float((hours*60+minutes+1440)%1440)*.25f);}
}
DEFINE_HOOK_SYMBOL("Z2SoundMgr::mixOut",void(Z2SoundMgr*),SceneAudioMix);
void initScene(){
 static u32 category=0,sound=0;
 number("sound_category","Sound test","category id:",0,9,[](){return category;},[](int64_t v){category=u32(v);});
 number("sound_id","Sound test","sound id:",0,65535,[](){return sound;},[](int64_t v){sound=u32(v);});
 action("sound_play","Sound test","play",[](){mDoAud_seStart((category<<16)|sound,nullptr,0,0);});
 action("sound_stop","Sound test","stop",[](){mDoAud_seStop((category<<16)|sound,0);});
 toggle("disable_bgm","Scene","disable bg music","Mute background, enemy and stream music.");
 toggle("disable_sfx","Scene","disable sfx","Mute all native sound-effect categories.");
 toggle("freeze_actors","Scene","freeze actors","Pause native actor execution.");
 toggle("freeze_camera","Scene","freeze camera","Hold the native event camera.");
 toggle("hide_actors","Scene","hide actors","Hide actor drawing with the native actor stop flag.");
 toggle("hide_hud","Scene","hide hud","Hide the game HUD while retaining GZ overlays.");
 toggle("freeze_time","Scene","freeze time","Stop in-game time progression.");
 number("time_hours","Scene","time (hrs):",0,23,[](){return int(normalizedTime()/15);},
  [](int64_t h){setClock(int(h),int(normalizedTime()*4)%60);});
 number("time_minutes","Scene","time (mins):",0,59,[](){return int(normalizedTime()*4)%60;},
  [](int64_t m){setClock(int(normalizedTime()/15),int(m));});
}
void sceneTick(){
 actors.apply(g_dComIfAc_gameInfo.mPause,on("freeze_actors"),true);
 camera.apply(dComIfGp_getEventManager().mCameraPlay,on("freeze_camera")||moveLinkActive(),1);
 hud.apply(g_drawHIO.mParentAlpha,on("hide_hud")||moveLinkActive(),0.f);
 auto flags=fopAc_ac_c::getStopStatus();
 if(on("hide_actors")){
  if(!hidden){wasHidden=(flags&0x100)!=0;hidden=true;}
  fopAc_ac_c::setStopStatus(flags|0x100);
 }else if(hidden){
  if(!wasHidden)fopAc_ac_c::setStopStatus(flags&~0x100);
  hidden=false;
 }
 const int time=dComIfGp_roomControl_getTimePass();
 if(on("freeze_time")){
  if(!timeOwned||time!=0){timeSaved=time;timeOwned=true;}
  dComIfGp_roomControl_setTimePass(0);
 }else if(timeOwned){
  if(time==0)dComIfGp_roomControl_setTimePass(timeSaved);
  timeOwned=false;
 }
}
void shutdownScene(){
 actors.apply(g_dComIfAc_gameInfo.mPause,false,false);
 camera.apply(dComIfGp_getEventManager().mCameraPlay,false,0);
 hud.apply(g_drawHIO.mParentAlpha,false,1.f);
 if(hidden&&!wasHidden)fopAc_ac_c::setStopStatus(fopAc_ac_c::getStopStatus()&~0x100);
 hidden=false;
 if(timeOwned&&dComIfGp_roomControl_getTimePass()==0)dComIfGp_roomControl_setTimePass(timeSaved);
 timeOwned=false;
}
ModResult installScene(){
 auto r=guardedPre<SceneAudioMix>([](ModContext*,void* args,void*,void*){
  auto* sound=mods::arg<Z2SoundMgr*>(args,0);
  const bool music=on("disable_bgm"),effects=on("disable_sfx");
  audioActive=music||effects;audioOwner=sound;
  if(audioActive)for(int i=0;i<19;i++){
   auto& value=audioVolume(sound,i);audioSaved[i]=value;
   if(i<2?music:effects)value=0;
  }
  return HOOK_CONTINUE;
 });
 if(r!=MOD_OK)return r;
 return guardedPost<SceneAudioMix>([](ModContext*,void* args,void*,void*){
  auto* sound=mods::arg<Z2SoundMgr*>(args,0);
  if(audioActive&&sound==audioOwner)for(int i=0;i<19;i++)audioVolume(sound,i)=audioSaved[i];
  audioActive=false;
 });
}
}
