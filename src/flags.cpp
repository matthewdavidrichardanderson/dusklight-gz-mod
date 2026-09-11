// TPGZ general/portal/rupee/dungeon flag editors, using native save objects.
#include "core.hpp"
#include "c/c_damagereaction.h"
#include "d/d_com_inf_game.h"
#include "d/d_item_data.h"
namespace gz {
static void eventFlag(const char* id,const char* label,u16 flag,bool temporary=false) {
 choice(id,"Flags",label,{"Off","On"},
  [flag,temporary](){return temporary?dComIfGs_isTmpBit(flag):dComIfGs_isEventBit(flag);},
  [flag,temporary](int64_t v){
   if(temporary){if(v)dComIfGs_onTmpBit(flag);else dComIfGs_offTmpBit(flag);}
   else{if(v)dComIfGs_onEventBit(flag);else dComIfGs_offEventBit(flag);}
  });
}
static int liveStage() {
 auto* info=dComIfGp_getStageStagInfo();
 return info?dStage_stagInfo_GetSaveTbl(info):-1;
}
template<class F> static void editStage(int stage,F edit) {
 auto& saved=g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit();
 edit(saved);
 // Preserve unrelated live flags rather than copying the whole saved block over them.
 if(stage==liveStage())edit(g_dComIfG_gameInfo.info.getMemory().getBit());
}
static dSv_memBit_c& savedBits(int stage) {
 return g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit();
}
void initFlags() {
 choice("boss_flags","Flags","Boss flag",{"Off","On"},[](){return cDmr_SkipInfo!=0;},[](int64_t v){cDmr_SkipInfo=v?255:0;});
 eventFlag("coro_td_flag","Coro text displacement",0x0002,true);
 eventFlag("rusl_td","Rusl text displacement",0x0006,true);
 eventFlag("epona_stolen","Epona stolen",0x0580);
 eventFlag("epona_tamed","Epona tamed",0x0601);
 eventFlag("malo_mart","Malo Mart in Castle Town",0x2210);
 eventFlag("map_warping","Map warping",0x0604);
 eventFlag("midna_charge","Midna charge",0x0501);
 eventFlag("midna_healed","Midna healed",0x1e08);
 eventFlag("midna_available","Midna available",0x0c10);
 eventFlag("transform_warp","Transform / warp",0x0d04);
 eventFlag("wolf_sense","Wolf sense",0x4308);
 choice("midna_on_back","Flags","Midna on Wolf Link's back",{"Off","On"},
  [](){return dComIfGs_isTransformLV(3);},
  [](int64_t v){if(v)dComIfGs_onTransformLV(3);else dComIfGs_offTransformLV(3);});
 eventFlag("fundraising_1","First fundraising complete",0x2e20);
 eventFlag("fundraising_2","Second fundraising complete",0x0f10);
 number("donation","Flags","Charlo donation amount",0,1000,
  [](){return (dComIfGs_getEventReg(0xf7ff)<<8)|dComIfGs_getEventReg(0xf8ff);},
  [](int64_t v){dComIfGs_setEventReg(0xf7ff,(v>>8)&255);dComIfGs_setEventReg(0xf8ff,v&255);});
 number("fundraising","Flags","Fundraising amount",0,2000,
  [](){return (dComIfGs_getEventReg(0xf9ff)<<8)|dComIfGs_getEventReg(0xfaff);},
  [](int64_t v){dComIfGs_setEventReg(0xf9ff,(v>>8)&255);dComIfGs_setEventReg(0xfaff,v&255);});
 choice("rupee_first_get","Flags","Rupee first-get flags",{"Clear all","Mark all obtained"},
  [](){for(int i=dItemNo_BLUE_RUPEE_e;i<=dItemNo_SILVER_RUPEE_e;i++)if(dComIfGs_isItemFirstBit(i))return 1;return 0;},
  [](int64_t v){for(int i=dItemNo_BLUE_RUPEE_e;i<=dItemNo_SILVER_RUPEE_e;i++){if(v)dComIfGs_onItemFirstBit(i);else dComIfGs_offItemFirstBit(i);}})
  .help="Matches GZ's blue-through-silver first-get flags. Dusklight's cutscene preferences remain active.";

 static int region=1;
 choice("map_region","Portals","Region",{"Ordon","Faron","Eldin","Lanayru","Desert","Snowpeak"},
  [](){return region-1;},[](int64_t v){region=static_cast<int>(v)+1;});
 choice("region_unlocked","Portals","Region unlocked",{"Locked","Unlocked"},
  [](){return (g_dComIfG_gameInfo.info.getPlayer().getPlayerFieldLastStayInfo().mRegion&(1<<region))?1:0;},
  [](int64_t v){auto& bits=g_dComIfG_gameInfo.info.getPlayer().getPlayerFieldLastStayInfo().mRegion;
   if(v)bits|=1<<region;else bits&=~(1<<region);});
 struct Portal{const char* id;const char* label;int stage;int flag;};
 // Save-table indices are GZ stage data, not console memory offsets.
 static constexpr Portal portals[]={
  {"ordon","Ordon Spring",0,52},{"south_faron","South Faron",2,71},{"north_faron","North Faron",2,2},
  {"grove","Sacred Grove",7,100},{"gorge","Eldin Gorge",6,21},{"kakariko","Kakariko Village",3,31},
  {"mountain","Death Mountain",3,21},{"bridge","Bridge of Eldin",6,99},{"town","Castle Town",6,3},
  {"lake","Lake Hylia",4,10},{"domain","Zora's Domain",4,2},{"river","Upper Zora's River",4,21},
  {"snowpeak","Snowpeak",8,21},{"mesa","Gerudo Mesa",10,21},{"mirror","Mirror Chamber",10,40}
 };
 for(auto p:portals) {
  const auto id=std::string("portal_")+p.id;
  choice(id.c_str(),"Portals",p.label,{"Locked","Unlocked"},[p](){return savedBits(p.stage).isSwitch(p.flag)?1:0;},
   [p](int64_t v){editStage(p.stage,[p,v](dSv_memBit_c& bits){if(v)bits.onSwitch(p.flag);else bits.offSwitch(p.flag);});});
 }
 static int dungeon=16;
 choice("dungeon","Dungeons","Dungeon",{"Forest Temple","Goron Mines","Lakebed Temple","Arbiter's Grounds","Snowpeak Ruins","Temple of Time","City in the Sky","Palace of Twilight","Hyrule Castle"},
  [](){return dungeon-16;},[](int64_t v){dungeon=static_cast<int>(v)+16;});
 number("small_keys","Dungeons","Small keys",0,5,[](){return savedBits(dungeon).getKeyNum();},
  [](int64_t v){editStage(dungeon,[v](dSv_memBit_c& bits){bits.setKeyNum(static_cast<u8>(v));});});
 struct DungeonItem{const char* id;const char* label;int flag;};
 static constexpr DungeonItem items[]={
  {"dungeon_map","Map",dSv_memBit_c::MAP},{"compass","Compass",dSv_memBit_c::COMPASS},
  {"boss_key","Boss key",dSv_memBit_c::BOSS_KEY},{"miniboss_dead","Miniboss defeated",dSv_memBit_c::STAGE_BOSS_ENEMY_2},
  {"boss_dead","Boss defeated",dSv_memBit_c::STAGE_BOSS_ENEMY}
 };
 for(auto item:items)choice(item.id,"Dungeons",item.label,{"Off","On"},
  [item](){return savedBits(dungeon).isDungeonItem(item.flag)?1:0;},
  [item](int64_t v){editStage(dungeon,[item,v](dSv_memBit_c& bits){if(v)bits.onDungeonItem(item.flag);else bits.offDungeonItem(item.flag);});});
 action("clear_dungeon","Dungeons","Clear selected dungeon flags",[](){editStage(dungeon,[](dSv_memBit_c& bits){bits.init();});});
 // Original flag rows are A-button toggles, not left/right selectors.
 for(auto& control:controls)
  if((control.group=="Flags"||control.group=="Portals"||control.group=="Dungeons")&&
     control.kind==UI_CONTROL_SELECT&&control.options.size()==2)
   control.kind=UI_CONTROL_TOGGLE;

}
}
