// Standalone callback fixture. Executes the exact generated production callback bodies.
// No game library is linked or launched. This verifies setup effects, not actor AI.
#include "practice_data.hpp"
#include <cstdint>
#include <cmath>
#include <string>
#include <string_view>
#include <set>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <numbers>
using s16=int16_t;using u16=uint16_t;using u32=uint32_t;using f32=float;
#ifndef M_PI
constexpr double M_PI=std::numbers::pi;
#endif
struct cXyz{float x=0,y=0,z=0;cXyz()=default;cXyz(float a,float b,float c):x(a),y(b),z(c){}void set(float a,float b,float c){x=a;y=b;z=c;}};
struct fopAc_ac_c{int name=0;u32 param=0,status=0;int room=0;struct{cXyz pos;}current;struct{s16 y=0;}shape_angle;};
struct Link:fopAc_ac_c{int mEquipItem=0,mNoResetFlg2=0,swords=0;u32 boots=0;void swordEquip(int){swords++;}void onNoResetFlg0(u32 b){boots|=b;}}link;
struct daB_ZANT_c:fopAc_ac_c{int mAction=-1,mMode=-1,mFightPhase=-1;};
struct daB_DS_c:fopAc_ac_c{};
struct daObjLv4Wall_c:fopAc_ac_c{int mRotCounter=0;float mHeight=0;};
struct daObjSwSpinner_c:fopAc_ac_c{int mRotSpeedY=0;};
struct daE_ZS_c:fopAc_ac_c{
 int mAction=-1,mMode=-1,field_0x65c=7,field_0x673=0,animation=-1,loop=-1;float blend=0,speed=0;
 struct{bool target=false,collision=false;void OnTgSetBit(){target=true;}void OnCoSetBit(){collision=true;}}mCyl;
 void setBck(int a,int l,float b,float s){animation=a;loop=l;blend=b;speed=s;}
};
struct daPy_py_c{static constexpr u32 FLG0_EQUIP_HVY_BOOTS=0x20;};
enum{fpcNm_E_RD_e=468,fpcNm_Obj_Stone_e=765,fpcNm_B_ZANT_e=249,fpcNm_E_ZS_e=1,fpcNm_B_DS_e=2,fpcNm_Obj_Lv4RailWall_e=3,fpcNm_Obj_SwSpinner_e=4};
enum{dItemNo_HOOKSHOT_e=0x44,dItemNo_YELLOW_RUPEE_e=2,TF_STATUS_HUMAN=0};
struct Camera{cXyz mCenter,mEye;}camera;
struct Memory{struct{u32 mSwitch[4]{};}mBit;};
struct GameInfo{
 struct{
  struct{u32 mLastMode=0;float mLastSpeedF=0;s16 mRoomAngleY=0,mStartPoint=0;cXyz mRoomPos;u32 mRoomParam=0;}mRestart;
  struct{u32 mSwitch[2]{};}mDan;
  struct{int mRoomNo=0;struct{u16 mSwitch[2]{};u16 mRoomSwitch=0;}mBit;}mZone[32];
  Memory mMemory;struct{Memory mSave[32];}mSavedata;
 }info;
}g_dComIfG_gameInfo;
uint8_t cDmr_SkipInfo=0;
std::set<int> events,tmp,items;std::set<std::pair<int,int>> switches;
int life=12,keys=0,form=1;
void dComIfGs_onEventBit(int b){events.insert(b);}void dComIfGs_onTmpBit(int b){tmp.insert(b);}
void dComIfGs_onItemFirstBit(int b){items.insert(b);}void dComIfGs_setLife(int v){life=v;}
void dComIfGs_setKeyNum(int v){keys=v;}void dComIfGs_setTransformStatus(int v){form=v;}
void dComIfGs_onSwitch(int b,int room){switches.insert({b,room});}void dComIfGs_offSwitch(int b,int room){switches.erase({b,room});}
Link* daAlink_getAlinkActorClass(){return &link;}Camera* dCam_getBody(){return &camera;}
int fopAcM_GetName(fopAc_ac_c* a){return a->name;}u32 fopAcM_GetParam(fopAc_ac_c* a){return a->param;}int fopAcM_GetRoomNo(fopAc_ac_c* a){return a->room;}
void fopAcM_OnStatus(fopAc_ac_c* a,u32 b){a->status|=b;}
std::vector<fopAc_ac_c*> actors;
template<class F>fopAc_ac_c* find_actor(F p){for(auto* a:actors)if(p(*a))return a;return nullptr;}
fopAc_ac_c* fopAcM_SearchByName(int name){return find_actor([=](auto& a){return a.name==name;});}
struct Spawn{int name=0;u32 params=0;cXyz pos;int room=0,count=0;}spawned;
int deleted=0;
u32 fopAcM_create(int name,u32 params,cXyz* pos,int room,void*,void*,int){spawned={name,params,*pos,room,spawned.count+1};return 99;}
void fopAcM_delete(fopAc_ac_c*){deleted++;}
std::string nextStage;int nextRoom,nextPoint,nextLayer;
void setNextStageName(const char* n){nextStage=n;}void setNextStageRoom(int n){nextRoom=n;}
void setNextStagePoint(int n){nextPoint=n;}void setNextStageLayer(int n){nextLayer=n;}
struct Setup {
 gz::PracticeEntry entry{};struct{void(*inject_options_before_load)()=nullptr;}mPracticeFileOpts;
 void injectDefault_during(){}
 void setSaveAngle(s16 n){entry.angle=n;}
 void setSavePosition(float x,float y,float z){entry.position[0]=x;entry.position[1]=y;entry.position[2]=z;}
 void setLinkInfo(){link.current.pos.set(entry.position[0],entry.position[1],entry.position[2]);link.shape_angle.y=s16(entry.angle);}
}setup;
#include "practice_callbacks.inc"
static void check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
static bool near(float a,float b){return std::abs(a-b)<0.002f;}
static void pos(cXyz p,float x,float y,float z){check(near(p.x,x)&&near(p.y,y)&&near(p.z,z),"position mismatch");}
struct Fixture{
 fopAc_ac_c hugo,rock;daE_ZS_c joseph;daB_ZANT_c zant;daB_DS_c stallord;daObjLv4Wall_c wall;daObjSwSpinner_c spinner;
 Fixture(){
  g_dComIfG_gameInfo={};link={};camera={};setup={};spawned={};deleted=0;events.clear();tmp.clear();items.clear();switches.clear();
  life=12;keys=0;form=1;cDmr_SkipInfo=0;nextStage="D_TEST";nextRoom=4;nextPoint=8;nextLayer=-1;
  setup.entry.angle=16384;setup.entry.position[0]=10;setup.entry.position[1]=20;setup.entry.position[2]=30;
  hugo.name=fpcNm_E_RD_e;rock.name=fpcNm_Obj_Stone_e;rock.param=0x00ff6511;
  joseph.name=fpcNm_E_ZS_e;joseph.current.pos.x=-920;zant.name=fpcNm_B_ZANT_e;
  stallord.name=fpcNm_B_DS_e;stallord.param=0x100;stallord.room=50;stallord.current.pos={12,34,56};
  wall.name=fpcNm_Obj_Lv4RailWall_e;spinner.name=fpcNm_Obj_SwSpinner_e;
  actors={&hugo,&rock,&joseph,&zant,&stallord,&wall,&spinner};
 }
 void run(std::string_view name){check(supportsSpecial(name),"missing callback");dispatchSpecial(name);}
};
int main(){try{
 // Every assigned phase in the real catalog executes with fully populated actor fixtures.
 unsigned phases=0;std::set<std::string_view> unique;
 for(auto& e:gz::practiceEntries)for(auto name:{e.during,e.after})if(std::string_view(name)!="nullptr"){
  Fixture f;setup.entry=e;f.run(name);phases++;unique.insert(name);
 }
 {Fixture f;f.run("SaveMngSpecial_SpawnHugo");pos(link.current.pos,2.9385f,396.9580f,-18150.087f);check(u16(link.shape_angle.y)==40166,"Hugo Link angle");
  pos(f.hugo.current.pos,-289.9785f,401.5400f,-18533.078f);check(f.hugo.shape_angle.y==5880,"Hugo enemy angle");}
 {Fixture f;f.run("SaveMngSpecial_Hugo");check(switches.contains({47,0})&&!switches.contains({63,0}),"Hugo spawn switches");}
 {Fixture f;f.run("SaveMngSpecial_OrdonRock");pos(f.rock.current.pos,400,307.8f,-11365);check(f.rock.shape_angle.y==5880,"rock angle");pos(link.current.pos,400,307.5f,-11270.2f);}
 {Fixture f;f.run("SaveMngSpecial_ForestBit");check((link.mNoResetFlg2&1)&&link.swords==1,"Forest BiT lantern and sword");pos(link.current.pos,45,1911.1345f,20425);}
 {Fixture f;f.run("SaveMngSpecial_Stallord2");check(spawned.count==1&&spawned.params==0x102&&spawned.room==50&&deleted==1,"Stallord phase two replacement");
  check(f.spinner.mRotSpeedY==3000&&f.wall.mRotCounter==101&&f.wall.mHeight==3370,"Stallord arena");}
 for(auto name:{"SaveMngSpecial_StallordCad","SaveMngSpecial_StallordDisplacementClip"}){
  Fixture f;f.run(name);check(f.joseph.animation==9&&f.joseph.loop==2&&f.joseph.blend==3&&f.joseph.speed==1,"Staltroop animation");
  check(f.joseph.mAction==1&&f.joseph.mMode==0&&f.joseph.field_0x65c==0&&f.joseph.field_0x673==1,"Staltroop state");
  check(f.joseph.mCyl.target&&f.joseph.mCyl.collision&&(f.joseph.status&0x200000),"Staltroop collision/status");
  if(std::string_view(name).ends_with("DisplacementClip"))pos(f.joseph.current.pos,-301.9f,1800,-4966);
 }
 {Fixture f;f.run("SaveMngSpecial_ZantFinal");check(f.zant.mAction==23&&f.zant.mFightPhase==5&&f.zant.mMode==0&&link.swords==1,"Zant final phase");}
 {Fixture f;f.run("SaveMngSpecial_ZantDangoro");pos(f.zant.current.pos,-200,-1050,-1250);pos(link.current.pos,-200,-800,-850);check(u16(link.shape_angle.y)==32768,"Zant Dangoro angle");}
 {Fixture f;f.run("SaveMngSpecial_CenterCamera");pos(camera.mCenter,383,270,30);pos(camera.mEye,10,125,30);}
 {Fixture f;f.run("SaveMngSpecial_SolBacktrackCamera");pos(camera.mCenter,0,170.850647f,533.318665f);pos(camera.mEye,0,93.3100357f,299.36438f);}
 {Fixture f;f.run("SaveMngSpecial_Morpheel");check(link.mEquipItem==dItemNo_HOOKSHOT_e&&link.boots,"Morpheel equipment");pos(link.current.pos,-1193,-23999,-770);}
 {Fixture f;f.run("SaveMngSpecial_Goats1");check(nextLayer==5&&tmp.contains(0x1480),"goats layer/text");}
 {Fixture f;f.run("SaveMngSpecial_Escort");check(nextRoom==13&&nextPoint==98&&nextLayer==2,"escort destination");}
 {Fixture f;f.run("SaveMngSpecial_AnyPlummOoB");check(nextStage=="F_SP112"&&nextRoom==1&&nextPoint==0&&nextLayer==4&&cDmr_SkipInfo==255&&g_dComIfG_gameInfo.info.mRestart.mLastMode==10,"Plumm destination/mode");}
 {Fixture f;f.run("SaveMngSpecial_EldinCollection");check(g_dComIfG_gameInfo.info.mRestart.mLastMode==1&&g_dComIfG_gameInfo.info.mRestart.mLastSpeedF==42,"Eldin horse speed");}
 {Fixture f;f.run("SaveMngSpecial_LakebedBKSkip");check(switches.contains({2,0})&&switches.contains({122,0}),"Lakebed bridge and intro");}
 {Fixture f;f.run("SaveMngSpecial_Argorok2_after");check(cDmr_SkipInfo==255&&g_dComIfG_gameInfo.info.mRestart.mStartPoint==2&&g_dComIfG_gameInfo.info.mZone[0].mRoomNo==50,"Argorok restart");check(g_dComIfG_gameInfo.info.mSavedata.mSave[22].mBit.mSwitch[1]==2395341057u,"Argorok saved flags");pos(link.current.pos,0,-300,4000);}

 for(auto name:{"SaveMngSpecial_MDHBridge","SaveMngSpecial_AGEarlyBk","SaveMngSpecial_ZD_Yellows"}){
  Fixture f;f.run(name);check(g_dComIfG_gameInfo.info.mRestart.mLastSpeedF==25,"practice spawn speed");
 }
 {Fixture f;f.run("SaveMngSpecial_WaterfallSidehop");check(g_dComIfG_gameInfo.info.mRestart.mLastSpeedF==10,"sidehop speed");}
 {Fixture f;f.run("SaveMngSpecial_HoldSol");check(g_dComIfG_gameInfo.info.mRestart.mLastMode==0x100000,"held Sol");}
 {Fixture f;f.run("SaveMngSpecial_SetDigging");check(g_dComIfG_gameInfo.info.mRestart.mLastMode==9,"digging");}
 {Fixture f;f.run("SaveMngSpecial_KargOoB");check(g_dComIfG_gameInfo.info.mRestart.mLastMode==10&&form==0,"Karg OoB");}
 for(auto name:{"SaveMngSpecial_Palace1","SaveMngSpecial_CaveOfOrdeals","SaveMngSpecial_FanTower"}){
  Fixture f;g_dComIfG_gameInfo.info.mDan.mSwitch[0]=~0u;f.run(name);check(g_dComIfG_gameInfo.info.mDan.mSwitch[0]==0,"dungeon clear");
 }
 for(auto name:{"SaveMngSpecial_Palace2","SaveMngSpecial_ArgorokCSSkip","SaveMngSpecial_PalaceBossKey","SaveMngSpecial_EarlyPlatform"}){
  Fixture f;f.run(name);check(link.swords==1,"sword setup");
 }
 {Fixture f;f.run("SaveMngSpecial_Darkhammer");check(events.contains(0x0b02)&&events.contains(0x0b04),"Darkhammer flags");}
 {Fixture f;f.run("SaveMngSpecial_NoSQAeralfos");check(life==4,"Aeralfos health");}
 {Fixture f;f.run("SaveMngSpecial_DeathSword");check(life==2,"Death Sword health");}
 {Fixture f;f.run("SaveMngSpecial_ToTEarlyHP");pos(link.current.pos,-6626,5250,-5587);check(switches.contains({224,4}),"ToT gate");}
 {Fixture f;f.run("SaveMngSpecial_ToTEarlyPoe");pos(link.current.pos,-2462.85f,2750,-7.1f);check(u16(link.shape_angle.y)==49299,"ToT Poe angle");}
 {Fixture f;f.run("SaveMngSpecial_CityPoeCycle");pos(link.current.pos,-13990,3000,-16200);}
 {Fixture f;f.run("SaveMngSpecial_EarlyEleSpawn");pos(link.current.pos,1130,-355.6f,-5569);check(u16(link.shape_angle.y)==43917,"Early Ele angle");}

 {Fixture f;f.run("SaveMngSpecial_EarlyEle");check(tmp.contains(2),"Early Ele text");}
 {Fixture f;f.run("SaveMngSpecial_SPRBossKey");check(nextRoom==11&&nextPoint==0,"SPR boss key spawn");}
 {Fixture f;f.run("SaveMngSpecial_KB2Skip");check(nextLayer==3,"KB2 layer");}
 {Fixture f;f.run("SaveMngSpecial_RopeSkip");check(nextLayer==2,"rope skip layer");}
 {Fixture f;f.run("BeastGanonSpecial_setLayer");check(nextLayer==1,"Beast Ganon layer");}
 {Fixture f;f.run("SaveMngSpecial_emptyLake");check(nextLayer==4&&cDmr_SkipInfo==255,"empty lake");}
 {Fixture f;f.run("SaveMngSpecial_PurpleMist");check(form==0,"purple mist form");}
 {Fixture f;f.run("SaveMngSpecial_EscortKeys");check(keys==2,"escort keys");}
 {Fixture f;f.run("SaveMngSpecial_KB4");pos(link.current.pos,-8566.32617f,200,-4870.11084f);check(u16(link.shape_angle.y)==24354&&link.swords==1,"KB4 equipment/angle");}
 {Fixture f;f.run("SaveMngSpecial_GorgeVoid");pos(g_dComIfG_gameInfo.info.mRestart.mRoomPos,-17316.703125f,-6450,67532.7578125f);check(g_dComIfG_gameInfo.info.mRestart.mRoomAngleY==26033,"Gorge restart angle");}
 {Fixture f;f.run("SaveMngSpecial_Ganondorf");pos(link.current.pos,0,-2000,0);check(g_dComIfG_gameInfo.info.mDan.mSwitch[0]==1048578&&g_dComIfG_gameInfo.info.mZone[0].mBit.mRoomSwitch==1024&&link.swords==1,"Ganondorf flags");}
 std::cout<<phases<<" catalog callback assignments executed; "<<unique.size()<<" callbacks covered; actor, camera, equipment and destination assertions passed\n";
 return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
