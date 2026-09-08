#include "core.hpp"
#include "d/d_a_item_static.h"
#include <vector>
namespace gz {
DEFINE_HOOK(&daItem_c::_daItem_execute, ItemExecute);
struct Lifetime {daItem_c* item; s16 wait; s16 disappear; bool frozen;};
static std::vector<Lifetime> frames;
ModResult initItemLifetime() {
 auto result=guardedPre<ItemExecute>([](ModContext*,void* args,void*,void*) {
  auto* item=mods::arg<daItem_c*>(args,0);
  bool frozen=item&&on("disable_item_timer");
  frames.push_back({item,item?item->mWaitTimer:s16(0),item?item->mDisappearTimer:s16(0),frozen});
  if(frozen)item->mWaitTimer=0x7fff;
  return HOOK_CONTINUE;
 });
 if(result!=MOD_OK)return result;
 return guardedPost<ItemExecute>([](ModContext*,void*,void*,void*) {
  if(frames.empty())return;
  auto f=frames.back();frames.pop_back();
  // Actor deletion is queued; the object is still alive at execute return.
  if(f.frozen){f.item->mWaitTimer=f.wait;f.item->mDisappearTimer=f.disappear;}
 });
}
}
