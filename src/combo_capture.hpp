#pragma once
#include <cstdint>
#include <optional>
namespace gz {
struct ComboCapture {
 bool active=false,waitingSelect=false;
 unsigned remaining=0;
 uint16_t previous=0;
 void begin(){active=true;waitingSelect=true;remaining=90;previous=0;}
 void reset(){*this={};}
 std::optional<uint16_t> update(uint16_t buttons){
  if(!active)return {};
  if(remaining--==0){reset();return {};}
  buttons&=0x1f7f;
  if(waitingSelect){if(!(buttons&0x100))waitingSelect=false;return {};}
  if(previous&~buttons){const auto value=previous;reset();return value;}
  previous=buttons;return {};
 }
};
}
