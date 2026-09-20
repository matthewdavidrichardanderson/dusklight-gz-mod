#pragma once

namespace gz {
// Dusklight mirrors the projected image while leaving raw pad axes unchanged.
bool mirrorModeEnabled();
inline int screenHorizontal(int raw, bool mirrored) { return mirrored ? -raw : raw; }
}
