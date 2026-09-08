// Native adaptations of TPGZ cheats.cpp and utils/hook.cpp (GPL-3.0).
#include "core.hpp"
#include "link_tools.hpp"

#include "d/d_com_inf_game.h"
#include "d/actor/d_a_alink.h"
#include "d/actor/d_a_e_zs.h"
#include "d/actor/d_a_e_s1.h"
#include "d/d_msg_flow.h"
#include "f_pc/f_pc_name.h"

#include <string_view>
#include <array>

namespace gz {
DEFINE_HOOK(&daAlink_c::execute, LinkExecute);
DEFINE_HOOK(&daAlink_c::checkCastleTownUseItem, CastleItems);
DEFINE_HOOK(&dMsgFlow_c::query042, TransformQuery);
static unsigned playerId=~0u;
static bool wallLease=false,wallWas=false,lineWas=false,invLease=false;
static std::array<bool,4> tgWas{};
static daAlink_c* player(){return static_cast<daAlink_c*>(daPy_getPlayerActorClass());}
static void restorePlayer() {
 auto* p=player();if(!p||fopAcM_GetID(p)!=playerId){wallLease=invLease=false;return;}
 if(wallLease){
  if(!wallWas)p->mLinkAcch.ClrWallNone();
  if(!lineWas)p->mLinkAcch.OffLineCheckNone();
  wallLease=false;
 }
 if(invLease){
  for(int i=0;i<3;i++)if(tgWas[i])p->mTgCyls[i].OnTgSetBit();
  if(tgWas[3])p->mAtSph.OnTgSetBit();
  invLease=false;
 }
}
static void applyPlayer(daAlink_c* p) {
 if(!p)return;
 if(fopAcM_GetID(p)!=playerId){wallLease=invLease=false;playerId=fopAcM_GetID(p);}
 if(on("infinite_hearts"))dComIfGs_setLife((dComIfGs_getMaxLife()/5)*4);
 if(on("infinite_air")){dComIfGp_setOxygen(600);dComIfGp_setNowOxygen(600);}
 if(on("infinite_oil"))dComIfGs_setOil(21600);
 if(on("infinite_arrows"))dComIfGs_setArrowNum(99);
 if(on("infinite_slingshot"))dComIfGs_setPachinkoNum(99);
 if(on("infinite_rupees"))dComIfGs_setRupee(1000);
 if(on("infinite_bombs"))for(int i=0;i<3;i++)dComIfGs_setBombNum(i,99);
 if(on("no_sinking"))p->mSinkShapeOffset=0;
 if(on("disable_walls")||moveLinkActive()){
  if(!wallLease){wallWas=(p->mLinkAcch.m_flags & dBgS_Acch::FLAG_WALL_NONE)!=0;lineWas=p->mLinkAcch.ChkLineCheckNone();wallLease=true;}
  p->mLinkAcch.SetWallNone();p->mLinkAcch.OnLineCheckNone();
 }else if(wallLease){if(!wallWas)p->mLinkAcch.ClrWallNone();if(!lineWas)p->mLinkAcch.OffLineCheckNone();wallLease=false;}
 if(on("invincible")){
  if(!invLease){for(int i=0;i<3;i++)tgWas[i]=p->mTgCyls[i].ChkTgSet();tgWas[3]=p->mAtSph.ChkTgSet();invLease=true;}
  for(auto& cyl:p->mTgCyls){cyl.OffTgSetBit();cyl.ResetTgHit();}
  if(p->checkWolf()){p->mAtSph.OffTgSetBit();p->mAtSph.ResetTgHit();}
 }else if(invLease){for(int i=0;i<3;i++)if(tgWas[i])p->mTgCyls[i].OnTgSetBit();if(tgWas[3])p->mAtSph.OnTgSetBit();invLease=false;}
}
static int protectSpecialEnemy(void* raw,void*) {
 auto* a=static_cast<fopAc_ac_c*>(raw);
 if(fopAcM_GetName(a)==fpcNm_E_ZS_e){
  auto* z=static_cast<daE_ZS_c*>(a);
  if(z->mAction==2){z->mAction=1;z->mMode=0;z->mCyl.OnTgSetBit();z->mCyl.OnCoSetBit();z->health=20;}
 }else if(fopAcM_GetName(a)==fpcNm_E_S1_e){
  auto* s=static_cast<e_s1_class*>(a);s->health=50;
  if(s->mAction==9||s->mAction==5||s->mAction==10)s->mAction=0;
 }
 return 1;
}
ModResult initEnemyDamage();
ModResult initItemLifetime(); ModResult initClawshot();
ModResult initCheats(){
 toggle("infinite_air","Cheats","Infinite air","Keeps oxygen at TPGZ's 600-unit value.");
 toggle("infinite_arrows","Cheats","Infinite arrows","Keeps 99 arrows.");
 toggle("infinite_bombs","Cheats","Infinite bombs","Keeps 99 bombs in each bag.");
 toggle("infinite_hearts","Cheats","Infinite hearts","Refills current health to maximum hearts.");
 toggle("infinite_oil","Cheats","Infinite oil","Keeps 21,600 units of lantern oil.");
 toggle("infinite_rupees","Cheats","Infinite rupees","Keeps 1,000 rupees.");
 toggle("infinite_slingshot","Cheats","Infinite slingshot","Keeps 99 seeds.");
 toggle("invincible","Cheats","Invincible Link","Disables Link's target collision volumes.");
 toggle("invincible_enemies","Cheats","Invincible enemies","Preserves GZ attack power and hit feedback while suppressing health subtraction, with Stalchild and Shadow Beast handling.");
 toggle("moon_jump","Cheats","Moon jump","Hold R + A by default. Vertical speed 56.");
 toggle("disable_walls","Cheats","Door storage / disable walls","Disables Link wall and line collision, matching GZ door storage.");
 toggle("super_clawshot","Cheats","Super clawshot","TPGZ speed, range and surface attachment via decomp routines; boss-specific parameters remain unchanged.");
 toggle("unrestricted_items","Cheats","Unrestricted items","Allows item use in Castle Town, matching upstream's restriction hook.");
 toggle("transform_anywhere","Cheats","Transform anywhere","Bypasses transformation location restrictions.");
 toggle("disable_item_timer","Cheats","Disable item timer","Freezes native item lifetime timers; resumes their countdown when disabled.");
 toggle("no_sinking","Tools","No sinking in sand","Clears Link's sinking offset each simulation step.");
 auto r=guardedPre<LinkExecute>([](ModContext*,void* a,void*,void*){applyPlayer(mods::arg<daAlink_c*>(a,0));return HOOK_CONTINUE;});if(r!=MOD_OK)return r;
 r=guardedPre<CastleItems>([](ModContext*,void*,void* ret,void*){
  if(!on("unrestricted_items"))return HOOK_CONTINUE;*static_cast<bool*>(ret)=true;return HOOK_SKIP_ORIGINAL;
 });if(r!=MOD_OK)return r;
 r=guardedPre<TransformQuery>([](ModContext*,void*,void* ret,void*){
  if(!on("transform_anywhere"))return HOOK_CONTINUE;*static_cast<u16*>(ret)=0;return HOOK_SKIP_ORIGINAL;
 });
 if(r!=MOD_OK)return r;
 r=initEnemyDamage();if(r!=MOD_OK)return r;
 r=initItemLifetime();if(r!=MOD_OK)return r;return initClawshot();
}
void cheatTick(){if(playable()&&on("invincible_enemies"))fopAcIt_Executor(protectSpecialEnemy,nullptr);}
void shutdownCheats(){restorePlayer();}
}
