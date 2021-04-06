#include "d2d_api.h"
#include "directx_helper.h"


BEGIN_NAMESPACE(WndDesign)


ID2D1SolidColorBrush& GetSolidColorBrush(Color color) {
    ID2D1SolidColorBrush& brush = GetD2DSolidColorBrush();
    brush.SetColor(Color2COLOR(color));
    return brush;
}


END_NAMESPACE(WndDesign)