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


WindowResource::WindowResource(HANDLE hwnd) : 
    swap_chain(nullptr), target(), has_presented(false) {
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = { 0 };
    swap_chain_desc.Width = 0;   // width and height will be calculated automatically
    swap_chain_desc.Height = 0;
    swap_chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swap_chain_desc.Stereo = false;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.Scaling = DXGI_SCALING_NONE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swap_chain_desc.Flags = 0;
    swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    hr = GetDXGIFactory().CreateSwapChainForHwnd(
        &GetD3DDevice(),
        static_cast<HWND>(hwnd),
        &swap_chain_desc,
        nullptr,
        nullptr,
        &swap_chain
    );

    target.Create(*swap_chain);
}

WindowResource::~WindowResource() {
    target.Destroy();
    SafeRelease(&swap_chain);
}

void WindowResource::WindowTarget::Create(IDXGISwapChain1& swap_chain) {
    IDXGISurface* dxgi_surface = nullptr;
    hr = swap_chain.GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));

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

void WindowResource::WindowTarget::Destroy() {
    SafeRelease(&bitmap);
}

void WindowResource::OnResize(Size size) {
    target.Destroy();
    hr = swap_chain->ResizeBuffers(0, size.width, size.height, DXGI_FORMAT_UNKNOWN, 0);
    target.Create(*swap_chain);
    has_presented = false;
}

void WindowResource::Present(vector<Rect>&& dirty_regions) {
    DXGI_PRESENT_PARAMETERS present_parameters = {};
    if (has_presented) {
        static_assert(sizeof(RECT) == sizeof(Rect));  // In-place convert Rect to RECT.
        for (auto& region : dirty_regions) {
            *reinterpret_cast<RECT*>(&region) = { region.left(), region.top(), region.right(), region.bottom() };
        }
        present_parameters.DirtyRectsCount = (uint)dirty_regions.size();
        present_parameters.pDirtyRects = reinterpret_cast<RECT*>(dirty_regions.data());
    } else {
        // The entire region must be presented for the first time.
        has_presented = true;
    }
    hr = swap_chain->Present1(0, 0, &present_parameters);
}


END_NAMESPACE(WndDesign)