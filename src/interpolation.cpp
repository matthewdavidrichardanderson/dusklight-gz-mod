#include "core.hpp"
#include "interpolation.hpp"
#include "dusk/game_clock.h"
namespace gz {
namespace {
using BoolQuery=bool(*)();
using StepQuery=float(*)();
using SequenceQuery=uint64_t(*)();
BoolQuery enabled=nullptr;
BoolQuery capture=nullptr;
BoolQuery presentation=nullptr;
StepQuery step=nullptr;
SequenceQuery sequence=nullptr;
const dusk::game_clock::FrameTiming* timing=nullptr;
template<class T> ModResult resolveCode(const char* name,T& out){
 void* address=nullptr;HookSymbolFlags flags{};
 const auto result=svc_hook->resolve(mod_ctx,name,&address,&flags);
 if(result!=MOD_OK||!address||!(flags&HOOK_SYMBOL_CODE))return MOD_UNAVAILABLE;
 out=reinterpret_cast<T>(address);return MOD_OK;
}
}
bool InterpolationClock::should_capture(){return capture&&capture();}
bool InterpolationClock::is_enabled(){return enabled&&enabled();}
bool InterpolationClock::is_presentation_active(){return presentation&&presentation();}
uint64_t InterpolationClock::sim_tick_seq(){return sequence?sequence():0;}
uint64_t InterpolationClock::presentation_epoch(){return timing?timing->presentationEpoch:0;}
float InterpolationClock::get_interpolation_step(){return step?step():1.0f;}
ModResult initInterpolation(){
 auto result=resolveCode("dusk::interp::is_enabled",enabled);if(result!=MOD_OK)return result;
 result=resolveCode("dusk::interp::should_capture",capture);if(result!=MOD_OK)return result;
 result=resolveCode("dusk::interp::is_presentation_active",presentation);if(result!=MOD_OK)return result;
 result=resolveCode("dusk::interp::get_interpolation_step",step);if(result!=MOD_OK)return result;
 result=resolveCode("dusk::interp::sim_tick_seq",sequence);if(result!=MOD_OK)return result;
 void* address=nullptr;HookSymbolFlags flags{};
#if defined(_MSC_VER)
 constexpr auto timingSymbol="?g_frameTiming@game_clock@dusk@@3UFrameTiming@12@A";
#else
 constexpr auto timingSymbol="_ZN4dusk10game_clock13g_frameTimingE";
#endif
 result=svc_hook->resolve(mod_ctx,timingSymbol,&address,&flags);
 if(result!=MOD_OK||!address||!(flags&HOOK_SYMBOL_DATA))return MOD_UNAVAILABLE;
 timing=static_cast<const dusk::game_clock::FrameTiming*>(address);
 return MOD_OK;
}
}
