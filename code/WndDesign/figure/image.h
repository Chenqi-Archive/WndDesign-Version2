#pragma once

#include "figure_base.h"
#include "color.h"
#include "../common/uncopyable.h"

#include <string>


BEGIN_NAMESPACE(WndDesign)

using std::wstring;


class Image : Uncopyable {
private:
	friend struct ImageFigure;
	alloc_ptr<void> wic_image;
	alloc_ptr<void> d2d_bitmap;
private:
	void LoadD2DBitmap();
public:
	Image(const wstring& file_name);
	Image(void* address, size_t size);
	~Image();
	const Size GetSize() const;
};


struct ImageFigure : Figure {
	const Image& image;
	uchar opacity;
	Rect region_on_image;

	ImageFigure(const Image& image, uchar opacity = 0xFF, Rect region_on_image = region_infinite) :
		image(image), opacity(opacity), region_on_image(region_on_image.Intersect(Rect(point_zero, image.GetSize()))) {
	}
	virtual const Rect GetRegion() const override {	return Rect(point_zero, image.GetSize()); }
	virtual void DrawOn(RenderTarget& target, Vector offset) const override; // defined in figure_types.cpp
};


END_NAMESPACE(WndDesign)