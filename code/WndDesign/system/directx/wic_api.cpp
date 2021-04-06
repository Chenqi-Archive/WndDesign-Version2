#include "directx_helper.h"
#include "wic_api.h"


#pragma comment(lib, "windowscodecs.lib")


BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)

class WICFactory {
public:
    WICFactory() : factory(nullptr) {
        hr = CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&factory)
        );
    }
    ~WICFactory() {
        SafeRelease(&factory);
    }
public:
    IWICImagingFactory2* factory;
};

END_NAMESPACE(Anonymous)


IWICImagingFactory2& GetWICFactory() {
    static WICFactory factory;
    return *factory.factory;
}


END_NAMESPACE(WndDesign)