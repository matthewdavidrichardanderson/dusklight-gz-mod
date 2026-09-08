// TPGZ item-wheel data/behavior adapted to decomp item accessors (GPL-3.0).
#include "core.hpp"
#include "item_data.hpp"
#include "d/d_com_inf_game.h"
#include <iterator>
#include <string_view>
#include <cstdio>
#include "gz_font.hpp"
#include "presentation.hpp"
#include "menu_logic.hpp"
namespace gz {
static int selectedSlot=0;
static void setWheelItem(int slot,u8 item,bool fixBombs) {
 if(fixBombs && slot>=15 && slot<=17 &&
    (item==dItemNo_NORMAL_BOMB_e||item==dItemNo_WATER_BOMB_e||item==dItemNo_POKE_BOMB_e))
  dComIfGs_setBombNum(slot-15,1);
 dComIfGs_setItem(slot,item);
}
void initItemWheel() {
 number("wheel_slot","Item wheel","Slot",0,23,[](){return selectedSlot;},
  [](int64_t v){selectedSlot=static_cast<int>(v);});
 std::vector<const char*> names;
 for(const auto& item:wheelItems)names.push_back(item.label);
 names.push_back("Unlisted item (unchanged)");
 choice("wheel_item","Item wheel","Item",names,
  [](){
   const auto item=dComIfGs_getItem(selectedSlot,false);
   for(size_t i=0;i<std::size(wheelItems);i++)if(wheelItems[i].item==item)return int64_t(i);
   return int64_t(std::size(wheelItems));
  },
  [](int64_t v){
   if(v>=0&&v<int64_t(std::size(wheelItems)))setWheelItem(selectedSlot,wheelItems[v].item,true);
  }).help="All 24 GZ slots and all 58 item choices. Selecting bombs in slots 15–17 sets the bag count to one.";
 action("wheel_default","Item wheel","Set slot default",[](){setWheelItem(selectedSlot,wheelDefaults[selectedSlot],false);});
 action("wheel_clear","Item wheel","Clear slot",[](){setWheelItem(selectedSlot,dItemNo_NONE_e,false);});
}
bool itemWheelInput(std::string_view page,uint16_t command,uint16_t edge){
 if(page!="Item wheel")return false;
 if(edge&0x200)return false;
 if(command&8)selectedSlot=(selectedSlot+23)%24;
 if(command&4)selectedSlot=(selectedSlot+1)%24;
 if(!playable())return true;
 const int delta=command&2?1:command&1?-1:0;
 if(delta){
  const auto current=dComIfGs_getItem(selectedSlot,false);size_t index=0;
  for(size_t i=0;i<std::size(wheelItems);i++)if(wheelItems[i].item==current)index=i;
  index=size_t(wrappedValue(index,delta,0,std::size(wheelItems)-1));
  setWheelItem(selectedSlot,wheelItems[index].item,true);
 }
 if(edge&16)setWheelItem(selectedSlot,wheelDefaults[selectedSlot],false);
 if(edge&0x400)setWheelItem(selectedSlot,dItemNo_NONE_e,false);
 return true;
}
bool drawItemWheel(std::string_view page){
 if(page!="Item wheel")return false;
 const auto p=spritePosition(Menu);char text[160];
 if(!playable()){drawGzText("No active game",p.x,p.y);return true;}
 menuScroll.update(selectedSlot,24);
 const int first=int(menuScroll.first);
 for(int slot=first;slot<24&&slot<first+15;slot++){
  const auto value=dComIfGs_getItem(slot,false);const char* name="unlisted";
  for(const auto& item:wheelItems)if(item.item==value)name=item.label;
  if(value==dItemNo_NONE_e)name="n/a";
  const auto color=slot==selectedSlot?cursorColor():0xffffffff;
  std::snprintf(text,sizeof(text),"Slot %d:",slot);
  drawGzText(text,p.x,p.y+(slot-first)*20,color);
  drawGzText(std::string(" <")+name+">",p.x+gzTextWidth("Slot 23:"),p.y+(slot-first)*20,color);
 }
 const char* defaultName="n/a";
 for(const auto& item:wheelItems)if(item.item==wheelDefaults[selectedSlot])defaultName=item.label;
 std::snprintf(text,sizeof(text),"Slot %d default: %s. Z: set default; X: reset.",selectedSlot,defaultName);
 drawGzText(text,p.x,440);
 if(first)drawGzText("^",p.x,p.y-12);
 if(first+15<24)drawGzText("v",p.x,p.y+298);
 return true;
}

}
