// TPGZ commands and frame advance adapted to the native decomp controller/scene.
#include "core.hpp"
#include "dusk/ui/nav_types.hpp"
#include <dolphin/pad.h>
namespace Rml {class Event;}
#include "actor_tools.hpp"
#include "input_logic.hpp"
#include "link_tools.hpp"
#include "native_pose.hpp"
#include "d/d_com_inf_game.h"
#include "d/d_s_play.h"
#include "d/d_camera.h"
#include "d/actor/d_a_alink.h"
#include "m_Do/m_Do_controller_pad.h"
#include <vector>
#include <optional>

namespace gz {bool comboCaptureActive();void loadGorgePractice();bool gorgePracticeEnabled();void checkerTick();void reloadArea();void reloadPractice();void toggleTimer();void resetTimer();bool gzMenuOpen();void gzMenuInput(uint16_t);
DEFINE_HOOK(&dScnPly_c::calcPauseTimer, PauseTimer);
DEFINE_HOOK(&mDoCPd_c::read, PadRead);
DEFINE_HOOK_SYMBOL("dusk::ui::map_nav_event",dusk::ui::NavCommand(const Rml::Event*),ReloadMenuGuard);
struct Combo {
 const char* id; const char* label; uint16_t defaultMask;
 ConfigVarHandle config=0; std::function<void()> fn;
 bool held=false; const char* enabled=nullptr;
};
static std::vector<Combo> combos;
void resetCommandBinding(const char* id){for(auto& c:combos)if(std::string_view(c.id)==id){svc_config->set_int(mod_ctx,c.config,c.defaultMask);return;}}
static ConfigVarHandle reloadModeVar=0;
static int64_t reloadMode(){int64_t v=0;svc_config->get_int(mod_ctx,reloadModeVar,&v);return v==1?1:0;}
static void reloadCommand(){if(reloadMode())reloadPractice();else reloadArea();}
static uint16_t previous=0, lastSimulationButtons=0;
static bool stored=false, paused=false, stepRequested=false;
static bool timerOwned=false;
static s8 savedPauseTimer=0;
int framePauseVisual(){return paused?(timerOwned?1:2):0;}
static FrameAdvance advance;
static ConfigVarHandle advanceBinding=0;
static cXyz storedPosition(0,0,0);
static csXyz storedAngle(0,0,0);
static bool storedCamera=false;
static cXyz storedEye,storedCenter;
static JUTGamePad* consumedPad=nullptr;
static std::optional<JUTGamePad::CButton> originalButtons;
static void restoreRawInput() {
 if(consumedPad && originalButtons && consumedPad==mDoCPd_c::getGamePad(0))consumedPad->mButton=*originalButtons;
 consumedPad=nullptr;
}

static void releaseTimer() {
 // Do not overwrite a timer another system has changed since our write.
 if(timerOwned && dScnPly_c::pauseTimer==1)dScnPly_c::pauseTimer=savedPauseTimer;
 timerOwned=false;
}
static void togglePause() {
 if(!playable())return;
 paused=!paused; advance.reset(previous); stepRequested=false;
 lastSimulationButtons=previous;
 if(!paused)releaseTimer();
 notify(paused?"Frame pause enabled":"Frame pause disabled");
}
static void storePosition() {
 auto* p=daPy_getPlayerActorClass();if(!playable())return;
 storedPosition=p->current.pos;storedAngle=p->shape_angle;
  storedCamera=false;
 if(auto* camera=dCam_getBody()){storedEye=camera->iEye();storedCenter=camera->iCenter();storedCamera=true;}
 stored=true;notify("Position stored");
}
static void loadPosition() {
 auto* p=daPy_getPlayerActorClass();if(!playable()||!stored)return;
 placeNativeLink(storedPosition,storedAngle.y);
 if(storedCamera)placeNativeCamera(storedCenter,storedEye);
 notify("Position restored");
}
void storedLinkPose(cXyz& position,s16& angle){position=storedPosition;angle=storedAngle.y;}
void reloadArea();void toggleFreeCamera();
static uint16_t binding(ConfigVarHandle handle) {
 int64_t v=0;
 if(svc_config->get_int(mod_ctx,handle,&v)!=MOD_OK||v<0||v>0xFFFF)return 0;
 return static_cast<uint16_t>(v);
}
static void consume(uint16_t mask) {
 if(mask)if(auto* pad=mDoCPd_c::getGamePad(0)) {
  // Restore before the next hardware poll so its edge detector retains the raw previous state.
  originalButtons=pad->mButton;consumedPad=pad;
  auto& b=pad->mButton;b.mButton&=~mask;b.mTrigger&=~mask;b.mRelease&=~mask;b.mRepeat&=~mask;
  if(mask&PAD_TRIGGER_L){b.mAnalogL=0;b.mAnalogLf=0;}
  if(mask&PAD_TRIGGER_R){b.mAnalogR=0;b.mAnalogRf=0;}
  if(mask&PAD_BUTTON_A)b.mAnalogA=0;
  if(mask&PAD_BUTTON_B)b.mAnalogB=0;
 }
 auto& p=mDoCPd_c::getCpadInfo(0);
 p.mButtonFlags&=~mask;p.mPressedButtonFlags&=~mask;
 if(mask&PAD_TRIGGER_L){p.mTriggerLeft=0;p.mHoldLockL=p.mTrigLockL=false;}
 if(mask&PAD_TRIGGER_R){p.mTriggerRight=0;p.mHoldLockR=p.mTrigLockR=false;}
 if(mask&PAD_BUTTON_A)p.mAnalogA=0;
 if(mask&PAD_BUTTON_B)p.mAnalogB=0;
}
ModResult initInput() {
 toggle("turbo","Tools","Turbo mode","Simulates repeated button presses while buttons are held.");
 toggle("teleport","Tools","Teleport hotkeys","R + Up stores position; R + Down restores it.");
 toggle("area_reload","Tools","Area reload hotkey","L + R + A + Start reloads the entrance snapshot, restoring temporary flags and tears. Choose area or last practice save below.");
 toggle("frame_advance","Tools","Frame pause and advance hotkeys","R + Left pauses; R advances. Hold R for 30 frames to repeat.");
 ConfigVarDesc reloadConfig=CONFIG_VAR_DESC_INIT;reloadConfig.name="reload_mode";reloadConfig.type=CONFIG_VAR_INT;
 auto reloadResult=svc_config->register_var(mod_ctx,&reloadConfig,&reloadModeVar);if(reloadResult!=MOD_OK)return reloadResult;
 choice("reload_mode","Practice","area reload behavior:",{"load area","load file"},reloadMode,
  [](int64_t v){svc_config->set_int(mod_ctx,reloadModeVar,v==1?1:0);}).gameOnly=false;
 action("store_position","Tools","Store position",storePosition);
 action("load_position","Tools","Restore position",loadPosition).available=[](){return stored;};
 action("reload_area","Tools","Reload current area",reloadArea);
 action("pause","Tools","Pause / resume simulation",togglePause);
 action("step","Tools","Advance one simulation frame",[](){stepRequested=true;}).available=[](){return paused;};
 combos={
  {"combo_menu","Open menu",0x64,0,[]{openMenu();}},
  {"combo_store","Store position",0x28,0,storePosition,false,"teleport"},
  {"combo_load","Restore position",0x24,0,loadPosition,false,"teleport"},
  {"combo_moon","Moon jump",0x120,0,[]{if(playable())daPy_getPlayerActorClass()->speed.y=56;},true,"moon_jump"},
  {"combo_gorge","Gorge void practice",0x50,0,loadGorgePractice,false,"gorge_void"},
  {"combo_reload","Reload area / practice",0x1160,0,reloadCommand,false,"area_reload"},
  {"combo_pause","Pause simulation",0x21,0,togglePause,false,"frame_advance"},
  {"combo_move_link","Move Link",0x860,0,toggleMoveLink,false,"move_link"},
  {"combo_free_cam","Free camera",0x310,0,toggleFreeCamera,false,"free_cam"},
  {"combo_timer","Start / stop timer",0x110,0,toggleTimer,false,"timer"},
  {"combo_timer_reset","Reset timer",0x210,0,resetTimer,false,"timer"},
  {"combo_step","Frame advance",0x20,0,{},false,"frame_advance"}
 };
 for(auto& c:combos) {
  ConfigVarDesc d=CONFIG_VAR_DESC_INIT;d.name=c.id;d.type=CONFIG_VAR_INT;d.default_int=c.defaultMask;
  auto r=svc_config->register_var(mod_ctx,&d,&c.config);if(r!=MOD_OK)return r;
  auto& control=number(c.id,"Hotkeys",c.label,0,0x1F7F,
   [&c](){return binding(c.config);},
   [&c](int64_t v){svc_config->set_int(mod_ctx,c.config,v);});
  control.gameOnly=false;
  control.help="GameCube button mask; 0 disables. Commands require exact combinations. Frame advance accepts additional gameplay buttons.";
 }
 advanceBinding=combos.back().config;
 auto r=guardedPost<ReloadMenuGuard>([](ModContext*,void*,void* result,void*){
  auto& command=*static_cast<dusk::ui::NavCommand*>(result);
  if(command!=dusk::ui::NavCommand::Menu||!playable()||!on("area_reload")||gzMenuOpen()||comboCaptureActive())return;
  bool visible=false;svc_ui->is_any_document_visible(mod_ctx,&visible);if(visible)return;
  uint16_t reloadMask=0;
  for(const auto& combo:combos)if(std::string_view(combo.id)=="combo_reload"){reloadMask=binding(combo.config);break;}
  if(!reloadReservesMenu(reloadMask,reloadMask))return;
  // UI events precede the game's pad read. Sample physical/keyboard mappings
  // here rather than the previous simulation frame's JUTGamePad snapshot.
  PADStatus status[PAD_MAX_CONTROLLERS]{};PADRead(status);
  if(status[0].err==PAD_ERR_NONE&&reloadReservesMenu(status[0].button,reloadMask))
   command=dusk::ui::NavCommand::None;
 });if(r!=MOD_OK)return r;
 r=guardedPre<PadRead>([](ModContext*,void*,void*,void*){restoreRawInput();return HOOK_CONTINUE;});
 if(r!=MOD_OK)return r;
 return guardedPre<PauseTimer>([](ModContext*,void*,void* ret,void*){
  if(!timerOwned)return HOOK_CONTINUE;
  // Keep the original scene/actor pause flag set without consuming engine hitstop.
  dScnPly_c::pauseTimer=1;*static_cast<s8*>(ret)=1;return HOOK_SKIP_ORIGINAL;
 });
}
void inputTick() {
 checkerTick();
 releaseTimer();
 uint16_t buttons=0;
 if(auto* p=mDoCPd_c::getGamePad(0))buttons=static_cast<uint16_t>(p->getButton());
 bool visible=false;svc_ui->is_any_document_visible(mod_ctx,&visible);
 if(!playable()||!on("frame_advance")){paused=false;stepRequested=false;advance.reset(buttons);}
 uint16_t consumed=0;
 const bool wasPaused=paused;
 const bool wasMenu=gzMenuOpen();visible=visible||wasMenu;
 for(auto& c:combos) {
  if(comboCaptureActive())break;
  const auto mask=binding(c.config);
  if(!c.fn||!comboMatches(buttons,mask))continue;
  if(moveLinkActive()&&std::string_view(c.id)=="combo_menu")continue;
  if(c.enabled){
   const bool timerCommand=std::string_view(c.enabled)=="timer";
   if(timerCommand){if(!(on("timer")||on("igt_timer")||(std::string_view(c.id)=="combo_timer_reset"&&on("load_timer"))))continue;}
   else if(std::string_view(c.id)=="combo_gorge"){if(!playable()||!gorgePracticeEnabled())continue;}
   else if(std::string_view(c.id)=="combo_store"){if(!playable()||!(on("teleport")||on("displacement")))continue;}
   else if(!playable()||!on(c.enabled))continue;
  }
  if(visible&&std::string_view(c.id)!="combo_menu")continue;
  if(c.held||comboTriggered(buttons,previous,mask))c.fn();
  consumed|=mask;break;
 }
 if(wasMenu&&!consumed)gzMenuInput(buttons);
 if(wasMenu||gzMenuOpen())consumed=0xffff;
 if(paused) {
  if(!wasPaused){advance.reset(buttons);lastSimulationButtons=buttons;}
  const auto mask=binding(advanceBinding);
  if(visible||consumed){advance.reset(buttons);}
  else if(advance.update(buttons,mask))stepRequested=true;
  if(stepRequested) {
   // Keep presses made while frozen visible on the next simulated frame.
   mDoCPd_c::getCpadInfo(0).mPressedButtonFlags=buttons&~lastSimulationButtons;
   lastSimulationButtons=buttons;
   consumed|=mask;
  } else {
   savedPauseTimer=dScnPly_c::pauseTimer;
   timerOwned=true;dScnPly_c::pauseTimer=1;
  }
 } else {
  advance.reset(buttons);lastSimulationButtons=buttons;
 }

 actorViewTick();
 moveLinkTick();
 consume(consumed);
 if(on("turbo")&&!gzMenuOpen())mDoCPd_c::getCpadInfo(0).mPressedButtonFlags=mDoCPd_c::getCpadInfo(0).mButtonFlags;
 stepRequested=false;previous=buttons;
}
void shutdownInput(){restoreRawInput();releaseTimer();paused=false;}
}
