#pragma once

#include "../../common/core.h"
#include "../../common/uncopyable.h"
#include "../../geometry/geometry.h"


//////////////////////////////////////////////////////////
////           Device Independent Resources           ////
//////////////////////////////////////////////////////////

struct ID2D1Factory1;


BEGIN_NAMESPACE(WndDesign)

WNDDESIGNCORE_API ID2D1Factory1& GetD2DFactory();

END_NAMESPACE(WndDesign)



//////////////////////////////////////////////////////////
////            Device Dependent Resources            ////
//////////////////////////////////////////////////////////

struct ID2D1DeviceContext;
struct ID2D1Bitmap1;


BEGIN_NAMESPACE(WndDesign)

class FigureQueue;


WNDDESIGNCORE_API ID2D1DeviceContext& GetD2DDeviceContext();

void BeginDraw();
void EndDraw();


class Target : Uncopyable {
protected:
	// Inherited by WindowTarget to use swap-chain surface as the bitmap.
	ID2D1Bitmap1* bitmap;
public:
	Target(Size size);
	Target(nullptr_t) : bitmap(nullptr) {}
	~Target();

	bool HasBitmap() const { return bitmap != nullptr; }  // Only read-only target doesn't have bitmap.
	ID2D1Bitmap1& GetBitmap() const { assert(HasBitmap()); return *bitmap; }

	void DrawFigureQueue(const FigureQueue& figure_queue, Vector offset, Rect clip_region);
};


END_NAMESPACE(WndDesign)