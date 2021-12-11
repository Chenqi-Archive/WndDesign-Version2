#include <string>

#include <d2d1_1.h>

#include <dxgi1_4.h>
#include <d3d11.h>

#pragma comment(lib, "d2d1.lib" )
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "dxguid.lib")  // For effects.


extern HWND hWnd;

ID2D1Factory1* pD2DFactory = nullptr;
ID2D1DeviceContext* pD2DDeviceContext = nullptr;
ID2D1Bitmap1* pD2DBitmap = nullptr;
ID2D1Bitmap1* pD2DTargetBitmap = nullptr;
ID2D1SolidColorBrush* pBrush = nullptr;
ID2D1Effect* pEffect = nullptr;

ID3D11Device* pD3DDevice;
IDXGISwapChain1* pSwapChain = nullptr;

HRESULT hr = S_OK;

template<class Interface>
inline void SafeRelease(Interface*& pInterfaceToRelease) {
    if (pInterfaceToRelease != nullptr) {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = nullptr;
    }
}


void Init();
void Final();

void CreateDeviceIndependentResources();
void DiscardDeviceIndependentResources();

void CreateDeviceDependentResources();
void DiscardDeviceDependentResources();

void CreateWindowSizeDependentResources();
void DiscardWindowSizeDependentResources();



void Init() {
    hr = CoInitialize(nullptr);
    if (FAILED(hr)) { throw; }

    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void Final() {
	DiscardWindowSizeDependentResources();
    DiscardDeviceDependentResources();
    DiscardDeviceIndependentResources();

    CoUninitialize();
}


void CreateDeviceIndependentResources() {
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &pD2DFactory
    );
    if (FAILED(hr)) { throw; }
}

void DiscardDeviceIndependentResources() {
    SafeRelease(pD2DFactory);
}

void CreateDeviceDependentResources() {
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
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,       
        featureLevels, ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,			            
        &pD3DDevice,				            
        &featureLevel,			                
        nullptr         		                
    );
    if (FAILED(hr)) { throw; }

    IDXGIDevice1* pDxgiDevice = nullptr;
    hr = pD3DDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice));
    if (FAILED(hr)) { throw; }

    ID2D1Device* pD2DDevice = nullptr;
    hr = pD2DFactory->CreateDevice(pDxgiDevice, &pD2DDevice);
    if (FAILED(hr)) { throw; }
    SafeRelease(pDxgiDevice);

    hr = pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pD2DDeviceContext);
    if (FAILED(hr)) { throw; }
    SafeRelease(pD2DDevice);


    hr = pD2DDeviceContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::DarkBlue), &pBrush);
    if (FAILED(hr)) { throw; }

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    hr = pD2DDeviceContext->CreateBitmap(D2D1::SizeU(100, 100), nullptr, NULL, &bitmapProperties, &pD2DBitmap);
    if (FAILED(hr)) { throw; }

    pD2DDeviceContext->SetTarget(pD2DBitmap);
    pD2DDeviceContext->BeginDraw();
    pD2DDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
    pD2DDeviceContext->EndDraw();
    pD2DDeviceContext->SetTarget(nullptr);

    pD2DDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &pEffect);
    pEffect->SetInput(0, pD2DBitmap);
}

void DiscardDeviceDependentResources() {
    SafeRelease(pEffect);
    SafeRelease(pD2DBitmap);
    SafeRelease(pBrush);

    SafeRelease(pD2DTargetBitmap);
    SafeRelease(pSwapChain);
    SafeRelease(pD2DDeviceContext);
    SafeRelease(pD3DDevice);
}

void CreateWindowSizeDependentResources() {
    DiscardWindowSizeDependentResources();

    RECT rect = { 0 }; GetClientRect(hWnd, &rect);

    if (pSwapChain == nullptr) {
        IDXGIDevice1* pDxgiDevice = nullptr;
        IDXGIAdapter* pDxgiAdapter = nullptr;
        IDXGIFactory2* pDxgiFactory = nullptr;

        hr = pD3DDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice));
        if (FAILED(hr)) { throw; }

        hr = pDxgiDevice->GetAdapter(&pDxgiAdapter);
        if (FAILED(hr)) { throw; }

        hr = pDxgiAdapter->GetParent(IID_PPV_ARGS(&pDxgiFactory));
        if (FAILED(hr)) { throw; }

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
        //swapChainDesc.Width = rect.right - rect.left; 
        //swapChainDesc.Height = rect.bottom - rect.top;
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

        hr = pDxgiFactory->CreateSwapChainForHwnd(
            pD3DDevice,
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &pSwapChain
        );
        if (FAILED(hr)) { throw; }

        hr = pDxgiDevice->SetMaximumFrameLatency(1);
        if (FAILED(hr)) { throw; }

        SafeRelease(pDxgiFactory);
        SafeRelease(pDxgiAdapter);
        SafeRelease(pDxgiDevice);

    } else {
        hr = pSwapChain->ResizeBuffers(
            2,
            rect.right - rect.left,
            rect.bottom - rect.top,
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0);
        if (FAILED(hr)) { throw; }
    }

    IDXGISurface* pDxgiBackBuffer = nullptr;
    hr = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pDxgiBackBuffer));
    if (FAILED(hr)) { throw; }

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );

    hr = pD2DDeviceContext->CreateBitmapFromDxgiSurface(
        pDxgiBackBuffer,
        &bitmapProperties,
        &pD2DTargetBitmap
    );
    if (FAILED(hr)) { throw; }


    SafeRelease(pDxgiBackBuffer);
}

void DiscardWindowSizeDependentResources() {
    SafeRelease(pD2DTargetBitmap);
}


FLOAT blur_radius = 3.0f;

void OnPaint() {
    pD2DDeviceContext->SetTarget(pD2DTargetBitmap);
    pD2DDeviceContext->BeginDraw();

    pD2DDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::White));
    //pD2DDeviceContext->DrawBitmap(pD2DBitmap, D2D1::RectF(100, 200, 200, 300));
    pEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blur_radius);
    pD2DDeviceContext->DrawImage(pEffect, D2D1::Point2F(100, 200));

    pD2DDeviceContext->EndDraw();
    pD2DDeviceContext->SetTarget(nullptr);

    pSwapChain->Present(0, 0);
}

void OnWheel(int delta) {
    blur_radius += delta / 100.0;
    if (blur_radius < 0) {
        blur_radius = 0;
    }
    OnPaint();
}

void OnSize(int width, int height) {
    CreateWindowSizeDependentResources();
}