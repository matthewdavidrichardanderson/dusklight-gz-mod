#include "rng_logic.hpp"
#include "clawshot_chain.hpp"
#include "upstream_samples.hpp"
#include "camera_logic.hpp"
#include "actor_motion.hpp"
#include <cstdlib>
#include <iostream>
static void check(bool value,const char* message){if(!value){std::cerr<<message<<'\n';std::exit(1);}}
static bool near(double a,double b){return std::abs(a-b)<0.000001;}
struct SampleClock {
 inline static uint64_t tick=1,epoch=0;
 inline static bool presenting=false,enabled=true;
 static bool should_capture(){return enabled&&!presenting;}
 static bool is_enabled(){return enabled;}
 static bool is_presentation_active(){return presenting;}
 static uint64_t sim_tick_seq(){return tick;}
 static uint64_t presentation_epoch(){return epoch;}
 static float get_interpolation_step(){return .5f;}
};
struct SampleLerp {
 void operator()(float& out,float previous,float current,float step)const{out=previous+(current-previous)*step;}
};
int main(){
 using namespace gz;
 check(near(chainRenderStep(2000.0f),5.0f),"Normal clawshot chains must keep native spacing");
 check(chainRenderStep(69420.0f)>5.0f,"Super Clawshot must widen its render stride");
 check(static_cast<int>(std::ceil(69420.0f/chainRenderStep(69420.0f)))<=kMaxRenderedChainLinks,"Super Clawshot chain draw count must be bounded");
 upstream::Samples<float, SampleClock, SampleLerp> chain;
 float anchors[4]={0,10,20,30};
 chain.capture(anchors,4);
 SampleClock::presenting=true;
 check(near(chain.read(1,anchors[1]),10),"First sample must not blend from zero");
 SampleClock::presenting=false;SampleClock::tick=2;
 for(auto& v:anchors)v+=100;
 chain.capture(anchors,4);SampleClock::presenting=true;
 check(near(chain.read(0,anchors[0]),50),"Chain must use upstream interpolation");
 SampleClock::presenting=false;
 anchors[0]=120;chain.capture(anchors,4);SampleClock::presenting=true;
 check(near(chain.read(0,anchors[0]),60),"Same-tick recapture must update current without advancing previous");
 SampleClock::presenting=false;
 check(near(chain.read(0,anchors[0]),120),"Simulation must read current chain, never presentation interpolation");
 SampleClock::presenting=true;SampleClock::epoch++;
 check(near(chain.read(0,anchors[0]),120),"Presentation reset must invalidate old samples immediately");
 SampleClock::presenting=false;chain.capture(anchors,4);SampleClock::presenting=true;
 check(near(chain.read(0,anchors[0]),120),"First capture after reset must not blend old history");
 SampleClock::presenting=false;SampleClock::tick=5;anchors[0]=70000;chain.capture(anchors,4);
 SampleClock::presenting=true;
 check(near(chain.read(0,anchors[0]),70000),"Interrupted ticks must not blend stale endpoints");
 anchors[0]=90000;chain.capture(anchors,4);
 check(near(chain.read(0,anchors[0]),70000),"Presentation draws must not capture samples");
 SampleClock::enabled=false;
 check(near(chain.read(0,anchors[0]),90000),"Disabled interpolation must use current chain");
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
 ActorMotion actor;int16_t yaw=0,pitch=0;
 auto offset=actor.eyeOffset();
 check(near(offset[0],0)&&near(offset[1],200)&&near(offset[2],-600),"Actor camera must be 600 behind and target 200 above the origin");
 auto delta=actor.update(yaw,pitch,0,72,0,0,0);
 check(near(delta[0],0)&&near(delta[2],72),"Actor forward movement must use raw stick units");
 delta=actor.update(yaw,pitch,72,0,0,0,0);
 check(near(delta[0],-72)&&near(delta[2],0),"Actor strafe must match GZ camera-relative direction");
 delta=actor.update(yaw,pitch,0,72,0,0,0x10);
 check(near(delta[2],180),"Z actor movement must be 2.5x");
 delta=actor.update(yaw,pitch,0,72,0,0,0x30);
 check(near(delta[2],720),"Z+R actor movement must be 10x");
 delta=actor.update(yaw,pitch,0,0,20,30,0);
 check(near(delta[1],30)&&yaw==600&&pitch==0,"C-stick raises and turns actors");
 const auto lockedAngle=actor.angle;
 delta=actor.update(yaw,pitch,0,0,20,30,0x40);
 check(near(delta[1],0)&&yaw==0&&pitch==900&&actor.angle==lockedAngle,"L locks the orbit and makes C-stick edit pitch and yaw");
 yaw=0x4000;pitch=0;
 delta=actor.update(yaw,pitch,0,72,0,0,0);
 check(std::abs(delta[0]-72)<0.0001&&std::abs(delta[2])<0.0001,"Selecting a turned actor must turn movement with the camera");
 offset=actor.eyeOffset();
 check(std::abs(offset[0]+600)<0.0001&&std::abs(offset[2])<0.0001,"Actor camera must orbit with facing");
 yaw=0;pitch=0;actor.update(yaw,pitch,0,0,1,1,0x70);
 check(yaw==-800&&pitch==800,"Z+R must apply GZ's fastest rotation tier");
 std::cout<<"RNG reference sequence and GZ free-camera behavior passed\n";
}
