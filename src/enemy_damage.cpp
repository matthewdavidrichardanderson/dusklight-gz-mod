/*
 * Source adaptation of cc_at_check in TP decomp / dusklight-upstream/src/d/d_cc_uty.cpp.
 * GZ suppresses health subtraction, retaining collision sound, direction and hitstop.
 * Native Bulblin actor_set replaces the fixed 0x129a console offset.
 * Host original remains unchanged when the cheat is disabled.
 */
#include "core.hpp"
#include "d/d_cc_uty.h"
#include "d/d_cc_d.h"
#include "d/d_com_inf_game.h"
#include "d/d_s_play.h"
#include "d/actor/d_a_player.h"
#include "d/actor/d_a_e_rd.h"
#include "Z2AudioLib/Z2Creature.h"
#include "SSystem/SComponent/c_math.h"
namespace gz {
DEFINE_HOOK(&cc_at_check, EnemyDamage);
static int getMapInfo(s8 param_0) {
    int map_info = 30;
    if (param_0 == 1) {
        map_info = 31;
    } else if (param_0 == 2) {
        map_info = 32;
    }

    return map_info;
}

static u32 getHitId(cCcD_Obj* i_ccObj, int i_useReboundSe) {
    dCcD_GObjInf* dObj = static_cast<dCcD_GObjInf*>(i_ccObj);
    return dObj->getHitSeID(dObj->GetAtSe(), i_useReboundSe);
}

static fopAc_ac_c* invincible_at_check(fopAc_ac_c* i_enemy, dCcU_AtInfo* i_AtInfo) {
    daPy_py_c* player_p = (daPy_py_c*)dComIfGp_getPlayer(0);
    i_AtInfo->mpActor = at_power_check(i_AtInfo);

    f32 x_diff;
    f32 z_diff;
    if (i_AtInfo->mpActor != NULL) {
        cXyz tmp = i_AtInfo->mpActor->speed;
        tmp.y = 0.0f;
        if (tmp.abs() > 100.0f) {
            f32 x = i_AtInfo->mpActor->speed.x;
            f32 z = i_AtInfo->mpActor->speed.z;
            i_AtInfo->mHitDirection.y = cM_atan2s(-x, -z) + (s16)cM_rndFX(4000.0f);
        } else {
            if (fopAcM_GetName(i_AtInfo->mpActor) == fpcNm_BOOMERANG_e) {
                x_diff = i_enemy->current.pos.x - player_p->current.pos.x;
                z_diff = i_enemy->current.pos.z - player_p->current.pos.z;
                i_AtInfo->mHitDirection.y = cM_atan2s(-x_diff, -z_diff) + (s16)cM_rndFX(10000.0f);
            } else {
                x_diff = i_enemy->current.pos.x - i_AtInfo->mpActor->current.pos.x;
                z_diff = i_enemy->current.pos.z - i_AtInfo->mpActor->current.pos.z;
                i_AtInfo->mHitDirection.y = cM_atan2s(-x_diff, -z_diff);
            }
        }

        if (i_AtInfo->mHitType == HIT_TYPE_LINK_NORMAL_ATTACK &&
            player_p->getCutType() == daPy_py_c::CUT_TYPE_HEAD_JUMP)
        {
            i_AtInfo->mHitDirection.y = player_p->shape_angle.y;
        }

        if (i_AtInfo->mpCollider->ChkAtType(AT_TYPE_HOOKSHOT) &&
            fopAcM_CheckStatus(i_enemy, 0x380000))
        {
            i_AtInfo->mAttackPower = 0;
        }

        if (static_cast<dCcD_GObjInf*>(i_AtInfo->mpCollider)->GetAtMtrl() == dCcD_MTRL_LIGHT) {
            if (fopAcM_GetName(i_enemy) == fpcNm_B_GND_e) {
                i_AtInfo->mAttackPower = 0;
            } else if (fopAcM_GetName(i_enemy) != fpcNm_B_ZANT_e) {
                i_AtInfo->mAttackPower = 100;
            }
        }

        if (i_AtInfo->mHitType == HIT_TYPE_LINK_NORMAL_ATTACK) {
            if (!daPy_py_c::checkNowWolf()) {
                if (player_p->checkMasterSwordEquip()) {
                    i_AtInfo->mAttackPower *= 2;
                }

                if (daPy_py_c::checkWoodSwordEquip()) {
                    i_AtInfo->mAttackPower /= 2;
                }
            }

            if (player_p->getSwordAtUpTime()) {
                i_AtInfo->mAttackPower *= 2;
                i_AtInfo->mHitStatus = 1;
            }
        }


        if (i_AtInfo->mAttackPower != 0) {
            // GZ suppresses health subtraction; retain attack power and feedback.
        }

        s8 pause_time = 0;
        if (i_AtInfo->mAttackPower != 0 && i_enemy->health <= 0) {
            i_AtInfo->mHitStatus = 2;
            i_enemy->health = 0;
        }

        int uvar8;
        if (i_AtInfo->mpCollider->ChkAtType(AT_TYPE_HOOKSHOT) &&
            !fopAcM_CheckStatus(i_enemy, 0x280000))
        {
            uvar8 = 1;
        } else {
            uvar8 = 0;
        }

        if (i_AtInfo->mpSound != NULL) {
            if (i_AtInfo->field_0x18 != 0) {
                i_AtInfo->mpSound->startCollisionSE(getHitId(i_AtInfo->mpCollider, uvar8),
                                                    i_AtInfo->field_0x18);
            } else {
                i_AtInfo->mpSound->startCollisionSE(getHitId(i_AtInfo->mpCollider, uvar8),
                                                    getMapInfo(i_AtInfo->mHitStatus));
            }
        }

        if (i_AtInfo->mHitStatus != 0) {
            pause_time = 5;
        } else {
            if (i_AtInfo->mAttackPower > 1) {
                pause_time = 2;
            }
        }

        if (i_AtInfo->mpCollider->ChkAtType(AT_TYPE_MIDNA_LOCK) ||
            ((daPy_py_c*)dComIfGp_getPlayer(0))->checkHorseRide())
        {
            // actor is Bulblin or Horseback Ganon
            if ((fopAcM_GetName(i_enemy) == fpcNm_E_RD_e && reinterpret_cast<e_rd_class*>(i_enemy)->actor_set != 0) ||
                fopAcM_GetName(i_enemy) == fpcNm_B_GND_e)
            {
                pause_time = 3;
            } else {
                pause_time = 0;
            }
        }

        s16 ac_name = fopAcM_GetName(i_enemy);
        // actor is Stalkin, Chu, Keese, Shadow Keese, Shadow Vermin, Baby Gohma, or Rat
        if (ac_name == fpcNm_E_BS_e || ac_name == fpcNm_E_SM2_e || ac_name == fpcNm_E_BA_e ||
            ac_name == fpcNm_E_YK_e || ac_name == fpcNm_E_YG_e || ac_name == fpcNm_E_GM_e ||
            ac_name == fpcNm_E_MS_e)
        {
            pause_time = 0;
        }

        if ((i_AtInfo->mHitType == HIT_TYPE_LINK_NORMAL_ATTACK ||
             i_AtInfo->mpCollider->ChkAtType(AT_TYPE_THROW_OBJ)) &&
            !player_p->checkCutJumpCancelTurn())
        {
            if (i_AtInfo->mpCollider->ChkAtType(AT_TYPE_THROW_OBJ)) {
                pause_time = 4;
            }
            dScnPly_c::setPauseTimer(pause_time);
        }
    }

    return i_AtInfo->mpActor;
}

ModResult initEnemyDamage() {
 return guardedPre<EnemyDamage>([](ModContext*,void* args,void* ret,void*) {
  if(!on("invincible_enemies"))return HOOK_CONTINUE;
  auto* enemy=mods::arg<fopAc_ac_c*>(args,0);
  auto* info=mods::arg<dCcU_AtInfo*>(args,1);
  if(!enemy||!info||!dComIfGp_getPlayer(0))return HOOK_CONTINUE;
  *static_cast<fopAc_ac_c**>(ret)=invincible_at_check(enemy,info);
  return HOOK_SKIP_ORIGINAL;
 });
}
}