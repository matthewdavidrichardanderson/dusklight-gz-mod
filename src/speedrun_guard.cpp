#include "core.hpp"
#include "dusk/game_mode.hpp"
namespace gz {
void resyncLoading();
static bool blocked=true;
static dusk::gamemode::GameModeManager* gameModeManager=nullptr;
#if defined(_WIN32)
static constexpr const char* gameModeManagerSymbol=
 "?g_GameModeManager@gamemode@dusk@@3VGameModeManager@12@A";
#else
static constexpr const char* gameModeManagerSymbol="dusk::gamemode::g_GameModeManager";
#endif
bool speedrunBlocked(){return blocked;}
static void transition(bool next){
 if(next==blocked)return;
 blocked=next;
 if(blocked)suspendGz();else resyncLoading();
 svc_log->info(mod_ctx,blocked?"GZ suspended for Speedrun Mode":"GZ resumed after Speedrun Mode");
}
void speedrunTick(){
 if(gameModeManager)transition(gameModeManager->isCurrentGameMode("vanilla_speedrun"));
}
DEFINE_HOOK_SYMBOL("dusk::gamemode::GameModeManager::setCurrentGameMode",bool(void*,const std::string*),SpeedrunModeChanged);
ModResult initSpeedrunGuard(){
 void* address=nullptr;HookSymbolFlags flags{};
 auto r=svc_hook->resolve(mod_ctx,gameModeManagerSymbol,&address,&flags);
 if(r!=MOD_OK||!address||!(flags&HOOK_SYMBOL_DATA))return MOD_UNAVAILABLE;
 gameModeManager=static_cast<dusk::gamemode::GameModeManager*>(address);
 blocked=gameModeManager->isCurrentGameMode("vanilla_speedrun");
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
