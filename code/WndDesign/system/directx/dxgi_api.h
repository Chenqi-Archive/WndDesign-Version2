#pragma once

#include "../../common/core.h"


struct ID3D11Device;
struct IDXGIDevice1;
struct IDXGIAdapter;
struct IDXGIFactory2;


BEGIN_NAMESPACE(WndDesign)


ID3D11Device& GetD3DDevice();
IDXGIDevice1& GetDXGIDevice();
IDXGIAdapter& GetDXGIAdapter();
IDXGIFactory2& GetDXGIFactory();


END_NAMESPACE(WndDesign)