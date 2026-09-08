#pragma once
#include "core.hpp"
namespace gz {
// Original GCN reference coordinates; the renderer expands horizontal space for wider output.
inline constexpr float CanvasWidth=608.f,CanvasHeight=448.f;
enum Sprite {Menu,InputViewer,LinkDebug,StageInfo,Timer,LoadTimer,IgtTimer,Fifo,Heap,Mash,Transform,Displacement,SpriteCount};
struct SpritePosition{float x,y;};
ModResult initPresentation();
SpritePosition spritePosition(Sprite);
void moveSprite(Sprite,float,float);
uint32_t cursorColor();
int fontChoice();
}
