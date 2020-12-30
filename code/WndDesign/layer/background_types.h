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
	virtual void DrawOn(Rect region, uchar opacity, RenderTarget& target, Vector offset) const override {
		Rectangle rectangle(region.size, BlendColorWithOpacity(color, opacity));
		rectangle.DrawOn(target, offset);
	}
};

struct ImageTileBackground : Background {
	const Image& image;
	uchar opacity;
	Vector offset_on_image;

	ImageTileBackground(const Image& image, uchar opacity = 0xFF, Vector offset_on_image = vector_zero) :
		image(image), opacity(opacity), offset_on_image(offset_on_image) {
	}
	virtual void DrawOn(Rect region, uchar opacity, RenderTarget& target, Vector offset) const override {

	}
};


END_NAMESPACE(WndDesign)