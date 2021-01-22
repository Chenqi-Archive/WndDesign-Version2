#include "directx_helper.h"
#include "wic_api.h"
#include "d2d_api.h"

#pragma comment(lib, "windowscodecs.lib")


BEGIN_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(Anonymous)

class WICFactory {
private:
    IWICImagingFactory2* factory;
    WICFactory();
    ~WICFactory();
public:
    static IWICImagingFactory2& Get();
};

WICFactory::WICFactory() : factory(nullptr) {
    hr = CoCreateInstance(
        CLSID_WICImagingFactory2,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory)
    );
}

WICFactory::~WICFactory() {
    SafeRelease(&factory);
}

IWICImagingFactory2& WICFactory::Get() {
    static WICFactory factory;
    return *factory.factory;
}

IWICImagingFactory2& WICFactory() { return WICFactory::Get(); }

END_NAMESPACE(Anonymous)


// helper functions
static IWICFormatConverter* LoadFromDecoder(IWICBitmapDecoder* decoder) {
	IWICFormatConverter* converter = nullptr;
	hr = WICFactory().CreateFormatConverter(&converter);

	IWICBitmapFrameDecode* source = nullptr;
	hr = decoder->GetFrame(0, &source);

	hr = converter->Initialize(
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

static IWICFormatConverter* LoadImageFromFile(const wstring& file_name) {
	IWICBitmapDecoder* decoder = nullptr;
	try {
		hr &= WICFactory().CreateDecoderFromFilename(
			file_name.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder
		);
	} catch (...) {
		return nullptr;
	}

	IWICFormatConverter* converter = nullptr;
	converter = LoadFromDecoder(decoder);
	SafeRelease(&decoder);

	return converter;
}

static IWICFormatConverter* LoadImageFromMemory(void* address, size_t size) {
	IWICStream* stream = nullptr;
	hr = WICFactory().CreateStream(&stream);
	hr = stream->InitializeFromMemory(
		reinterpret_cast<BYTE*>(address),
		static_cast<DWORD>(size)
	);

	IWICBitmapDecoder* decoder = nullptr;
	try {
		hr &= WICFactory().CreateDecoderFromStream(
			stream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);
	} catch (...) {
		SafeRelease(&stream);
		return nullptr;
	}
	SafeRelease(&stream);

	IWICFormatConverter* converter = nullptr;
	converter = LoadFromDecoder(decoder);
	SafeRelease(&decoder);

	return converter;
}


ImageResource::ImageResource(const wstring& file_name) :
	wic_image(LoadImageFromFile(file_name)) {
	LoadD2DBitmap();
}

ImageResource::ImageResource(void* address, size_t size):
	wic_image(LoadImageFromMemory(address, size)) {
	LoadD2DBitmap();
}

void ImageResource::LoadD2DBitmap() {
	if (wic_image == nullptr) { return; }
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_NONE,  // Only used as source.
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);
	hr = GetD2DDeviceContext().CreateBitmapFromWicBitmap(
		wic_image,
		&bitmap_properties,
		&d2d_bitmap
	);
}

ImageResource::~ImageResource() {
	SafeRelease(&d2d_bitmap);
	SafeRelease(&wic_image);
}

const Size ImageResource::GetSize() const {
	if (IsEmpty()) { throw std::logic_error("image file not loaded"); }
	Size size = size_empty;
	wic_image->GetSize(&size.width, &size.height);
	return size;
}

ID2D1Bitmap1& ImageResource::GetD2DBitmap() const {
	if (IsEmpty()) { throw std::logic_error("image file not loaded"); }
	return *d2d_bitmap;
}


END_NAMESPACE(WndDesign)