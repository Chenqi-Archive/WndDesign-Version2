#pragma once

#include "color.h"
#include "../geometry/geometry.h"


BEGIN_NAMESPACE(WndDesign)

class DeviceContext;  // An alias for ID2D1DeviceContext.

class Image;
class TextBlock;
class Layer;


// The Figure abstract base class.
// It issues drawing commands to the target (attached to device context).
struct Figure {
	// Do any drawing operation on the device context, whose target must have already been set.
	virtual void DrawOn(DeviceContext& target, Vector offset) const pure;
	// virtual ~Figure() pure {}  // Doesn't need yet.
};


struct Line : Figure {
	Point begin, end;
	Color color;
	float width;
	// stroke style ...

	Line(Point begin, Point end, Color color, float width) :
		begin(begin), end(end), color(color), width(width) {
	}
	virtual void DrawOn(DeviceContext& target, Vector offset) const override;
};

struct Rectangle : Figure {
	Size size;
	float border_width;
	Color border_color;
	Color fill_color;

	Rectangle(Size size, float border_width, Color border_color, Color fill_color) :
		size(size), border_width(border_width), border_color(border_color), fill_color(fill_color) {
	}
	virtual void DrawOn(DeviceContext& target, Vector offset) const override;
};

struct RoundedRectangle : Figure {
	Size size;
	uint radius;
	float border_width;
	Color border_color;
	Color fill_color;

	RoundedRectangle(Size size, uint radius, float border_width, Color border_color, Color fill_color) :
		size(size), radius(radius), border_width(border_width), border_color(border_color), fill_color(fill_color) {
	}
	virtual void DrawOn(DeviceContext& target, Vector offset) const override;
};

struct ImageFigure : Figure {
	const Image& image;
	uchar opacity;
	Rect region_on_image;

	ImageFigure(const Image& image, uchar opacity = 0xFF, Rect region_on_image = region_infinite) :
		image(image), opacity(opacity), region_on_image(region_on_image) {
	}
	virtual void DrawOn(DeviceContext& target, Vector offset) const override;
};

struct TextBlockFigure : Figure {
	const TextBlock& text_block;

	TextBlockFigure(const TextBlock& text_block) : text_block(text_block) {}
	virtual void DrawOn(DeviceContext& target, Vector offset) const override;
};

struct LayerFigure : Figure {
	const Layer& layer;
	Rect region_on_layer;

	LayerFigure(const Layer& image, Rect region_on_layer = region_infinite) :
		layer(layer), region_on_layer(region_on_layer) {
	}
	virtual void DrawOn(DeviceContext& target, Vector offset) const override;
};


END_NAMESPACE(WndDesign)