/*
 * Adapted from upstream TP decomp routines in d_a_alink_hook.inc.
 * Only used while Super Clawshot is enabled. Original routines are untouched otherwise.
 * TPGZ's four HIO values are changed in a local copy, preserving boss-specific values.
 * No writable-const tricks or Dusklight cheat settings.
 */
#include "core.hpp"
#include "clawshot_chain.hpp"
#include <cmath>
#include "d/actor/d_a_alink.h"
#include "d/actor/d_a_obj_swhang.h"
#include "d/actor/d_a_b_dr.h"
#include "d/d_com_inf_game.h"
#include "d/d_bomb.h"
#include "f_op/f_op_kankyo_mng.h"
#include "d/d_meter2_info.h"
#include "SSystem/SComponent/c_math.h"
#include "JSystem/J3DGraphBase/J3DMaterial.h"
enum { HS_MODE_NONE_e, HS_MODE_READY_e, HS_MODE_SHOOT_e=3, HS_MODE_FLY_e, HS_MODE_RETURN_e=6 };
static cXyz l_hookSnowSandHitScale(0.5f,0.5f,0.5f);
void daAlink_c::setHookshotSight() {
    auto parameters = daAlinkHIO_hookshot_c0::m;
    parameters.mShootSpeed = 2870.0f;
    parameters.mMaxLength = 69420.0f;
    parameters.mReturnSpeed = 2870.0f;
    parameters.mStickReturnSpeed = 500.0f;
    cXyz sight_pos;
    f32 max_length;
    if (checkLv7BossRoom()) {
        max_length = parameters.mBossMaxLength;
    } else {
        max_length = parameters.mMaxLength;
    }

    BOOL line_cross = checkSightLine(max_length, &sight_pos);

    if (mHookTargetAcKeep.getActor() != NULL) {
        mSight.setPos(&mHookTargetAcKeep.getActor()->eyePos);
    } else {
        mSight.setPos(&sight_pos);
    }

    if (mItemMode == HS_MODE_READY_e) {
        mSight.onDrawFlg();

        if ((line_cross && field_0x3494 < 0.0f && checkHookshotStickBG(mRopeLinChk)) ||
            mHookTargetAcKeep.getActor() != NULL)
        {
            mSight.onLockFlg();

            if (mHookTargetAcKeep.getActor() != NULL &&
                fopAcM_GetName(mHookTargetAcKeep.getActor()) == fpcNm_B_DR_e)
            {
                ((daB_DR_c*)mHookTargetAcKeep.getActor())->onTarget();
            }
        } else {
            mSight.offLockFlg();
        }
    } else {
        mSight.offDrawFlg();
    }

    mHookTargetAcKeep.clearData();
    field_0x3494 = -1.0f;
}


