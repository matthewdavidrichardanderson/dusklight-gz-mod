// TPGZ area reload: preserve native entrance state, temporary flags and tears.
#include "core.hpp"
#include "loading.hpp"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_scene.h"
#include "SSystem/SComponent/c_phase.h"
#include <array>
#include <optional>
namespace gz {
bool practiceLoadPending();
DEFINE_HOOK_SYMBOL("dScnPly_Create",int(scene_class*),PlaySceneCreate);
static std::optional<dSv_memory_c> savedMemory;
static std::array<u8,4> savedTears{};
static std::string savedStage;
static int savedRoom=0,savedPoint=0,savedLayer=-1;
static bool valid=false,pending=true;

void resetReloadForSpeedrun(){valid=false;pending=true;savedMemory.reset();}
void reloadTick() {
 if(!pending||!playable()||sceneLoading()||practiceLoadPending())return;
 const auto* stage=dComIfGp_getNextStartStage();
 savedMemory=g_dComIfG_gameInfo.info.getMemory();
 for(int i=0;i<4;i++)savedTears[i]=dComIfGs_getLightDropNum(i);
 savedStage=stage->getName();savedRoom=stage->getRoomNo();
 savedPoint=stage->getPoint();savedLayer=stage->getLayer();
 valid=true;pending=false;
}
void reloadArea() {
 if(!playable()||sceneLoading()||practiceLoadPending())return;
 if(!valid||savedStage!=dComIfGp_getStartStageName()){
  notify("Area reload is waiting for an entrance snapshot");return;
 }
 g_dComIfG_gameInfo.info.getMemory()=*savedMemory;
 for(int i=0;i<4;i++)dComIfGs_setLightDropNum(i,savedTears[i]);
 g_dComIfG_gameInfo.play.setNextStage(savedStage.c_str(),savedRoom,savedPoint,savedLayer,13,0);
}
ModResult initReload() {
 return guardedPost<PlaySceneCreate>([](ModContext*,void*,void* ret,void*){
  if(*static_cast<int*>(ret)==cPhs_COMPLEATE_e){pending=true;valid=false;}
 });
}
}
