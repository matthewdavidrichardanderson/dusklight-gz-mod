#include "core.hpp"
#include "loading.hpp"
#include "f_op/f_op_overlap_req.h"
#include "f_op/f_op_overlap_mng.h"
#include <dolphin/os.h>
#include "gz_font.hpp"
#include "foreground.hpp"
#include "presentation.hpp"
#include "timer_logic.hpp"
#include "m_Do/m_Do_ext.h"
#include "JSystem/JKernel/JKRExpHeap.h"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_player.h"
#include "m_Do/m_Do_graphic.h"
#include "m_Do/m_Do_controller_pad.h"
#include "SSystem/SComponent/c_counter.h"
#include <chrono>
#include <cstdio>
namespace gz {int framePauseVisual();void sampleTransformIndicator();void drawTransformIndicator();bool checkerOverlaysVisible();void drawCheckers();void drawDisplacement();void drawLinkDebug();void drawStageInfo();void sampleInputViewer();void drawInputViewer(float,float);
static PracticeTimers timers;
static bool timerLoading=false;
static OSTime previousGameTime=0;


static uint32_t previousFrame=0;
static std::chrono::steady_clock::time_point previousTime;
static void sampleTimers(){
 const auto now=std::chrono::steady_clock::now();
 const auto frame=g_Counter.mCounter0;
 const auto gameTime=OSGetTime();
 if(previousTime.time_since_epoch().count()){
  timers.advance(std::chrono::duration<double>(now-previousTime).count(),uint32_t(frame-previousFrame),gameTime-previousGameTime,timerLoading,on("load_timer"),on("timer"),on("igt_timer"));
 }
 previousTime=now;previousFrame=frame;previousGameTime=gameTime;
}
void overlayTick(){
 sampleTimers();
 sampleInputViewer();sampleTransformIndicator();
}
void toggleTimer(){sampleTimers();timers.running=!timers.running;}
void resyncTimerClock(){
 previousTime=std::chrono::steady_clock::now();previousFrame=g_Counter.mCounter0;previousGameTime=OSGetTime();
 timerLoading=fopOvlpM_IsDoingReq()!=0;
}
void resetTimer(){timers.reset();resyncTimerClock();}
DEFINE_HOOK_SYMBOL("fopOvlpReq_phase_Create",int(overlap_request_class*),TimerLoadStart);
DEFINE_HOOK_SYMBOL("fopOvlpReq_phase_Done",int(overlap_request_class*),TimerLoadEnd);
ModResult installTimerHooks(){
 resyncTimerClock();
 auto r=guardedPost<TimerLoadStart>([](ModContext*,void*,void*,void*){
  sampleTimers();timerLoading=true;
 });if(r!=MOD_OK)return r;
 return guardedPost<TimerLoadEnd>([](ModContext*,void*,void* result,void*){
  // Deletion can take multiple attempts; Dusk resumes only on success.
  if(*static_cast<int*>(result)!=cPhs_NEXT_e)return;
  sampleTimers();timerLoading=false;sampleTimers();
 });
}
void initOverlays(){
 toggle("heap_debug","Tools","Heap debug info","Show native Zelda, Game and Archive heap free / total free sizes.");
 toggle("transform_indicator","Tools","Transform indicator","Shows the destination form; dimmed when native Midna transformation conditions are not met.");
 toggle("igt_timer","Tools","IGT timer","Dusk game-clock timing, excluding overlap/fade loads. Independent Z+A / Z+B controls.");
 toggle("load_timer","Tools","Load timer","Accumulates overlap/fade loads using the Dusk game clock; Z+B resets all GZ timers.");
 toggle("timer","Tools","Timer","RTA stopwatch and logical frame count. Z + A starts/stops; Z + B resets.");
 toggle("input_viewer","Tools","Input viewer","Original GZ controller diagram, analog sticks and triggers, and raw axis values.");
 toggle("stage_info","Tools","Stage info","Current and saved stage, room, spawn point and layer.");
 toggle("displacement","Tools","Displacement","Angle and distance from the position stored with R + Up, shared with teleport.");
 toggle("link_debug","Tools","Link debug information","Original GZ time, angle, look angle, speed and position; advanced action, slope, acch and demo fields.");
 action("toggle_timer","Tools","Start / stop timer",toggleTimer);
 action("reset_timer","Tools","Reset timer",resetTimer);
}
bool overlaysVisible(){return (on("frame_advance")&&framePauseVisual())||on("heap_debug")||on("transform_indicator")||checkerOverlaysVisible()||on("timer")||on("igt_timer")||on("load_timer")||on("input_viewer")||on("link_debug")||on("stage_info")||on("displacement");}
void drawOverlays(){
 drawCheckers();drawTransformIndicator();
 if(on("frame_advance"))if(const int mode=framePauseVisual()){
  const auto texture=foregroundResourceTexture(mode==1?"tex/framePause.tex":"tex/framePlay.tex");
  if(texture)foregroundQuad(texture,550,5,582,37,0,0,1,1,0xffffffff);
 }
 char line[128];
 auto timerText=[&](uint64_t ms,Sprite sprite,float dy=0.f){
  std::snprintf(line,sizeof(line),"%02llu:%02llu:%02llu.%03llu",ms/3600000,(ms/60000)%60,(ms/1000)%60,ms%1000);
  const auto p=spritePosition(sprite);drawGzText(line,p.x,p.y+dy);
 };
 if(on("heap_debug")){
  JKRHeap* heaps[]={mDoExt_getZeldaHeap(),mDoExt_getGameHeap(),mDoExt_getArchiveHeap()};
  if(heaps[0]&&heaps[1]&&heaps[2]){
   const auto pos=spritePosition(Heap);
   drawGzText("-- Heap Free / Total Free (KB) --",pos.x,pos.y);
   const char* labels[]={"  Zelda","   Game","Archive"};
   for(unsigned i=0;i<3;i++){
    std::snprintf(line,sizeof(line),"%s %5u / %5u",labels[i],unsigned(heaps[i]->getFreeSize())>>10,unsigned(heaps[i]->getTotalFreeSize())>>10);
    drawGzText(line,pos.x+55,pos.y+20*(i+1));
   }
  }
 }
 if(on("timer")){const auto p=spritePosition(Timer);drawGzText(std::to_string(timers.frames),p.x,p.y);timerText(uint64_t(timers.rta*1000),Timer,15);}
 if(on("igt_timer"))timerText(OSTicksToMilliseconds(timers.igt),IgtTimer);
 if(on("load_timer"))timerText(OSTicksToMilliseconds(timers.loads),LoadTimer);
 if(on("displacement"))drawDisplacement();
 if(on("link_debug"))drawLinkDebug();
 if(on("stage_info"))drawStageInfo();
 if(on("input_viewer")){const auto p=spritePosition(InputViewer);drawInputViewer(p.x,p.y);}
}
}
