#pragma once

#include "../../common/common.h"
#include "../../common/uncopyable.h"
#include "../../figure/text_style.h"


struct IDWriteTextLayout;
struct IDWriteTextFormat;


BEGIN_NAMESPACE(WndDesign)


class TextLayoutResource {
private:
	IDWriteTextFormat* format;
	IDWriteTextLayout* layout;

public:
	TextLayoutResource(const TextStyle& default_style);
	~TextLayoutResource();
	void TextChanged(const wstring& text);
	IDWriteTextLayout& DWriteTextLayout() const { 
		assert(layout != nullptr);
		return *layout; 
	}
};


END_NAMESPACE(WndDesign)