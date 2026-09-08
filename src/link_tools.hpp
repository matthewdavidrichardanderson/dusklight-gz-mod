#pragma once
#include "SSystem/SComponent/c_xyz.h"
namespace gz {
void storedLinkPose(cXyz&,s16&);
ModResult initLinkTools();
void toggleMoveLink();
void moveLinkTick();
bool moveLinkActive();
void shutdownMoveLink();
void shutdownCamera();
}
