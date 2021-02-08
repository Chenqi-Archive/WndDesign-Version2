#pragma once

#include "../figure/figure_base.h"


BEGIN_NAMESPACE(WndDesign)


// Background abstract base class.
// Background is an infinitly large "figure" that can be drawn on target.
struct ABSTRACT_BASE Background {
	// Drawn directly to the layer.
	virtual void Clear(Rect region, RenderTarget& target, Vector offset) const pure;

	// Drawn as a figure.
	// "opacity" is only used at composition time when tile is not allocated.
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset, uchar opacity = 0xFF) const pure;

	// Background may contain allocated resources, like Image.
	virtual ~Background() pure {}
};


END_NAMESPACE(WndDesign)