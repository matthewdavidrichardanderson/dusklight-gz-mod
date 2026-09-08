#pragma once
#include "core.hpp"
#include "dusk/config_var.hpp"
#include <string_view>
// Read-only native CVar access; no dependency on UserSettings member offsets.
namespace gz {
static dusk::config::ConfigVar<bool>* movementSettings[4]{};
static dusk::config::ConfigVarBase* (*findHostSetting)(std::string_view)=nullptr;
bool movementSetting(unsigned index){return movementSettings[index]->getValue();}
static ModResult initMovementSettings(){
 void* address=nullptr;HookSymbolFlags flags{};
 auto r=svc_hook->resolve(mod_ctx,"dusk::config::GetConfigVar",&address,&flags);
 if(r!=MOD_OK||!address||!(flags&HOOK_SYMBOL_CODE))return MOD_UNAVAILABLE;
 findHostSetting=reinterpret_cast<decltype(findHostSetting)>(address);
 const char* names[]={"game.fastRoll","game.enableFastIronBoots","game.invertAirSwimX","game.invertAirSwimY"};
 for(unsigned i=0;i<4;i++){
  movementSettings[i]=dynamic_cast<dusk::config::ConfigVar<bool>*>(findHostSetting(names[i]));
  if(!movementSettings[i]){
   svc_log->error(mod_ctx,"Native movement settings differ from this upstream build.");
   return MOD_UNSUPPORTED;
  }
 }
 return MOD_OK;
}

}
