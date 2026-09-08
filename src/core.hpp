#pragma once
#include "mods/service.hpp"
#include "mods/svc/config.h"
#include "mods/svc/hook.hpp"
#include "mods/svc/log.h"
#include "mods/svc/ui.h"
#include "mods/svc/host.h"
#include <deque>
#include <vector>
#include <functional>
#include <string>
#include <cstdint>
#include "hook_guard.hpp"
namespace gz {
bool speedrunBlocked();
ModResult initSpeedrunGuard();
void speedrunTick();
void suspendGz();
struct Control {
 std::string id, group, label, help;
 UiControlKind kind = UI_CONTROL_TOGGLE;
 ConfigVarHandle config = 0;
 int64_t min = 0, max = 1;
 std::function<int64_t()> get;
 std::function<void(int64_t)> set;
 std::function<std::string()> getText;
 std::function<void(const std::string&)> setText;
 std::function<void()> action;
 std::function<bool()> available;
 std::string cachedText;
 std::vector<std::string> options;
 std::vector<const char*> optionPointers;
 bool gameOnly = true;
 bool value = false;
 std::string reason;
};
extern std::deque<Control> controls;
Control& toggle(const char* id, const char* group, const char* label, const char* help, bool initial=false);
Control& number(const char* id, const char* group, const char* label, int64_t min, int64_t max, std::function<int64_t()> get, std::function<void(int64_t)> set);
Control& action(const char* id, const char* group, const char* label, std::function<void()> fn);
Control& textInput(const char* id, const char* group, const char* label, std::function<std::string()> get, std::function<void(const std::string&)> set);
Control& choice(const char* id,const char* group,const char* label,const std::vector<const char*>& options,std::function<int64_t()> get,std::function<void(int64_t)> set);
bool on(const char* id);
bool playable();
void notify(const std::string& message);
void refresh();
void openMenu();void closeMenu();
ModResult initUi();
ModResult initCheats();
ModResult initInput();
void inputTick();
void cheatTick();
void shutdownCheats();
void initInventory();
void initFlags();
void initScene();
void sceneTick();
void shutdownScene();
void initPractice();
void practiceTick();
}