void daAlink_c::setHookshotPos() {
    auto parameters = daAlinkHIO_hookshot_c0::m;
    parameters.mShootSpeed = 2870.0f;
    parameters.mMaxLength = 69420.0f;
    parameters.mReturnSpeed = 2870.0f;
    parameters.mStickReturnSpeed = 500.0f;
    mDoMtx_stack_c::copy(mpLinkModel->getAnmMtx(mLeftItemJntNo));
    mDoMtx_stack_c::transM(-2.0f, 1.0f, 1.0f);
    mDoMtx_stack_c::XYZrotM(cM_deg2s(5.7f), cM_deg2s(162.0f), 0);

    J3DModel* model;
    if (field_0x3020 == 0) {
        model = mHeldItemModel;
    } else {
        model = field_0x0710;
    }
    model->setBaseTRMtx(mDoMtx_stack_c::get());

    mDoMtx_stack_c::copy(mpLinkModel->getAnmMtx(mRightItemJntNo));
    mDoMtx_stack_c::transM(-2.0f, 0.0f, 1.0f);
    mDoMtx_stack_c::XYZrotM(cM_deg2s(-78.0f), cM_deg2s(182.0f), cM_deg2s(-99.0f));

    if (field_0x3020 == 0) {
        model = field_0x0710;
    } else {
        model = mHeldItemModel;
    }
    model->setBaseTRMtx(mDoMtx_stack_c::get());

    if (mItemMode == 2 || mItemMode == HS_MODE_SHOOT_e) {
        field_0x33dc += 1.0f;

        if (field_0x33dc >= mItemBck.getBckAnm()->getFrameMax()) {
            field_0x33dc -= mItemBck.getBckAnm()->getFrameMax();
        }

        field_0x33e0 = mHookTipBck.getBckAnm()->getFrameMax();
    } else if (mItemMode == HS_MODE_FLY_e || mItemMode == 5 || mItemMode == HS_MODE_RETURN_e) {
        field_0x33dc -= 1.0f;
        if (field_0x33dc < 0.0f) {
            field_0x33dc += mItemBck.getBckAnm()->getFrameMax();
        }

        if (mItemMode == HS_MODE_RETURN_e && mHookTargetAcKeep.getActor() == NULL) {
            field_0x33e0 = 0.0f;
        } else {
            field_0x33e0 = 14.0f;
        }
    } else if (mItemMode == HS_MODE_READY_e) {
        if (!checkHookshotAnime() && mProcID != PROC_HOOKSHOT_WALL_SHOOT &&
            mProcID != PROC_HOOKSHOT_ROOF_SHOOT)
        {
            resetHookshotMode();
            field_0x33e0 = 0.0f;
        } else {
            if (field_0x33e0 < 0.1f) {
                seStartOnlyReverb(Z2SE_AL_HS_OPEN);
            }

            field_0x33e0 += 1.0f;
            if (field_0x33e0 > mHookTipBck.getBckAnm()->getFrameMax()) {
                field_0x33e0 = mHookTipBck.getBckAnm()->getFrameMax();
            }
        }
    } else {
        field_0x33e0 = 0.0f;
    }

    mItemBck.entry(field_0x0710->getModelData(), 0.0f);
    field_0x0710->calc();

    static Vec const hookRoot = {0.0f, 0.0f, 23.5f};
    mDoMtx_multVec(mHeldItemModel->getBaseTRMtx(), &hookRoot, &mHeldItemRootPos);
    mDoMtx_multVec(field_0x0710->getBaseTRMtx(), &hookRoot, &field_0x3810);

    mpHookSound->framework(0, mVoiceReverbIntensity);

    fopAc_ac_c* targetAc_p = mHookTargetAcKeep.getActor();
    f32 var_f29;
    f32 return_speed;
    f32 shoot_speed;
    f32 max_length;
    if (checkLv7BossRoom()) {
        return_speed = parameters.mBossReturnSpeed;
        shoot_speed = parameters.mBossShootSpeed;
        max_length = parameters.mBossMaxLength;
    } else {
        return_speed = parameters.mReturnSpeed;
        shoot_speed = parameters.mShootSpeed;
        max_length = parameters.mMaxLength;
    }

    if (mItemMode == HS_MODE_RETURN_e) {
        if (targetAc_p != NULL) {
            if (checkLv7BossRoom()) {
                return_speed = parameters.mBossStickReturnSpeed;
            } else {
                return_speed = parameters.mStickReturnSpeed;
            }
        }

        if (checkModeFlg(0x400)) {
            return_speed += current.pos.abs(field_0x3798);
        }

        if (field_0x3026 != 0) {
            field_0x3026--;
        } else if (mProcID != PROC_ELEC_DAMAGE || !checkHookshotAnime()) {
            if (cLib_chasePos(&mHookshotTopPos, mHeldItemRootPos, return_speed)) {
                setHookshotReturnEnd();
            } else {
                cXyz sp1AC = mHookshotTopPos - mHeldItemRootPos;
                field_0x301c = sp1AC.atan2sY_XZ();
                field_0x301e = sp1AC.atan2sX_Z();
                seStartOnlyReverbLevel(Z2SE_LK_HS_WIND_UP);
            }
        }

        if (targetAc_p != NULL) {
            if (fopAcM_checkHookCarryNow(targetAc_p)) {
                targetAc_p->current.pos = mHookshotTopPos - mIronBallCenterPos;

                if (mItemMode == HS_MODE_NONE_e) {
                    cancelHookshotCarry();
                }
            } else {
                mHookTargetAcKeep.clearData();
            }
        }
    }

    if (checkHookshotWait() || mItemMode == 2) {
        csXyz* var_r28;
        if (mProcID == PROC_HOOKSHOT_WALL_SHOOT || mProcID == PROC_HOOKSHOT_ROOF_SHOOT) {
            var_r28 = (csXyz*)&mProcVar3.field_0x300e;
        } else {
            var_r28 = &mBodyAngle;
        }

        if (mProcID == PROC_HOOKSHOT_WALL_SHOOT) {
            field_0x301e = mProcVar4.field_0x3010;
        } else {
            field_0x301e = (s16)(shape_angle.y + mBodyAngle.y);
        }

        if (mTargetedActor != NULL && mItemMode == 2) {
            s16 var_r25 = getBodyAngleXAtnActor(0);
            if (cLib_distanceAngleS(var_r25, var_r28->x) < 0x3000) {
                cXyz sp1A0;
                getBodyAngleXBasePos(&sp1A0);
                mDoMtx_stack_c::transS(sp1A0);
                mDoMtx_stack_c::ZXYrotM(var_r25, field_0x301e, 0);
                mDoMtx_stack_c::XrotM(-var_r28->x);
                mDoMtx_stack_c::YrotM(-field_0x301e);
                mDoMtx_stack_c::transM(-sp1A0.x, -sp1A0.y, -sp1A0.z);
                mDoMtx_stack_c::multVec(&mHookshotTopPos, &mHookshotTopPos);
                var_r28->x = var_r25;
            }
        }

        field_0x301c = var_r28->x;
        mDoMtx_stack_c::ZXYrotS(field_0x301c, field_0x301e, 0);
        mDoMtx_stack_c::multVec(&cXyz::BaseZ, &mIronBallCenterPos);

        if (mItemMode == 2) {
            seStartOnlyReverb(Z2SE_LK_HS_SHOOT);
            if (mTargetedActor != NULL) {
                field_0x3028 = 1;
            } else {
                field_0x3028 = 0;
            }
        } else {
            field_0x3828 = mHeldItemRootPos;
        }

        mDoMtx_stack_c::copy(mHeldItemModel->getBaseTRMtx());

        mDoMtx_stack_c::transM(cXyz(hookRoot));
        if (mTargetedActor != NULL || mItemMode != 2) {
            mDoMtx_stack_c::multVecZero(&mHookshotTopPos);
        }

        if (mItemMode == 2) {
            mItemMode = HS_MODE_SHOOT_e;
        }
    } else {
        cXyz sp194(mHookshotTopPos);

        if (mItemMode != HS_MODE_RETURN_e && mItemMode == HS_MODE_SHOOT_e) {
            if (mAtCps[0].ChkAtHit() &&
                (mHookTargetAcKeep.getActor() != NULL || field_0x3494 > 0.0f))
            {
                if (mHookTargetAcKeep.getActor() != NULL) {
                    mHookshotTopPos = field_0x381c;

                    targetAc_p = mHookTargetAcKeep.getActor();
                    if (targetAc_p != NULL && (targetAc_p->actor_status & (fopAcStts_UNK_0x200000_e | fopAcStts_UNK_0x80000_e))) {
                        mHookTargetAcKeep.setData(targetAc_p);
                        mIronBallCenterPos = mHookshotTopPos - targetAc_p->current.pos;
                        setHookshotCatchNow();

                        if (targetAc_p->actor_status & fopAcStts_UNK_0x200000_e) {
                            mItemMode = 5;
                            field_0x316c.set(field_0x301c, field_0x301e, 0);
                            mDoMtx_stack_c::ZrotS(-targetAc_p->shape_angle.z);
                            mDoMtx_stack_c::XrotM(-targetAc_p->shape_angle.x);
                            mDoMtx_stack_c::YrotM(-targetAc_p->shape_angle.y);
                            mDoMtx_stack_c::multVecSR(&mIronBallCenterPos, &mIronBallCenterPos);
                        } else {
                            mItemMode = HS_MODE_RETURN_e;
                        }

                        fopAcM_setHookCarryNow(targetAc_p);
                    } else {
                        mHookTargetAcKeep.clearData();
                        mItemMode = HS_MODE_RETURN_e;
                        dComIfGp_getVibration().StartShock(VIBMODE_S_POWER1, 1, cXyz(0.0f, 1.0f, 0.0f));
                    }
                } else {
                    mItemMode = HS_MODE_RETURN_e;
                    dComIfGp_getVibration().StartShock(VIBMODE_S_POWER1, 1, cXyz(0.0f, 1.0f, 0.0f));
                }
            } else {
                if (checkChaseHookshot()) {
                    cXyz sp188 = mTargetedActor->eyePos - mHookshotTopPos;
                    if (sp188.inprod(mIronBallCenterPos) >= 0.0f) {
                        mIronBallCenterPos = sp188;
                        mIronBallCenterPos.normalizeZP();
                        field_0x301c = mIronBallCenterPos.atan2sY_XZ();
                        field_0x301e = mIronBallCenterPos.atan2sX_Z();
                    }
                }

                mHookshotTopPos += mIronBallCenterPos * shoot_speed;

                if (checkModeFlg(0x400)) {
                    mHookshotTopPos += current.pos - field_0x3798;
                }

                cXyz sp17C = mHookshotTopPos - mHeldItemRootPos;
                var_f29 = sp17C.abs();
                sp17C *= 1.0f / var_f29;

                f32 var_f26 = max_length - 15.0f;
                if (var_f29 >= var_f26) {
                    mHookshotTopPos = mHeldItemRootPos + (sp17C * var_f26);
                    mItemMode = HS_MODE_RETURN_e;
                }

                cXyz sp170;
                if (field_0x3828.abs2(mHeldItemRootPos) > 400.0f ||
                    current.pos.abs2(field_0x3798) > 1.0f || shape_angle.y != mPrevAngleY)
                {
                    field_0x3028 = 1;
                }

                if (field_0x3028 != 0) {
                    sp170 = mHeldItemRootPos;
                } else {
                    sp170 = field_0x3828;
                }

                sp170 -= mIronBallCenterPos * 100.0f;
                cXyz sp164 = mHookshotTopPos + (mIronBallCenterPos * 15.0f);
                mRopeLinChk.Set(&sp170, &sp164, this);

                if (dComIfG_Bgsp().LineCross(&mRopeLinChk)) {
                    u32 hit_se;

                    if (checkHookshotStickBG(mRopeLinChk)) {
                        setHookshotCatchNow();
                        mItemMode = HS_MODE_FLY_e;
                        hit_se = Z2SE_HIT_HOOKSHOT_STICK;

                        if (dComIfGp_checkPlayerStatus1(0, 0x2010000) != 0 &&
                            mCargoCarryAcKeep.getActor() == NULL)
                        {
                            mPolyInfo3.SetPolyInfo(mPolyInfo2);
                        } else {
                            mPolyInfo3.ClearPi();
                        }

                        mPolyInfo2.SetPolyInfo(mRopeLinChk);
                        if (dComIfG_Bgsp().ChkMoveBG_NoDABg(mRopeLinChk)) {
                            targetAc_p = dComIfG_Bgsp().GetActorPointer(mRopeLinChk);
                            mHookTargetAcKeep.setData(targetAc_p);
                            fopAcM_setHookCarryNow(targetAc_p);
                        }
                    } else {
                        int poly_att0 = dComIfG_Bgsp().GetPolyAtt0(mRopeLinChk);
                        dComIfGp_getVibration().StartShock(VIBMODE_S_POWER1, 1, cXyz(0.0f, 1.0f, 0.0f));
                        mItemMode = HS_MODE_RETURN_e;
                        hit_se = Z2SE_HIT_HOOKSHOT_REBOUND;

                        cM3dGPla tripla;
                        dComIfG_Bgsp().GetTriPla(mRopeLinChk, &tripla);

                        csXyz sp30;
                        if (poly_att0 == 0xD || poly_att0 == 3) {
                            u16 particle_id;
                            if (poly_att0 == 0xD) {
                                particle_id = dPa_RM(ID_ZI_S_DOWNSNOW_A);
                            } else {
                                particle_id = dPa_RM(ID_ZI_S_DOWNSAND_A);
                            }

                            sp30.set(cM_atan2s(tripla.mNormal.absXZ(), tripla.mNormal.y),
                                     tripla.mNormal.atan2sX_Z(), 0);

                            dComIfGp_particle_setPolyColor(
                                particle_id, mRopeLinChk, mRopeLinChk.GetCrossP(), &tevStr, &sp30,
                                (cXyz*)&l_hookSnowSandHitScale, 0, NULL, -1, NULL);
                            if (poly_att0 == 0xD) {
                                dComIfGp_particle_setPolyColor(
                                    dPa_RM(ID_ZI_S_DOWNSNOW_B), mRopeLinChk, mRopeLinChk.GetCrossP(), &tevStr, &sp30,
                                    (cXyz*)&l_hookSnowSandHitScale, 0, NULL, -1, NULL);
                            }
                        } else {
                            sp30.set(cM_atan2s(tripla.mNormal.y, tripla.mNormal.absXZ()),
                                     cM_atan2s(-tripla.mNormal.x, -tripla.mNormal.z), 0);
                            dComIfGp_setHitMark(9, NULL, mRopeLinChk.GetCrossP(), &sp30, NULL, 0);
                        }
                    }

                    mHookshotTopPos = mRopeLinChk.GetCross() - (mIronBallCenterPos * 15.0f);

                    cM3dGPla tripla;
                    dComIfG_Bgsp().GetTriPla(mRopeLinChk, &tripla);
                    field_0x316c.set(cM_atan2s(tripla.mNormal.y, tripla.mNormal.absXZ()),
                                     cM_atan2s(-tripla.mNormal.x, -tripla.mNormal.z), 0);

                    mZ2Link.startHitItemSE(hit_se, dKy_pol_sound_get(&mRopeLinChk), mpHookSound, -1.0f);
                } else {
                    seStartOnlyReverbLevel(Z2SE_LK_HS_CHAIN);
                }
            }
        }

        mDoMtx_stack_c::transS(mHookshotTopPos);
        mDoMtx_stack_c::ZXYrotM(field_0x301c, field_0x301e, 0);
        mpHookshotLinChk->Set(&sp194, &mHookshotTopPos, this);

        if (dComIfG_Bgsp().LineCross(mpHookshotLinChk) &&
            dComIfG_Bgsp().GetPolyAtt0(*mpHookshotLinChk) != 6)
        {
            fopKyM_createWpillar(mpHookshotLinChk->GetCrossP(), 0.5f, 0);
            mDoAud_seStart(Z2SE_CM_BODYFALL_WATER_S, mpHookshotLinChk->GetCrossP(), 0,
                           mVoiceReverbIntensity);
        }
    }

    mHookTipBck.entry(mpHookTipModel->getModelData(), field_0x33e0);
    mpHookTipModel->setBaseTRMtx(mDoMtx_stack_c::get());
    mpHookTipModel->calc();

    f32 bck_frame;
    if (dComIfGp_checkPlayerStatus1(0, 0x10000)) {
        mDoMtx_stack_c::transS(mIronBallBgChkPos);
        mDoMtx_stack_c::ZXYrotM(-0x4000, field_0x3022, 0);
        bck_frame = 14.0f;
    } else if (dComIfGp_checkPlayerStatus1(0, 0x2000000)) {
        mDoMtx_stack_c::transS(mIronBallBgChkPos);
        mDoMtx_stack_c::ZXYrotM(0, field_0x3022, 0);
        bck_frame = 14.0f;
    } else {
        if (field_0x3024 != 0) {
            cLib_chasePos(&mIronBallBgChkPos, field_0x3810, 2.0f * return_speed);
            cXyz sp158 = mIronBallBgChkPos - field_0x3810;

            if (sp158.abs2() < 1.0f) {
                field_0x3024 = 0;
            } else {
                mDoMtx_stack_c::transS(mIronBallBgChkPos);
                mDoMtx_stack_c::ZXYrotM(sp158.atan2sY_XZ(), sp158.atan2sX_Z(), 0);
            }
        }

        if (field_0x3024 == 0) {
            mDoMtx_stack_c::copy(field_0x0710->getBaseTRMtx());
            mDoMtx_stack_c::transM(cXyz(hookRoot));
            mIronBallBgChkPos = field_0x3810;
        }
        bck_frame = 0.0f;
    }

    mHookTipBck.entry(field_0x0714->getModelData(), bck_frame);
    field_0x0714->setBaseTRMtx(mDoMtx_stack_c::get());
    field_0x0714->calc();
}


