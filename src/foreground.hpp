#pragma once
#include "core.hpp"
#include <cstdint>
namespace gz {
ModResult initForeground();
void shutdownForeground();
bool beginForeground();
uint64_t foregroundResourceTexture(const char*);
uint64_t foregroundTexture(unsigned,unsigned,const void*);
uint64_t foregroundConsoleTexture(unsigned,unsigned,const void*);
void foregroundQuad(uint64_t,float,float,float,float,float,float,float,float,uint32_t);
void foregroundLine(float,float,float,float,uint32_t,float);
void foregroundTriangle(float,float,float,float,float,float,uint32_t);
}
