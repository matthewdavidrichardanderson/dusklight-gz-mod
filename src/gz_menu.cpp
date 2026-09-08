#include "helpers/gx_helper.h"
// GZ hierarchy, original atlas, 15-row scrolling and controller navigation.
#include "core.hpp"
#include "gz_font.hpp"
#include "practice_data.hpp"
#include "menu_logic.hpp"
#include "presentation.hpp"
#include "foreground.hpp"
#include "SSystem/SComponent/c_counter.h"
#include <algorithm>
#include <string_view>
namespace gz {void shutdownTransformIndicator();void drawRngValues();void cancelComboCapture();bool comboMenuInput(std::string_view,uint16_t,uint16_t,uint16_t);bool drawComboMenu(std::string_view);bool itemWheelInput(std::string_view,uint16_t,uint16_t);bool drawItemWheel(std::string_view);bool warpMenuInput(std::string_view,uint16_t,uint16_t);bool drawWarpMenu(std::string_view);bool flagRecordsInput(std::string_view,uint16_t,uint16_t);bool drawFlagRecords(std::string_view);bool actorMenuInput(std::string_view,uint16_t,uint16_t,uint16_t);bool drawActorMenu(std::string_view);uint32_t cursorColor();void drawOverlays();bool overlaysVisible();
void openHostMenu();void closeHostMenu();
void actorMenuUnloaded(std::string_view,bool);void flagRecordsDeleted();
#include "menu_reference.inc"
#include "credits.inc"
struct Page{std::string name;size_t cursor=0;};
struct Row{std::string label,destination;Control* control=nullptr;std::string help;};
static std::vector<Page> pages;
static bool menuVisible=false;
static uint16_t lastButtons=0;
static MenuButtons menuButtons;
static MenuOpeningGuard openingGuard;
static MenuCursorMemory cursorMemory;
static void pushPage(const std::string& name){pages.push_back({name,cursorMemory.get(name)});}
static void popPage(){const auto& page=pages.back();cursorMemory.remember(page.name,page.cursor);actorMenuUnloaded(page.name,true);if(page.name=="flag records")flagRecordsDeleted();pages.pop_back();}
static constexpr Sprite positionOrder[]={Menu,InputViewer,LinkDebug,Displacement,StageInfo,Timer,LoadTimer,IgtTimer,Fifo,Heap,Mash,Transform};
static int movingSprite=-1;static float moveSpeed=1;
DEFINE_HOOK_SYMBOL("JFWDisplay::endGX",void(void*),GzForeground);
bool gzMenuOpen(){return menuVisible&&!pages.empty();}
std::string_view gzCurrentPage(){return gzMenuOpen()?std::string_view(pages.back().name):std::string_view{};}
void closeMenu(){cancelComboCapture();if(!pages.empty())actorMenuUnloaded(pages.back().name,false);menuVisible=false;closeHostMenu();}
void openMenu(){
 if(speedrunBlocked())return;
 if(gzMenuOpen()){closeMenu();return;}
 closeHostMenu();if(pages.empty())pushPage("main");menuVisible=true;lastButtons=0;menuButtons={};openingGuard={};
}
static std::vector<Row> rows(){
 std::vector<Row> out;
 if(pages.empty())return out;
 const auto& name=pages.back().name;
 auto control=[&](const char* id,const char* label=nullptr){
  for(auto& c:controls)if(c.id==id){out.push_back({label?label:c.label,"",&c});return;}
 };
 auto sub=[&](const char* label,const char* dest){out.push_back({label,dest,nullptr,referenceHelp(name,label)});};
 if(name=="main"){
  sub("cheats","Cheats");sub("flags","flags");sub("inventory","inventory");
  sub("practice","practice");sub("scene","Scene");
  sub("settings","settings");sub("tools","tools");sub("warping","Warping");return out;
 }
 if(name=="Scene"){
  for(auto& c:controls)if(c.group=="Scene")out.push_back({c.label,"",&c});
  sub("actor spawner","actor spawner");sub("actor list","actor list");sub("collision viewer","Collision");sub("projection viewer","Projection");sub("trigger viewer","Triggers");sub("sound test","Sound test");return out;
 }
 if(name=="practice"){
  sub("any%","practice:any");sub("any% BiTE","practice:any_bite");sub("100%","practice:hundo");
  sub("all dungeons","practice:ad");sub("no save-quit","practice:nosq");sub("glitchless","practice:glitchless");return out;
 }else if(name=="inventory"){
  sub("item wheel","Item wheel");sub("pause menu","pause menu");sub("amounts","Amounts");return out;
 }else if(name=="pause menu"){
  sub("equipment","Equipment");sub("golden bugs","Golden bugs");sub("hidden skills","hidden skills");control("scent","scent:");return out;
 }else if(name=="hidden skills"){
  for(auto& c:controls)if(c.group=="Collection"&&c.id!="scent")out.push_back({c.label,"",&c});return out;
 }else if(name=="Amounts"){
  control("health","health:");control("hearts","hearts:");control("arrows","arrows:");
  control("bomb_count_0","bomb bag 1 num:");control("bomb_count_1","bomb bag 2 num:");control("bomb_count_2","bomb bag 3 num:");
  control("seeds","seeds:");control("heart_pieces","heart pieces:");control("poes","poes:");control("rupees","rupees:");return out;
 }else if(name=="flags"){
  sub("general flags","Flags");sub("dungeon flags","Dungeons");sub("portal flags","Portals");sub("rupee flags","rupee flags");sub("flag records","flag records");sub("flag log","Flag log");return out;
 }else if(name=="settings"){
  control("advanced_mode","advanced mode");control("reload_mode","area reload behavior:");control("cursor_color","cursor color:");
  control("font","font:");control("drop_shadows","drop shadows");control("practice_swap_equips","swap equips");
  sub("command combos","Hotkeys");sub("menu positions","Positions");control("reset_positions");sub("credits","credits");sub("developer controls","host");return out;
 }else if(name=="tools"){
  sub("checkers","Checkers");sub("controller","controller");sub("link","link");sub("scene","tools scene");sub("timers","timers");sub("rng","RNG");return out;
 }else if(name=="RNG"){
  control("freeze_rng","freeze rng values");control("advance_rng","randomize values");control("load_rng_preset","load preset");control("rng_preset","rng presets:");return out;
 }else if(name=="Checkers"){
  control("coro_td","coro td");control("ebmb","ebmb");control("elevator_escape","elevator escape");control("lfc","ladder freezard cancel");
  control("mash_checker","a/b mash rate");control("rolling","rolling");control("umd","universal map delay");control("fast_eel_regrab","fast eel regrab");control("gorge_void","gorge void");return out;
 }else if(name=="controller"){control("input_viewer","input viewer");control("turbo","turbo mode");return out;
 }else if(name=="link"){
  control("fast_bonk","fast bonk recovery");control("fast_movement","fast movement");control("link_debug","link debug info");control("stage_info","stage info");control("no_sinking","no sinking in sand");control("teleport","teleport");control("displacement","displacement");control("move_link","move link");control("tunic_color","link tunic color:");return out;
 }else if(name=="tools scene"){
  control("area_reload","area reload");control("frame_advance","frame advance");control("free_cam","free cam");control("heap_debug","heap debug info");return out;
 }else if(name=="timers"){control("timer","timer");control("load_timer","load timer");control("igt_timer","igt timer");return out;
 }else if(name=="rupee flags"){
  control("donation","donation amount:");control("fundraising","fundraising amount:");control("fundraising_1","fundraising 1");control("fundraising_2","fundraising 2");control("rupee_first_get","rupee cutscenes");return out;
 }
 if(name=="Positions"){
  for(const auto* label:{"main menu","input viewer","link debug info","displacement","stage info","timer","load timer","igt timer","fifo queue","heap info","mash checker","transform indicator"})out.push_back({label,"",nullptr,referenceHelp(name,label)});
  return out;
 }
 if(name.starts_with("practice:")){
  for(const auto& entry:practiceEntries)if(name=="practice:"+std::string(entry.category)){
   const auto id="practice_"+std::string(entry.category)+"_"+std::to_string(entry.index);
   for(auto& c:controls)if(c.id==id){out.push_back({entry.label,"",&c});break;}
  }
 }else for(auto& c:controls){
  const auto group=name=="practice"?"Practice":name=="tools"?"Tools":name.c_str();
  if(c.group==group&&!(name=="Flags"&&(c.id=="rupee_first_get"||c.id=="fundraising_1"||c.id=="fundraising_2"||c.id=="donation"||c.id=="fundraising"))&&!(name=="practice"&&c.id.starts_with("practice_")&&c.id!="practice_swap_equips"))
   out.push_back({c.label,"",&c});
 }
 auto rank=[&](const Row& row){
  int i=0;for(const auto& ref:menuReference){if(ref.page==name&&row.control&&row.control->id==ref.id)return i;++i;}
  return i;
 };
 std::stable_sort(out.begin(),out.end(),[&](const Row& a,const Row& b){return rank(a)<rank(b);});
 return out;
}
static bool disabled(const Control& c){
 return !c.reason.empty()||(c.gameOnly&&!playable())||(c.available&&!c.available());
}
void gzMenuInput(uint16_t buttons){
 if(pages.empty()){lastButtons=buttons;menuButtons={};return;}
 const uint16_t edge=buttons&~lastButtons;
 auto command=menuButtons.update(buttons,pages.back().name=="Positions"&&movingSprite>=0?3:4);
 if(!openingGuard.update(buttons))command&=uint16_t(~15);
 lastButtons=buttons;
 if(comboMenuInput(pages.back().name,buttons,command,edge)||itemWheelInput(pages.back().name,command,edge)||warpMenuInput(pages.back().name,command,edge)||flagRecordsInput(pages.back().name,command,edge))return;
 if(actorMenuInput(pages.back().name,buttons,command,edge))return;
 if(edge&0x200){if(movingSprite>=0)movingSprite=-1;else popPage();return;}
 if(pages.back().name=="Positions"){
  if(edge&0x100){movingSprite=movingSprite<0?int(positionOrder[pages.back().cursor]):-1;moveSpeed=1;return;}
  if(movingSprite>=0){
   moveSprite(static_cast<Sprite>(movingSprite),((command&2?1.f:0.f)-(command&1?1.f:0.f))*moveSpeed,
    ((command&4?1.f:0.f)-(command&8?1.f:0.f))*moveSpeed);
   if(buttons&15)moveSpeed=std::min(20.f,moveSpeed*1.05f);else moveSpeed=1;
   return;
  }
 }
 auto list=rows();if(list.empty())return;
 auto& page=pages.back();page.cursor=std::min(page.cursor,list.size()-1);
 const bool single=page.name=="main"||page.name=="inventory"||page.name=="flags"||page.name=="practice"||
  page.name=="tools"||page.name=="Cheats"||page.name=="controller"||page.name=="timers"||
  page.name=="tools scene"||page.name=="link"||page.name=="Flags"||page.name.starts_with("practice:");
 if(single)page.cursor=singleColumnMove(page.cursor,list.size(),command);
 else {
  if(command&8)page.cursor=(page.cursor+list.size()-1)%list.size();
  if(command&4)page.cursor=(page.cursor+1)%list.size();
 }
 auto row=list[page.cursor];
 if((edge&0x100)&&!row.destination.empty()){
  if(row.destination=="host"){menuVisible=false;openHostMenu();return;}
  pushPage(row.destination);return;
 }
 if(!row.control||disabled(*row.control))return;
 auto& c=*row.control;
 if((edge&0x100)&&c.action){c.action();return;}
 const int direction=c.id=="tunic_color"?(command&0x800?-1:command&0x400?1:0):(single?0:(c.kind==UI_CONTROL_SELECT||c.id=="small_keys")?(command&1?-1:command&2?1:0):listDelta(command));
 if(c.set&&c.get){
  if(c.kind==UI_CONTROL_TOGGLE){if(edge&0x100)c.set(!c.get());}
  else if(direction){
   const auto v=c.get();
   if(c.group=="Amounts"){
    const int64_t storageMax=(c.id=="health"||c.id=="hearts"||c.id=="heart_pieces"||c.id=="rupees")?65535:255;
    const auto value=wrappedValue(v,direction,0,storageMax);
    c.set((c.id=="health"||c.id=="hearts")?std::min(value,c.max):value);
   }else if(c.id=="donation"||c.id=="fundraising")c.set(std::min(wrappedValue(v,direction,0,65535),c.max));
   else if(c.id=="collision_range"){const auto value=wrappedValue(v,direction,0,65535);c.set(value>1000?0:value);}
   else if(c.id=="collision_raise"||c.id=="collision_opacity"||c.id=="trigger_opacity")c.set(wrappedValue(v,direction,0,255));
   else c.set((c.kind==UI_CONTROL_SELECT||c.id=="small_keys")?wrappedValue(v,direction,c.min,c.max):std::clamp(v+direction,c.min,c.max));
  }
 }
}
static void drawMenu(){
 if(pages.empty())return;
 auto list=rows();auto& page=pages.back();
 const float x=spritePosition(Menu).x;
 const float y=spritePosition(Menu).y;
 drawGzText("tpgz v1.2.0-dev",x+35,25,cursorColor());
 const auto icon=foregroundResourceTexture("tex/tpgz.tex");
 if(icon)foregroundQuad(icon,x,5,x+30,35,0,0,1,1,0xffffffff);
 if(page.name=="RNG")drawRngValues();
 // These custom renderers use GZ_drawMenuLines in the original too.
 if(page.name=="Hotkeys"||page.name=="Warping"||page.name=="actor list"||page.name=="actor spawner")menuScroll.update(0,12);
 if(drawCredits(page.name)||drawComboMenu(page.name)||drawItemWheel(page.name)||drawWarpMenu(page.name)||drawFlagRecords(page.name)||drawActorMenu(page.name))return;
 if(movingSprite>=0){
  const auto p=spritePosition(static_cast<Sprite>(movingSprite));
  auto cross=[&](float offset,uint32_t color){
   beginGzShape(4,GX_LINES,10);gzShapeVertex(p.x-10+offset,p.y+offset,color);gzShapeVertex(p.x+10+offset,p.y+offset,color);
   gzShapeVertex(p.x+offset,p.y-10+offset,color);gzShapeVertex(p.x+offset,p.y+10+offset,color);endGzShape();
  };
  if(on("drop_shadows"))cross(1,0x00000060);
  cross(0,(g_Counter.mCounter0/8)%2?cursorColor():0xffffffff);
 }
 if(list.empty()){drawGzText("Not implemented yet",x,y,0x999999ff);return;}
 page.cursor=std::min(page.cursor,list.size()-1);
 menuScroll.update(page.cursor,list.size());
 const auto first=menuScroll.first,last=menuScroll.last;
 float labelWidth=0;for(const auto& r:list)labelWidth=std::max(labelWidth,gzTextWidth(r.label));
 for(size_t i=first;i<std::min(list.size(),last+1);i++){
  const auto& row=list[i];std::string label=row.label,valueText;bool emptyToggle=false;
  // Save lists retain their normal colors while a load is pending.
  // Activation still uses disabled() to prevent overlapping requests.
  const bool unavailable=row.control&&(page.name.starts_with("practice:")?
   !row.control->reason.empty():disabled(*row.control));
  if(auto* c=row.control;c&&(!c->gameOnly||playable())){
   if(c->get){
    const auto value=c->get();
    if(c->kind==UI_CONTROL_TOGGLE){if(value)valueText=" [X]";else emptyToggle=true;}
    else if(c->kind==UI_CONTROL_SELECT&&value>=0&&size_t(value)<c->options.size())valueText+=" <"+c->options[value]+">";
    else valueText+=" <"+std::to_string(value)+">";
   }else if(c->getText)valueText+=" "+c->getText();
  }
  const auto color=i==page.cursor?cursorColor():unavailable?0x7f7f7fff:0xffffffff;
  const float rowY=y+20*float(i-first);
  drawGzText(label,x,rowY,color);
  if(emptyToggle){
   drawGzText(" [",x+labelWidth,rowY,color);
   drawGzText(" ]",x+labelWidth+gzTextWidth("[X"),rowY,color);
  }else drawGzText(valueText,x+labelWidth,rowY,color);
 }
 if(first)drawGzText("^",x,y-12);
 if(last+1<list.size())drawGzText("v",x,y+298);
 // Original GZ shows only the selected row's description at y=440.
 // Rows without descriptions have no fallback control legend.
 if(auto* c=list[page.cursor].control){
  const auto& help=c->reason.empty()?c->help:c->reason;
  if(!help.empty())drawGzText(help,x,440);
 }else if(!list[page.cursor].help.empty())drawGzText(list[page.cursor].help,x,440);
}
ModResult initGzMenu(){
 applyMenuReference();
 auto r=initForeground();if(r!=MOD_OK)return r;
 r=initGzFont();if(r!=MOD_OK)return r;
 return guardedPost<GzForeground>([](ModContext*,void*,void*,void*){
  if(gzMenuOpen()||overlaysVisible()){
   beginGzDraw();drawOverlays();if(gzMenuOpen())drawMenu();endGzDraw();
  }
 });
}
void shutdownGzMenu(){menuVisible=false;pages.clear();cursorMemory.rows.clear();menuScroll={};shutdownTransformIndicator();shutdownGzFont();shutdownForeground();}
}
