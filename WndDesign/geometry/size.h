#pragma once

#include "../common/common.h"


BEGIN_NAMESPACE(WndDesign)


struct Size {
	uint width;
	uint height;

	explicit constexpr Size() : width(0), height(0) {}
	explicit constexpr Size(uint width, uint height) : width(width), height(height) {}

	bool operator==(const Size& size) const { return width == size.width && height == size.height; }
	bool operator!=(const Size& size) const { return width != size.width || height != size.height; }

	bool IsEmpty() const { return width == 0 || height == 0; }
};


END_NAMESPACE(WndDesign)