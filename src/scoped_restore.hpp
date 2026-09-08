#pragma once
#include <functional>
#include <vector>
#include <cstddef>
namespace gz {
struct Restore {
 std::vector<std::function<void()>> actions;
 template<class T> void hold(T& value){actions.emplace_back([&value,old=value](){value=old;});}
 template<class T,size_t N> void hold(T (&values)[N]){for(auto& value:values)hold(value);}
 ~Restore(){for(auto it=actions.rbegin();it!=actions.rend();++it)(*it)();}
};
}
