// Original GZ seven tunic color choices applied only during native Link drawing.
#include "core.hpp"
#include "d/actor/d_a_alink.h"
#include "SSystem/SComponent/c_counter.h"
#include <array>
#include <algorithm>
namespace gz {
DEFINE_HOOK_SYMBOL("daAlink_c::draw",int(daAlink_c*),TunicDraw);
static ConfigVarHandle tunicVar=0;
static std::array<std::array<s16,3>,2> saved;
static bool applied=false;
static int cycleR=0,cycleG=0,cycleB=0;
static uint32_t cycleFrame=~0u;
static int colorChoice(){int64_t v=0;svc_config->get_int(mod_ctx,tunicVar,&v);return int(std::clamp<int64_t>(v,0,6));}
ModResult initTunic(){
 ConfigVarDesc desc=CONFIG_VAR_DESC_INIT;desc.name="tunic_color";desc.type=CONFIG_VAR_INT;
 auto result=svc_config->register_var(mod_ctx,&desc,&tunicVar);if(result!=MOD_OK)return result;
 choice("tunic_color","Tools","Link tunic color",{"green","blue","red","orange","yellow","white","cycle"},
  [](){return colorChoice();},[](int64_t v){svc_config->set_int(mod_ctx,tunicVar,v);}).help="Changes Link\'s tunic color. X/Y to cycle through colors.";
 result=guardedPre<TunicDraw>([](ModContext*,void* args,void*,void*){
  const int selected=colorChoice();applied=false;if(!selected)return HOOK_CONTINUE;
  auto* link=mods::arg<daAlink_c*>(args,0);if(!link)return HOOK_CONTINUE;
  constexpr int colors[][3]={{16,16,16},{0,8,32},{24,0,0},{32,16,0},{32,32,0},{32,28,32}};
  int r,g,b;
  if(selected==6){
   if(cycleFrame!=g_Counter.mCounter0){
    cycleFrame=g_Counter.mCounter0;
    if(cycleR<16&&cycleG==0&&cycleB==0)++cycleR;
    else if(cycleG<16&&cycleB==0&&cycleR==16)++cycleG;
    else if(cycleB<16&&cycleG==16&&cycleR==16)++cycleB;
    else if(cycleR>0&&cycleG==16&&cycleB==16)--cycleR;
    else if(cycleG>0&&cycleB==16&&cycleR==0)--cycleG;
    else --cycleB;
   }
   r=cycleR;g=cycleG;b=cycleB;
  }else{r=colors[selected][0];g=colors[selected][1];b=colors[selected][2];}
  for(unsigned i=0;i<2;i++){
   auto& color=link->field_0x32a0[i];saved[i]={color.r,color.g,color.b};
   color.r=r-16;color.g=g-16;color.b=b-16;
  }
  applied=true;return HOOK_CONTINUE;
 });if(result!=MOD_OK)return result;
 return guardedPost<TunicDraw>([](ModContext*,void* args,void*,void*){
  if(!applied)return;auto* link=mods::arg<daAlink_c*>(args,0);
  for(unsigned i=0;i<2;i++){auto& color=link->field_0x32a0[i];color.r=saved[i][0];color.g=saved[i][1];color.b=saved[i][2];}
  applied=false;
 });
}
}
