#pragma once
namespace gz {
// TPGZ gz_flags.cpp: direct title-screen loads bypass file-select itemInit.
// Only initialize an absent oxygen capacity; never refill an active air meter.
template<class Play> void initializeOxygen(Play& play){
 if(play.getMaxOxygen()!=0)return;
 play.setOxygen(600);
 play.setNowOxygen(600);
 play.setMaxOxygen(600);
}
}
