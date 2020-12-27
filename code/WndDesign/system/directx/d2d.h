#pragma once

#include "../../common/common.h"
#include "../../common/uncopyable.h"
#include "../../geometry/geometry.h"
#include "../../figure/color.h"


//////////////////////////////////////////////////////////
////           Device Independent Resources           ////
//////////////////////////////////////////////////////////

struct ID2D1Factory1;


BEGIN_NAMESPACE(WndDesign)

ID2D1Factory1& D2DFactory();

END_NAMESPACE(WndDesign)



//////////////////////////////////////////////////////////
////            Device Dependent Resources            ////
//////////////////////////////////////////////////////////

struct ID2D1DeviceContext;
struct ID2D1Bitmap1;
struct ID2D1SolidColorBrush;
struct ID2D1BitmapBrush;


BEGIN_NAMESPACE(WndDesign)

class ImageResource;

class Layer;


class Target : Uncopyable {
private:
	const Layer& layer;
	mutable ID2D1Bitmap1* bitmap;

public:
	Target(const Layer& layer) : layer(layer), bitmap(nullptr) {}
	~Target();

	ID2D1Bitmap1& GetBitmap() const;
	void Clear();

public:
	// Draw myself on another target. For composition.
	void DrawOn(Vector tile_offset, Rect region, CompositeEffect style, RenderTarget& target, Vector offset) const;
};


ID2D1DeviceContext& D2DDeviceContext();

ID2D1SolidColorBrush& SolidColorBrush(Color color);


END_NAMESPACE(WndDesign)



//////////////////////////////////////////////////////////
////            Window Dependent Resources            ////
//////////////////////////////////////////////////////////


struct IDXGISwapChain1;

using HANDLE = void*;


BEGIN_NAMESPACE(WndDesign)


class WindowResources : Uncopyable {
private:
	IDXGISwapChain1* swap_chain;
	ID2D1Bitmap1* bitmap;
	HANDLE hwnd;

	void CreateBitmapTarget();
	void DestroyBitmapTarget();

public:
	WindowResources(HANDLE hwnd);
	~WindowResources();

	void OnResize();
};


END_NAMESPACE(WndDesign)