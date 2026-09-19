#pragma once
#include "mods/api.h"
#include "SSystem/SComponent/c_xyz.h"
#include "upstream_samples.hpp"
#include <cstdint>
namespace gz {
ModResult initInterpolation();
struct InterpolationClock {
 static bool should_capture();
 static bool is_enabled();
 static bool is_presentation_active();
 static uint64_t sim_tick_seq();
 static uint64_t presentation_epoch();
 static float get_interpolation_step();
};
struct PositionLerp {
 void operator()(cXyz& out,const cXyz& previous,const cXyz& current,float step) const {
  out.x=previous.x+(current.x-previous.x)*step;
  out.y=previous.y+(current.y-previous.y)*step;
  out.z=previous.z+(current.z-previous.z)*step;
 }
};
using PositionSamples=upstream::Samples<cXyz,InterpolationClock,PositionLerp>;
}
