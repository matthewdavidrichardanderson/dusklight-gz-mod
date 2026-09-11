// GZ freezes the main Wichmann-Hill stream before each process execution.
#include "core.hpp"
#include "rng_logic.hpp"
#include "gz_font.hpp"
#include "presentation.hpp"
#include <cstdio>
#include "SSystem/SComponent/c_math.h"
#include "f_pc/f_pc_base.h"
namespace gz {
DEFINE_HOOK(&fpcBs_Execute, RngProcess);
static std::array<const s32*,3> values{};
static RngState frozen{};
static bool observedFreeze=false,available=false;
static int selectedPreset=0;
void resetRngForSpeedrun(){observedFreeze=false;}
static RngState readState(){return {*values[0],*values[1],*values[2]};}
static void writeState(RngState state){cM_initRnd(state[0],state[1],state[2]);frozen=state;}
ModResult initRng() {
 const auto first=controls.size();
 auto& freeze=toggle("freeze_rng","RNG","Freeze RNG values","Restores the selected three RNG seeds before each process, matching GZ.");
 for(int i=0;i<3;i++){
  const char* names[]={"r0","r1","r2"};void* address=nullptr;HookSymbolFlags flags{};
  auto result=svc_hook->resolve(mod_ctx,names[i],&address,&flags);
  if(result==MOD_OK&&address&&(flags&HOOK_SYMBOL_DATA))values[i]=static_cast<const s32*>(address);
 }
 available=values[0]&&values[1]&&values[2];
 const auto setFreeze=freeze.set;
 freeze.set=[setFreeze](int64_t v){
  if(!available)return;
  if(v)frozen=readState();observedFreeze=v!=0;setFreeze(v);
 };
 for(int i=0;i<3;i++){
  const auto id="rng_seed_"+std::to_string(i);const auto label="Seed "+std::to_string(i);
  constexpr int maxima[]={30268,30306,30322};
  number(id.c_str(),"RNG",label.c_str(),0,maxima[i],[i](){return available?*values[i]:0;},
   [i](int64_t v){if(available){auto state=readState();state[i]=static_cast<int32_t>(v);writeState(state);}});
 }
 action("advance_rng","RNG","Advance one RNG iteration",[](){if(available)writeState(advanceRng(readState()));});
 struct Preset{const char* label;RngState state;};
 static constexpr Preset presets[]={
  {"zant head 1st platform",{4134,7345,3379}},{"zant head 3rd platform",{25170,3588,2141}},
  {"zant head back left",{4995,3011,718}},{"zant head back mid",{12361,7069,29087}},
  {"zant head back right",{10870,5151,9268}},{"horseback A (middle)",{5872,10996,94}},
  {"horseback B (down)",{2793,21116,3179}},{"horseback C (up)",{29379,13886,25014}},
  {"kb1 left after 3rd hit",{5872,10996,94}},
  {"kb1 right after 3rd hit",{25170,3588,2141}}
 };
 std::vector<const char*> labels;for(const auto& preset:presets)labels.push_back(preset.label);
 choice("rng_preset","RNG","Preset",labels,[](){return selectedPreset;},[](int64_t v){selectedPreset=static_cast<int>(v);});
 action("load_rng_preset","RNG","Load preset and freeze",[&freeze](){
  if(!available)return;writeState(presets[selectedPreset].state);freeze.set(1);
 });
 if(!available){
  for(size_t i=first;i<controls.size();i++)controls[i].reason="The current Dusklight symbol manifest could not resolve all three native RNG data symbols.";
  return MOD_OK;
 }
 return guardedPre<RngProcess>([](ModContext*,void*,void*,void*){
  const bool enabled=on("freeze_rng");
  if(enabled){if(!observedFreeze)frozen=readState();cM_initRnd(frozen[0],frozen[1],frozen[2]);}
  observedFreeze=enabled;return HOOK_CONTINUE;
 });
}
void drawRngValues(){
 const auto p=spritePosition(Menu);char text[80];
 for(int i=0;i<3;i++){
  if(available)std::snprintf(text,sizeof(text),"r%d: %d",i,*values[i]);
  else std::snprintf(text,sizeof(text),"r%d: unavailable",i);
  drawGzText(text,p.x+220,p.y+i*20,0xfffba6ff);
 }
}

}
