#pragma once

#include "TextBox.h"
#include "../message/message.h"


BEGIN_NAMESPACE(WndDesign)


class Button : public TextBox {
public:
	struct Style : TextBox::Style {
		BackgroundStyle background_hover;
		BackgroundStyle background_press;
		Style() {
			background_press = background_hover = background;
		}
	};

public:
	Button(unique_ptr<Style> style, const wstring& text = L"") : TextBox(std::move(style), text) {}
	~Button() {}


	//// style ////
protected:
	Style& GetStyle() { return static_cast<Style&>(TextBox::GetStyle()); }
	const Style& GetStyle() const { return static_cast<const Style&>(TextBox::GetStyle()); }


	//// message handling ////
private:
	enum class State { Normal, Hover, Down } _state = State::Normal;
protected:
	bool Handler(Msg msg, Para para) override {
		switch (msg) {
		case Msg::MouseEnter: assert(_state == State::Normal); _state = State::Hover; OnHover(); break;
		case Msg::LeftDown: assert(_state == State::Hover); _state = State::Down; OnPress(); break;
		case Msg::LeftUp: if (_state == State::Down) { OnClick(); } break;
		case Msg::MouseLeave: _state = State::Normal; OnLeave(); break;
		default:break;
		}
		return true;
	}
protected:
	virtual void OnHover() { SetBackground(GetStyle().background_hover.Get()); }
	virtual void OnPress() { SetBackground(GetStyle().background_press.Get()); }
	virtual void OnClick() { SetBackground(GetStyle().background_hover.Get()); }
	virtual void OnLeave() { SetBackground(GetStyle().background.Get()); }
};


END_NAMESPACE(WndDesign)