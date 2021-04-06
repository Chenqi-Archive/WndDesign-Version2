#pragma once

#include "../../common/core.h"


struct ID3D11Device;
struct IDXGIDevice1;
struct IDXGIAdapter;
struct IDXGIFactory2;

struct ID2D1Factory1;
struct ID2D1DeviceContext;
struct ID2D1SolidColorBrush;


BEGIN_NAMESPACE(WndDesign)


class DirectXResources {
	/// D3D, DXGI ///
public:
	ID3D11Device* d3d_device;
	IDXGIDevice1* dxgi_device;
	IDXGIAdapter* dxgi_adapter;
	IDXGIFactory2* dxgi_factory;

	/// D2D ///
public:
	ID2D1Factory1* d2d_factory;
	ID2D1DeviceContext* d2d_device_context;
	ID2D1SolidColorBrush* d2d_solid_color_brush;

public:
	DirectXResources();
	~DirectXResources();

public:
	WNDDESIGNCORE_API static const DirectXResources& Get();
};


END_NAMESPACE(WndDesign)