#pragma once

#include "../../common/core.h"
#include "../../common/uncopyable.h"
#include "../../geometry/geometry.h"
#include "../../figure/color.h"


//////////////////////////////////////////////////////////
////           Device Independent Resources           ////
//////////////////////////////////////////////////////////

struct ID2D1Factory1;


BEGIN_NAMESPACE(WndDesign)

ID2D1Factory1& GetD2DFactory();

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


ID2D1DeviceContext& GetD2DDeviceContext();

ID2D1SolidColorBrush& GetSolidColorBrush(Color color);


// Add the count when a target begin draws, and decrease the count when end draw,
// when the count decreased to zero, the D2DDeviceContext really end draws.
void PushDraw();
void PopDraw();


class Target : Uncopyable {
protected:
	// Inherited by WindowTarget to use swap-chain surface as the bitmap.
	ID2D1Bitmap1* bitmap;
private:
	bool has_begun;

public:
	Target(Size size);
	Target(nullptr_t) : bitmap(nullptr), has_begun(false) {}
	~Target();

	bool HasBitmap() const { return bitmap != nullptr; }  // Only read-only target doesn't have bitmap.
	ID2D1Bitmap1& GetBitmap() const { assert(HasBitmap()); return *bitmap; }

	// Begin drawing on the target, and push the bounding_region as axis-aligned-clip.
	// Returns true if already begun.
	bool BeginDraw(Rect bounding_region);
	void EndDraw();
};


END_NAMESPACE(WndDesign)