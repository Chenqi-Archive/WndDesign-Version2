#include "d2d.h"
#include "dxgi.h"
#include "directx_helper.h"


BEGIN_NAMESPACE(WndDesign)


//////////////////////////////////////////////////////////
////           Device Independent Resources           ////
//////////////////////////////////////////////////////////

BEGIN_NAMESPACE(Anonymous)

class D2DFactory {
private:
    ID2D1Factory1* factory;
    D2DFactory();
    ~D2DFactory();
public:
    static ID2D1Factory1& Get();
};

D2DFactory::D2DFactory() :factory(nullptr) {
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&factory
	);
}

D2DFactory::~D2DFactory() {
	SafeRelease(&factory);
}

ID2D1Factory1& D2DFactory::Get() {
	static D2DFactory factory;
	return *factory.factory;
}

END_NAMESPACE(Anonymous)

ID2D1Factory1& D2DFactory() { return D2DFactory::Get(); }


//////////////////////////////////////////////////////////
////            Device Dependent Resources            ////
//////////////////////////////////////////////////////////


BEGIN_NAMESPACE(Anonymous)

class D2DDeviceContext {
private:
    ID2D1DeviceContext* device_context;
    D2DDeviceContext();
    ~D2DDeviceContext();
public:
    static ID2D1DeviceContext& Get();
};

D2DDeviceContext::D2DDeviceContext() :device_context(nullptr) {
	IDXGIDevice1* dxgi_device = nullptr;
	hr = D3DDevice().QueryInterface(IID_PPV_ARGS(&dxgi_device));

	ID2D1Device* d2d_device = nullptr;
	hr = D2DFactory::Get().CreateDevice(dxgi_device, &d2d_device);
	SafeRelease(&dxgi_device);

	hr = d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &device_context);
	SafeRelease(&d2d_device);
}

D2DDeviceContext::~D2DDeviceContext() {
	SafeRelease(&device_context);
}

ID2D1DeviceContext& D2DDeviceContext::Get() {
	static D2DDeviceContext device_context;
	return *device_context.device_context;
}

END_NAMESPACE(Anonymous)

ID2D1DeviceContext& D2DDeviceContext() { return D2DDeviceContext::Get(); }


Target::Target(Size size) : bitmap(nullptr) {
    D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    D2DDeviceContext::Get().CreateBitmap(
        D2D1::SizeU(size.width, size.height),
        nullptr, 0,
        &bitmap_properties,
        &bitmap
    );
}

Target::~Target() {
    SafeRelease(&bitmap);
}


BEGIN_NAMESPACE(Anonymous)

class SolidColorBrush {
private:
    ID2D1SolidColorBrush* brush;
    SolidColorBrush();
    ~SolidColorBrush();
public:
    static ID2D1SolidColorBrush& Get(Color color);
};

SolidColorBrush::SolidColorBrush() {
    hr = D2DDeviceContext::Get().CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);
}

SolidColorBrush::~SolidColorBrush() {
    SafeRelease(&brush);
}

ID2D1SolidColorBrush& SolidColorBrush::Get(Color color) {
    static SolidColorBrush brush;
    brush.brush->SetColor(Color2COLOR(color));
    return *brush.brush;
}

END_NAMESPACE(Anonymous)

ID2D1SolidColorBrush& SolidColorBrush(Color color) { return SolidColorBrush::Get(color); }


//////////////////////////////////////////////////////////
////            Window Dependent Resources            ////
//////////////////////////////////////////////////////////

WindowResources::WindowResources(HANDLE hwnd) {
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

    hr = DXGIFactory().CreateSwapChainForHwnd(
        &D3DDevice(),
        static_cast<HWND>(hwnd),
        &swapChainDesc,
        nullptr,
        nullptr,
        &swap_chain
    );

    CreateBitmapTarget();
}

WindowResources::~WindowResources() {
    DestroyBitmapTarget();
    SafeRelease(&swap_chain);
}

void WindowResources::CreateBitmapTarget() {
    IDXGISurface* dxgi_surface = nullptr;
    hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));

    D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    hr = D2DDeviceContext::Get().CreateBitmapFromDxgiSurface(
        dxgi_surface,
        &bitmap_properties,
        &bitmap
    );
    SafeRelease(&dxgi_surface);
}

void WindowResources::DestroyBitmapTarget() {
    SafeRelease(&bitmap);
}

void WindowResources::OnResize() {
    DestroyBitmapTarget();
    hr = swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    CreateBitmapTarget();
}


END_NAMESPACE(WndDesign)
