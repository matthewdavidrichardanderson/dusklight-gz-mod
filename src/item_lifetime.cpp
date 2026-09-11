#include "core.hpp"
#include "d/d_a_item_static.h"
namespace gz {
DEFINE_HOOK(&daItem_c::CreateInit, ItemCreateInit);
ModResult initItemLifetime() {
 // TPGZ changes daItemBase_c::m_data.mWaitTime before CreateInit copies it to
 // the actor. The host data table is const, so apply the same resulting value
 // immediately after that copy. Existing drops continue counting normally,
 // including after the cheat is switched off, exactly as in TPGZ.
 return guardedPost<ItemCreateInit>([](ModContext*,void* args,void*,void*) {
  auto* item=mods::arg<daItem_c*>(args,0);
  if(item&&on("disable_item_timer"))item->mWaitTimer=0x7fff;
 });
}
}
