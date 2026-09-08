// Original four GZ flag records, with native dungeon words serialized explicitly.
#include "core.hpp"
#include "gz_font.hpp"
#include "presentation.hpp"
#include "foreground.hpp"
#include "menu_logic.hpp"
#include "d/d_com_inf_game.h"
#include <string_view>
#include <algorithm>
#include <cstdio>
namespace gz {
uint32_t cursorColor();
namespace {
int record=0,row=0,bit=0;
int recordSize(){return record==0?32:record==1?256:24;}
u8* recordBytes(){
 auto& info=g_dComIfG_gameInfo.info;
 if(record==0)return reinterpret_cast<u8*>(&info.mMemory.mBit);
 if(record==1)return info.getSavedata().mEvent.mEvent;
 return reinterpret_cast<u8*>(&info.getSavedata().mMiniGame);
}
u32& dungeonWord(int index){
 auto& dan=g_dComIfG_gameInfo.info.mDan;
 return index<2?dan.mSwitch[index]:dan.mItem[index-2];
}
u8 readByte(int index){
 if(record!=3)return recordBytes()[index];
 return u8(dungeonWord(index/4)>>((3-index%4)*8));
}
void toggleBit(int index){
 if(record!=3)recordBytes()[index]^=u8(1<<bit);
 else dungeonWord(index/4)^=u32(1)<<(bit+(3-index%4)*8);
}
}
void flagRecordsDeleted(){record=0;bit=0;row=int(wrappedValue(row,0,0,32));}
bool flagRecordsInput(std::string_view page,uint16_t command,uint16_t edge){
 if(page!="flag records")return false;
 if(edge&0x200)return false;
 if(!playable())return true;
 if(edge&16)row=0;
 if(command&8)row=int(wrappedValue(row,-1,0,recordSize()));
 if(command&4)row=int(wrappedValue(row,1,0,recordSize()));
 if(edge&0x800)row=std::max(0,row-16);
 if(edge&0x400)row=std::min(recordSize(),row+16);
 if(row==0){
  if(command&1)record=(record+3)%4;
  if(command&2)record=(record+1)%4;
 }else{
  if(command&1)bit=(bit+1)%8;
  if(command&2)bit=(bit+7)%8;
  if(edge&0x100)toggleBit(row-1);
 }
 return true;
}
bool drawFlagRecords(std::string_view page){
 if(page!="flag records")return false;
 auto p=spritePosition(Menu);char text[80];
 if(!playable()){drawGzText("No active game",p.x,p.y+30);return true;}
 const char* names[]={"stage","event","minigame","dungeon"};
 std::snprintf(text,sizeof(text)," <%s>",names[record]);
 drawGzText(text,12,60,row==0?cursorColor():0xffffffff);
 const auto flagOn=foregroundResourceTexture("tex/flagOn.tex"),flagOff=foregroundResourceTexture("tex/flagOff.tex");
 const int first=std::max(0,row-8);
 for(int i=0;i<8&&first+i<recordSize();i++){
  const int index=first+i;const float y=80+i*20;
  std::snprintf(text,sizeof(text),"0x%02X:",index);
  drawGzText(text,20,y,row==index+1?cursorColor():0xffffffff);
  const float flagX=20+gzTextWidth(text);
  const auto byte=readByte(index);
  for(int b=7;b>=0;b--){
   const float x=flagX+(7-b)*20;
   const auto texture=byte&(1<<b)?flagOn:flagOff;
   if(texture)foregroundQuad(texture,x,y-13,x+16,y+3,0,0,1,1,0xffffffff);
   if(row==index+1&&bit==b){
    foregroundTriangle(x,y-13,x+16,y-13,x+16,y+3,0x0080ff77);
    foregroundTriangle(x,y-13,x+16,y+3,x,y+3,0x0080ff77);
   }
  }
 }
 drawGzText("DPad/Y/X to move cursor, A to toggle flag, Z to top",25,440);
 return true;
}
}
