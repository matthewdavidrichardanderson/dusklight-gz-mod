// Original GZ icons and tint; transformation eligibility comes from native Midna logic.
#include "core.hpp"
#include "foreground.hpp"
#include "presentation.hpp"
#include "loading.hpp"
#include "mods/svc/resource.h"
#include "d/actor/d_a_alink.h"
#include "d/actor/d_a_midna.h"
#include "d/d_com_inf_game.h"
#include <cstring>
namespace gz {
static uint64_t humanIcon=0,wolfIcon=0;
static bool attempted=false,visible=false,allowed=false,wolf=false;
void sampleTransformIndicator(){
 visible=false;
 if(!on("transform_indicator")||!playable()||sceneLoading())return;
 auto* player=daAlink_getAlinkActorClass();
 auto* midna=daPy_py_c::getMidnaActor();
 if(!player||!midna)return;
 visible=true;wolf=player->checkWolf();
 // Includes Midna riding, shadow crystal, twilight and the native NPC search.
 // It also retains upstream's transform-anywhere NPC policy.
 allowed=midna->checkMetamorphoseEnableBase()!=0;
}
static uint64_t loadIcon(const char* name){
 ResourceBuffer buffer=RESOURCE_BUFFER_INIT;
 if(svc_resource->load(mod_ctx,name,&buffer)!=MOD_OK)return 0;
 const auto* bytes=static_cast<const unsigned char*>(buffer.data);
 uint64_t icon=0;
 auto read=[&](unsigned i){return (uint32_t(bytes[i])<<24)|(uint32_t(bytes[i+1])<<16)|(uint32_t(bytes[i+2])<<8)|bytes[i+3];};
 if(buffer.size==1040&&std::memcmp(bytes,"TEX0",4)==0&&read(4)==0&&read(8)==16&&read(12)==16)
  icon=foregroundConsoleTexture(16,16,bytes+16);
 svc_resource->free(mod_ctx,&buffer);return icon;
}
void drawTransformIndicator(){
 if(!visible)return;
 if(!attempted){
  attempted=true;humanIcon=loadIcon("tex/hand.tex");wolfIcon=loadIcon("tex/wolf.tex");
  if(!humanIcon||!wolfIcon)svc_log->error(mod_ctx,"GZ transformation indicator icon could not be loaded.");
 }
 const auto icon=wolf?humanIcon:wolfIcon;if(!icon)return;
 const auto pos=spritePosition(Transform);
 foregroundQuad(icon,pos.x,pos.y,pos.x+30,pos.y+30,0,0,1,1,allowed?0xffffffff:0x3f3f3f7f);
}
void shutdownTransformIndicator(){humanIcon=wolfIcon=0;attempted=visible=allowed=wolf=false;}
}
