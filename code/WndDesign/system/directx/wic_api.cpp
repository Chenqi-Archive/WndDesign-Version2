#include "directx_helper.h"
#include "wic_api.h"

#pragma comment(lib, "windowscodecs.lib")


BEGIN_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(Anonymous)


class WICFactory {
private:
    IWICImagingFactory2* factory;
    WICFactory();
    ~WICFactory();
public:
    static IWICImagingFactory2& Get();
};

WICFactory::WICFactory() : factory(nullptr) {
    hr = CoCreateInstance(
        CLSID_WICImagingFactory2,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory)
    );
}

WICFactory::~WICFactory() {
    SafeRelease(&factory);
}

IWICImagingFactory2& WICFactory::Get() {
    static WICFactory factory;
    return *factory.factory;
}


END_NAMESPACE(Anonymous)


IWICImagingFactory2& GetWICFactory() { return WICFactory::Get(); }


END_NAMESPACE(WndDesign)