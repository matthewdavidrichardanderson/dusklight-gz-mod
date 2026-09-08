// TPGZ trigger viewer (GPL-3.0), native actor types and loaded stage paths.
#include "core.hpp"
#include "d/d_com_inf_game.h"
#include "d/d_debug_viewer.h"
#include "d/d_path.h"
#include "d/d_attention.h"
#include "d/actor/d_a_alink.h"
#include "f_op/f_op_actor_iter.h"
#include "f_pc/f_pc_name.h"
#include "d/actor/d_a_scene_exit.h"
#include "d/actor/d_a_no_chg_room.h"
#include "d/actor/d_a_tag_mstop.h"
#include "d/actor/d_a_swc00.h"
#include "d/actor/d_a_tag_evtarea.h"
#include "d/actor/d_a_tag_chgrestart.h"
#include "d/actor/d_a_kytag08.h"
#include "d/actor/d_a_e_rb.h"
#include "d/actor/d_a_e_s1.h"
#include "d/actor/d_a_npc_myna2.h"
#include <cmath>
namespace gz {u8 geometryOpacity();void setGeometryOpacity(int);}

namespace gz { namespace TriggerViewer {

typedef void (*drawCallback)(fopAc_ac_c*);
void searchActorForCallback(s16 actorName, drawCallback callback) {
    struct Request{s16 name;drawCallback fn;} request{actorName,callback};
    fopAcIt_Executor([](void* raw,void* data){
        auto* actor=static_cast<fopAc_ac_c*>(raw);
        const auto& r=*static_cast<Request*>(data);
        if(r.fn&&(r.name==-1||fopAcM_GetName(actor)==r.name))r.fn(actor);
        return 1;
    },&request);
}

void drawSceneExit(fopAc_ac_c* actor) {
    daScex_c* scex = (daScex_c*)actor;

    cXyz points[8];
    points[0].set(-actor->scale.x, actor->scale.y, -actor->scale.z);
    points[1].set(actor->scale.x, actor->scale.y, -actor->scale.z);
    points[2].set(-actor->scale.x, actor->scale.y, actor->scale.z);
    points[3].set(actor->scale.x, actor->scale.y, actor->scale.z);
    points[4].set(-actor->scale.x, 0.0f, -actor->scale.z);
    points[5].set(actor->scale.x, 0.0f, -actor->scale.z);
    points[6].set(-actor->scale.x, 0.0f, actor->scale.z);
    points[7].set(actor->scale.x, 0.0f, actor->scale.z);

    mDoMtx_inverse(scex->mMatrix, mDoMtx_stack_c::get());
    mDoMtx_multVecArray(mDoMtx_stack_c::get(), points, points, 8);

    GXColor color = {0xFF, 0x00, 0xFF, geometryOpacity()};
    dDbVw_drawCube8pXlu(points, color);
}

void drawNoChangeRoomTrigger(fopAc_ac_c* actor) {

    daNocrm_c* nocrm = (daNocrm_c*)actor;

    cXyz points[8];
    points[0].set(-actor->scale.x, actor->scale.y, -actor->scale.z);
    points[1].set(actor->scale.x, actor->scale.y, -actor->scale.z);
    points[2].set(-actor->scale.x, actor->scale.y, actor->scale.z);
    points[3].set(actor->scale.x, actor->scale.y, actor->scale.z);
    points[4].set(-actor->scale.x, 0.0f, -actor->scale.z);
    points[5].set(actor->scale.x, 0.0f, -actor->scale.z);
    points[6].set(-actor->scale.x, 0.0f, actor->scale.z);
    points[7].set(actor->scale.x, 0.0f, actor->scale.z);

    mDoMtx_inverse(nocrm->mInvMtx, mDoMtx_stack_c::get());
    mDoMtx_multVecArray(mDoMtx_stack_c::get(), points, points, 8);

    GXColor color = {0x00, 0xFF, 0xFF, geometryOpacity()};  // Different color to distinguish from scene exits
    dDbVw_drawCube8pXlu(points, color);
}

// this one could probably be made more accurate
void drawMidnaStop(fopAc_ac_c* actor) {
    daTagMstop_c* mstop = (daTagMstop_c*)actor;

    GXColor color = {0x4A, 0x36, 0xBA, geometryOpacity()};
    dDbVw_drawCylinderXlu(mstop->current.pos, std::sqrt(mstop->field_0x5c0), mstop->field_0x5c4-mstop->current.pos.y, color, 1);
}

// used a large cylinder height to represent the xz check as accurately as possible
void drawPlumTag(fopAc_ac_c* actor) {
    dDbVw_drawCylinderXlu(actor->current.pos,actor->scale.x*100.f,1000000.f,{0,255,0,geometryOpacity()},1);
}
void drawPlummSearch(fopAc_ac_c* actor) {
    dDbVw_drawCircleXlu(actor->attention_info.position,daNpc_myna2_Param_c::m.common.search_distance+160.f,{255,0,0,geometryOpacity()},1,12);
}
void drawSwitchArea(fopAc_ac_c* actor) {
    daSwc00_c* swc = (daSwc00_c*)actor;
    int shape_type = (fopAcM_GetParam(actor) >> 0x12) & 3;

    GXColor color = {0x00, 0x00, 0xFF, geometryOpacity()};
    if (shape_type == 3) {
        dDbVw_drawCylinderXlu(swc->current.pos, std::sqrt(swc->scale.x) - 30.0f, swc->scale.y,
                              color, 1);
    } else if (shape_type == 0) {
        cXyz size = swc->field_0x574 - swc->field_0x568;  // diameter
        size *= 0.5f;                         // radius

        cXyz pos = swc->field_0x568 + size;  // base + radius = center point
        csXyz angle(swc->current.angle.x, swc->current.angle.y, swc->current.angle.z);
        dDbVw_drawCubeXlu(pos, size, angle, color);
    }
}

void drawEventArea(fopAc_ac_c* actor) {

    u8 type = (actor->shape_angle.z & 0xFF);
    if (type == 0xFF) {
        type = 0;
    }

    if (type == 15 || type == 16) {
        GXColor color = {0xFF, 0xFF, 0x00, geometryOpacity()};
        cXyz points[8];
        points[0].set(-actor->scale.x, actor->scale.y, -actor->scale.z);
        points[1].set(actor->scale.x, actor->scale.y, -actor->scale.z);
        points[2].set(-actor->scale.x, actor->scale.y, actor->scale.z);
        points[3].set(actor->scale.x, actor->scale.y, actor->scale.z);
        points[4].set(-actor->scale.x, 0.0f, -actor->scale.z);
        points[5].set(actor->scale.x, 0.0f, -actor->scale.z);
        points[6].set(-actor->scale.x, 0.0f, actor->scale.z);
        points[7].set(actor->scale.x, 0.0f, actor->scale.z);

        mDoMtx_stack_c::transS(actor->home.pos.x, actor->home.pos.y, actor->home.pos.z);
        mDoMtx_stack_c::YrotM(actor->current.angle.y);
        mDoMtx_multVecArray(mDoMtx_stack_c::get(), points, points, 8);

        dDbVw_drawCube8pXlu(points, color);
    } else {
        // Native chkPointInArea uses a rotated ellipse, not GZ's guessed inner radius.
        // Type 21 ignores height; its horizontal footprint is shown at Link's height.
        const float height=type==21?1.f:actor->scale.y;
        const float baseY=type==21?daAlink_getAlinkActorClass()->current.pos.y:actor->current.pos.y-10.f;
        if(actor->scale.x==0||actor->scale.z==0||height<=0)return;
        mDoMtx_stack_c::transS(actor->current.pos.x,baseY+height*.5f,actor->current.pos.z);
        mDoMtx_stack_c::YrotM(actor->shape_angle.y);
        mDoMtx_stack_c::scaleM(std::fabs(actor->scale.x),height*.5f,std::fabs(actor->scale.z));
        mDoMtx_stack_c::XrotM(0x4000);
        dDbVw_drawCylinderMXlu(mDoMtx_stack_c::get(),{255,0,0,geometryOpacity()},1);
    }
}

void drawEventTag(fopAc_ac_c* actor) {
    GXColor color = {0x00, 0xC8, 0xFF, geometryOpacity()};
    u16 area_type = actor->home.angle.x & 0x8000;

    if (area_type == 0x8000) {
        cXyz points[8];

        cXyz start(actor->current.pos.x - (actor->scale.x * 0.5f), actor->current.pos.y,
                   actor->current.pos.z - (actor->scale.z * 0.5f));

        cXyz end(actor->current.pos.x + (actor->scale.x * 0.5f),
                 actor->current.pos.y + actor->scale.y,
                 actor->current.pos.z + (actor->scale.z * 0.5f));

        points[0].set(start.x, start.y, start.z);
        points[1].set(start.x, start.y, end.z);
        points[2].set(end.x, start.y, end.z);
        points[3].set(end.x, start.y, start.z);
        points[4].set(start.x, end.y, start.z);
        points[5].set(start.x, end.y, end.z);
        points[6].set(end.x, end.y, end.z);
        points[7].set(end.x, end.y, start.z);

        dDbVw_drawCube8pXlu(points, color);
    } else {
        cXyz pos = actor->current.pos;
        pos.y -= actor->scale.y;

        dDbVw_drawCylinderXlu(pos, actor->scale.x, actor->scale.y * 2, color, 1);
    }
}

void drawTWGate(fopAc_ac_c* actor) {
    GXColor color = {0xFF, 0xFF, 0xFF, geometryOpacity()};
    dDbVw_drawCylinderXlu(actor->current.pos, actor->scale.x * 100.0f, actor->scale.y * 100.0f,
                          color, 1);
}

static uint8_t pathColorIndex = 0;

void drawPaths(dStage_dPath_c* paths) {
    static const GXColor colors[8] = {
        {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00}, {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00},
        {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0x00}, {0xFF, 0x00, 0xFF}, {0x00, 0xFF, 0xFF},
    };

    cXyz cubeSize = {30.0f, 30.0f, 30.0f};
    csXyz cubeAngle = {0, 0, 0};

    for (int i = 0; i < paths->num; i++) {
        dPath* path = &paths->m_path[i];
        GXColor color = colors[(pathColorIndex++) & 7];
        color.a = geometryOpacity();
        if(!path->m_points||path->m_num==0)continue;
        cXyz a, b=static_cast<Vec>(path->m_points[0].m_position);

        // Draw a line back to the beginning if the path loops
        if (dPath_ChkClose(path) && path->m_num > 2) {
            a = path->m_points[0].m_position;
            b = path->m_points[path->m_num - 1].m_position;
            dDbVw_drawLineXlu(a, b, color, 1, 10);
        }

        // Iterate over all of the points of the path
        for (int j = 0; j < path->m_num - 1; j++) {
            a = path->m_points[j].m_position;
            b = path->m_points[j + 1].m_position;
            dDbVw_drawLineXlu(a, b, color, 1, 10);  // Param 3 is if z is enabled or not
            dDbVw_drawCubeXlu(a, cubeSize, cubeAngle, color);
        }
        dDbVw_drawCubeXlu(b, cubeSize, cubeAngle, color);  // Draw the cube for the end of the path
    }
}

void drawStagePaths() {
    dStage_dPath_c* stagePaths = g_dComIfG_gameInfo.play.mStageData.mPath2Info;
    if (stagePaths) {
        drawPaths(stagePaths);
    }
}

void drawCurrentRoomPaths() {
    daAlink_c* player = daAlink_getAlinkActorClass();
    if (player == NULL) {
        return;
    }

    s32 roomNo = fopAcM_GetRoomNo(player);
    if (roomNo < 0 || roomNo >= 64) {
        return;
    }

    dStage_dPath_c* roomPaths = dComIfGp_roomControl_getStatusRoomDt(roomNo)->getPath2Inf();
    if (roomPaths) {
        drawPaths(roomPaths);
    }
}

void drawCheckpointTag(fopAc_ac_c* actor) {
    daTagChgRestart_c* chk = (daTagChgRestart_c*)actor;
    
    GXColor color = {0x29, 0xF0, 0xFF, geometryOpacity()};
    cXyz points[8];

    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->current.angle.y);