int daAlink_c::procHookshotFly() {
    auto parameters = daAlinkHIO_hookshot_c0::m;
    parameters.mShootSpeed = 2870.0f;
    parameters.mMaxLength = 69420.0f;
    parameters.mReturnSpeed = 2870.0f;
    parameters.mStickReturnSpeed = 500.0f;
    fopAc_ac_c* targetAc_p = mHookTargetAcKeep.getActor();

    s16 targetAc_name;
    if (targetAc_p != NULL) {
        targetAc_name = fopAcM_GetName(targetAc_p);
    } else {
        targetAc_name = fpcNm_ALINK_e;
    }

    BOOL var_r29 = FALSE;
    if (targetAc_name == fpcNm_Obj_SwHang_e) {
        int swhang_type = static_cast<daObjSwHang_c*>(targetAc_p)->getType();
        if (swhang_type == 3 || swhang_type == 4) {
            var_r29 = TRUE;
        }
    }

    s16 temp_r24 = field_0x301e;
    cXyz spAC(mHookshotTopPos);
    setHookshotTopPosFly();

    field_0x37d4 = mHookshotTopPos - mHeldItemRootPos;
    if (mProcVar0.field_0x3008 != 0 && cLib_distanceAngleS(field_0x37d4.atan2sX_Z(), temp_r24) > 0x4000) {
        setHookshotReturnEnd();
    } else {
        mProcVar0.field_0x3008 = 0;
        f32 temp_f31 = field_0x37d4.abs();
        f32 temp_f30 = parameters.mStickReturnSpeed + spAC.abs(mHookshotTopPos);

        if (temp_f31 < temp_f30 || mProcVar1.field_0x300a == 0) {
            setHookshotReturnEnd();
        } else {
            field_0x37d4 *= temp_f30 / temp_f31;
            seStartOnlyReverbLevel(Z2SE_LK_HS_WIND_UP);
            if (temp_f31 < temp_f30 * 1.5f) {
                mProcVar1.field_0x300a--;
            }
        }
    }

    current.pos += field_0x37d4;
    if (checkSetItemTrigger(dItemNo_W_HOOKSHOT_e) != 0) {
        mProcVar5.field_0x3012 = 1;
    } else if (mProcVar5.field_0x3012 != 0 && !itemButton()) {
        mProcVar5.field_0x3012 = 0;
    }

    if (mItemMode != 5 && mItemMode != HS_MODE_FLY_e) {
        if (targetAc_name == fpcNm_B_OB_e ||
            (targetAc_name == fpcNm_B_DR_e && static_cast<daB_DR_c*>(targetAc_p)->isBack()))
        {
            dComIfGp_getVibration().StartShock(1, 1, cXyz(0.0f, 1.0f, 0.0f));
            return procBossBodyHangInit(targetAc_p);
        } else {
            cM3dGPla tripla;
            BOOL var_r28 = FALSE;
            BOOL var_r27 = FALSE;
            BOOL is_force_fall = checkStageName("D_MN10") && fopAcM_GetRoomNo(this) == 4;

            if (mProcVar2.field_0x300c == 4 && dComIfG_Bgsp().ChkPolySafe(mPolyInfo2)) {
                var_r28 = dComIfG_Bgsp().GetTriPla(mPolyInfo2, &tripla);
                var_r27 = cBgW_CheckBRoof(tripla.mNormal.y);
                if (!checkHookshotStickBG(mPolyInfo2)) {
                    var_r28 = FALSE;
                }
            }

            if (!is_force_fall && !mLinkAcch.ChkGroundHit() && !var_r27 && checkFrontWallTypeAction())
            {
                voiceStart(Z2SE_AL_V_CLIMB);
                dComIfGp_getVibration().StartShock(1, 1, cXyz(0.0f, 1.0f, 0.0f));
                return 1;
            } else {
                cXyz spA0 = current.pos - field_0x37c8;
                if (commonLineCheck(&field_0x37c8, &current.pos)) {
                    current.pos = mLinkLinChk.GetCross();

                    spA0.y = 0.0f;
                    spA0.normalizeZP();

                    current.pos.x -= spA0.x * 35.0f;
                    current.pos.z -= spA0.z * 35.0f;
                }

                if (var_r28 && !var_r27) {
                    cXyz sp94(mHookshotTopPos.x + tripla.mNormal.x * 35.0f, mHookshotTopPos.y + 5.0f,
                              mHookshotTopPos.z + tripla.mNormal.z * 35.0f);
                    mLinkGndChk.SetPos(&sp94);

                    f32 temp_f29 = dComIfG_Bgsp().GroundCross(&mLinkGndChk);
                    if (temp_f29 > mHookshotTopPos.y - 150.0f || is_force_fall) {
                        current.pos.x = sp94.x;
                        current.pos.z = sp94.z;
                        is_force_fall = true;
                    }
                }

                setJumpMode();
                if (mLinkAcch.ChkGroundHit()) {
                    checkNextAction(0);
                } else if (is_force_fall) {
                    procFallInit(1, 5.0f);
                    field_0x2f99 = 0x70;
                } else if (targetAc_name == fpcNm_E_PH_e || targetAc_name == fpcNm_B_DR_e || var_r29) {
                    procHookshotRoofWaitInit(1, targetAc_p, mProcVar5.field_0x3012);
                } else if (var_r28 && dComIfG_Bgsp().GetMonkeyBarsCode(mPolyInfo2)) {
                    cXyz sp88;
                    mDoMtx_stack_c::ZXYrotS(field_0x301c, field_0x301e, 0);
                    mDoMtx_stack_c::multVec(&cXyz::BaseZ, &sp88);
                    sp88 = mHookshotTopPos + (sp88 * 15.0f);

                    procRoofHangStartInit(mPolyInfo2, sp88, 0);
                } else if (var_r28 && var_r27) {
                    procHookshotRoofWaitInit(1, NULL, mProcVar5.field_0x3012);
                } else if (var_r28 && field_0x2f91 != 3 && fabsf(tripla.mNormal.y) < 0.05f) {
                    procHookshotWallWaitInit(1, tripla.mNormal.atan2sX_Z(), mProcVar5.field_0x3012);
                } else {
                    procFallInit(1, 5.0f);
                    field_0x2f99 = 0x70;
                }

                voiceStart(Z2SE_AL_V_CLIMB);
                if (mProcID != PROC_FALL) {
                    dComIfGp_getVibration().StartShock(1, 1, cXyz(0.0f, 1.0f, 0.0f));
                }
            }
        }
    } else {
        cXyz sp7C = mHookshotTopPos - current.pos;
        cLib_addCalcAngleS(&shape_angle.x, sp7C.atan2sY_XZ(), 2, 0x2000, 0x800);
        cLib_addCalcAngleS(&shape_angle.y, sp7C.atan2sX_Z(), 2, 0x2000, 0x800);
        current.angle.y = shape_angle.y;
        field_0x37c8 = field_0x3798;
    }

    return 1;
}


