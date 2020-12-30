#pragma once

#include "d2d_api.h"


//////////////////////////////////////////////////////////
////            Window Dependent Resources            ////
//////////////////////////////////////////////////////////


struct IDXGISwapChain1;

using HANDLE = void*;


BEGIN_NAMESPACE(WndDesign)


class WindowResource : Uncopyable {
private:
	HANDLE hwnd;
	IDXGISwapChain1* swap_chain;
	ID2D1Bitmap1* bitmap;

	// The target is directly drawn on, and is then bitblted to swap chain when present is called.
	// This complex technique is used because when double buffering is used, 
	//   the back buffer may not be consistent with the front buffer.
	Target target;    

	void CreateBitmapTarget();
	void DestroyBitmapTarget();

public:
	WindowResource(HANDLE hwnd);
	~WindowResource();

	void OnResize(Size size);

	Target& GetTarget() { return target; }
	const Size GetSize();

	void Present(Rect dirty_regions);
};


END_NAMESPACE(WndDesign)