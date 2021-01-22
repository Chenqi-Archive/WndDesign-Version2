#include "directx_helper.h"
#include "dxgi_api.h"

#pragma comment(lib, "d3d11.lib")

BEGIN_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(Anonymous)

class D3DDevice {
private:
    D3DDevice();
    ~D3DDevice();
public:
    ID3D11Device* d3d_device;
    IDXGIDevice1* dxgi_device;
    IDXGIAdapter* dxgi_adapter;
    IDXGIFactory2* dxgi_factory;
    static D3DDevice& Get();
};

D3DDevice::D3DDevice() : d3d_device(nullptr), dxgi_device(nullptr) {
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };
    D3D_FEATURE_LEVEL featureLevel;

    hr = D3D11CreateDevice(
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

    hr = d3d_device->QueryInterface(
        IID_PPV_ARGS(&dxgi_device)
    );

    hr = dxgi_device->GetAdapter(&dxgi_adapter);

    hr = dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory));
}

D3DDevice::~D3DDevice() {
    SafeRelease(&dxgi_factory);
    SafeRelease(&dxgi_adapter);
    SafeRelease(&dxgi_device);
    SafeRelease(&d3d_device);
}

D3DDevice& D3DDevice::Get() {
    static D3DDevice device;
    return device;
}

END_NAMESPACE(Anonymous)

ID3D11Device& GetD3DDevice() { return *D3DDevice::Get().d3d_device; }
IDXGIDevice1& GetDXGIDevice() { return *D3DDevice::Get().dxgi_device; }
IDXGIAdapter& GetDXGIAdapter() { return *D3DDevice::Get().dxgi_adapter; }
IDXGIFactory2& GetDXGIFactory() { return *D3DDevice::Get().dxgi_factory; }


END_NAMESPACE(WndDesign)