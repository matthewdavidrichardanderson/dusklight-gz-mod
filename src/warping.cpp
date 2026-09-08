#include "core.hpp"
#include "warp_data.hpp"
#include "d/d_com_inf_game.h"
#include <iterator>
#include <string_view>
#include <algorithm>
#include "gz_font.hpp"
#include "presentation.hpp"
#include "menu_logic.hpp"
#include <cstdio>
namespace gz {
static size_t selectedStage=0;
static int selectedRoom=0,selectedSpawn=0,selectedLayer=-1;
static const WarpRoom* room(){
 const auto& stage=warpStages[selectedStage];
 for(size_t i=stage.firstRoom;i<stage.firstRoom+stage.roomCount;i++)if(warpRooms[i].id==selectedRoom)return &warpRooms[i];
 return nullptr;
}
static bool validDestination(){
 const auto* r=room();if(!r)return false;
 for(size_t i=r->firstSpawn;i<r->firstSpawn+r->spawnCount;i++)if(warpSpawns[i]==selectedSpawn)return true;
 return false;
}
static void resetSpawn(){auto* r=room();selectedSpawn=r&&r->spawnCount?warpSpawns[r->firstSpawn]:0;selectedLayer=-1;}
static void performWarp(){
  if(!validDestination())return;
  auto& restart=g_dComIfG_gameInfo.info.getRestart();restart.mLastMode=0;
  g_dComIfG_gameInfo.play.setNextStage(warpStages[selectedStage].id,selectedRoom,selectedSpawn,selectedLayer,13,0);
  closeMenu();
}
static void saveWarpLocation(){
  if(!validDestination())return;
  g_dComIfG_gameInfo.info.getPlayer().getPlayerReturnPlace().set(warpStages[selectedStage].id,static_cast<s8>(selectedRoom),static_cast<u8>(selectedSpawn));
  notify("Save return location changed");
}
void initWarping(){
 std::vector<std::string> labels;
 for(const auto& stage:warpStages)labels.push_back(std::string(stage.category)+" / "+stage.name+" ("+stage.id+")");
 std::vector<const char*> pointers;for(const auto& label:labels)pointers.push_back(label.c_str());
 choice("warp_stage","Warping","Stage",pointers,[](){return static_cast<int64_t>(selectedStage);},
  [](int64_t v){selectedStage=static_cast<size_t>(v);selectedRoom=warpRooms[warpStages[selectedStage].firstRoom].id;resetSpawn();});
 selectedRoom=warpRooms[warpStages[0].firstRoom].id;resetSpawn();
 number("warp_room","Warping","Room",0,255,[](){return selectedRoom;},[](int64_t v){selectedRoom=static_cast<int>(v);resetSpawn();});
 auto& roomInfo=textInput("warp_room_info","Warping","Available rooms",[](){
  std::string text;const auto& stage=warpStages[selectedStage];
  for(size_t i=stage.firstRoom;i<stage.firstRoom+stage.roomCount;i++){
   if(!text.empty())text+=", ";text+=std::to_string(warpRooms[i].id)+": "+warpRooms[i].name;
  }return text;
 },{});
 roomInfo.available=[](){return false;};
 number("warp_spawn","Warping","Spawn",0,255,[](){return selectedSpawn;},[](int64_t v){selectedSpawn=static_cast<int>(v);selectedLayer=-1;});
 auto& spawnInfo=textInput("warp_spawn_info","Warping","Available spawns",[](){
  auto* r=room();if(!r)return std::string("Choose a listed room");
  std::string text;
  for(size_t i=r->firstSpawn;i<r->firstSpawn+r->spawnCount;i++){if(!text.empty())text+=", ";text+=std::to_string(warpSpawns[i]);}
  return text;
 },{});
 spawnInfo.available=[](){return false;};
 number("warp_layer","Warping","Layer (-1 = default)",-1,15,[](){return selectedLayer;},[](int64_t v){selectedLayer=static_cast<int>(v);});
 action("warp_now","Warping","Warp",performWarp).available=validDestination;
 action("warp_return","Warping","Set save return location",saveWarpLocation).available=validDestination;
}
namespace {
int warpRow=0;
const char* warpTypes[]={"cave","dungeon","interior","overworld","special"};
int warpType(){
 for(int i=0;i<5;i++)if(std::string_view(warpTypes[i])==warpStages[selectedStage].category)return i;
 return 0;
}
void selectStage(size_t i){selectedStage=i;selectedRoom=warpRooms[warpStages[i].firstRoom].id;resetSpawn();}
}
bool warpMenuInput(std::string_view page,uint16_t command,uint16_t edge){
 if(page!="Warping")return false;
 if(edge&0x200)return false;
 if(command&8)warpRow=(warpRow+6)%7;
 if(command&4)warpRow=(warpRow+1)%7;
 const int delta=command&2?1:command&1?-1:0;
 if(delta){
  if(warpRow==0){
   const int type=int(wrappedValue(warpType(),delta,0,4));
   for(size_t i=0;i<std::size(warpStages);i++)if(std::string_view(warpStages[i].category)==warpTypes[type]){selectStage(i);break;}
  }else if(warpRow==1){
   std::vector<size_t> indices;
   for(size_t i=0;i<std::size(warpStages);i++)if(std::string_view(warpStages[i].category)==warpTypes[warpType()])indices.push_back(i);
   auto pos=std::find(indices.begin(),indices.end(),selectedStage)-indices.begin();
   selectStage(indices[size_t(wrappedValue(pos,delta,0,indices.size()-1))]);
  }else if(warpRow==2){
   const auto& stage=warpStages[selectedStage];
   size_t index=0;for(size_t i=0;i<stage.roomCount;i++)if(warpRooms[stage.firstRoom+i].id==selectedRoom)index=i;
   selectedRoom=warpRooms[stage.firstRoom+size_t(wrappedValue(index,delta,0,stage.roomCount-1))].id;resetSpawn();
  }else if(warpRow==3){
   const auto* r=room();if(r&&r->spawnCount){
    size_t index=0;for(size_t i=0;i<r->spawnCount;i++)if(warpSpawns[r->firstSpawn+i]==selectedSpawn)index=i;
    selectedSpawn=warpSpawns[r->firstSpawn+size_t(wrappedValue(index,delta,0,r->spawnCount-1))];selectedLayer=-1;
   }
  }else if(warpRow==4)selectedLayer=int(wrappedValue(selectedLayer,delta,-1,15));
 }
 if(playable()&&(edge&0x100)){if(warpRow==5)performWarp();else if(warpRow==6)saveWarpLocation();}
 return true;
}
bool drawWarpMenu(std::string_view page){
 if(page!="Warping")return false;
 auto p=spritePosition(Menu);char text[160];int row=0;
 auto draw=[&](const char* format,auto... args){
  if constexpr(sizeof...(args)==0)std::snprintf(text,sizeof(text),"%s",format);else std::snprintf(text,sizeof(text),format,args...);
  const auto color=row==warpRow?cursorColor():0xffffffff;
  const std::string full=text;const auto split=full.find(" <");
  if(split==std::string::npos)drawGzText(full,p.x,p.y+20*row,color);
  else {drawGzText(full.substr(0,split),p.x,p.y+20*row,color);drawGzText(full.substr(split),p.x+gzTextWidth("spawn:"),p.y+20*row,color);}
  row++;
 };
 draw("type: <%s>",warpTypes[warpType()]);
 draw("stage: <%s>",warpStages[selectedStage].name);
 const auto* r=room();draw("room: <%s>",r?r->name:"unlisted");
 draw("spawn: <%d>",selectedSpawn);
 if(selectedLayer<0)draw("layer: <default>");else draw("layer: <%d>",selectedLayer);
 draw("warp");draw("save");
 static const char* help[]={"The type of stage","Current stage name","Current room name","Current spawn number","Current layer number","Trigger warp","Set savefile location to selected location"};
 drawGzText(help[warpRow],p.x,440);
 return true;
}

}
