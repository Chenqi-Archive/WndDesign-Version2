#pragma once

#include "../../common/common.h"
#include "../../common/uncopyable.h"
#include "../../geometry/geometry.h"
#include "../../figure/color.h"

#include <string>


struct IWICFormatConverter;
struct IWICBitmap;
struct IWICBitmapLock;

struct ID2D1Bitmap1;


BEGIN_NAMESPACE(WndDesign)

using std::wstring;


class ImageResource : Uncopyable {
private:
	IWICFormatConverter* wic_image;
	ID2D1Bitmap1* d2d_bitmap;

	void LoadD2DBitmap();

public:
	ImageResource(const wstring& file_name);
	ImageResource(void* address, size_t size);
	~ImageResource();

	bool IsEmpty() const { return wic_image == nullptr; }
	const Size GetSize() const;

	ID2D1Bitmap1& GetD2DBitmap() const;
};


END_NAMESPACE(WndDesign)