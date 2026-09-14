#pragma once
#include <cstdint>
namespace gz {
inline constexpr int kMaxRenderedChainLinks = 600;
inline constexpr float chainRenderStep(float distance) {
 const float budgetStep=distance/static_cast<float>(kMaxRenderedChainLinks - 1);
 return budgetStep>5.0f?budgetStep:5.0f;
}
}
