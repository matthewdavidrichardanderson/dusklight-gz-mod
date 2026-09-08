// GZ actor spawner/inspector with native process IDs.
#define PROCS_DUMP_NAMES 1
#include "f_pc/f_pc_name.h"
#include "core.hpp"
#include "gz_font.hpp"
#include "presentation.hpp"
#include "menu_logic.hpp"
#include "native_pose.hpp"
#include "d/d_debug_viewer.h"
#include "f_op/f_op_actor_iter.h"
#include "mods/svc/actor.h"
#include <algorithm>
#include <cstdio>
#include <string_view>
#include <vector>
IMPORT_SERVICE(ActorService,svc_actor);
namespace gz {
std::string_view gzCurrentPage();
namespace {
fpc_ProcID selected=~fpc_ProcID(0);
int spawnProfile=0,spawnSubtype=-1,spawnRow=0,actorRow=0,paramDigit=0;
u32 spawnParams=0xffffffff;
bool editingParams=false;
MenuButtons actorEditRepeat;
std::string profileName(int id){
 const auto* name=GetProcName(id);
 if(!name)return "mod actor";
 std::string text(name);if(text.starts_with("fpcNm_"))text.erase(0,6);
 if(text.ends_with("_e"))text.resize(text.size()-2);
 return text;
}
std::vector<fpc_ProcID> actorIds(){
 std::vector<fpc_ProcID> ids;
 fopAcIt_Executor([](void* raw,void* out){
  auto* actor=static_cast<fopAc_ac_c*>(raw);
  static_cast<std::vector<fpc_ProcID>*>(out)->push_back(fopAcM_GetID(actor));return 1;
 },&ids);
 return ids;
}
fopAc_ac_c* selectedActor(){
 if(!playable())return nullptr;
 if(auto* actor=fopAcM_SearchByID(selected))return actor;
 const auto ids=actorIds();
 if(ids.empty()){selected=~fpc_ProcID(0);return nullptr;}
 selected=ids.front();return fopAcM_SearchByID(selected);
}
void spawn(){
 auto* link=daAlink_getAlinkActorClass();if(!playable()||!link)return;
 ActorSpawnParams params{};params.parameters=spawnParams;params.argument=s8(spawnSubtype);
 params.room_num=link->current.roomNo;
 params.position={link->current.pos.x,link->current.pos.y,link->current.pos.z};
 params.angle={link->current.angle.x,link->current.angle.y,link->current.angle.z};
 params.scale={1,1,1};
 ActorId id=0;const auto result=svc_actor->create_actor(mod_ctx,s16(spawnProfile),&params,&id);
 if(result==MOD_OK){selected=id;notify("Spawn requested: "+profileName(spawnProfile));}
 else notify("Actor spawn failed: "+profileName(spawnProfile)+" ("+std::to_string(result)+")");
}
}
void actorMenuUnloaded(std::string_view page,bool deleted){
 // Original spawner's edit mode belongs to the recreated menu, while its
 // parameters/cursor are permanent. Inspector actor index lasts until Back.
 if(page=="actor spawner")editingParams=false;
 if(page=="actor list"){actorEditRepeat={};if(deleted)selected=~fpc_ProcID(0);}
}
bool actorMenuInput(std::string_view page,uint16_t buttons,uint16_t command,uint16_t edge){
 if(page!="actor spawner"&&page!="actor list")return false;
 if(page=="actor spawner"){
  if(editingParams){
   if(edge&0x200){editingParams=false;return true;}
   if(command&1)paramDigit=(paramDigit+7)%8;
   if(command&2)paramDigit=(paramDigit+1)%8;
   if(command&8)spawnParams+=u32(0x10000000)>>(4*paramDigit);
   if(command&4)spawnParams-=u32(0x10000000)>>(4*paramDigit);
   return true;
  }
  if(edge&0x200)return false;
  if(command&8)spawnRow=(spawnRow+3)%4;
  if(command&4)spawnRow=(spawnRow+1)%4;
  const int delta=listDelta(command);
  if(spawnRow==0&&delta)spawnProfile=int(wrappedValue(spawnProfile,delta,0,fpcNm_MAX_NUM-1));
  if(spawnRow==2&&delta)spawnSubtype=int(wrappedValue(spawnSubtype,delta,-128,127));
  if(edge&0x100){if(spawnRow==1)editingParams=true;else if(spawnRow==3)spawn();}
  return true;
 }
 auto* actor=selectedActor();

 if(edge&0x200)return false;
 if(command&8)actorRow=(actorRow+9)%10;
 if(command&4)actorRow=(actorRow+1)%10;
 if(!actor)return true;
 const auto edit=actorEditRepeat.update(actorRow>=1&&actorRow<=6?buttons&3:0,1);
 const int direction=((actorRow>=1&&actorRow<=6?edit:command)&2)?1:((actorRow>=1&&actorRow<=6?edit:command)&1)?-1:0;
 if(actorRow==0){
  if(direction){
   const auto ids=actorIds();const auto it=std::find(ids.begin(),ids.end(),selected);
   if(!ids.empty())selected=ids[size_t(wrappedValue(it==ids.end()?0:it-ids.begin(),direction,0,ids.size()-1))];
   actor=selectedActor();
  }
  if(command&0x100)actor->pause_flag=actor->pause_flag?0:1;
  if((command&0x1000)&&fopAcM_GetName(actor)!=fpcNm_ALINK_e){fopAcM_delete(actor);selected=~fpc_ProcID(0);}
 }else if(direction&&actorRow<=6){
  const float step=(buttons&0x800)?1000.f:(buttons&0x400)?1.f:100.f;
  if(actorRow<=3){
   cXyz pos=actor->current.pos;
   float* axis=actorRow==1?&pos.x:actorRow==2?&pos.y:&pos.z;*axis+=direction*step;
   if(fopAcM_GetName(actor)==fpcNm_ALINK_e)placeNativeLink(pos,actor->shape_angle.y);
   else {actor->current.pos=pos;actor->old.pos=pos;}
  }else{
   s16* axis=actorRow==4?&actor->shape_angle.x:actorRow==5?&actor->shape_angle.y:&actor->shape_angle.z;
   *axis=s16(u16(*axis)+int(direction*step));
  }
 }
 return true;
}
bool drawActorMenu(std::string_view page){
 if(page!="actor spawner"&&page!="actor list")return false;
 auto p=spritePosition(Menu);char text[256];int row=0;
 auto draw=[&](int selectedRow,const char* fmt,auto... args){
  std::snprintf(text,sizeof(text),fmt,args...);
  drawGzText(text,p.x,p.y+20*row,row==selectedRow?cursorColor():0xffffffff);++row;
 };
 if(page=="actor spawner"){
  const float valueX=p.x+gzTextWidth("actor subtype:");
  draw(spawnRow,"actor name:");
  std::snprintf(text,sizeof(text),"[%04X] <%s>",spawnProfile,profileName(spawnProfile).c_str());
  drawGzText(text,valueX,p.y,spawnRow==0?cursorColor():0xffffffff);
  draw(spawnRow,"actor params:");
  char digits[9];std::snprintf(digits,sizeof(digits),"%08X",spawnParams);
  float digitX=p.x+gzTextWidth("actor params:  ");
  for(int i=0;i<8;++i){
   const std::string digit(1,digits[i]);
   drawGzText(digit,digitX,p.y+20,editingParams?(i==paramDigit?cursorColor():0xffffffff):(spawnRow==1?cursorColor():0xffffffff));
   digitX+=gzTextWidth(digit);
  }
  draw(spawnRow,"actor subtype:");
  std::snprintf(text,sizeof(text)," <%d>",spawnSubtype);
  drawGzText(text,valueX,p.y+40,spawnRow==2?cursorColor():0xffffffff);
  draw(spawnRow,"spawn");
  static const char* help[]={"Actor Name (Dpad / X/Y to scroll)","Actor Parameters (default: 0)","Actor subtype (default: -1) (Dpad / X/Y to scroll)","Spawn actor at current position"};
  drawGzText(help[spawnRow],p.x,440);
  return true;
 }
 auto* actor=selectedActor();
 if(!actor){draw(-1,"No actors in this scene");return true;}

 draw(actorRow,"name: <%s>%s",profileName(fopAcM_GetName(actor)).c_str(),actor->pause_flag?" [frozen]":"");
 draw(actorRow,"pos-x: <%.1f>",actor->current.pos.x);draw(actorRow,"pos-y: <%.1f>",actor->current.pos.y);draw(actorRow,"pos-z: <%.1f>",actor->current.pos.z);
 draw(actorRow,"rot-x: <0x%04X>",unsigned(u16(actor->shape_angle.x)));draw(actorRow,"rot-y: <0x%04X>",unsigned(u16(actor->shape_angle.y)));draw(actorRow,"rot-z: <0x%04X>",unsigned(u16(actor->shape_angle.z)));
 draw(actorRow,"addr: %p",static_cast<void*>(actor));draw(actorRow,"proc id: %d",int(fopAcM_GetName(actor)));draw(actorRow,"params: 0x%08X",unsigned(fopAcM_GetParam(actor)));
 const char* help=actorRow==0?"A: freeze actor, START: delete actor":
  actorRow<=3?"dpad: +/-100.0, X+dpad: +/-1.0, Y+dpad: +/-1000.0":
  actorRow<=6?"dpad: +/-100, X+dpad: +/-1, Y+dpad: +/-1000":
  actorRow==7?"current actor address":actorRow==8?"current actor process id":"current actor parameters";
 drawGzText(help,p.x,440);
 return true;
}
DEFINE_HOOK_SYMBOL("dDbVw_deleteDrawPacketList",void(),ActorGizmoFrame);
ModResult initActorTools(){
 return guardedPost<ActorGizmoFrame>([](ModContext*,void*,void*,void*){
  if(gzCurrentPage()!="actor list")return;
  auto* actor=selectedActor();if(!actor)return;
  cXyz size(10,10,10);csXyz rotation(0,0,0);GXColor white{255,255,255,255};
  dDbVw_drawCubeXlu(actor->current.pos,size,rotation,white);
  const GXColor colors[]={{255,0,0,255},{0,255,0,255},{0,0,255,255}};
  for(int i=0;i<3;i++){
   cXyz a=actor->current.pos,b=a;
   if(i==0){a.x+=200;b.x-=200;}else if(i==1){a.y+=200;b.y-=200;}else{a.z+=200;b.z-=200;}
   dDbVw_drawLineXlu(a,b,colors[i],0,20);
  }
 });
}
}
