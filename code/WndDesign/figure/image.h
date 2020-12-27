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
		image(image), opacity(opacity), region_on_image(region_on_image) {
	}
	virtual void DrawOn(RenderTarget& target, Vector offset) const override; // defined in figure_types.cpp
};


END_NAMESPACE(WndDesign)