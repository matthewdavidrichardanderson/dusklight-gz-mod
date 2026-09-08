// GZ free-camera motion with Dusk camera submission and native event control.
#include "core.hpp"
#include "link_tools.hpp"
#include "camera_logic.hpp"
#include "mods/svc/camera.h"
#include "d/d_com_inf_game.h"
#include "d/d_camera.h"
#include "d/actor/d_a_player.h"
#include "m_Do/m_Do_controller_pad.h"
namespace gz {
bool gzMenuOpen();
static bool active=false,leased=false;
static u8 savedEventStatus=0;
static fpc_ProcID owner=~fpc_ProcID(0);
static CameraOperatorHandle cameraHandle=0;
static FreeCamera camera;
bool freeCameraPosition(cXyz& out){if(!active)return false;out.set(float(camera.eye[0]),float(camera.eye[1]),float(camera.eye[2]));return true;}
static void releaseEvent(){
 auto* player=daPy_getPlayerActorClass();
 if(leased&&player&&fopAcM_GetID(player)==owner&&dComIfGp_getEvent()->mEventStatus==1)
  dComIfGp_getEvent()->mEventStatus=savedEventStatus;
 leased=false;
}
void toggleFreeCamera(){
 if(!playable())return;
 if(active){active=false;releaseEvent();notify("Free camera disabled");return;}
 auto* body=dCam_getBody();if(!body)return;
 shutdownMoveLink();
 auto eye=body->iEye(),center=body->iCenter();
 camera.initialize({eye.x,eye.y,eye.z},{center.x,center.y,center.z});
 owner=fopAcM_GetID(daPy_getPlayerActorClass());
 savedEventStatus=dComIfGp_getEvent()->mEventStatus;
 leased=true;active=true;notify("Free camera enabled");
}
void cameraTick(){
 auto* player=daPy_getPlayerActorClass();
 if(!playable()||!on("free_cam")||!player||fopAcM_GetID(player)!=owner){
  active=false;releaseEvent();return;
 }
 if(active)dComIfGp_getEvent()->mEventStatus=1; // GZ's mHalt is this decomp field.
}
ModResult initCamera(){
 toggle("free_cam","Tools","Free camera hotkey","Z + B + A toggles. Main stick moves, C-stick looks, L/R move vertically; Z speeds up.");
 action("toggle_free_camera","Tools","Toggle free camera",toggleFreeCamera);
 CameraOperatorDesc desc=CAMERA_OPERATOR_DESC_INIT;
 desc.debug_name="TPGZ free camera";
 desc.operate=[](ModContext*,CameraOperatorState* state,void*){
  if(!active||!playable())return false;
  bool visible=false;svc_ui->is_any_document_visible(mod_ctx,&visible);
  if(!visible&&!gzMenuOpen())if(auto* pad=mDoCPd_c::getGamePad(0)){
   const auto& buttons=pad->mButton;
   const auto left=buttons.mAnalogL>=10?buttons.mAnalogL:0;
   const auto right=buttons.mAnalogR>=10?buttons.mAnalogR:0;
   camera.update(pad->mMainStick.mRawY,pad->mMainStick.mRawX,left-right,
                 pad->mSubStick.mRawY,pad->mSubStick.mRawX,(buttons.mButton&PAD_TRIGGER_Z)!=0);
  }
  for(int i=0;i<3;i++){state->eye[i]=static_cast<float>(camera.eye[i]);state->center[i]=static_cast<float>(camera.center[i]);}
  // Keep the host's fovy, aspect and bank values.
  return true;
 };
 return svc_camera->register_camera_operator(mod_ctx,&desc,&cameraHandle);
}
void shutdownCamera(){active=false;releaseEvent();}
}
