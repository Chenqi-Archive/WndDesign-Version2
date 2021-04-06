#include "directx_helper.h"
#include "dwrite_api.h"


#pragma comment(lib, "dwrite.lib")


BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)

struct DWriteFactory {
public:
	DWriteFactory() : factory(NULL) {
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&factory)
		);
	}
	~DWriteFactory() {
		SafeRelease(&factory);
	}
public:
	IDWriteFactory* factory;
};

END_NAMESPACE(Anonymous)


IDWriteFactory& GetDWriteFactory() { 
	static DWriteFactory factory;
	return *factory.factory;
}


END_NAMESPACE(WndDesign)