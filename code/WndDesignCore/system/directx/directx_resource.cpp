#include "directx_resource.h"
#include "directx_helper.h"


#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")


BEGIN_NAMESPACE(WndDesign)


DirectXResources::DirectXResources() {
    // Create D3D resources.
    D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
    };
    D3D_FEATURE_LEVEL featureLevel;
    
#pragma message(Remark"Use Microsoft::WRL::ComPtr(#include <wrl/client.h>) or std::unique_ptr to wrap COM objects.")
    hr << D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT
    #ifdef _DEBUG
        | D3D11_CREATE_DEVICE_DEBUG
    #endif
        ,
        featureLevels, ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &d3d_device,
        &featureLevel,
        nullptr
    );
    hr << d3d_device->QueryInterface(IID_PPV_ARGS(&dxgi_device));
    hr << dxgi_device->GetAdapter(&dxgi_adapter);
    hr << dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory));


    // Create D2D factory.
    hr << D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory);

	// Create D2D device context.
	ID2D1Device* d2d_device = nullptr;
	hr << d2d_factory->CreateDevice(dxgi_device, &d2d_device);

	hr << d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d_device_context);
	SafeRelease(&d2d_device);

    hr << d2d_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &d2d_solid_color_brush);
}

DirectXResources::~DirectXResources() {
    SafeRelease(&d2d_solid_color_brush);
    SafeRelease(&d2d_device_context);
	SafeRelease(&d2d_factory);

    SafeRelease(&dxgi_factory);
    SafeRelease(&dxgi_adapter);
    SafeRelease(&dxgi_device);
#ifdef _DEBUG
    ID3D11Debug* d3d_debug;
    hr << d3d_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3d_debug));
    d3d_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    SafeRelease(&d3d_debug);
#endif
    SafeRelease(&d3d_device);
}


BEGIN_NAMESPACE(Anonymous)

DirectXResources* directx_resources = nullptr;

END_NAMESPACE(Anonymous)


void DirectXResources::Create() {
    if (directx_resources != nullptr) { throw std::invalid_argument("WndDesignCore initialized twice"); }
    directx_resources = new DirectXResources;
}

void DirectXResources::Destroy() {
    if (directx_resources == nullptr) { throw std::invalid_argument("WndDesignCore uninitialized"); }
    delete directx_resources; directx_resources = nullptr;
}

WNDDESIGNCORE_API const DirectXResources& DirectXResources::Get() {
    if (directx_resources == nullptr) { throw std::invalid_argument("WndDesignCore uninitialized"); }
    return *directx_resources;
}


WNDDESIGNCORE_API void WndDesignCoreInitialize() {
    hr << CoInitialize(NULL); 
    DirectXResources::Create();
}

WNDDESIGNCORE_API void WndDesignCoreUninitialize() {
    DirectXResources::Destroy();
    CoUninitialize();
}


END_NAMESPACE(WndDesign)