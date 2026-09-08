#include "input_logic.hpp"
#include <cstdlib>
#include <iostream>
static void check(bool ok,const char* what){if(!ok){std::cerr<<what<<'\n';std::exit(1);}}
int main(){
 using namespace gz;
 check(comboTriggered(0x64,0,0x64),"Menu opens on first exact press");
 check(!comboTriggered(0x64,0x64,0x64),"Holding menu combo must not close it");
 check(!comboMatches(0x164,0x64),"An extra button must not trigger an overlapping combo");
 check(!comboMatches(0,0),"Zero disables a binding");
 check(comboTriggered(0x64,0x60,0x64),"Adding final button completes a combo");
 check(comboTriggered(0x64,0,0x64),"Released and pressed combo can retrigger");
 FrameAdvance f;
 check(f.update(0x20,0x20),"First R press advances once");
 for(int i=2;i<30;i++)check(!f.update(0x20,0x20),"No repeat before the 30th held frame");
 check(f.update(0x20,0x20),"Upstream repeats from held frame 30");
 check(f.update(0x20,0x20),"Held repeat continues");
 check(!f.update(0,0x20),"Release stops repeat");
 check(f.update(0x120,0x20),"Upstream frame advance permits accompanying buttons");
 f.reset();
 check(!f.update(0x20,0x120),"Partial multibutton advance binding must not trigger");
 check(f.update(0x120,0x120),"Completing the advance binding triggers");
 f.reset();
 check(f.update(0x320,0x120),"Complete advance binding permits extra gameplay buttons");
 f.reset(0x21);
 check(!f.update(0x21,0x20),"Pausing must not immediately advance from the pause chord");
 check(!f.update(0x20,0),"Disabled frame-advance chord never advances");
 std::cout<<"Input transition and 30-frame repeat checks passed\n";
}
