#pragma once

#include "../../figure/color.h"


struct ID2D1Factory1;

struct ID2D1DeviceContext;
struct ID2D1Bitmap1;
struct ID2D1SolidColorBrush;
struct ID2D1BitmapBrush;


BEGIN_NAMESPACE(WndDesign)


WNDDESIGNCORE_API ID2D1Factory1& GetD2DFactory();

WNDDESIGNCORE_API ID2D1DeviceContext& GetD2DDeviceContext();

ID2D1SolidColorBrush& GetSolidColorBrush(Color color);


END_NAMESPACE(WndDesign)