// GZ flag logging through native event/switch operations.
#include "core.hpp"
#include "d/d_com_inf_game.h"
#include <cstdio>
namespace gz {
void pushGzMessage(const char*,uint32_t);
DEFINE_HOOK(&dSv_event_c::onEventBit,FlagEventOn);
DEFINE_HOOK(&dSv_event_c::offEventBit,FlagEventOff);
DEFINE_HOOK(&dSv_info_c::onSwitch,FlagSwitchOn);
namespace {
void eventMessage(void* args,bool enabled){
 if(!on("flag_log"))return;
 auto* record=mods::arg<dSv_event_c*>(args,0);
 const auto flag=mods::arg<u16>(args,1);char message[80];
 std::snprintf(message,sizeof(message),"%s[0x%X] : %X | %s",
  record==&g_dComIfG_gameInfo.info.mTmp?"Event Tmp":"Event",flag>>8,flag&255,enabled?"ON":"OFF");
 pushGzMessage(message,0xffffff00);
}
}
ModResult initFlagLog(){
 toggle("flag_log","Flag log","log activated","Show native event and switch operations in the GZ FIFO and mod log.");
 auto r=guardedPre<FlagEventOn>([](ModContext*,void* args,void*,void*){eventMessage(args,true);return HOOK_CONTINUE;});
 if(r!=MOD_OK)return r;
 r=guardedPre<FlagEventOff>([](ModContext*,void* args,void*,void*){eventMessage(args,false);return HOOK_CONTINUE;});
 if(r!=MOD_OK)return r;
 return guardedPre<FlagSwitchOn>([](ModContext*,void* args,void*,void*){
  if(on("flag_log")){
   int flag=mods::arg<int>(args,1);
   const char* label=flag<0x80?"Memory Switch":flag<0xc0?"Dan Switch":flag<0xe0?"Zone Switch":"Zone OneSwitch";
   if(flag>=0xe0)flag-=0xe0;else if(flag>=0xc0)flag-=0xc0;else if(flag>=0x80)flag-=0x80;
   char message[80];std::snprintf(message,sizeof(message),"%s[%d] : %d | ON",label,flag>>5,flag&31);
   pushGzMessage(message,0xffffff00);
  }
  return HOOK_CONTINUE;
 });
}
}
