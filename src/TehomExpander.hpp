#pragma once
#include "plugin.hpp"

// Message sent each sample from Tehom to an adjacent Tehom Out expander
// via Rack's double-buffered expander messages.
//
//   pre  = each deck's signal after its Source crossfade, before the XY mixer
//   post = each deck's contribution to the main mix (XY mixer level, and pan
//          when "XY pad pans audio" is on), before the global Tone/saturation
//          and noise stages, which act on the summed mix
struct TehomOutMessage {
	float preL[4] = {};
	float preR[4] = {};
	float postL[4] = {};
	float postR[4] = {};
};
