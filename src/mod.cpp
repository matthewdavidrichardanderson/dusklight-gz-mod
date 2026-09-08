#include "core.hpp"
#include "presentation.hpp"
#include "loading.hpp"
#include "link_tools.hpp"
#include "mods/svc/camera.h"
#include "mods/svc/resource.h"
#include "mods/svc/gfx.h"

DEFINE_MOD();
IMPORT_SERVICE(LogService,svc_log);
IMPORT_SERVICE(ConfigService,svc_config);
IMPORT_SERVICE(HookService,svc_hook);
IMPORT_SERVICE(UiService,svc_ui);
IMPORT_SERVICE(HostService,svc_host);
IMPORT_SERVICE(CameraService,svc_camera);
IMPORT_SERVICE(ResourceService,svc_resource);
IMPORT_SERVICE(GfxService,svc_gfx);
namespace gz {ModResult initProjection();ModResult initTunic();ModResult initFastMovement();ModResult initFlagLog();ModResult initTriggers();ModResult initCollision();ModResult initActorTools();ModResult installScene();ModResult initCheckers();void initOverlays();void overlayTick();ModResult initGzMenu();void shutdownGzMenu();ModResult installPractice();void shutdownInput();void initItemWheel();void initCollectibles();void initWarping();ModResult initReload();ModResult initRng();ModResult initCamera();void cameraTick();void shutdownCamera();void reloadTick();}
namespace gz {
void resetRngForSpeedrun();void cancelPracticeForSpeedrun();void resetReloadForSpeedrun();void resetTimer();
void suspendGz(){
 closeMenu();shutdownMoveLink();shutdownCamera();shutdownScene();shutdownInput();shutdownCheats();
 cancelPracticeForSpeedrun();resetReloadForSpeedrun();resetTimer();resetRngForSpeedrun();
}
}
extern "C" {
MOD_EXPORT ModResult mod_initialize(ModError*) {
 auto r=gz::initSpeedrunGuard();if(r!=MOD_OK)return r;
 r=gz::initPresentation();if(r!=MOD_OK)return r;
 r=gz::initLoading();if(r!=MOD_OK)return r;
 r=gz::initCheats();if(r!=MOD_OK)return r;
 r=gz::initInput();if(r!=MOD_OK)return r;
 r=gz::initReload();if(r!=MOD_OK)return r;
 gz::initInventory();gz::initItemWheel();gz::initCollectibles();gz::initFlags();gz::initWarping();
 r=gz::initRng();if(r!=MOD_OK)return r;
 r=gz::initFastMovement();if(r!=MOD_OK)return r;
 r=gz::initTunic();if(r!=MOD_OK)return r;
 r=gz::initLinkTools();if(r!=MOD_OK)return r;
 r=gz::initCamera();if(r!=MOD_OK)return r;
 r=gz::initCheckers();if(r!=MOD_OK)return r;
 r=gz::initFlagLog();if(r!=MOD_OK)return r;
 r=gz::initTriggers();if(r!=MOD_OK)return r;
 r=gz::initProjection();if(r!=MOD_OK)return r;
 r=gz::initCollision();if(r!=MOD_OK)return r;
 r=gz::initActorTools();if(r!=MOD_OK)return r;
 gz::initScene();r=gz::installScene();if(r!=MOD_OK)return r;
 gz::initOverlays();gz::initPractice();r=gz::installPractice();if(r!=MOD_OK)return r;
 r=gz::initUi();if(r!=MOD_OK)return r;
 r=gz::initGzMenu();if(r!=MOD_OK)return r;
 gz::refresh();svc_log->info(mod_ctx,"Dusk GZ initialized");return MOD_OK;
}
MOD_EXPORT ModResult mod_update(ModError*) {gz::speedrunTick();if(gz::speedrunBlocked())return MOD_OK;gz::refresh();gz::practiceTick();gz::reloadTick();gz::overlayTick();gz::inputTick();gz::cameraTick();gz::sceneTick();gz::cheatTick();return MOD_OK;}
MOD_EXPORT ModResult mod_shutdown(ModError*) {gz::shutdownGzMenu();gz::shutdownMoveLink();gz::shutdownCamera();gz::shutdownScene();gz::shutdownInput();gz::shutdownCheats();return MOD_OK;}
}
