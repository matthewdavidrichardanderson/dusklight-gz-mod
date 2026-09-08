// GZ loading flag semantics via native lifecycle hooks; no private data-symbol dependency.
#include "loading.hpp"
#include "actor_tools.hpp"
#include "loading_logic.hpp"
#include "link_tools.hpp"
#include "f_op/f_op_scene_req.h"
#include "f_op/f_op_overlap_mng.h"
#include "SSystem/SComponent/c_phase.h"
namespace gz {
DEFINE_HOOK(&fopScnRq_Request,SceneRequested);
DEFINE_HOOK_SYMBOL("fopScnRq_phase_Done",cPhs_Step(scene_request_class*),SceneFinished);
static SceneLoadState state;
bool sceneLoading(){return state.active;}
void resyncTimerClock();
void resyncLoading(){state.active=fopOvlpM_IsDoingReq()!=0;resyncTimerClock();}
ModResult initLoading(){
 state.active=fopOvlpM_IsDoingReq()!=0;
 auto r=guardedPost<SceneRequested>([](ModContext*,void* args,void* result,void*){
  state.requested(*static_cast<fpc_ProcID*>(result),mods::arg<s16>(args,4));
  // Release tool-owned actor and event state before scene teardown.
  if(state.active){shutdownActorView();shutdownMoveLink();shutdownCamera();}
 });
 if(r!=MOD_OK)return r;
 return guardedPost<SceneFinished>([](ModContext*,void*,void*,void*){state.completed();});
}
}
