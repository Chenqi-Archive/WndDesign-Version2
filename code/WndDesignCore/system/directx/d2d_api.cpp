#include "directx_helper.h"
#include "d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


void BeginDraw() {
    auto& device_context = GetD2DDeviceContext();
    device_context.BeginDraw();
}

void EndDraw() {
    auto& device_context = GetD2DDeviceContext();
    hr = device_context.EndDraw();
    device_context.SetTarget(nullptr);
}


inline ID2D1Bitmap1* D2DCreateBitmap(Size size) {
    ID2D1Bitmap1* bitmap;
    D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    hr = GetD2DDeviceContext().CreateBitmap(
        D2D1::SizeU(size.width, size.height),
        nullptr, 0,
        &bitmap_properties,
        &bitmap
    );
    return bitmap;
}


Target::Target(Size size) : bitmap(D2DCreateBitmap(size)) {}

Target::~Target() { SafeRelease(&bitmap); }


END_NAMESPACE(WndDesign)