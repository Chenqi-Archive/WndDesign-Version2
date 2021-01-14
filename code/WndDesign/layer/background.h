#pragma once

#include "../figure/figure_base.h"


BEGIN_NAMESPACE(WndDesign)

class RenderTarget;  // An alias for ID2D1DeviceContext.


// Background abstract base class.
// Background is an infinitly large "figure" that can be drawn on target.
struct Background {
	// Drawn directly to the layer.
	virtual void Clear(Rect region, RenderTarget& target, Vector offset) const pure;

	// Drawn as a figure.
	// "opacity" is only used at composition time when tile is not allocated.
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset, uchar opacity = 0xFF) const pure;
};


struct BackgroundFigure : Figure {
	const Background& background;
	Rect region;
	BackgroundFigure(const Background& background, Rect region) : background(background), region(region) {}
	virtual void DrawOn(RenderTarget& target, Vector offset) const override {
		background.DrawOn(region, target, offset);
	}
};


END_NAMESPACE(WndDesign)