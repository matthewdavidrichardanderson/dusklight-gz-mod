#include "core.hpp"
#include <array>
#include <algorithm>
namespace gz {
static UiWindowHandle window=0;
static bool disabled(ModContext*,void* u) {
 auto& c=*static_cast<Control*>(u);
 return speedrunBlocked() || !c.reason.empty() || (c.gameOnly&&!playable()) || (c.available&&!c.available());
}
static void get(ModContext*,void* u,UiControlValue* v) {
 auto& c=*static_cast<Control*>(u);
 if(c.gameOnly&&!playable())return;
 if(c.get){v->int_value=c.get();v->bool_value=v->int_value!=0;}
 if(c.getText){c.cachedText=c.getText();v->string_value=c.cachedText.c_str();}
}
static void set(ModContext*,void* u,const UiControlValue* v) {
 if(disabled(nullptr,u))return;
 auto& c=*static_cast<Control*>(u);
 if(c.set)c.set(c.kind==UI_CONTROL_TOGGLE?v->bool_value:std::clamp(v->int_value,c.min,c.max));
 if(c.setText&&v->string_value)c.setText(v->string_value);
}
static void pressed(ModContext*,void* u) {if(!disabled(nullptr,u)){auto& c=*static_cast<Control*>(u);if(c.action)c.action();}}
static ModResult buildGroup(ModContext*,UiElementHandle pane,void* u,ModError*) {
 const char* group=static_cast<const char*>(u);
 for(auto& c:controls)if(c.group==group) {
  UiControlDesc d=UI_CONTROL_DESC_INIT;d.label=c.label.c_str();d.kind=c.kind;d.help_rml=c.help.c_str();
  d.get=get;d.set=set;d.on_pressed=pressed;d.user_data=&c;d.is_disabled=disabled;d.min=c.min;d.max=c.max;d.step=1;
  if(!c.options.empty()){
   c.optionPointers.clear();for(const auto& label:c.options)c.optionPointers.push_back(label.c_str());
   d.options=c.optionPointers.data();d.option_count=c.optionPointers.size();
  }
  auto r=svc_ui->pane_add_control(mod_ctx,pane,&d,nullptr);if(r!=MOD_OK)return r;
  if(!c.reason.empty())svc_ui->pane_add_text(mod_ctx,pane,c.reason.c_str(),nullptr);
 }
 return MOD_OK;
}
static const char* groups[]={"Cheats","Amounts","Equipment","Item wheel","Collection","Golden bugs","Flags","Flag log","Portals","Dungeons","Warping","Practice","Tools","RNG","Scene","Collision","Projection","Triggers","Sound test","Checkers","Settings","Hotkeys"};
static ModResult buildTab(ModContext*,UiWindowHandle,UiElementHandle left,UiElementHandle,void* u,ModError* e) {return buildGroup(mod_ctx,left,u,e);}
void closeHostMenu(){if(window){svc_ui->window_close(mod_ctx,window);window=0;}}
void openHostMenu() {
 if(speedrunBlocked())return;
 if(window){svc_ui->window_close(mod_ctx,window);window=0;return;}
 std::array<UiTabDesc,std::size(groups)> tabs;
 for(size_t i=0;i<tabs.size();i++){tabs[i]=UI_TAB_DESC_INIT;tabs[i].title=groups[i];tabs[i].build=buildTab;tabs[i].user_data=const_cast<char*>(groups[i]);}
 UiWindowDesc d=UI_WINDOW_DESC_INIT;d.tabs=tabs.data();d.tab_count=tabs.size();
 d.on_closed=[](ModContext*,UiWindowHandle,void*){window=0;};
 auto r=svc_ui->window_push(mod_ctx,&d,&window);if(r!=MOD_OK)notify("Cannot open GZ menu");
}
// Render credits as a separate block: host descriptions collapse newlines.
ModResult initUi() {
 UiModsPanelDesc panel=UI_MODS_PANEL_DESC_INIT;
 panel.build=[](ModContext*,UiElementHandle pane,void*,ModError*) {
  return svc_ui->pane_add_text(mod_ctx,pane,"credit goes to tpgz devs",nullptr);
 };
 return svc_ui->register_mods_panel(mod_ctx,&panel);
}
}
