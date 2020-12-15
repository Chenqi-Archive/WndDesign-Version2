#pragma once

#include "color.h"
#include "../geometry/geometry.h"

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


END_NAMESPACE(WndDesign)