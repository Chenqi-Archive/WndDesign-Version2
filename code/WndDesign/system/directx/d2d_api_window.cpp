#include "directx_helper.h"
#include "d2d_api_window.h"
#include "dxgi_api.h"


//////////////////////////////////////////////////////////
////            Window Dependent Resources            ////
//////////////////////////////////////////////////////////


BEGIN_NAMESPACE(WndDesign)


inline const Size GetWindowSize(HANDLE hwnd) {
    RECT rc;
    GetClientRect(static_cast<HWND>(hwnd), &rc);
    return Size(rc.right - rc.left, rc.bottom - rc.top);
}


WindowResource::WindowResource(HANDLE hwnd) : hwnd(hwnd), target(GetWindowSize(hwnd)){
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.Flags = 0;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    hr = GetDXGIFactory().CreateSwapChainForHwnd(
        &GetD3DDevice(),
        static_cast<HWND>(hwnd),
        &swapChainDesc,
        nullptr,
        nullptr,
        &swap_chain
    );

    CreateBitmapTarget();
}

WindowResource::~WindowResource() {
    DestroyBitmapTarget();
    SafeRelease(&swap_chain);
}

void WindowResource::CreateBitmapTarget() {
    IDXGISurface* dxgi_surface = nullptr;
    hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));

    D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    hr = GetD2DDeviceContext().CreateBitmapFromDxgiSurface(
        dxgi_surface,
        &bitmap_properties,
        &bitmap
    );
    SafeRelease(&dxgi_surface);
}

void WindowResource::DestroyBitmapTarget() {
    SafeRelease(&bitmap);
}

void WindowResource::OnResize(Size size) {
    DestroyBitmapTarget();
    target.SetSize(size);
    hr = swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    CreateBitmapTarget();
}

const Size WindowResource::GetSize() { 
    return SIZE2Size(bitmap->GetSize()); 
}

void WindowResource::Present(Rect region) {
    PushDraw();
    target.EndDraw();
    auto& device_context = GetD2DDeviceContext();
    device_context.SetTarget(bitmap);
    device_context.DrawBitmap(
        &target.GetBitmap(),
        Rect2RECT(region),
        1.0F,
        D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        Rect2RECT(region)
    );
    PopDraw();

    DXGI_PRESENT_PARAMETERS present_parameters = {};
    if (region.size != SIZE2Size(bitmap->GetSize())) {
        *reinterpret_cast<RECT*>(&region) = { region.left(), region.top(), region.right(), region.bottom() };
        present_parameters.DirtyRectsCount = 1;
        present_parameters.pDirtyRects = reinterpret_cast<RECT*>(&region);
    }
    hr = swap_chain->Present1(0, 0, &present_parameters);
}


END_NAMESPACE(WndDesign)