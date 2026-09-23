#include "core.hpp"
#include "presentation.hpp"
#include "loading.hpp"
#include "link_tools.hpp"
#include "mods/svc/camera.h"
#include "mods/svc/resource.h"
#include "mods/svc/gfx.h"
#include <cstdio>

DEFINE_MOD();
IMPORT_SERVICE(LogService,svc_log);
IMPORT_SERVICE(ConfigService,svc_config);
IMPORT_SERVICE(HookService,svc_hook);
IMPORT_SERVICE(UiService,svc_ui);
IMPORT_SERVICE(HostService,svc_host);
IMPORT_SERVICE(CameraService,svc_camera);
IMPORT_SERVICE(ResourceService,svc_resource);
IMPORT_SERVICE(GfxService,svc_gfx);
namespace gz {ModResult initInterpolation();ModResult initProjection();ModResult initTunic();ModResult initFastMovement();ModResult initFlagLog();ModResult initTriggers();ModResult initCollision();ModResult initActorTools();ModResult installScene();ModResult initCheckers();void initOverlays();ModResult installTimerHooks();void overlayTick();ModResult initGzMenu();void shutdownGzMenu();ModResult installPractice();void shutdownInput();void initItemWheel();void initCollectibles();void initWarping();ModResult initReload();ModResult initRng();ModResult initCamera();void cameraTick();void shutdownCamera();void reloadTick();}
namespace gz {
void initializeNativeOxygen();void resetRngForSpeedrun();void cancelPracticeForSpeedrun();void resetReloadForSpeedrun();void resetTimer();
void suspendGz(){
 closeMenu();shutdownMoveLink();shutdownCamera();shutdownScene();shutdownInput();shutdownCheats();
 cancelPracticeForSpeedrun();resetReloadForSpeedrun();resetTimer();resetRngForSpeedrun();
}
}
static const char* initResultName(ModResult result){
 switch(result){
 case MOD_OK:return "MOD_OK";
 case MOD_ERROR:return "MOD_ERROR";
 case MOD_UNAVAILABLE:return "MOD_UNAVAILABLE";
 case MOD_UNSUPPORTED:return "MOD_UNSUPPORTED";
 case MOD_CONFLICT:return "MOD_CONFLICT";
 case MOD_INVALID_ARGUMENT:return "MOD_INVALID_ARGUMENT";
 }
 return "unknown result";
}
static ModResult reportInitFailure(const char* phase,ModResult result,ModError* error){
 char message[MOD_ERROR_MESSAGE_SIZE];
 std::snprintf(message,sizeof(message),"GZ %s returned %s (%d)",phase,initResultName(result),int(result));
 if(svc_log)svc_log->error(mod_ctx,message);
 if(error&&error->struct_size>=sizeof(ModError)){
  error->code=result;
  std::snprintf(error->message,sizeof(error->message),"%s",message);
 }
 return result;
}
extern "C" {
MOD_EXPORT ModResult mod_initialize(ModError* error) {
 auto r=gz::initSpeedrunGuard();if(r!=MOD_OK)return reportInitFailure("initSpeedrunGuard",r,error);
 r=gz::initPresentation();if(r!=MOD_OK)return reportInitFailure("initPresentation",r,error);
 r=gz::initInterpolation();if(r!=MOD_OK)return reportInitFailure("initInterpolation",r,error);
 r=gz::initLoading();if(r!=MOD_OK)return reportInitFailure("initLoading",r,error);
 r=gz::initCheats();if(r!=MOD_OK)return reportInitFailure("initCheats",r,error);
 r=gz::initInput();if(r!=MOD_OK)return reportInitFailure("initInput",r,error);
 r=gz::initReload();if(r!=MOD_OK)return reportInitFailure("initReload",r,error);
 gz::initInventory();gz::initItemWheel();gz::initCollectibles();gz::initFlags();gz::initWarping();
 r=gz::initRng();if(r!=MOD_OK)return reportInitFailure("initRng",r,error);
 r=gz::initFastMovement();if(r!=MOD_OK)return reportInitFailure("initFastMovement",r,error);
 r=gz::initTunic();if(r!=MOD_OK)return reportInitFailure("initTunic",r,error);
 r=gz::initLinkTools();if(r!=MOD_OK)return reportInitFailure("initLinkTools",r,error);
 r=gz::initCamera();if(r!=MOD_OK)return reportInitFailure("initCamera",r,error);
 r=gz::initCheckers();if(r!=MOD_OK)return reportInitFailure("initCheckers",r,error);
 r=gz::initFlagLog();if(r!=MOD_OK)return reportInitFailure("initFlagLog",r,error);
 r=gz::initTriggers();if(r!=MOD_OK)return reportInitFailure("initTriggers",r,error);
 r=gz::initProjection();if(r!=MOD_OK)return reportInitFailure("initProjection",r,error);
 r=gz::initCollision();if(r!=MOD_OK)return reportInitFailure("initCollision",r,error);
 r=gz::initActorTools();if(r!=MOD_OK)return reportInitFailure("initActorTools",r,error);
 gz::initScene();r=gz::installScene();if(r!=MOD_OK)return reportInitFailure("installScene",r,error);
 gz::initOverlays();r=gz::installTimerHooks();if(r!=MOD_OK)return reportInitFailure("installTimerHooks",r,error);gz::initPractice();r=gz::installPractice();if(r!=MOD_OK)return reportInitFailure("installPractice",r,error);
 r=gz::initUi();if(r!=MOD_OK)return reportInitFailure("initUi",r,error);
 r=gz::initGzMenu();if(r!=MOD_OK)return reportInitFailure("initGzMenu",r,error);
 gz::refresh();svc_log->info(mod_ctx,"Dusklight GZ initialized");return MOD_OK;
}
MOD_EXPORT ModResult mod_update(ModError*) {gz::speedrunTick();if(gz::speedrunBlocked())return MOD_OK;gz::refresh();gz::initializeNativeOxygen();gz::practiceTick();gz::reloadTick();gz::overlayTick();gz::inputTick();gz::cameraTick();gz::sceneTick();gz::cheatTick();return MOD_OK;}
MOD_EXPORT ModResult mod_shutdown(ModError*) {gz::shutdownGzMenu();gz::shutdownMoveLink();gz::shutdownCamera();gz::shutdownScene();gz::shutdownInput();gz::shutdownCheats();return MOD_OK;}
}
