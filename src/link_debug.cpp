// TPGZ utils/link.cpp display fields, order, precision and advanced mode.
#include "core.hpp"
#include "gz_font.hpp"
#include "presentation.hpp"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_alink.h"
#include "Z2AudioLib/Z2StatusMgr.h"
#include <cstdio>
#include <cmath>
#include "link_tools.hpp"
namespace gz {
void drawLinkDebug(){
 const auto pos=spritePosition(LinkDebug);
 char line[128];float y=pos.y;
 auto draw=[&](const char* format,auto... args){std::snprintf(line,sizeof(line),format,args...);drawGzText(line,pos.x,y);y+=20;};
 const auto* audio=Z2GetStatusMgr();
 if(audio)draw("time: %02d:%02d",int(audio->mHour),int(audio->mMinute));else draw("time: n/a");
 auto* p=daAlink_getAlinkActorClass();
 if(p){
  draw("angle: %d",int(u16(p->shape_angle.y)));
  // GZ's mLookAngleY was at 0x59c: the decomp identifies it as mBodyAngle.x.
  draw("y-angle: %d",int(p->mBodyAngle.x));
  draw("speed: %.4f",p->speedF);
  draw("x-pos: %.4f",p->current.pos.x);draw("y-pos: %.4f",p->current.pos.y);draw("z-pos: %.4f",p->current.pos.z);
  if(on("advanced_mode")){
   draw("action: %d",int(p->mProcID));
   const int slope=p->mLinkAcch.ChkGroundHit()?p->getGroundAngle(&p->mLinkAcch.m_gnd,p->current.angle.y):0;
   draw("slope: %d",slope);draw("acch: %08X",unsigned(p->mLinkAcch.m_flags));draw("demo: %d",int(p->mDemo.getDemoMode()));
  }
 }else{
  for(const auto* field:{"angle","y-angle","speed","x-pos","y-pos","z-pos"})draw("%s: n/a",field);
  if(on("advanced_mode"))for(const auto* field:{"action","slope","acch","demo"})draw("%s: n/a",field);
 }
}
void drawDisplacement(){
 auto* player=daAlink_getAlinkActorClass();if(!player)return;
 cXyz saved;s16 angle;storedLinkPose(saved,angle);
 const auto pos=spritePosition(Displacement);float y=pos.y;char line[128];
 auto draw=[&](const char* format,auto value){std::snprintf(line,sizeof(line),format,value);drawGzText(line,pos.x,y);y+=20;};
 draw("da: %d",int(player->shape_angle.y)-int(angle));
 draw("dx: %.4f",player->current.pos.x-saved.x);
 draw("dy: %.4f",player->current.pos.y-saved.y);
 draw("dz: %.4f",player->current.pos.z-saved.z);
 const double dx=double(player->current.pos.x)-double(saved.x);
 const double dy=double(player->current.pos.y)-double(saved.y);
 const double dz=double(player->current.pos.z)-double(saved.z);
 draw("dxz: %.4f",std::sqrt(dx*dx+dz*dz));
 draw("dxyz: %.4f",std::sqrt(dx*dx+dy*dy+dz*dz));
}
void drawStageInfo(){
 const auto p=spritePosition(StageInfo);char line[128];
 auto draw=[&](float x,float y,const char* format,auto... args){std::snprintf(line,sizeof(line),format,args...);drawGzText(line,x,y);};
 draw(p.x,p.y+20,"Stage: %s",dComIfGp_getStartStageName());
 draw(p.x,p.y+40,"Room: %d",int(dStage_roomControl_c::getStayNo()));
 draw(p.x,p.y+60,"Point: %d",int(dComIfGp_getStartStagePoint()));
 draw(p.x,p.y+80,"Layer: %d",int(dComIfG_play_c::getLayerNo(0)));
 auto& save=g_dComIfG_gameInfo.info.getPlayer().getPlayerReturnPlace();
 draw(p.x+150,p.y+20,"Save Stage: %s",save.getName());
 draw(p.x+150,p.y+40,"Save Room: %d",int(save.getRoomNo()));
 draw(p.x+150,p.y+60,"Save Point: %d",int(save.getPlayerStatus()));
}
}
