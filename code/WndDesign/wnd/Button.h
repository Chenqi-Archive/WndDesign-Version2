#pragma once

#include "TextBox.h"


BEGIN_NAMESPACE(WndDesign)


class Button : public TextBox {
	struct Style : TextBox::Style {
		BackgroundStyle background_hover;
		BackgroundStyle background_click;
		Style() {
			background_click = background_hover = background;
		}
	};
	Style& GetStyle() { return static_cast<Style&>(WndObject::GetStyle()); }

};


END_NAMESPACE(WndDesign)