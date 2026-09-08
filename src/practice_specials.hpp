// Native bridge for original GZ practice callbacks. Bodies are generated, not reimplemented.
#pragma once
#include "d/actor/d_a_alink.h"
#include "d/actor/d_a_b_zant.h"
#include "d/actor/d_a_b_ds.h"
#include "d/actor/d_a_e_zs.h"
#include "d/actor/d_a_obj_lv4RailWall.h"
#include "d/actor/d_a_obj_swspinner.h"
#include "f_op/f_op_actor_iter.h"
#include "f_pc/f_pc_name.h"
#include "c/c_damagereaction.h"
#include <string_view>
#include <cmath>
#include "native_pose.hpp"
namespace gz {
using std::string_view;
namespace practice_detail {
struct Setup {
 PracticeEntry entry{};
 struct {void (*inject_options_before_load)()=nullptr;} mPracticeFileOpts;
 void injectDefault_during(){}
 void setSaveAngle(int16_t value){entry.angle=value;}
 void setSavePosition(float x,float y,float z){entry.position[0]=x;entry.position[1]=y;entry.position[2]=z;}
 void setLinkInfo(){
  placeNativeLink(cXyz(entry.position[0],entry.position[1],entry.position[2]),static_cast<s16>(entry.angle));
 }
};
static Setup setup;
template<class Predicate> static fopAc_ac_c* find_actor(const Predicate& predicate){
 return static_cast<fopAc_ac_c*>(fopAcIt_Judge([](void* raw,void* data)->void*{
  auto* actor=static_cast<fopAc_ac_c*>(raw);
  return (*static_cast<const Predicate*>(data))(*actor)?actor:nullptr;
 },const_cast<Predicate*>(&predicate)));
}
// Setter changes must not request a second load when used by an After callback.
static void destination(const char* name,int room,int point,int layer){
 auto* next=dComIfGp_getNextStartStage();next->set(name,s8(room),s16(point),s8(layer));
}
static void setNextStageName(const char* name){auto* n=dComIfGp_getNextStartStage();destination(name,n->getRoomNo(),n->getPoint(),n->getLayer());}
static void setNextStageRoom(int room){auto* n=dComIfGp_getNextStartStage();destination(n->getName(),room,n->getPoint(),n->getLayer());}
static void setNextStagePoint(int point){auto* n=dComIfGp_getNextStartStage();destination(n->getName(),n->getRoomNo(),point,n->getLayer());}
static void setNextStageLayer(int layer){auto* n=dComIfGp_getNextStartStage();destination(n->getName(),n->getRoomNo(),n->getPoint(),layer);}
#include "practice_callbacks.inc"
}
static bool supportsSpecial(string_view name){return practice_detail::supportsSpecial(name);}
static bool specialActorsReady(string_view name){
 using practice_detail::find_actor;
 if(name=="SaveMngSpecial_SpawnHugo")return fopAcM_SearchByName(fpcNm_E_RD_e)!=nullptr;
 if(name=="SaveMngSpecial_ZantFinal"||name=="SaveMngSpecial_ZantDangoro")return fopAcM_SearchByName(fpcNm_B_ZANT_e)!=nullptr;
 if(name=="SaveMngSpecial_Stallord2")return fopAcM_SearchByName(fpcNm_B_DS_e)&&fopAcM_SearchByName(fpcNm_Obj_Lv4RailWall_e)&&fopAcM_SearchByName(fpcNm_Obj_SwSpinner_e);
 if(name=="SaveMngSpecial_OrdonRock")return find_actor([](auto& p){return fopAcM_GetName(&p)==fpcNm_Obj_Stone_e&&fopAcM_GetParam(&p)==0x00ff6511;})!=nullptr;
 if(name=="SaveMngSpecial_StallordCad"||name=="SaveMngSpecial_StallordDisplacementClip")return find_actor([](auto& p){return fopAcM_GetName(&p)==fpcNm_E_ZS_e&&int(p.current.pos.x)==-920;})!=nullptr;
 return true;
}
static void runSpecial(string_view name,const PracticeEntry& entry){
 practice_detail::setup.entry=entry;
 practice_detail::dispatchSpecial(name);
 if(name=="SaveMngSpecial_CenterCamera"||name=="SaveMngSpecial_SolBacktrackCamera"||
    name=="SaveMngSpecial_KB4"||name=="SaveMngSpecial_GorgeVoid"||name=="SaveMngSpecial_StallordCad"){
  if(auto* camera=dCam_getBody())placeNativeCamera(camera->mCenter,camera->mEye);
 }
}
}
