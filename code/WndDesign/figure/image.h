#pragma once

#include "figure_base.h"
#include "color.h"

#include <string>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::wstring;
using std::shared_ptr;

class ImageResource;


class Image {
private:
	shared_ptr<ImageResource> image;

public:
	Image() : image(nullptr) {}
	Image(const wstring& file_name): image(nullptr) { Load(file_name); }
	Image(void* address, size_t size): image(nullptr) { Load(address, size); }
	~Image() {}

	void Load(const wstring& file_name);
	void Load(void* address, size_t size);

	void Clear() { image.reset(); }
	bool IsEmpty() const;

	const ImageResource& GetImageResource() const { return *image; }
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