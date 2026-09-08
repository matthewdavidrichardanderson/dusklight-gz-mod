#include "scoped_restore.hpp"
#include <array>
struct AnimationHistory {float position[3];int frame;};
int main(){
 float matrix[3][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12}};
 AnimationHistory animation{{10,20,30},7};
 int status=1;
 try{
  gz::Restore restore;
  restore.hold(matrix);restore.hold(animation);restore.hold(status);
  // Simulate multiple prediction steps changing shared and actor-owned state.
  for(int frame=0;frame<40;frame++){matrix[2][3]+=1;animation.position[0]+=2;animation.frame++;}
  status=5;
  throw 0;
 }catch(int){}
 if(matrix[2][3]!=12||animation.position[0]!=10||animation.frame!=7||status!=1)return 1;
 {gz::Restore outer;outer.hold(status);status=2;
  {gz::Restore inner;inner.hold(status);status=3;}
  if(status!=2)return 2;
 }
 return status!=1;
}
