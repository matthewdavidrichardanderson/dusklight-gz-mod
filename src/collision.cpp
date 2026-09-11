// Original GZ collision colors, bounds and ordering using native debug geometry.
#include "core.hpp"
#include "JSystem/J3DGraphBase/J3DSys.h"
#include "d/d_com_inf_game.h"
#include "d/d_bg_s_capt_poly.h"
#include "d/d_debug_viewer.h"
#include "d/actor/d_a_alink.h"
#include "SSystem/SComponent/c_bg_w.h"
#include <algorithm>
namespace gz {
bool freeCameraPosition(cXyz&);
namespace {
int range=100,raise=1,opacity=128;
// Both passes must use the same world list, regardless of the last actor's list.
struct MainXluList {
 J3DDrawBuffer* previous=j3dSys.getDrawBuffer(1);
 MainXluList(){g_dComIfG_gameInfo.drawlist.setXluList();}
 ~MainXluList(){j3dSys.setDrawBuffer(previous,1);}
};
int polygon(dBgS_CaptPoly*,cBgD_Vtx_t* v,int a,int b,int c,cM3dGPla* plane,bool edges){
 const float y=plane->mNormal.y;
 const bool ground=cBgW_CheckBGround(y),roof=!ground&&cBgW_CheckBRoof(y);
 if(!on(ground?"collision_ground":roof?"collision_roof":"collision_wall"))return 0;
 cXyz points[3]={v[a],v[b],v[c]};
 for(auto& point:points){point.x+=plane->mNormal.x*raise;point.y+=y*raise;point.z+=plane->mNormal.z*raise;}
 if(edges){
  for(int i=0;i<3;i++)dDbVw_drawLineXlu(points[i],points[(i+1)%3],{255,255,255,255},1,12);
 }else{
  GXColor color=ground?(y>=1?GXColor{255,197,197,u8(opacity)}:GXColor{255,0,0,u8(opacity)}):
   roof?GXColor{0,0,255,u8(opacity)}:GXColor{0,255,0,u8(opacity)};
  dDbVw_drawTriangleXlu(points,color,1);
 }
 return 0;
}
int faces(dBgS_CaptPoly* p,cBgD_Vtx_t* v,int a,int b,int c,cM3dGPla* plane){return polygon(p,v,a,b,c,plane,false);}
int edges(dBgS_CaptPoly* p,cBgD_Vtx_t* v,int a,int b,int c,cM3dGPla* plane){return polygon(p,v,a,b,c,plane,true);}
void capture(dBgS_CaptPoly& query){
 // Native virtual traversal also supports moving/dynamic background geometry.
 for(auto& entry:dComIfG_Bgsp().m_chk_element)
  if(entry.ChkUsed()&&entry.m_bgw_base_ptr)entry.m_bgw_base_ptr->CaptPoly(query);
}
}
u8 geometryOpacity(){return u8(opacity);}
void setGeometryOpacity(int v){opacity=v;}
// GZ appends polygons after the preceding actor draw, before painting it.
// Dusklight separates presentation: append at the end of packet collection instead.
// entryImm prepends, so faces/edges render before depth-writing colliders.
DEFINE_HOOK_SYMBOL("mDoGph_AfterOfDraw",int(),CollisionFrame);
DEFINE_HOOK_SYMBOL("dCcS::Draw",void(dCcS*),CollisionActors);
ModResult initCollision(){
 toggle("collision_at","Collision","attack colliders","Red attack geometry.");
 toggle("collision_tg","Collision","target colliders","Blue target geometry.");
 toggle("collision_co","Collision","push colliders","White push geometry.");
 toggle("collision_ground","Collision","ground polys","Red ground, pale red flat ground.");
 toggle("collision_roof","Collision","roof polys","Blue roof polygons.");
 toggle("collision_wall","Collision","wall polys","Green wall polygons.");
 toggle("collision_edges","Collision","poly edges","White polygon outlines.");
 number("collision_range","Collision","poly draw range:",0,1000,[](){return range;},[](int64_t v){range=int(v);});
 number("collision_raise","Collision","poly draw raise:",0,255,[](){return raise;},[](int64_t v){raise=int(v);});
 number("collision_opacity","Collision","opacity:",0,255,[](){return opacity;},[](int64_t v){opacity=int(v);});
 auto r=guardedPre<CollisionFrame>([](ModContext*,void*,void*,void*){
  if(!playable()||!(on("collision_ground")||on("collision_roof")||on("collision_wall")))return HOOK_CONTINUE;
  auto* link=daAlink_getAlinkActorClass();if(!link)return HOOK_CONTINUE;
  MainXluList list;
  cXyz base=link->current.pos;freeCameraPosition(base);
  cXyz min(base.x-range,base.y-range,base.z-range),max(base.x+range,base.y+range,base.z+range);
  dBgS_CaptPoly query;query.OnFullGrp();query.Set(min,max);
  if(on("collision_edges")){query.SetCallback(edges);capture(query);}
  query.SetCallback(faces);capture(query);
  return HOOK_CONTINUE;
 });
 if(r!=MOD_OK)return r;
 return guardedPost<CollisionActors>([](ModContext*,void* args,void*,void*){
  if(!playable())return;
  MainXluList list;
  auto* cc=mods::arg<dCcS*>(args,0);
  auto draw=[](auto& list,unsigned count,GXColor color){
   for(unsigned i=0;i<std::min(count,unsigned(std::size(list)));i++)if(list[i])list[i]->Draw(color);
  };
  if(on("collision_at"))draw(cc->mpObjAt,cc->field_0x280c,{255,0,0,u8(opacity)});
  if(on("collision_tg"))draw(cc->mpObjTg,cc->field_0x280e,{58,130,240,u8(opacity)});
  if(on("collision_co"))draw(cc->mpObjCo,cc->field_0x2810,{255,255,255,u8(opacity)});
 });
}
}