// Sample the same four anchors as the host's Alink draw routine. Keep the
// history in the mod: the host's AlinkInterp is private, not an SDK ABI.
namespace {
using InterpEnabled = bool(*)();
using InterpStep = float(*)();
using SimSequence = uint64_t(*)();
InterpEnabled chainInterpEnabled = nullptr;
InterpStep chainInterpStep = nullptr;
SimSequence chainSimSequence = nullptr;
gz::ChainHistory<cXyz> chainHistory;
cXyz chainAnchors[4];
fpc_ProcID chainOwner = ~fpc_ProcID(0);
void readChainAnchors(daAlink_c* link, cXyz* anchors) {
    anchors[0] = link->getHsChainTopPos();
    anchors[1] = link->getHsChainRootPos();
    anchors[2] = link->getHsSubChainRootPos();
    anchors[3] = link->getHsSubChainTopPos();
}
}

// Original chain geometry, spacing, twist, swing, lighting and fog; no 600-link
// cutoff. Iron Ball continues through the native renderer.
void daAlink_c::hsChainShape_c::draw() {
    daAlink_c* alink = (daAlink_c*)getUserArea();
    J3DModelData* modelData = alink->getItemModelData();
    J3DMaterial* material = modelData->getMaterialNodePointer(0);
    daAlink_hsChainLight_c* chainLight = (daAlink_hsChainLight_c*)&alink->tevStr;

    j3dSys.setVtxPos(modelData->getVtxPosArray(), modelData->getVtxNum());
    j3dSys.setVtxNrm(modelData->getVtxNrmArray(), modelData->getNrmNum());
    j3dSys.setVtxCol(modelData->getVtxColorArray(0), modelData->getColNum());
    j3dSys.setTexture(modelData->getTexture());
    J3DShape::resetVcdVatCache();

    material->loadSharedDL();
    material->getShape()->loadPreDrawSetting();

    GXColor ambColor;
    ambColor.r = chainLight->AmbCol.r;
    ambColor.g = chainLight->AmbCol.g;
    ambColor.b = chainLight->AmbCol.b;
    ambColor.a = chainLight->AmbCol.a;

    GXSetChanAmbColor(GX_COLOR0A0, ambColor);
    GXSetChanMatColor(GX_COLOR0A0, g_whiteColor);

    dKy_setLight_again();
    dKy_GxFog_tevstr_set(chainLight);
    GXLoadLightObjImm(chainLight->getLightObj(), GX_LIGHT0);


    cXyz anchors[4];
    readChainAnchors(alink, anchors);
    if (chainOwner == fopAcM_GetID(alink) && chainInterpEnabled()) {
        chainHistory.interpolate(anchors, chainInterpStep());
    }
        const cXyz& chainRootPos = anchors[1];
        const cXyz& chainTopPos = anchors[0];
        cXyz maxDistance = chainRootPos - chainTopPos;

        f32 maxDistanceF = maxDistance.abs();
        f32 var_f30;
        cXyz sp98;
        csXyz sp6C;

        if (std::isfinite(maxDistanceF) && maxDistanceF > 1.0f) {
            maxDistance *= (1.0f / maxDistanceF);
            var_f30 = 0.0f;

            sp98 = chainTopPos;
            sp6C.set(maxDistance.atan2sY_XZ(), maxDistance.atan2sX_Z(), 0);
            sp98 = chainTopPos;

            csXyz sp64(sp6C);

            f32 sp34 = M_PI / maxDistanceF;

            f32 temp_f27;
            f32 var_f26 = 0.0f;
            f32 var_f28;

            var_f28 = 2.5f * alink->getHookshotStopTime();
            if (alink->getHookshotStopTime() & 1) {
                var_f28 *= -1.0f;
            }
            (void)0;



            while (maxDistanceF > var_f30) {
                temp_f27 = var_f28 * cM_fsin(sp34 * var_f30);
                s16 spC = cM_atan2s(temp_f27 - var_f26, 5.0f);
                sp64.x = sp6C.x + spC;

                mDoMtx_stack_c::transS(sp98);
                mDoMtx_stack_c::ZXYrotM(sp64);

                static const Vec hsVec = {0.0f, 0.0f, 5.0f};
                mDoMtx_stack_c::multVec(&hsVec, &sp98);

                mDoMtx_stack_c::revConcat(j3dSys.getViewMtx());

                GXLoadPosMtxImm(mDoMtx_stack_c::get(), GX_PNMTX0);
                GXLoadNrmMtxImm(mDoMtx_stack_c::get(), GX_PNMTX0);

                material->getShape()->simpleDrawCache();

                ANGLE_ADD_2(sp64.z, 0x3000);

                var_f26 = temp_f27;
                var_f30 += fabsf(cM_scos(spC)) * 5.0f;


            }
        }

        const cXyz& subChainRootPos = anchors[2];
        const cXyz& subChainTopPos = anchors[3];
        maxDistance = subChainRootPos - subChainTopPos;

        maxDistanceF = maxDistance.abs();
        if (std::isfinite(maxDistanceF) && maxDistanceF > 1.0f) {
            maxDistance *= (1.0f / maxDistanceF);
            var_f30 = 0.0f;

            sp98 = subChainTopPos;
            sp6C.set(maxDistance.atan2sY_XZ(), maxDistance.atan2sX_Z(), 0);



            while (maxDistanceF > var_f30) {
                mDoMtx_stack_c::copy(j3dSys.getViewMtx());
                mDoMtx_stack_c::transM(sp98);
                mDoMtx_stack_c::ZXYrotM(sp6C);

                GXLoadPosMtxImm(mDoMtx_stack_c::get(), GX_PNMTX0);
                GXLoadNrmMtxImm(mDoMtx_stack_c::get(), GX_PNMTX0);

                material->getShape()->simpleDrawCache();

                sp98 += maxDistance * 5.0f;
                ANGLE_ADD_2(sp6C.z, 0x3000);
                var_f30 += 5.0f;

            }
        }

}


