#include "d2d_api.h"
#include "directx_helper.h"

// #pragma comment(lib, "dxguid.lib")  // For later use.


BEGIN_NAMESPACE(WndDesign)


class D2DDeviceDependentResources {
private:
    D2DDeviceDependentResources();
    ~D2DDeviceDependentResources();
public:
    static D2DDeviceDependentResources& Get();
    ID2D1SolidColorBrush* solid_color_brush;
};

D2DDeviceDependentResources::D2DDeviceDependentResources() : 
    solid_color_brush(nullptr) {
    ID2D1DeviceContext& device_context = GetD2DDeviceContext();
    hr = device_context.CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &solid_color_brush);
}

D2DDeviceDependentResources::~D2DDeviceDependentResources() {
    SafeRelease(&solid_color_brush);
}

D2DDeviceDependentResources& D2DDeviceDependentResources::Get() {
    static D2DDeviceDependentResources resources;
    return resources;
}


ID2D1SolidColorBrush& GetSolidColorBrush(Color color) {
    ID2D1SolidColorBrush& brush = *D2DDeviceDependentResources::Get().solid_color_brush;
    brush.SetColor(Color2COLOR(color));
    return brush;
}


END_NAMESPACE(WndDesign)