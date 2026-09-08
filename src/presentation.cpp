#include "presentation.hpp"
#include "presentation_layout.hpp"
#include "gz_font.hpp"
#include "helpers/gx_helper.h"
#include <array>
#include <algorithm>
#include <cmath>
namespace gz {
static ConfigVarHandle colorVar=0,fontVar=0;
static ConfigVarHandle positions[SpriteCount][2]{};
static constexpr const char* names[]={"menu","input","debug","stage","timer","load_timer","igt","fifo","heap","mash","transform","displacement"};
// Pinned TPGZ modules/init/src/main.cpp, GZ_PosSettings_initDefaults (GCN).
static constexpr SpritePosition defaults[]={{25,60},{220,380},{450,200},{145,350},{450,420},{450,30},{35,30},{5,440},{145,25},{450,400},{465,30},{450,60}};
static constexpr SpritePosition legacyDefaults[]={{25,60},{20,363},{360,70},{360,295},{20,315},{20,350},{20,335},{20,290},{360,30},{20,270},{570,30},{360,235}};
int fontChoice(){int64_t v=0;svc_config->get_int(mod_ctx,fontVar,&v);return int(std::clamp<int64_t>(v,0,6));}
uint32_t cursorColor(){
 static constexpr uint32_t colors[]={0x00cc00ff,0x0080ffff,0xcc0000ff,0xee8000ff,0xffcc00ff,0x6600ccff,0xec80ffff,0x7ae6f0ff};
 int64_t v=0;svc_config->get_int(mod_ctx,colorVar,&v);return colors[std::clamp<int64_t>(v,0,7)];
}
static float canvasWidth(){
 u32 width=0,height=0;AuroraGetRenderSize(&width,&height);
 return overlayViewport(float(width),float(height)).canvasWidth;
}
static float spriteWidth(Sprite s){
 // Reserve the visible block, not just its origin, when positioning at an edge.
 static constexpr float minimum[]={320,183,158,300,158,158,150,300,280,150,32,158};
 static constexpr const char* samples[]={"ladder freezard cancel [X]","","x-pos: -123456.1234",
 "Save Stage: F_SP121","00:00:00.000","00:00:00.000","00:00:00.000",
 "Jump attack 10f early","total free: 123456789","A: 255","","dxyz: 123456.1234"};
 float width=std::max(minimum[s],gzTextWidth(samples[s]));
 if(s==StageInfo)width=std::max(width,150+gzTextWidth(samples[s]));
 return std::min(width,CanvasWidth-25);
}
static void resetPositions(){
 for(int i=0;i<SpriteCount;i++){
  svc_config->set_float(mod_ctx,positions[i][0],defaults[i].x);
  svc_config->set_float(mod_ctx,positions[i][1],defaults[i].y);
 }
}
ModResult initPresentation(){
 toggle("advanced_mode","Settings","Advanced mode","Display additional GZ tool information.");
 toggle("drop_shadows","Settings","Drop shadows","Adds the original translucent shadow to text and overlays.");
 auto select=[](const char* id,const char* label,std::vector<const char*> opts,ConfigVarHandle& handle){
  ConfigVarDesc d=CONFIG_VAR_DESC_INIT;d.name=id;d.type=CONFIG_VAR_INT;
  auto r=svc_config->register_var(mod_ctx,&d,&handle);if(r!=MOD_OK)return r;
  const auto h=handle;const auto max=int64_t(opts.size()-1);
  auto& c=choice(id,"Settings",label,opts,[h,max](){int64_t v=0;svc_config->get_int(mod_ctx,h,&v);return std::clamp(v,int64_t(0),max);},
   [h](int64_t v){svc_config->set_int(mod_ctx,h,v);});c.gameOnly=false;return MOD_OK;
 };
 auto r=select("cursor_color","Cursor color",{"green","blue","red","orange","yellow","purple","pink","cyan"},colorVar);if(r!=MOD_OK)return r;
 r=select("font","Font",{"consola","calamity-bold","lib-sans","lib-sans-bold","lib-serif","lib-serif-bold","press-start-2p"},fontVar);if(r!=MOD_OK)return r;
 for(int i=0;i<SpriteCount;i++)for(int axis=0;axis<2;axis++){
  const auto key=std::string("pos_")+names[i]+(axis?"_y":"_x");
  ConfigVarDesc d=CONFIG_VAR_DESC_INIT;d.name=key.c_str();d.type=CONFIG_VAR_FLOAT;d.default_float=axis?defaults[i].y:defaults[i].x;
  r=svc_config->register_var(mod_ctx,&d,&positions[i][axis]);if(r!=MOD_OK)return r;
 }
 // Migrate untouched defaults from the earlier port once. Preserve custom pairs.
 ConfigVarHandle migrated=0;ConfigVarDesc migration=CONFIG_VAR_DESC_INIT;
 migration.name="tpgz_position_defaults_v1";migration.type=CONFIG_VAR_BOOL;
 r=svc_config->register_var(mod_ctx,&migration,&migrated);if(r!=MOD_OK)return r;
 bool done=false;svc_config->get_bool(mod_ctx,migrated,&done);
 if(!done){
  for(int i=0;i<SpriteCount;i++){
   double x=0,y=0;svc_config->get_float(mod_ctx,positions[i][0],&x);svc_config->get_float(mod_ctx,positions[i][1],&y);
   if(x==legacyDefaults[i].x&&y==legacyDefaults[i].y){
    svc_config->set_float(mod_ctx,positions[i][0],defaults[i].x);
    svc_config->set_float(mod_ctx,positions[i][1],defaults[i].y);
   }
  }
  svc_config->set_bool(mod_ctx,migrated,true);
 }
 auto& reset=action("reset_positions","Settings","reset overlay positions",resetPositions);
 reset.gameOnly=false;reset.help="Restore all GZ overlay positions; other settings are unchanged.";
 return MOD_OK;
}
SpritePosition spritePosition(Sprite s){
 double x=defaults[s].x,y=defaults[s].y;
 svc_config->get_float(mod_ctx,positions[s][0],&x);svc_config->get_float(mod_ctx,positions[s][1],&y);
 if(!std::isfinite(x))x=defaults[s].x;if(!std::isfinite(y))y=defaults[s].y;
 return {positionToWide(float(x),canvasWidth(),spriteWidth(s)),float(y)};
}
void moveSprite(Sprite s,float dx,float dy){
 auto p=spritePosition(s);
 svc_config->set_float(mod_ctx,positions[s][0],positionFromWide(p.x+dx,canvasWidth(),spriteWidth(s)));
 svc_config->set_float(mod_ctx,positions[s][1],p.y+dy);
}
}
