#include "directx_helper.h"
#include "dwrite_api.h"

#pragma comment(lib, "dwrite.lib")


BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)


class DWriteFactory {
private:
	IDWriteFactory* factory;
	DWriteFactory();
	~DWriteFactory();
public:
	static IDWriteFactory& Get();
};

DWriteFactory::DWriteFactory() :factory(NULL) {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&factory)
	);
}

DWriteFactory::~DWriteFactory() {
	SafeRelease(&factory);
}

IDWriteFactory& DWriteFactory::Get() {
	static DWriteFactory factory;
	return *factory.factory;
}


END_NAMESPACE(Anonymous)


IDWriteFactory& GetDWriteFactory() { return  DWriteFactory::Get(); }


END_NAMESPACE(WndDesign)