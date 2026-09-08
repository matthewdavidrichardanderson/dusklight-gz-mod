#pragma once
#include "core.hpp"
namespace gz {
ModResult initGzFont();
void shutdownGzFont();
void beginGzDraw();
void endGzDraw();
float gzTextWidth(const std::string&,float size=17);
void drawGzTextPlain(const std::string&,float,float,uint32_t,float);
void beginGzShape(unsigned count,unsigned primitive,unsigned lineWidth=16);
void gzShapeVertex(float,float,uint32_t);
void endGzShape();
void drawGzText(const std::string&,float x,float y,uint32_t color=0xffffffff,float size=17);
}
