#include "dwrite.h"
#include "directx_helper.h"


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

IDWriteFactory& DWriteFactory() { return  DWriteFactory::Get(); }

END_NAMESPACE(Anonymous)


TextLayoutResource::TextLayoutResource(const TextStyle& default_style) : format(nullptr), layout(nullptr) {
	hr = DWriteFactory().CreateTextFormat(
		default_style.font,
		nullptr,
		default_style.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
		default_style.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		default_style.size,
		nullptr,
		&format
	);
}

TextLayoutResource::~TextLayoutResource() {
	SafeRelease(&format);
	SafeRelease(&layout);
}

void TextLayoutResource::TextChanged(const wstring& text) {
	SafeRelease(&layout);
	hr = DWriteFactory().CreateTextLayout(
		text.c_str(), text.length(),
		format,
		0, 0,
		&layout
	);
}


END_NAMESPACE(WndDesign)