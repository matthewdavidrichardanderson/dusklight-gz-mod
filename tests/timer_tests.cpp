#include "timer_logic.hpp"
#include <iostream>
int main(){
 gz::PracticeTimers t;
 t.advance(1,30,1000,false,true);
 if(t.rta||t.igt||t.frames)return 1;
 t.running=true;t.advance(2,60,2000,false,true);t.advance(3,90,500,true,true);t.advance(1,30,1000,false,true);
 if(t.rta!=6||t.igt!=3000||t.loads!=500||t.frames!=180)return 2;
 // A host stall contributes to RTA, but not to a stopped game clock.
 t.advance(10,0,0,false,true);
 if(t.rta!=16||t.igt!=3000||t.loads!=500)return 3;
 t.running=false;t.advance(2,60,2000,true,true);
 if(t.igt!=3000||t.loads!=500)return 4;
 t.running=true;t.advance(1,30,1000,false,true);
 if(t.rta!=19||t.igt!=4000||t.loads!=2500)return 5;
 t.reset();if(t.running||t.rta||t.igt||t.loads||t.frames)return 6;
 t.running=true;t.advance(-1,1,1,false,true);t.advance(1,1,-1,false,true);
 if(t.rta||t.igt||t.frames)return 7;
 // Reset inside a load counts only the remaining interval, once it completes.
 t.advance(3,90,750,true,true);if(t.loads||t.igt)return 8;
 t.advance(0,0,0,false,true);if(t.loads!=750||t.igt)return 9;
 t.advance(1,30,1000,false,false,false,true);if(t.igt!=1000||t.loads!=750)return 10;
 std::cout<<"Independent GZ controls, Dusklight clock ticks, load exclusion and reset verified\n";
}
