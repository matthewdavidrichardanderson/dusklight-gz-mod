#pragma once
#include <array>
#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
namespace gz {
// TPGZ keeps cursor data by menu ID for the whole session, even after Back.
struct MenuCursorMemory {
 std::unordered_map<std::string,size_t> rows;
 size_t get(const std::string& page)const{auto it=rows.find(page);return it==rows.end()?0:it->second;}
 void remember(const std::string& page,size_t row){rows[page]=row;}
};
// GZ_drawMenuLines has one shared window; hiding/reopening does not reset it.
struct MenuScroll {
 size_t first=0,last=15;
 void update(size_t cursor,size_t count){
  if(count<=15){first=0;last=14;}
  if(cursor>last){last=cursor;first=last-14;}
  if(cursor<first){first=cursor;last=first+14;}
 }
};
inline MenuScroll menuScroll;
// Original opening guard: Up enables immediately; otherwise release shoulders
// for four controller polls. Other held buttons must not lock navigation out.
struct MenuOpeningGuard {
 bool enabled=false;unsigned delay=0;
 bool update(uint16_t buttons){
  if(enabled)return true;
  if(buttons&8)enabled=true;
  else if(buttons&0x60)delay=0;
  else if(delay<1)delay=1;
  if(delay>=4)enabled=true;else if(delay>0)++delay;
  return enabled;
 }
};
// GZ repeats each held button independently, after five frames, every four frames.
struct MenuButtons {
 uint16_t previous=0;
 std::array<unsigned,16> age{};
 uint16_t update(uint16_t buttons,unsigned interval=4){
  uint16_t result=0;
  for(unsigned i=0;i<16;i++){
   const uint16_t bit=uint16_t(1u<<i);
   if(!(buttons&bit)){age[i]=0;continue;}
   if(!(previous&bit)){age[i]=0;result|=bit;}
   else if(++age[i]>5&&age[i]%std::max(1u,interval)==0)result|=bit;
  }
  previous=buttons;return result;
 }
};
inline size_t singleColumnMove(size_t row,size_t count,uint16_t command){
 if(!count)return 0;
 if(command&8)row=(row+count-1)%count;
 if(command&4)row=(row+1)%count;
 if(command&2)row=std::min(row+10,count-1);
 if(command&1)row=row>10?row-10:0;
 return row;
}
inline int listDelta(uint16_t command){
 return command&1?-1:command&2?1:command&0x800?-10:command&0x400?10:0;
}
inline int64_t wrappedValue(int64_t value,int64_t delta,int64_t min,int64_t max){
 const auto n=max-min+1;return min+((value-min+delta)%n+n)%n;
}
}
