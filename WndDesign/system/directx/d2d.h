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

// Device context.
struct ID2D1DeviceContext;

// Bitmap.
struct ID2D1Bitmap1;

// Brushes.
struct ID2D1SolidColorBrush;
struct ID2D1BitmapBrush;


BEGIN_NAMESPACE(WndDesign)

class ImageResource;


class Target : Uncopyable {
private:
	ID2D1Bitmap1* bitmap;
public:
	Target(Size size);
	~Target();
	ID2D1Bitmap1& GetBitmap() const { return *bitmap; }
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