// Uses real native stage classes and copied native setter bodies, without launching/linking Dusklight.
#include "helpers/string.hpp"
#include "d/d_stage.h"
#include "practice_load_sequence.hpp"
#include <cstring>
#include <cstdlib>
// Non-game dependencies: strings retain bounds checking; graphics are never used.
void SafeStringCopy(char* dst,size_t size,const char* src){
 if(!size||dst==src||std::strlen(src)>=size)std::abort();
 std::memcpy(dst,src,std::strlen(src)+1);
}
extern "C" void GXDestroyTexObj(GXTexObj*){}
namespace mDoRst {void onReset(){std::abort();}}
#include "native_stage_setters.inc"
int main(){
 dStage_nextStage_c old;
 old.set("D_MN05",0,0,0,13,0);
 old.set("F_SP121",3,2,14,13,0);
 if(std::strcmp(old.getName(),"D_MN05"))return 1; // Reproduce the reported failure.
 for(const char* base:{"D_MN05","F_SP121"}){
  dStage_nextStage_c next;
  bool callbackWhileDisabled=false;
  gz::composePracticeDestination(next,base,0,0,0,[&](){
   callbackWhileDisabled=!next.isEnable();
   next.getStartStage()->set("F_SP121",3,2,14);
  });
  if(!callbackWhileDisabled||!next.isEnable()||std::strcmp(next.getName(),"F_SP121")||
     next.getRoomNo()!=3||next.getPoint()!=2||next.getLayer()!=14||next.getWipe()!=13)return 2;
 }
 dStage_nextStage_c plain;
 gz::composePracticeDestination(plain,"D_MN05",11,2,4,[](){});
 if(!plain.isEnable()||std::strcmp(plain.getName(),"D_MN05")||plain.getRoomNo()!=11||plain.getPoint()!=2||plain.getLayer()!=4)return 3;
}
