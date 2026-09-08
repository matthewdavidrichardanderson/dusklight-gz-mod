#include "oxygen_init.hpp"
struct Play {
 int air=0,shown=0,capacity=0,writes=0;
 int getMaxOxygen(){return capacity;}
 void setOxygen(int v){air=v;++writes;}
 void setNowOxygen(int v){shown=v;++writes;}
 void setMaxOxygen(int v){capacity=v;++writes;}
};
int main(){
 // A direct title-screen practice load has never visited file select.
 Play title;
 gz::initializeOxygen(title);
 if(title.air!=600||title.shown!=600||title.capacity!=600||title.writes!=3)return 1;
 // Repeated updates must preserve depletion, including genuine drowning.
 title.air=450;title.shown=500;title.writes=0;
 gz::initializeOxygen(title);
 if(title.air!=450||title.shown!=500||title.writes)return 2;
 title.air=title.shown=0;
 gz::initializeOxygen(title);
 if(title.air||title.shown||title.writes)return 3;
 // Respect an already initialized nondefault capacity.
 Play custom{50,100,900,0};
 gz::initializeOxygen(custom);
 if(custom.air!=50||custom.shown!=100||custom.capacity!=900||custom.writes)return 4;
}
