// Adapted from TPGZ menu_amounts and menu_equipment (GPL-3.0).
#include "core.hpp"
#include "d/d_com_inf_game.h"
#include "d/d_item_data.h"
#include <algorithm>
namespace gz {
static void itemBit(u8 item,bool value) {
 if(value)dComIfGs_onItemFirstBit(item);else dComIfGs_offItemFirstBit(item);
}
static void equipmentFlag(const char* id,const char* label,u8 item) {
 choice(id,"Equipment",label,{"none",label},
  [item](){return dComIfGs_isItemFirstBit(item)?1:0;},
  [item](int64_t v){itemBit(item,v!=0);});
}
void initInventory() {
 number("health","Amounts","Health (quarters)",0,80,[](){return dComIfGs_getLife();},
  [](int64_t v){dComIfGs_setLife(static_cast<u16>(std::min(v,int64_t(dComIfGs_getMaxLife()/5*4))));});
 number("hearts","Amounts","Heart containers",0,20,[](){return dComIfGs_getMaxLife()/5;},
  [](int64_t v){
   dComIfGs_setMaxLife(static_cast<u8>(v*5+dComIfGs_getMaxLife()%5));
   if(dComIfGs_getLife()>v*4)dComIfGs_setLife(static_cast<u16>(v*4));
  }).help="Preserves the remainder of collected heart pieces and clamps current health.";
 number("heart_pieces","Amounts","Heart pieces (total)",0,255,[](){return dComIfGs_getMaxLife();},
  [](int64_t v){dComIfGs_setMaxLife(static_cast<u8>(v));}).help="GZ edits the raw total: five pieces per heart container.";
 number("arrows","Amounts","Arrows",0,255,[](){return dComIfGs_getArrowNum();},
  [](int64_t v){dComIfGs_setArrowNum(static_cast<u8>(v));});
 for(int i=0;i<3;i++) {
  const auto id="bomb_count_"+std::to_string(i);
  const auto label="Bomb bag "+std::to_string(i+1)+" count";
  number(id.c_str(),"Amounts",label.c_str(),0,255,[i](){return dComIfGs_getBombNum(i);},
   [i](int64_t v){dComIfGs_setBombNum(i,static_cast<u8>(v));});
 }
 number("seeds","Amounts","Slingshot seeds",0,255,[](){return dComIfGs_getPachinkoNum();},
  [](int64_t v){dComIfGs_setPachinkoNum(static_cast<u8>(v));});
 number("poes","Amounts","Poe souls",0,255,[](){return dComIfGs_getPohSpiritNum();},
  [](int64_t v){dComIfGs_setPohSpiritNum(static_cast<u8>(v));});
 number("rupees","Amounts","Rupees",0,65535,[](){return dComIfGs_getRupee();},
  [](int64_t v){dComIfGs_setRupee(static_cast<u16>(v));});

 choice("ordon_sword","Equipment","Ordon sword",{"none","wooden sword","ordon sword"},
  [](){return dComIfGs_isItemFirstBit(dItemNo_SWORD_e)?2:dComIfGs_isItemFirstBit(dItemNo_WOOD_STICK_e)?1:0;},
  [](int64_t v){itemBit(dItemNo_WOOD_STICK_e,v==1);itemBit(dItemNo_SWORD_e,v==2);});
 choice("master_sword","Equipment","Master sword",{"none","master sword","light sword"},
  [](){return dComIfGs_isItemFirstBit(dItemNo_LIGHT_SWORD_e)?2:dComIfGs_isItemFirstBit(dItemNo_MASTER_SWORD_e)?1:0;},
  [](int64_t v){
   if(v!=2)itemBit(dItemNo_MASTER_SWORD_e,v==1);
   itemBit(dItemNo_LIGHT_SWORD_e,v==2);
  });
 choice("wood_shield","Equipment","Wooden shield",{"none","ordon shield","wooden shield"},
  [](){return dComIfGs_isItemFirstBit(dItemNo_SHIELD_e)?2:dComIfGs_isItemFirstBit(dItemNo_WOOD_SHIELD_e)?1:0;},
  [](int64_t v){
   // Select exclusively so a previous higher-priority shield does not mask the choice.
   itemBit(dItemNo_WOOD_SHIELD_e,v==1);itemBit(dItemNo_SHIELD_e,v==2);
  });
 equipmentFlag("hylian_shield","hylian shield",dItemNo_HYLIA_SHIELD_e);
 equipmentFlag("hero_tunic","hero's tunic",dItemNo_WEAR_KOKIRI_e);
 equipmentFlag("zora_armor","zora armor",dItemNo_WEAR_ZORA_e);
 equipmentFlag("magic_armor","magic armor",dItemNo_ARMOR_e);
 choice("bomb_capacity","Equipment","Bomb capacity",{"30/15/10","60/30/20"},
  [](){return dComIfGs_isItemFirstBit(dItemNo_BOMB_BAG_LV2_e)?1:0;},
  [](int64_t v){itemBit(dItemNo_BOMB_BAG_LV2_e,v!=0);});
 choice("wallet","Equipment","Wallet",{"300 Rupees","600 Rupees","1000 Rupees"},
  [](){return dComIfGs_getWalletSize();},[](int64_t v){dComIfGs_setWalletSize(static_cast<u8>(v));});
 choice("quiver","Equipment","Arrow capacity",{"30 Arrows","60 Arrows","100 Arrows"},
  [](){return dComIfGs_getArrowMax()==100?2:dComIfGs_getArrowMax()==60?1:0;},
  [](int64_t v){constexpr u8 capacities[]={30,60,100};dComIfGs_setArrowMax(capacities[v]);});
}
}
