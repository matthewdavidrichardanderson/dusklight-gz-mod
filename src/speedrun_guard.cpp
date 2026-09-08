#include "core.hpp"
namespace gz {
void resyncLoading();
static bool blocked=true;
static bool (*nativeSpeedrunActive)()=nullptr;
bool speedrunBlocked(){return blocked;}
static void transition(bool next){
 if(next==blocked)return;
 blocked=next;
 if(blocked)suspendGz();else resyncLoading();
 svc_log->info(mod_ctx,blocked?"GZ suspended for Speedrun Mode":"GZ resumed after Speedrun Mode");
}
void speedrunTick(){if(nativeSpeedrunActive)transition(nativeSpeedrunActive());}
DEFINE_HOOK_SYMBOL("dusk::gamemode::GameModeManager::setCurrentGameMode",bool(void*,const std::string*),SpeedrunModeChanged);
ModResult initSpeedrunGuard(){
 void* address=nullptr;HookSymbolFlags flags{};
 auto r=svc_hook->resolve(mod_ctx,"dusk::speedrun::isActive",&address,&flags);
 if(r!=MOD_OK||!address||!(flags&HOOK_SYMBOL_CODE))return MOD_UNAVAILABLE;
 nativeSpeedrunActive=reinterpret_cast<bool(*)()>(address);
 blocked=nativeSpeedrunActive();
 // Guard entry before built-in activation/reset callbacks run. These two
 // observers deliberately remain live while all GZ feature hooks are gated.
 r=mods::hook::add_pre<SpeedrunModeChanged>([](ModContext*,void* args,void*,void*){
  const auto& id=*mods::arg<const std::string*>(args,1);
  if(id=="vanilla_speedrun")transition(true);
  return HOOK_CONTINUE;
 });
 if(r!=MOD_OK)return r;
 return mods::hook::add_post<SpeedrunModeChanged>([](ModContext*,void*,void*,void*){speedrunTick();});
}
}
