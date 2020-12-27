#pragma once

#include "../geometry/geometry.h"
#include "composite_style.h"

BEGIN_NAMESPACE(WndDesign)

class RenderTarget;  // An alias for ID2D1DeviceContext.


// Background abstract base class.
// Background is an infinitly large "figure" that can be drawn on target.
struct Background {
	virtual void DrawOn(Rect region, uchar opacity, RenderTarget& target, Vector offset) const pure;
};


END_NAMESPACE(WndDesign)