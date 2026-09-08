#include "timer_logic.hpp"
#include <iostream>
int main(){
 gz::PracticeTimers t;
 t.advance(1,30,false,true);
 if(t.rta||t.igt||t.frames)return 1;
 t.running=true;t.advance(2,60,false,true);t.advance(3,90,true,true);t.advance(1,30,false,true);
 if(t.rta!=6||t.igt!=3||t.loads!=3||t.frames!=180)return 2;
 t.running=false;t.advance(2,60,true,true);
 if(t.rta!=6||t.igt!=3||t.loads!=3||t.frames!=180)return 3;
 t.running=true;t.advance(1,30,false,true);
 if(t.rta!=9||t.igt!=6||t.loads!=5||t.frames!=210)return 4;
 t.reset();
 if(t.running||t.rta||t.igt||t.loads||t.frames)return 5;
 t.running=true;t.advance(-1,1,false,true);
 if(t.rta||t.igt||t.frames)return 6;
 std::cout<<"TPGZ clock-origin retention, completed-load display and common reset verified\n";
}
