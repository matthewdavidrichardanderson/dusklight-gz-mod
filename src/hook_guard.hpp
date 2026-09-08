#pragma once
// Requires the SDK hook declarations. Keep feature callbacks inert while suspended.
namespace gz {
bool speedrunBlocked();
template<class Entry> ModResult guardedPre(HookPreFn callback){
 static HookPreFn fn=nullptr;fn=callback;
 return mods::hook::add_pre<Entry>([](ModContext* c,void* a,void* v,void* u){
  return speedrunBlocked()?HOOK_CONTINUE:fn(c,a,v,u);
 });
}
template<class Entry> ModResult guardedPost(HookPostFn callback){
 static HookPostFn fn=nullptr;fn=callback;
 return mods::hook::add_post<Entry>([](ModContext* c,void* a,void* v,void* u){
  if(!speedrunBlocked())fn(c,a,v,u);
 });
}
}
