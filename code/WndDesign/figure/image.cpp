#include "image.h"
#include "../system/directx/directx_helper.h"
#include "../system/directx/d2d_api.h"
#include "../system/directx/wic_api.h"


BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)


//// helper functions ////

IWICFormatConverter* LoadFromDecoder(IWICBitmapDecoder* decoder) {
	IWICFormatConverter* converter = nullptr;
	hr << GetWICFactory().CreateFormatConverter(&converter);

	IWICBitmapFrameDecode* source = nullptr;
	hr << decoder->GetFrame(0, &source);

	hr << converter->Initialize(
		source,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeMedianCut
	);
	SafeRelease(&source);

	return converter;
}

IWICFormatConverter* LoadImageFromFile(const wstring& file_name) {
	IWICBitmapDecoder* decoder = nullptr;

	try {
		hr << GetWICFactory().CreateDecoderFromFilename(
			file_name.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder
		);
	} catch (std::runtime_error&) {
		throw std::invalid_argument("invalid iamge file");
	}

	IWICFormatConverter* converter = nullptr;
	converter = LoadFromDecoder(decoder);
	SafeRelease(&decoder);

	return converter;
}

IWICFormatConverter* LoadImageFromMemory(void* address, size_t size) {
	IWICStream* stream = nullptr;
	hr << GetWICFactory().CreateStream(&stream);
	hr << stream->InitializeFromMemory(
		reinterpret_cast<BYTE*>(address),
		static_cast<DWORD>(size)
	);

	IWICBitmapDecoder* decoder = nullptr;
	try {
		hr << GetWICFactory().CreateDecoderFromStream(
			stream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);
	} catch (std::runtime_error&) {
		SafeRelease(&stream);
		throw std::invalid_argument("invalid iamge address");
	}
	SafeRelease(&stream);

	IWICFormatConverter* converter = nullptr;
	converter = LoadFromDecoder(decoder);
	SafeRelease(&decoder);

	return converter;
}


IWICFormatConverter* AsIWICFormatConverter(void* wic_image) { return reinterpret_cast<IWICFormatConverter*>(wic_image); }
ID2D1Bitmap1* AsID2D1Bitmap1(void* d2d_bitmap) { return reinterpret_cast<ID2D1Bitmap1*>(d2d_bitmap); }
IWICFormatConverter** AsIWICFormatConverter(void** wic_image) { return reinterpret_cast<IWICFormatConverter**>(wic_image); }
ID2D1Bitmap1** AsID2D1Bitmap1(void** d2d_bitmap) { return reinterpret_cast<ID2D1Bitmap1**>(d2d_bitmap); }


END_NAMESPACE(Anonymous)


Image::Image(const wstring& file_name) :
	wic_image(LoadImageFromFile(file_name)) {
	LoadD2DBitmap();
}

Image::Image(void* address, size_t size) :
	wic_image(LoadImageFromMemory(address, size)) {
	LoadD2DBitmap();
}

void Image::LoadD2DBitmap() {
	if (wic_image == nullptr) { return; }
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_NONE,  // Only used as source.
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);
	hr << GetD2DDeviceContext().CreateBitmapFromWicBitmap(
		AsIWICFormatConverter(wic_image),
		&bitmap_properties,
		AsID2D1Bitmap1(&d2d_bitmap)
	);
}

Image::~Image() {
	SafeRelease(AsID2D1Bitmap1(&d2d_bitmap));
	SafeRelease(AsIWICFormatConverter(&wic_image));
}

const Size Image::GetSize() const {
	Size size = size_empty;
	AsIWICFormatConverter(wic_image)->GetSize(&size.width, &size.height);
	return size;
}


END_NAMESPACE(WndDesign)