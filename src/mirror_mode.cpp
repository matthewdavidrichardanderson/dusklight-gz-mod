#include "mirror_mode.hpp"
#include "d/d_camera.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_camera_mng.h"
#include "m_Do/m_Do_graphic.h"
#include "m_Do/m_Do_lib.h"
#include <cmath>

namespace gz {
bool mirrorModeEnabled() {
    // The online mod uses the host's projection rather than depending on the
    // private settings layout. mDoLib_project includes Dusklight's X flip.
    auto* camera = dComIfGp_getCamera(0);
    if (!camera) return false;
    Mtx inverseView;
    if (!MTXInverse(j3dSys.getViewMtx(), inverseView)) return false;
    cXyz center = camera->view.lookat.center;
    cXyz cameraRight(inverseView[0][0], inverseView[1][0], inverseView[2][0]);
    cXyz rightPoint = center + cameraRight * 100.0f;
    cXyz centerScreen, rightScreen;
    mDoLib_project(&center, &centerScreen);
    mDoLib_project(&rightPoint, &rightScreen);
    return std::isfinite(centerScreen.x) && std::isfinite(rightScreen.x) &&
           std::fabs(rightScreen.x - centerScreen.x) >= 0.01f &&
           rightScreen.x < centerScreen.x;
}
}