    points[0] = chk->mVertices[0];
    points[1] = chk->mVertices[1];
    points[2] = chk->mVertices[3];
    points[3] = chk->mVertices[2];
    points[4] = chk->mVertices[0];
    points[5] = chk->mVertices[1];
    points[6] = chk->mVertices[3];
    points[7] = chk->mVertices[2];
    
    mDoMtx_multVecArray(mDoMtx_stack_c::get(), points, points, 8);
    // actor only checks XZ axis', so copy player y to keep it more accurate
    for (int i = 0; i < 8; i++) {
        points[i].y = daAlink_getAlinkActorClass()->current.pos.y;
    }

    for (int i = 0; i < 4; i++) {
        points[i].y += 1000.0f;
    }

    dDbVw_drawCube8pXlu(points, color);
}

void drawTransformDists(fopAc_ac_c* actor) {
    if (fopAcM_GetGroup(actor) == 4 && !fopAcM_CheckStatus(actor, 0x8000000)) {
        GXColor near_color = {0x00, 0xFF, 0x00, geometryOpacity()};
        GXColor far_color = {0xFF, 0x00, 0x00, geometryOpacity()};

        const f32 near_dist = 400.0f;
        const f32 far_dist = 5000.0f;
        
        dDbVw_drawCircleXlu(actor->eyePos, near_dist, near_color, 1, 20);
        dDbVw_drawCircleXlu(actor->eyePos, far_dist, far_color, 1, 20);

        const s16 view_range = 0x4000;

        cXyz offset(0.0f, 0.0f, far_dist);  // set line dist
        cXyz endpos;

        // draw one view range edge
        mDoMtx_stack_c::transS(actor->eyePos.x, actor->eyePos.y, actor->eyePos.z);
        mDoMtx_stack_c::YrotM(actor->shape_angle.y);
        mDoMtx_stack_c::YrotM(-view_range);
        mDoMtx_stack_c::multVec(&offset, &endpos);
        dDbVw_drawLineXlu(actor->eyePos, endpos, far_color, 1, 10);

        // draw other view range edge
        mDoMtx_stack_c::transS(actor->eyePos.x, actor->eyePos.y, actor->eyePos.z);
        mDoMtx_stack_c::YrotM(actor->shape_angle.y);
        mDoMtx_stack_c::YrotM(view_range);
        mDoMtx_stack_c::multVec(&offset, &endpos);
        dDbVw_drawLineXlu(actor->eyePos, endpos, far_color, 1, 10);

        // draw facing direction
        mDoMtx_stack_c::transS(actor->eyePos.x, actor->eyePos.y, actor->eyePos.z);
        mDoMtx_stack_c::YrotM(actor->shape_angle.y);
        mDoMtx_stack_c::multVec(&offset, &endpos);
        dDbVw_drawLineXlu(actor->eyePos, endpos, far_color, 1, 10);
    }
}

void drawAttentionDists(fopAc_ac_c* actor) {
    GXColor lock_color = {0x00, 0x00, 0xFF, geometryOpacity()};
    GXColor talk_color = {0x00, 0xFF, 0x00, geometryOpacity()};

    dist_entry* lock_inf = &dAttention_c::getDistTable(actor->attention_info.distances[fopAc_attn_LOCK_e]);
    dist_entry* talk_inf = &dAttention_c::getDistTable(actor->attention_info.distances[fopAc_attn_TALK_e]);
    cXyz& pos = actor->attention_info.position;

    if (fopAcM_GetGroup(actor) == 4) {
        dDbVw_drawCircleXlu(pos, lock_inf->mDistMax, lock_color, 1, 20);
        dDbVw_drawCircleXlu(pos, talk_inf->mDistMax, talk_color, 1, 20);
    }
}

void drawPurpleMistAvoid(fopAc_ac_c* actor) {
    kytag08_class* tag = (kytag08_class*)actor;

    GXColor avoidColor = {0x00, 0xFF, 0x00, geometryOpacity()};
    GXColor targetColor = {0xFF, 0x00, 0xFF, geometryOpacity()};

    dDbVw_drawCircleXlu(tag->mAvoidPos, tag->mSize.x * 45.0f * tag->mSizeScale, avoidColor, 1, 20);

    cXyz cubeSize(10.0f, 10.0f, 10.0f);
    csXyz cubeAngle(0, 0, 0);

    dDbVw_drawCubeXlu(tag->mAvoidPos, cubeSize, cubeAngle, avoidColor);
    dDbVw_drawCubeXlu(tag->mTargetAvoidPos, cubeSize, cubeAngle, targetColor);
}

void drawLeeverData(fopAc_ac_c* actor) {
    e_rb_class* leever = (e_rb_class*)actor;

    if (!leever->isChild) {
        GXColor color = {0xFF, 0x00, 0x00, geometryOpacity()};
        GXColor color2 = {0x00, 0x00, 0xFF, geometryOpacity()};

        cXyz pos(actor->current.pos);
        if (pos.y < daAlink_getAlinkActorClass()->mLinkAcch.GetGroundH()) {
            pos.y = daAlink_getAlinkActorClass()->mLinkAcch.GetGroundH() + 100.0f;
        }

        dDbVw_drawCircleXlu(pos, leever->appearRange * 100.0f, color, 1, 20);
        dDbVw_drawCircleXlu(pos, leever->field_0xa69 * 100.0f, color2, 1, 20);
    }
}

void drawShadowbeastDetect(fopAc_ac_c* actor) {

    e_s1_class* s1 = (e_s1_class*)actor;
    
    f32 posy = s1->current.pos.y + 30.0f;
    
    // home position circle
    cXyz home = {actor->home.pos.x, posy, s1->home.pos.z};
    GXColor circleColor = {0x00, 0x00, 0xFF, geometryOpacity()};
    dDbVw_drawCircleXlu(home, s1->mSearchRange, circleColor, 1, 20);

    // fov lines
    GXColor lineColor = {0xFF, 0xFF, 0x00, geometryOpacity()};
    cXyz center = actor->current.pos;
    center.y = posy;
    f32 lineDistance = s1->mSearchRange;
    cXyz offset(0.0f, 0.0f, lineDistance);

    // edge 1
    mDoMtx_stack_c::transS(center.x, center.y, center.z);
    mDoMtx_stack_c::YrotM((s16)(s1->shape_angle.y + 0x7000));
    cXyz endpos1;
    mDoMtx_stack_c::multVec(&offset, &endpos1);
    dDbVw_drawLineXlu(center, endpos1, lineColor, 1, 10);

    // edge 2
    mDoMtx_stack_c::transS(center.x, center.y, center.z);
    mDoMtx_stack_c::YrotM((s16)(s1->shape_angle.y - 0x7000));
    cXyz endpos2;
    mDoMtx_stack_c::multVec(&offset, &endpos2);
    dDbVw_drawLineXlu(center, endpos2, lineColor, 1, 10);
}

void execute() {
    if(!playable()||!daAlink_getAlinkActorClass())return;
    if (on("trigger_loads")) {
        searchActorForCallback(fpcNm_SCENE_EXIT_e, drawSceneExit);
        searchActorForCallback(fpcNm_NO_CHG_ROOM_e, drawNoChangeRoomTrigger);
    }

    if (on("trigger_midna")) {
        searchActorForCallback(fpcNm_Tag_Mstop_e, drawMidnaStop);
    }

    if (on("trigger_switches")) {
        searchActorForCallback(fpcNm_SWC00_e, drawSwitchArea);
    }

    if (on("trigger_events")) {
        searchActorForCallback(fpcNm_TAG_EVENT_e, drawEventTag);
        searchActorForCallback(fpcNm_TAG_EVTAREA_e, drawEventArea);
        searchActorForCallback(fpcNm_TAG_MYNA2_e, drawPlumTag);
        searchActorForCallback(fpcNm_MYNA2_e, drawPlummSearch);
    }

    if (on("trigger_twilight")) {
        searchActorForCallback(fpcNm_Tag_TWGate_e, drawTWGate);
    }

    if (on("trigger_paths")) {
        pathColorIndex = 0;
        drawStagePaths();
        drawCurrentRoomPaths();
    }

    if (on("trigger_restarts")) {
        searchActorForCallback(fpcNm_Tag_ChgRestart_e, drawCheckpointTag);
    }

    if (on("trigger_transform")) {
        searchActorForCallback(-1, drawTransformDists);
    }

    if (on("trigger_attention")) {
        searchActorForCallback(-1, drawAttentionDists);
        searchActorForCallback(fpcNm_E_S1_e, drawShadowbeastDetect);
    }

    if (on("trigger_mist")) {
        searchActorForCallback(fpcNm_KYTAG08_e, drawPurpleMistAvoid);
    }

    if (on("trigger_leevers")) {
        searchActorForCallback(fpcNm_E_RB_e, drawLeeverData);
    }
}
}  // namespace TriggerViewer
DEFINE_HOOK_SYMBOL("dDbVw_deleteDrawPacketList",void(),TriggerFrame);
ModResult initTriggers(){
 toggle("trigger_attention","Triggers","attention distances","Native attention distances geometry.");
 toggle("trigger_events","Triggers","event areas","Native event areas geometry.");
 toggle("trigger_loads","Triggers","load zones","Native load zones geometry.");
 toggle("trigger_midna","Triggers","midna stops","Native midna stops geometry.");
 toggle("trigger_paths","Triggers","paths","Native paths geometry.");
 toggle("trigger_mist","Triggers","purple mist avoid","Native purple mist avoid geometry.");
 toggle("trigger_restarts","Triggers","restart changes","Native restart changes geometry.");
 toggle("trigger_switches","Triggers","switch areas","Native switch areas geometry.");
 toggle("trigger_transform","Triggers","transform distances","Native transform distances geometry.");
 toggle("trigger_twilight","Triggers","twilight gates","Native twilight gates geometry.");
 toggle("trigger_leevers","Triggers","leever ranges","Native leever ranges geometry.");
 number("trigger_opacity","Triggers","opacity:",0,255,[](){return geometryOpacity();},[](int64_t v){setGeometryOpacity(int(v));});
 return guardedPost<TriggerFrame>([](ModContext*,void*,void*,void*){TriggerViewer::execute();});
}
}
