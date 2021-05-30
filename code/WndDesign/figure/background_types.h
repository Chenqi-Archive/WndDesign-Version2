#pragma once

#include "background_base.h"
#include "image.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::shared_ptr;


shared_ptr<Background> GetNullBackground();


struct SolidColorBackground : Background {
	Color color;

	SolidColorBackground(Color color) : color(color) {}
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset) const override;
};


struct ImageRepeatBackground : Background {
	const Image& image;
	uchar opacity;
	Vector offset_on_image;

	ImageRepeatBackground(const Image& image, uchar opacity = 0xFF, Vector offset_on_image = vector_zero) :
		image(image), opacity(opacity), offset_on_image(offset_on_image) {
	}
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset) const override;
};


END_NAMESPACE(WndDesign)