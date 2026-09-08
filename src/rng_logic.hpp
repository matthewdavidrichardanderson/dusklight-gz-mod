#pragma once
#include <array>
#include <cstdint>
namespace gz {
using RngState=std::array<int32_t,3>;
constexpr RngState advanceRng(RngState state) {
 return {static_cast<int32_t>((int64_t(state[0])*171)%30269),
         static_cast<int32_t>((int64_t(state[1])*172)%30307),
         static_cast<int32_t>((int64_t(state[2])*170)%30323)};
}
}
