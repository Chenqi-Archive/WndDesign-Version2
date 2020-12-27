#pragma once

#include "../../common/common.h"


struct ID3D11Device;
struct IDXGIDevice1;
struct IDXGIAdapter;
struct IDXGIFactory2;


BEGIN_NAMESPACE(WndDesign)


ID3D11Device& D3DDevice();
IDXGIDevice1& DXGIDevice();
IDXGIAdapter& DXGIAdapter();
IDXGIFactory2& DXGIFactory();


END_NAMESPACE(WndDesign)