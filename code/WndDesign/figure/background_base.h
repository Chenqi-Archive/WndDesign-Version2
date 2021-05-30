#pragma once

#include "figure_base.h"


BEGIN_NAMESPACE(WndDesign)


// Background abstract base class.
// Background is an infinitly large "figure" that can be drawn on target.
struct ABSTRACT_BASE Background {
	// Drawn as a figure.
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset) const pure;

	// Background may contain allocated resources, like Image.
	virtual ~Background() pure {}
};


struct BackgroundFigure : Figure {
	const Background& background;
	Rect region;
	BackgroundFigure(const Background& background, Rect region) :background(background), region(region) {}
	virtual const Rect GetRegion() const override { return Rect(point_zero, region.size); }
	virtual void DrawOn(RenderTarget& target, Vector offset) const override {
		background.DrawOn(region, target, offset);
	}
};


END_NAMESPACE(WndDesign)