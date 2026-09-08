// Original 40-step GZ LJA/Midna projections through native movement.
// Unlike the console snapshot, restore animation history, wind/slip state,
// native query caches and the shared matrix stack as well as the player pose.
#include "core.hpp"
#include "loading.hpp"
#include "d/actor/d_a_alink.h"
#include "d/d_com_inf_game.h"
#include "d/d_debug_viewer.h"
#include "m_Do/m_Do_ext.h"
#include "m_Do/m_Do_mtx.h"
#include "SSystem/SComponent/c_counter.h"
#include <array>
#include "scoped_restore.hpp"
namespace gz {
u8 geometryOpacity();
DEFINE_HOOK_SYMBOL("daAlink_c::posMove",void(daAlink_c*),ProjectionMove);
DEFINE_HOOK_SYMBOL("daAlink_c::seStartOnlyReverbLevel",void(daAlink_c*,u32),ProjectionSound);
DEFINE_HOOK_SYMBOL("dDbVw_deleteDrawPacketList",void(),ProjectionDraw);
static bool projecting=false,ljaValid=false,midnaValid=false,ljaGot=false;
static unsigned actorId=~0u;
static std::array<cXyz,40> lja,midna;
struct PredictionScope {
 Restore state;
 PredictionScope(daAlink_c* p){
#define SAVE(field) state.hold(p->field)
  SAVE(current);SAVE(old);SAVE(shape_angle);SAVE(speed);SAVE(speedF);SAVE(gravity);SAVE(maxFallSpeed);
  SAVE(mNormalSpeed);SAVE(eyePos);SAVE(mNoResetFlg3);SAVE(mLinkAcch);SAVE(mAcchCir);SAVE(mCcStts);
  SAVE(mLinkLinChk);SAVE(mLinkGndChk);SAVE(mMagneLineChk);
  SAVE(mProcVar0.field_0x3008);SAVE(field_0x37c8);SAVE(field_0x2f99);
  SAVE(field_0x342c);SAVE(field_0x3430);SAVE(field_0x3400);SAVE(field_0x3404);
  SAVE(field_0x3464);SAVE(field_0x3468);SAVE(field_0x33a0);SAVE(field_0x33b0);
  SAVE(field_0x34d4);SAVE(field_0x3798);SAVE(mSinkShapeOffset);SAVE(mLastJumpPos);SAVE(mFallHeight);
  SAVE(field_0x37b0);SAVE(field_0x35c4);SAVE(field_0x35d0);SAVE(field_0x3594);SAVE(field_0x35a0);
  SAVE(field_0x35b8);SAVE(mWindSpeed);SAVE(field_0x30cc);SAVE(field_0x3750);
#undef SAVE
  state.hold(*p->field_0x2060->getOldFrameTransInfo(0));
  state.hold(fopAcM_gc_c::mGndCheck);state.hold(fopAcM_gc_c::mGroundY);
  state.hold(*reinterpret_cast<Mtx*>(mDoMtx_stack_c::get()));
  projecting=true;
 }
 ~PredictionScope(){projecting=false;}
};
static bool ljaAction(const daAlink_c* p){return p->mProcID==daAlink_c::PROC_ATN_ACTOR_WAIT||p->mProcID==daAlink_c::PROC_CUT_JUMP;}
static bool midnaAction(const daAlink_c* p){return p->mProcID==daAlink_c::PROC_WOLF_ROLL_ATTACK_MOVE||p->mProcID==daAlink_c::PROC_WOLF_LOCK_ATTACK||p->mProcID==daAlink_c::PROC_WOLF_LOCK_ATTACK_TURN;}
static void predict(daAlink_c* p){
 ljaValid=midnaValid=false;actorId=fopAcM_GetID(p);
 if(on("lja_projection")&&ljaAction(p)&&p->mTargetedActor){
  PredictionScope restore(p);
  if(p->mProcID==daAlink_c::PROC_ATN_ACTOR_WAIT){
   p->mNormalSpeed=25;p->speed.y=27;p->setCutJumpSpeed(FALSE);p->current.angle.y=p->shape_angle.y;
  }
  ljaGot=false;
  for(auto& position:lja){ljaGot|=p->mNormalSpeed>70;ProjectionMove::g_orig(p);position=p->current.pos;}
  ljaValid=true;
 }else if(on("midna_projection")&&midnaAction(p)&&p->mWolfLockNum){
  PredictionScope restore(p);
  if(p->mProcID==daAlink_c::PROC_WOLF_ROLL_ATTACK_MOVE){
   if(auto* target=p->mWolfLockAcKeep[0].getActor())p->field_0x37c8=target->eyePos;
   p->shape_angle.y=cLib_targetAngleY(&p->current.pos,&p->field_0x37c8);p->current.angle.y=p->shape_angle.y;
   cXyz delta=p->field_0x37c8-p->eyePos;
   delta.y=std::clamp(delta.y,10.f,700.f);
   const float horizontal=delta.absXZ();
   if(horizontal>1000){const float factor=1000/horizontal;delta.x*=factor;delta.z*=factor;}
   const float distance=delta.abs(),frames=std::max(distance/85.f,1.f);
   p->mNormalSpeed=(85.f/distance)*delta.absXZ();
   p->setSpecialGravity((-2*delta.y)/(frames*frames),p->maxFallSpeed,FALSE);
   p->speed.y=-p->gravity*frames;
  }
  for(auto& position:midna){ProjectionMove::g_orig(p);position=p->current.pos;}
  midnaValid=true;
 }
}
ModResult initProjection(){
 toggle("lja_projection","Projection","LJA","Display the projected jump-attack path; green above 70 speed.");
 toggle("midna_projection","Projection","Midna charge","Display the projected path taken by a super jump.");
 auto result=guardedPre<ProjectionMove>([](ModContext*,void* args,void*,void*){
  auto* p=mods::arg<daAlink_c*>(args,0);
  if(!projecting&&p==daAlink_getAlinkActorClass()&&playable()&&!sceneLoading())predict(p);
  return HOOK_CONTINUE;
 });if(result!=MOD_OK)return result;
 result=guardedPre<ProjectionSound>([](ModContext*,void*,void*,void*){
  return projecting?HOOK_SKIP_ORIGINAL:HOOK_CONTINUE;
 });if(result!=MOD_OK)return result;
 return guardedPost<ProjectionDraw>([](ModContext*,void*,void*,void*){
  auto* p=daAlink_getAlinkActorClass();
  if(!p||sceneLoading()||fopAcM_GetID(p)!=actorId)return;
  if(ljaValid&&on("lja_projection")&&ljaAction(p)&&p->mTargetedActor){
   const GXColor color=ljaGot?GXColor{0,255,0,u8(geometryOpacity())}:GXColor{255,0,0,u8(geometryOpacity())};
   for(unsigned i=1;i<lja.size();i++)dDbVw_drawLineXlu(lja[i-1],lja[i],color,1,20);
  }
  if(midnaValid&&on("midna_projection")&&midnaAction(p)&&p->mWolfLockNum)
   for(unsigned i=1;i<midna.size();i++)dDbVw_drawLineXlu(midna[i-1],midna[i],{255,0,0,u8(geometryOpacity())},1,40);
 });
}
}
