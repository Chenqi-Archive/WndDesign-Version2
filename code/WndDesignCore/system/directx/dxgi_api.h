#pragma once

#include "directx_resource.h"


BEGIN_NAMESPACE(WndDesign)


inline ID3D11Device& GetD3DDevice() { return *DirectXResources::Get().d3d_device; }
inline IDXGIDevice1& GetDXGIDevice() { return *DirectXResources::Get().dxgi_device; }
inline IDXGIAdapter& GetDXGIAdapter() { return *DirectXResources::Get().dxgi_adapter; }
inline IDXGIFactory2& GetDXGIFactory() { return *DirectXResources::Get().dxgi_factory; }


END_NAMESPACE(WndDesign)