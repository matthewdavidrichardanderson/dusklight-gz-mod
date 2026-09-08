#pragma once
#include <cstdint>
namespace gz {
constexpr bool comboMatches(uint16_t buttons,uint16_t mask){return mask!=0&&buttons==mask;}
constexpr bool comboTriggered(uint16_t buttons,uint16_t previous,uint16_t mask){return comboMatches(buttons,mask)&&previous!=buttons;}
// Reserve the reload chord before its final A press, as in Dusk's native tools.
constexpr bool reloadReservesMenu(uint16_t held,uint16_t reloadMask){
 constexpr uint16_t menuChord=0x1020;
 const uint16_t prefix=reloadMask&uint16_t(~0x100);
 return (prefix&menuChord)==menuChord&&(held&prefix)==prefix;
}
struct FrameAdvance {
 uint16_t previous=0;
 unsigned heldFrames=0;
 bool update(uint16_t buttons,uint16_t mask) {
  const bool held=(buttons&mask)!=0;
  heldFrames=held?(heldFrames<30?heldFrames+1:30):0;
  const bool triggered=(buttons&mask)==mask&&((buttons&~previous)&mask)!=0;
  previous=buttons;
  return mask!=0&&(triggered||heldFrames>=30);
 }
 void reset(uint16_t buttons=0){previous=buttons;heldFrames=0;}
};
}
