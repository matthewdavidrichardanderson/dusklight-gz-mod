#include "menu_logic.hpp"
#include "combo_capture.hpp"
#include "loading_logic.hpp"
#include <iostream>
int main(){
 using namespace gz;
 if(singleColumnMove(5,105,2)!=15||singleColumnMove(5,105,1)!=0||singleColumnMove(100,105,2)!=104)return 1;
 if(singleColumnMove(0,105,8)!=104||singleColumnMove(104,105,4)!=0)return 2;
 if(listDelta(0x400)!=10||listDelta(0x800)!=-10||wrappedValue(0,-1,0,5)!=5||wrappedValue(5,1,0,5)!=0)return 3;
 MenuCursorMemory memory;memory.remember("practice:any",37);memory.remember("Flags",4);
 if(memory.get("practice:any")!=37||memory.get("Flags")!=4||memory.get("practice:hundo")!=0)return 21;
 MenuScroll scroll;scroll.update(0,8);scroll.update(37,105);
 if(scroll.first!=23||scroll.last!=37)return 22;
 scroll.update(37,105);if(scroll.first!=23)return 23; // hide/load/reopen retains bottom
 scroll.update(30,105);if(scroll.first!=23)return 24; // upward selection retains lookahead
 scroll.update(0,6);scroll.update(37,105);if(scroll.first!=23)return 25; // parent uses shared window
 scroll.update(0,105);if(scroll.first!=0||scroll.last!=14)return 26;
 MenuOpeningGuard guard;if(guard.update(0x64))return 27;
 for(int i=0;i<3;i++)if(guard.update(0x100))return 28;
 if(!guard.update(0x100))return 29; // held A cannot block opening forever
 guard={};if(!guard.update(8))return 30;
 MenuButtons b;if(b.update(2)!=2)return 4;
 for(int i=1;i<8;i++)if(b.update(2))return 5;
 if(b.update(2)!=2)return 6;
 // Adding a second button must not reset repeat timing of the first.
 b.update(2|0x100);b.update(2|0x100);b.update(2);
 if(b.update(2)!=2)return 7;
 b.update(0);if(b.update(2)!=2)return 8;
 ComboCapture capture;capture.begin();
 if(capture.update(0x100)||capture.update(0)||capture.update(0x60)||capture.update(0x260))return 14;
 auto combination=capture.update(0x60);
 if(!combination||*combination!=0x260||capture.active)return 15;
 capture.begin();for(int i=0;i<92;i++)if(capture.update(0))return 16;
 if(capture.active)return 17;
 capture.begin();capture.update(0);
 capture.update(0x64);combination=capture.update(0);
 if(!combination||*combination!=0x64)return 18;
 MenuButtons fast;fast.update(2,1);
 for(int i=0;i<5;i++)if(fast.update(2,1))return 19;
 if(fast.update(2,1)!=2||fast.update(2,1)!=2)return 20;
 SceneLoadState load;
 load.requested(UINT32_MAX,13);if(load.active)return 9;
 load.requested(1,0x7fff);if(load.active)return 10;
 load.requested(2,13);if(!load.active)return 11;
 load.requested(UINT32_MAX,13);if(!load.active)return 12;
 load.requested(UINT32_MAX,13);if(!load.active)return 12;
 load.completed();if(load.active)return 13;
 std::cout<<"GZ row paging, wrap, X/Y editing, independent repeat and release verified\n";
}