namespace gz {
DEFINE_HOOK_SYMBOL("daAlink_c::setHookshotSight",void(daAlink_c*),ClawSight);
DEFINE_HOOK_SYMBOL("daAlink_c::setHookshotPos",void(daAlink_c*),ClawPos);
DEFINE_HOOK_SYMBOL("daAlink_c::procHookshotFly",int(daAlink_c*),ClawFly);
DEFINE_HOOK(&daAlink_c::checkHookshotStickBG,ClawStick);
DEFINE_HOOK_SYMBOL("daAlink_c::hsChainShape_c::draw",void(daAlink_c::hsChainShape_c*),ClawChainDraw);
DEFINE_HOOK_SYMBOL("daAlink_c::draw",int(daAlink_c*),ClawAnchorCapture);
ModResult initClawshot(){
 void* address=nullptr;HookSymbolFlags flags{};
 auto resolved=svc_hook->resolve(mod_ctx,"dusk::interp::is_enabled",&address,&flags);
 if(resolved!=MOD_OK||!address||!(flags&HOOK_SYMBOL_CODE))return MOD_UNAVAILABLE;
 chainInterpEnabled=reinterpret_cast<InterpEnabled>(address);
 resolved=svc_hook->resolve(mod_ctx,"dusk::interp::get_interpolation_step",&address,&flags);
 if(resolved!=MOD_OK||!address||!(flags&HOOK_SYMBOL_CODE))return MOD_UNAVAILABLE;
 chainInterpStep=reinterpret_cast<InterpStep>(address);
 resolved=svc_hook->resolve(mod_ctx,"dusk::interp::sim_tick_seq",&address,&flags);
 if(resolved!=MOD_OK||!address||!(flags&HOOK_SYMBOL_CODE))return MOD_UNAVAILABLE;
 chainSimSequence=reinterpret_cast<SimSequence>(address);
 // This observer also clears history while suspended; it never changes game state.
 resolved=mods::hook::add_post<ClawAnchorCapture>([](ModContext*,void* a,void*,void*){
  auto* link=mods::arg<daAlink_c*>(a,0);
  if(speedrunBlocked()||!on("super_clawshot")||!link->checkHookshotItem(link->mEquipItem)||!chainInterpEnabled()){
   chainHistory.reset();chainOwner=~fpc_ProcID(0);return;
  }
  const auto owner=fopAcM_GetID(link);
  if(owner!=chainOwner){chainHistory.reset();chainOwner=owner;}
  readChainAnchors(link,chainAnchors);
  chainHistory.capture(chainAnchors,chainSimSequence());
 });if(resolved!=MOD_OK)return resolved;
 resolved=guardedPre<ClawChainDraw>([](ModContext*,void* a,void*,void*){
  if(!on("super_clawshot"))return HOOK_CONTINUE;
  auto* packet=mods::arg<daAlink_c::hsChainShape_c*>(a,0);
  auto* link=reinterpret_cast<daAlink_c*>(packet->getUserArea());
  if(!link||!link->checkHookshotItem(link->mEquipItem))return HOOK_CONTINUE;
  packet->daAlink_c::hsChainShape_c::draw();return HOOK_SKIP_ORIGINAL;
 });if(resolved!=MOD_OK)return resolved;
 auto r=guardedPre<ClawSight>([](ModContext*,void* a,void*,void*){
  if(!on("super_clawshot"))return HOOK_CONTINUE;
  mods::arg<daAlink_c*>(a,0)->setHookshotSight();return HOOK_SKIP_ORIGINAL;
 });if(r!=MOD_OK)return r;
 r=guardedPre<ClawPos>([](ModContext*,void* a,void*,void*){
  if(!on("super_clawshot"))return HOOK_CONTINUE;
  mods::arg<daAlink_c*>(a,0)->setHookshotPos();return HOOK_SKIP_ORIGINAL;
 });if(r!=MOD_OK)return r;
 r=guardedPre<ClawFly>([](ModContext*,void* a,void* ret,void*){
  if(!on("super_clawshot"))return HOOK_CONTINUE;
  *static_cast<int*>(ret)=mods::arg<daAlink_c*>(a,0)->procHookshotFly();return HOOK_SKIP_ORIGINAL;
 });if(r!=MOD_OK)return r;
 return guardedPre<ClawStick>([](ModContext*,void*,void* ret,void*){
  if(!on("super_clawshot"))return HOOK_CONTINUE;
  *static_cast<BOOL*>(ret)=TRUE;return HOOK_SKIP_ORIGINAL;
 });
}
}
