#pragma once

#include "FinalWnd.h"
#include "../figure/text_layout.h"


BEGIN_NAMESPACE(WndDesign)


class TextBox : public FinalWnd {
public:
	using Style = TextBoxStyle;

private:
	TextLayout _layout;
	

public:
	TextBox(const wstring& text) {}
};


END_NAMESPACE(WndDesign)