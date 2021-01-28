#pragma once

#include "TextBox.h"
#include "../message/message.h"


BEGIN_NAMESPACE(WndDesign)


class Button : public TextBox {
public:
	struct Style : TextBox::Style {
		BackgroundStyle background_hover;
		BackgroundStyle background_click;
		Style() {
			background_click = background_hover = background;
		}
	};
	Style& GetStyle() { return static_cast<Style&>(WndObject::GetStyle()); }
	
private:
	enum { Normal, Hover, Down } _state = Normal;

protected:
	bool Handler(Msg msg, Para para) override {
		switch (msg) {
		case Msg::MouseEnter: assert(_state == Normal); _state = Hover; OnHover(); break;
		case Msg::LeftDown: assert(_state == Hover); _state = Down; OnPress(); break;
		case Msg::LeftUp: if (_state == Down) { OnClick(); } break;
		case Msg::MouseLeave: _state = Normal; OnLeave(); break;
		default:break;
		}
		return true;
	}
protected:
	virtual void OnHover() {}
	virtual void OnPress() {}
	virtual void OnClick() {}
	virtual void OnLeave() {}
};


class ButtonWithText


END_NAMESPACE(WndDesign)