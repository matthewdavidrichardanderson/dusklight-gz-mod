// Native adapters for TPGZ practice timing tools and its FIFO display.
#include "core.hpp"
#include "loading.hpp"
#include "gz_font.hpp"
#include "presentation.hpp"
#include "gorge_timing.hpp"
#include "d/d_com_inf_game.h"
#include "d/d_meter2_info.h"
#include "d/d_menu_window.h"
#include "d/actor/d_a_alink.h"
#include "f_pc/f_pc_manager.h"
#include "f_pc/f_pc_name.h"
#include "SSystem/SComponent/c_counter.h"
#include "m_Do/m_Do_controller_pad.h"
#include <array>
#include <chrono>
#include <cstdio>
#include <string>
namespace gz {
bool gzMenuOpen();
void loadCheckerPractice(const char*,const char*,void(*)(),void(*)());
namespace {
enum GZPad {DPAD_LEFT=1,DPAD_RIGHT=2,DPAD_DOWN=4,DPAD_UP=8,Z=16,R=32,L=64,A=256,B=512,X=1024,Y=2048,START=4096};
constexpr int POST_GAME_LOOP=1;
static uint16_t buttons=0,previous=0;
static ConfigVarHandle gorgeMode=0;
int gorgeChoice(){int64_t value=0;svc_config->get_int(mod_ctx,gorgeMode,&value);return value>=0&&value<=2?int(value):0;}
bool GZ_getButtonPressed(int mask){return (buttons&mask)!=0;}
// Original records pressed_frame = current + 1 before the game loop, then
// compensates pre-loop reads by +1. Both phases therefore test the same poll
// edge. This adapter samples once before simulation and retains it post-loop.
bool GZ_getButtonHold(int mask,int=0){return (buttons&~previous&mask)==0;}
struct Message{std::string text;int ttl=0;uint32_t rgba=0;};
static std::array<Message,25> Queue;
struct FIFOQueue{
 static void push(const char* text,std::array<Message,25>& queue,uint32_t color=0xffffff00){
  for(size_t i=queue.size()-1;i>0;--i)queue[i]=queue[i-1];
  queue[0]={text,120,color};
  svc_log->info(mod_ctx,(std::string("Checker: ")+text).c_str());
 }
};
static bool gorgeWarpActive(){
 auto* p=daAlink_getAlinkActorClass();
 return p&&GorgeTiming::arrivalActive(dComIfGp_getEvent()->mEventStatus,
  p->mProcID==daAlink_c::PROC_WARP&&p->mProcVar2.field_0x300c!=0);
}
static bool gorgeControlReady(){
 auto* p=daAlink_getAlinkActorClass();
 return p&&!p->checkEventRun()&&p->mProcID!=daAlink_c::PROC_WARP;
}
static void gorgeTrace(const char* text){svc_log->info(mod_ctx,text);}
#define GCN_PLATFORM 1
#include "checkers_native.inc"
#include "gorge_native.inc"
#undef GCN_PLATFORM
static uint8_t pressesA=0,pressesB=0,bpsA=0,bpsB=0;
static std::chrono::steady_clock::time_point mashStart;
static bool mashEnabled=false;
static uint32_t speedColor(uint8_t speed){return speed>=11?0x00cc00ff:speed>=9?0xcccc00ff:speed?0xcc0000ff:0xffffffff;}
static bool checkerInputBlocked=false;
static void resetChangedCheckers(){
 struct State{const char* id;void(*reset)();bool active=false;};
 static State states[]={
  {"coro_td",coro_td::reset},{"ebmb",ebmb::reset},{"elevator_escape",elevator_escape::reset},
  {"lfc",lfc::reset},{"rolling",rolling::reset},{"umd",umd::reset},{"fast_eel_regrab",fast_eel_regrab::reset}};
 for(auto& state:states){
  const bool active=on(state.id);
  if(active!=state.active){state.reset();state.active=active;}
 }
 static int lastGorge=0;const int mode=gorgeChoice();
 if(mode!=lastGorge){gorge::reset();lastGorge=mode;}
}
}
void pushGzMessage(const char* text,uint32_t color){FIFOQueue::push(text,Queue,color);}
DEFINE_HOOK(&fpcM_Management, CheckerPostLoop);
DEFINE_HOOK(&daAlink_c::execute, GorgeBeforeLink);
void checkerTick(){
 resetChangedCheckers();
 // A load starts a new attempt; do not erase transitions on every fade frame.
 static bool previousLoad=false;
 const bool loading=sceneLoading();
 if(loading&&!previousLoad)gorge::reset();
 previousLoad=loading;
 previous=buttons;buttons=0;
 if(auto* pad=mDoCPd_c::getGamePad(0))buttons=uint16_t(pad->getButton());
 bool hostUi=false;svc_ui->is_any_document_visible(mod_ctx,&hostUi);
 checkerInputBlocked=hostUi||gzMenuOpen();
 const auto rawButtons=buttons;
 if(checkerInputBlocked)buttons=0;

 {
  if(!gzMenuOpen())for(auto& message:Queue)if(message.ttl>0)--message.ttl;
  if(on("coro_td"))coro_td::execute();
  if(on("ebmb"))ebmb::execute();
  if(on("elevator_escape"))elevator_escape::execute();
  if(on("lfc"))lfc::execute();
  // The original condition assumes player exists before testing its form.
  if(on("rolling")&&daAlink_getAlinkActorClass())rolling::execute();
  if(on("fast_eel_regrab"))fast_eel_regrab::execute();
 }
 buttons=rawButtons;
 if(!on("mash_checker")){mashEnabled=false;return;}
 const auto now=std::chrono::steady_clock::now();
 if(!mashEnabled){mashStart=now;pressesA=pressesB=bpsA=bpsB=0;mashEnabled=true;}
 if(now-mashStart>=std::chrono::seconds(1)){
  bpsA=pressesA;bpsB=pressesB;pressesA=pressesB=0;mashStart=now;
 }
 if(buttons&~previous&A)++pressesA;
 if(buttons&~previous&B)++pressesB;
}
void loadGorgePractice(){
 const int mode=gorgeChoice();if(!mode)return;
 // TPGZ deliberately starts Human Gorge from FRST_BIT, then overrides its
 // destination and both callbacks. Use resource identity instead of menu indices.
 loadCheckerPractice(mode==1?"practice/any/gorge_void.bin":"practice/any/forest_bit.bin",
  mode==1?"Gorge Void checker (wolf)":"Gorge Void checker (human)",gorge::warpToPosition,gorge::initState);
}
bool gorgePracticeEnabled(){return gorgeChoice()!=0;}
ModResult initCheckers(){
 ConfigVarDesc config=CONFIG_VAR_DESC_INIT;config.name="gorge_void";config.type=CONFIG_VAR_INT;
 auto r=svc_config->register_var(mod_ctx,&config,&gorgeMode);if(r!=MOD_OK)return r;
 auto& gorgeControl=choice("gorge_void","Checkers","gorge void",{"off","wolf","human"},[](){return gorgeChoice();},[](int64_t v){svc_config->set_int(mod_ctx,gorgeMode,v);});
 gorgeControl.gameOnly=false;gorgeControl.help="L + Z loads GZ's Gorge Void checker setup; jump-attack and boots timing feedback.";

 toggle("coro_td","Checkers","Coro TD","Original ten-frame item timing feedback.");
 toggle("ebmb","Checkers","Ending blow moon boots","Original fourth-frame iron-boots release feedback.");
 toggle("elevator_escape","Checkers","Elevator escape","Original Goron elder room transformation/roll timing.");
 toggle("lfc","Checkers","Ladder freezard cancel","Original courtyard actor check, ladder drop heights and regrab windows.");
 toggle("mash_checker","Checkers","A/B mash rate","Button presses per second with GZ's color thresholds.");
 toggle("rolling","Checkers","Rolling","Original roll-chain windows, including land-dive rolls and pause handling.");
 toggle("umd","Checkers","Universal map delay","Original map-opening A/B timing, checked after the native game loop.");
 toggle("fast_eel_regrab","Checkers","Fast eel regrab","Original clawshot-to-boots first, second and third frame feedback.");
 r=guardedPre<GorgeBeforeLink>([](ModContext*,void* args,void*,void*){
  if(gorgeChoice()&&mods::arg<daAlink_c*>(args,0)==daAlink_getAlinkActorClass()){
   const auto rawButtons=buttons;
   if(checkerInputBlocked)buttons=0;
   gorge::execute();buttons=rawButtons;
  }
  return HOOK_CONTINUE;
 });
 if(r!=MOD_OK)return r;
 return guardedPost<CheckerPostLoop>([](ModContext*,void*,void*,void*){
  const auto rawButtons=buttons;
  if(checkerInputBlocked)buttons=0;
  if(on("umd")&&g_meter2_info.mMenuWindowClass)umd::execute();
  buttons=rawButtons;
 });
}
bool checkerOverlaysVisible(){
 if(on("mash_checker"))return true;
 for(const auto& message:Queue)if(message.ttl)return true;
 return false;
}
void drawCheckers(){
 if(on("mash_checker")){
  const auto p=spritePosition(Mash);char text[16];
  std::snprintf(text,sizeof(text),"A: %d",int(bpsA));drawGzText(text,p.x,p.y,speedColor(bpsA));
  std::snprintf(text,sizeof(text),"B: %d",int(bpsB));drawGzText(text,p.x,p.y+20,speedColor(bpsB));
 }
 if(gzMenuOpen())return;
 const auto p=spritePosition(Fifo);
 for(size_t i=0;i<Queue.size();++i){
  const auto& message=Queue[i];if(!message.ttl)continue;
  const uint32_t alpha=message.ttl<30?uint32_t(message.ttl*8.5):255;
  drawGzTextPlain(message.text,p.x,p.y-float(i)*14,message.rgba|alpha,17);
 }
}
}
