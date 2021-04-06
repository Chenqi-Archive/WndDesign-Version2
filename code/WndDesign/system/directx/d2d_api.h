#pragma once

#include "../../figure/color.h"
#include "../../../WndDesignCore/system/directx/d2d_api.h"


struct ID2D1SolidColorBrush;


BEGIN_NAMESPACE(WndDesign)


ID2D1SolidColorBrush& GetSolidColorBrush(Color color);


END_NAMESPACE(WndDesign)