// Movement and controls from TPGZ features/moveactor (GPL-3.0).
#include "core.hpp"
#include "actor_tools.hpp"
#include "link_tools.hpp"
#include "loading.hpp"
#include "mods/svc/camera.h"
#include "d/d_com_inf_game.h"
#include "d/d_camera.h"
#include "d/d_meter_HIO.h"
#include "d/actor/d_a_alink.h"
#include "m_Do/m_Do_controller_pad.h"
#include <cmath>
#include <numbers>
namespace gz {
bool gzMenuOpen();
DEFINE_HOOK(&daAlink_c::setSpecialGravity, MoveGravity);
DEFINE_HOOK(&daAlink_c::setSingleAnime, BonkStart);
DEFINE_HOOK(&daAlink_c::setWaterInAnmRate, BonkRate);
static bool active=false,leased=false;
static fpc_ProcID owner=~fpc_ProcID(0);
static u32 savedCollision=0;
static u8 savedEvent=0;
static float angle=0;
static cXyz eye(0,0,0),center(0,0,0);
static CameraOperatorHandle cameraHandle=0;
static constexpr u32 collisionMask=dBgS_Acch::FLAG_GRND_NONE|dBgS_Acch::FLAG_ROOF_NONE;
bool moveLinkActive(){return active;}
void shutdownMoveLink(){
 active=false;
 if(!leased)return;
 auto* p=daAlink_getAlinkActorClass();
 if(p&&fopAcM_GetID(p)==owner){
  p->mLinkAcch.m_flags=(p->mLinkAcch.m_flags&~collisionMask)|savedCollision;
 }
 // Global engine state survives actor replacement; release even across a scene change.
 if(dComIfGp_getEvent()->mEventStatus==1)dComIfGp_getEvent()->mEventStatus=savedEvent;
 leased=false;
}
void toggleMoveLink(){
 if(active){shutdownMoveLink();notify("Move Link disabled");return;}
 if(!playable()||sceneLoading())return;
 auto* p=daAlink_getAlinkActorClass();if(!p||!dCam_getBody())return;
 shutdownCamera();
 owner=fopAcM_GetID(p);savedCollision=p->mLinkAcch.m_flags&collisionMask;
 savedEvent=dComIfGp_getEvent()->mEventStatus;
 angle=float(p->shape_angle.y)/65536.f*(2*std::numbers::pi);
 eye=dCam_getBody()->iEye();center=dCam_getBody()->iCenter();
 active=leased=true;notify("Move Link enabled");
}
void moveLinkTick(){
 if(!active)return;
 auto* p=daAlink_getAlinkActorClass();
 if(!on("move_link")||!playable()||sceneLoading()||!p||fopAcM_GetID(p)!=owner){shutdownMoveLink();return;}
 p->mLinkAcch.m_flags|=collisionMask;
 dComIfGp_getEvent()->mEventStatus=1;
 p->speed.setall(0);
 bool visible=false;svc_ui->is_any_document_visible(mod_ctx,&visible);
 if(visible||gzMenuOpen())return;
 auto* pad=mDoCPd_c::getGamePad(0);if(!pad)return;
 const auto buttons=pad->getButton();
 const bool lock=(buttons&PAD_TRIGGER_L)!=0;
 if(!lock)angle=float(p->shape_angle.y)/65536.f*(2*std::numbers::pi);
 auto& pos=p->current.pos;
 center.set(pos.x,pos.y+200.f,pos.z);
 eye.set(pos.x-600*std::sin(angle),pos.y+200.f,pos.z-600*std::cos(angle));
 const double yaw=std::atan2(center.z-eye.z,center.x-eye.x);
 const double horizontal=std::sqrt(double(center.x-eye.x)*(center.x-eye.x)+double(center.z-eye.z)*(center.z-eye.z));
 const double pitch=std::atan2(center.y-eye.y,horizontal);
 const int sx=pad->mMainStick.mRawX,sy=pad->mMainStick.mRawY;
 const int vertical=pad->mSubStick.mRawY,horizontalInput=-pad->mSubStick.mRawX;
 const bool fast=(buttons&PAD_TRIGGER_Z)!=0,veryFast=(buttons&PAD_TRIGGER_R)!=0;
 const double speed=fast?(veryFast?10.:2.5):1.;
 const int rotation=fast?(veryFast?800:80):30;
 pos.x+=speed*(sy*std::cos(yaw)*std::cos(pitch)-sx*std::sin(yaw));
 pos.y+=speed*(lock?0:vertical);
 pos.z+=speed*(sy*std::sin(yaw)*std::cos(pitch)+sx*std::cos(yaw));
 if(lock){
  p->shape_angle.x=s16(p->shape_angle.x+vertical*rotation);
  p->shape_angle.y=s16(p->shape_angle.y+horizontalInput*rotation);
 }else p->shape_angle.y=s16(p->shape_angle.y-horizontalInput*rotation);
}
ModResult initLinkTools(){
 toggle("fast_bonk","Tools","Fast bonk recovery","Original GZ collision animation: start at frame 50, with zero playback speed.");
 toggle("move_link","Tools","Move Link","L + R + Y toggles. Main stick moves; C-stick raises/turns. Z speeds up, Z+R fastest; L locks camera and changes pitch.");
 action("toggle_move_link","Tools","Toggle Move Link",toggleMoveLink);
 CameraOperatorDesc desc=CAMERA_OPERATOR_DESC_INIT;desc.debug_name="TPGZ Move Link";desc.priority=10;
 desc.operate=[](ModContext*,CameraOperatorState* state,void*){
  if(!active||!playable()||sceneLoading())return false;
  for(int i=0;i<3;i++){state->eye[i]=(&eye.x)[i];state->center[i]=(&center.x)[i];}
  return true;
 };
 auto r=svc_camera->register_camera_operator(mod_ctx,&desc,&cameraHandle);if(r!=MOD_OK)return r;
 r=guardedPre<BonkStart>([](ModContext*,void* args,void*,void*){
  auto* p=mods::arg<daAlink_c*>(args,0);
  if(on("fast_bonk")&&p->mProcID==daAlink_c::PROC_FRONT_ROLL_CRASH&&mods::arg<daAlink_c::daAlink_ANM>(args,1)==daAlink_c::ANM_ROLL_CRASH)
   mods::arg_ref<float>(args,3)=50.f;
  return HOOK_CONTINUE;
 });if(r!=MOD_OK)return r;
 r=guardedPre<BonkRate>([](ModContext*,void* args,void*,void*){
  auto* p=mods::arg<daAlink_c*>(args,0);
  if(on("fast_bonk")&&p->mProcID==daAlink_c::PROC_FRONT_ROLL_CRASH&&mods::arg<daPy_frameCtrl_c*>(args,1)==p->mUnderFrameCtrl)
   mods::arg_ref<float>(args,2)=0.f;
  return HOOK_CONTINUE;
 });if(r!=MOD_OK)return r;
 return guardedPre<MoveGravity>([](ModContext*,void* args,void*,void*){
  if(active||actorViewActive())mods::arg_ref<float>(args,1)=0.f;
  return HOOK_CONTINUE;
 });
}
}
