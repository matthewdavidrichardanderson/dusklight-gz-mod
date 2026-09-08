// TPGZ hidden skills, scent and golden bugs, using decomp save accessors.
#include "core.hpp"
#include "collectible_data.hpp"
#include "d/d_com_inf_game.h"
#include <iterator>
namespace gz {
static void setEvent(u16 flag,bool on) {
 if(on)dComIfGs_onEventBit(flag);else dComIfGs_offEventBit(flag);
}
void initCollectibles() {
 struct Skill {const char* id;const char* label;u16 flag;};
 static constexpr Skill skills[]={
  {"ending_blow","Ending blow",0x2904},{"shield_bash","Shield bash",0x2908},
  {"backslice","Backslice",0x2902},{"helm_splitter","Helm splitter",0x2901},
  {"mortal_draw","Mortal draw",0x2a80},{"jump_strike","Jump strike",0x2a40},
  {"great_spin","Great spin",0x2a20}
 };
 for(const auto& skill:skills){
  const auto flag=skill.flag;
  choice(skill.id,"Collection",skill.label,{"Not learned","Learned"},
   [flag](){return dComIfGs_isEventBit(flag)?1:0;},
   [flag](int64_t v){setEvent(flag,v!=0);}).kind=UI_CONTROL_TOGGLE;
 }
 static constexpr u8 scents[]={255,180,176,178,179,181};
 choice("scent","Collection","Scent",{"None","Youths' scent","Scent of Ilia","Poe scent","Reekfish scent","Medicine scent"},
  [](){
   const auto scent=dComIfGs_getCollectSmell();
   for(size_t i=0;i<std::size(scents);i++)if(scents[i]==scent)return int64_t(i);
   return int64_t(0);
  },[](int64_t v){dComIfGs_setCollectSmell(scents[v]);});
 for(const auto& bug:bugs){
  const auto item=bug.item;const auto flag=bug.flag;
  const auto id="bug_"+std::to_string(item);
  choice(id.c_str(),"Golden bugs",bug.label,{"Not caught","Caught","Given to Agitha"},
   [item,flag](){return dComIfGs_isEventBit(flag)?2:dComIfGs_isItemFirstBit(item)?1:0;},
   [item,flag](int64_t v){
    if(v)dComIfGs_onItemFirstBit(item);else dComIfGs_offItemFirstBit(item);
    setEvent(flag,v==2);
   });
 }
}
}
