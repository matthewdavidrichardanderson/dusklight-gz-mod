// Exercise the production wrappers against a minimal SDK registration harness.
using ModResult=int;
struct ModContext{};
enum HookAction{HOOK_CONTINUE,HOOK_SKIP};
using HookPreFn=HookAction(*)(ModContext*,void*,void*,void*);
using HookPostFn=void(*)(ModContext*,void*,void*,void*);
namespace mods::hook {
template<class T> HookPreFn pre=nullptr;
template<class T> HookPostFn post=nullptr;
template<class T> ModResult add_pre(HookPreFn fn){pre<T> =fn;return 0;}
template<class T> ModResult add_post(HookPostFn fn){post<T> =fn;return 0;}
}
#include "hook_guard.hpp"
static bool blocked=true;
bool gz::speedrunBlocked(){return blocked;}
struct Feature{};struct Another{};
static int calls=0;
int main(){
 gz::guardedPre<Feature>([](ModContext*,void* a,void*,void*){++calls;++*static_cast<int*>(a);return HOOK_SKIP;});
 gz::guardedPost<Feature>([](ModContext*,void* a,void*,void*){++calls;++*static_cast<int*>(a);});
 gz::guardedPre<Another>([](ModContext*,void*,void*,void*){calls+=10;return HOOK_CONTINUE;});
 int value=0;
 // Starting in speedrun must pass through without mutations.
 if(mods::hook::pre<Feature>(nullptr,&value,nullptr,nullptr)!=HOOK_CONTINUE)return 1;
 mods::hook::post<Feature>(nullptr,&value,nullptr,nullptr);
 if(calls||value)return 2;
 for(int cycle=0;cycle<3;cycle++){
  blocked=false;
  if(mods::hook::pre<Feature>(nullptr,&value,nullptr,nullptr)!=HOOK_SKIP)return 3;
  mods::hook::post<Feature>(nullptr,&value,nullptr,nullptr);
  mods::hook::pre<Another>(nullptr,nullptr,nullptr,nullptr);
  if(calls!=(cycle+1)*12||value!=(cycle+1)*2)return 4;
  blocked=true;
  mods::hook::pre<Feature>(nullptr,&value,nullptr,nullptr);
  mods::hook::post<Feature>(nullptr,&value,nullptr,nullptr);
  mods::hook::pre<Another>(nullptr,nullptr,nullptr,nullptr);
  if(calls!=(cycle+1)*12||value!=(cycle+1)*2)return 5;
 }
}
