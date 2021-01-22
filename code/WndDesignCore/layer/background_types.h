#pragma once

#include "background.h"
#include "../figure/figure_types.h"
#include "../figure/image.h"
#include "../geometry/rect_point_iterator.h"
#include "../geometry/geometry_helper.h"


BEGIN_NAMESPACE(WndDesign)


struct SolidColorBackground : Background {
	Color color;

	SolidColorBackground(Color color) : color(color) {}
	virtual void Clear(Rect region, RenderTarget& target, Vector offset) const override;  // defined in figure_types.cpp
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset, uchar opacity) const override;
};


struct ImageTileBackground : Background {
	const Image& image;
	uchar opacity;
	Vector offset_on_image;

	ImageTileBackground(const Image& image, uchar opacity = 0xFF, Vector offset_on_image = vector_zero) :
		image(image), opacity(opacity), offset_on_image(offset_on_image) {
	}
	virtual void Clear(Rect region, RenderTarget& target, Vector offset) const override;
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset, uchar opacity) const override;
};


END_NAMESPACE(WndDesign)