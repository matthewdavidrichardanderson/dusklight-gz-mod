#include "core.hpp"
#include "loading.hpp"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_player.h"
#include <algorithm>
namespace gz {
std::deque<Control> controls;
bool playable() { return !speedrunBlocked() && !sceneLoading() && daPy_getPlayerActorClass() != nullptr && !dComIfGp_isEnableNextStage(); }
Control& toggle(const char* id,const char* group,const char* label,const char* help,bool initial) {
 controls.emplace_back(); auto& c=controls.back(); c.id=id;c.group=group;c.label=label;c.help=help;
 ConfigVarDesc d=CONFIG_VAR_DESC_INIT;d.name=id;d.type=CONFIG_VAR_BOOL;d.default_bool=initial;
 if(svc_config->register_var(mod_ctx,&d,&c.config)!=MOD_OK) { c.reason="Configuration registration failed"; }
 c.get=[&c] { bool v=false; svc_config->get_bool(mod_ctx,c.config,&v);return int64_t(v); };
 c.set=[&c](int64_t v) {svc_config->set_bool(mod_ctx,c.config,v!=0);c.value=v!=0;};
 c.gameOnly=false; return c;
}
Control& number(const char* id,const char* group,const char* label,int64_t min,int64_t max,std::function<int64_t()> get,std::function<void(int64_t)> set) {
 controls.emplace_back();auto& c=controls.back();c.id=id;c.group=group;c.label=label;c.kind=UI_CONTROL_NUMBER;
 c.min=min;c.max=max;c.get=std::move(get);c.set=std::move(set);return c;
}
Control& action(const char* id,const char* group,const char* label,std::function<void()> fn) {
 controls.emplace_back();auto& c=controls.back();c.id=id;c.group=group;c.label=label;c.kind=UI_CONTROL_BUTTON;c.action=std::move(fn);return c;
}
Control& textInput(const char* id,const char* group,const char* label,std::function<std::string()> get,std::function<void(const std::string&)> set) {
 controls.emplace_back();auto& c=controls.back();c.id=id;c.group=group;c.label=label;c.kind=UI_CONTROL_STRING;
 c.getText=std::move(get);c.setText=std::move(set);return c;
}
Control& choice(const char* id,const char* group,const char* label,const std::vector<const char*>& options,std::function<int64_t()> get,std::function<void(int64_t)> set) {
 auto& c=number(id,group,label,0,static_cast<int64_t>(options.size())-1,std::move(get),std::move(set));
 c.kind=UI_CONTROL_SELECT;for(auto option:options)c.options.emplace_back(option);return c;
}
bool on(const char* id) { if(speedrunBlocked())return false; for(auto& c:controls) if(c.id==id) return c.value && c.reason.empty();return false; }
void refresh() {for(auto& c:controls) if(c.config) c.value=c.get()!=0;}
void notify(const std::string& message) {
 svc_log->info(mod_ctx,message.c_str());
 UiToastDesc d=UI_TOAST_DESC_INIT;d.title_rml="Dusk GZ";std::string escaped;
 for(char c:message) {if(c=='&')escaped+="&amp;";else if(c=='<')escaped+="&lt;";else if(c=='>')escaped+="&gt;";else escaped+=c;}
 d.body_rml=escaped.c_str();svc_ui->push_toast(mod_ctx,&d);
}
}
