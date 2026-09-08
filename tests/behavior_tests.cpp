#include "rng_logic.hpp"
#include "camera_logic.hpp"
#include <cstdlib>
#include <iostream>
static void check(bool value,const char* message){if(!value){std::cerr<<message<<'\n';std::exit(1);}}
static bool near(double a,double b){return std::abs(a-b)<0.000001;}
int main(){
 using namespace gz;
 RngState state{100,100,100};
 constexpr RngState expected[]={{17100,17200,17000},{18276,18621,9315},{7489,20577,6754},{9321,23632,26229}};
 for(auto next:expected){state=advanceRng(state);check(state==next,"RNG sequence diverged from GZ Wichmann-Hill constants");}
 check(advanceRng({0,0,0})==RngState{0,0,0},"Zero seeds must remain zero, matching game behavior");
 FreeCamera camera;
 camera.initialize({0,0,0},{1,0,0});camera.update(72,0,0,0,0,false);
 check(near(camera.eye[0],14.4)&&near(camera.eye[1],0)&&near(camera.eye[2],0),"Default forward speed must match GZ");
 camera.initialize({0,0,0},{1,0,0});camera.update(0,72,0,0,0,false);
 check(near(camera.eye[2],14.4),"Camera-relative strafe direction must match GZ");
 camera.initialize({0,0,0},{1,0,0});camera.update(72,0,0,0,0,true);
 check(near(camera.eye[0],144),"Fast speed must be ten times default");
 camera.initialize({0,0,0},{1,0,0});camera.update(0,0,255,0,0,false);
 check(near(camera.eye[1],51),"Vertical trigger movement must match GZ");
 camera.initialize({0,0,0},{1,0,0});
 for(int i=0;i<100;i++)camera.update(0,0,0,72,72,false);
 check(near(camera.pitch,std::numbers::pi/2-0.1),"Pitch must stop short of the vertical singularity");
 check(camera.yaw>=0&&camera.yaw<2*std::numbers::pi,"Yaw must wrap");
 std::cout<<"RNG reference sequence and GZ free-camera behavior passed\n";
}
