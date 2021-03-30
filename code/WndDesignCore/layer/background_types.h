#pragma once

#include "background_base.h"


BEGIN_NAMESPACE(WndDesign)


struct NullBackground : public Background {
private:
	NullBackground() {}
public:
	static const NullBackground& Get();
	virtual void Clear(Rect region, RenderTarget& target, Vector offset) const override;  // defined in figure_types.cpp
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset) const override {}
};


struct BackgroundFigure : Figure {
	const Background& background;
	bool clear;
	Rect region;
	BackgroundFigure(const Background& background, Rect region, bool clear) :
		background(background), clear(clear), region(region) {
	}
	virtual const Rect GetRegion() const override { return Rect(point_zero, region.size); }
	virtual void DrawOn(RenderTarget& target, Vector offset) const override {
		clear ? background.Clear(region, target, offset) : background.DrawOn(region, target, offset);
	}
};


END_NAMESPACE(WndDesign)