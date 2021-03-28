#pragma once

#include "../wnd/TextBox.h"
#include "../message/timer.h"


BEGIN_NAMESPACE(WndDesign)


class ToolTip : protected TextBox {
public:
	struct Style : public TextBox::Style {
		Style() {
			width.max(200px);
			height.max(500px);
			border.width(1).color(ColorSet::Gray);
			padding.setAll(3px);
			background.setColor(ColorSet::Ivory);
		}
	};

public:
	ToolTip(const wstring& text) : TextBox(std::make_unique<Style>(), text) {}
	~ToolTip() {}

public:
	static ToolTip& Get();

private:
	static constexpr uint delay_time = 500;
	static constexpr uint expire_time = 5000;
	Timer timer = Timer([]() {});

private:
	void Show() {
		Point position = GetCursorPosition();
		GetStyle().position.left(px(position.x-10)).top(px(position.y+10));
		desktop.AddChild(*this); SetFocus();
		timer.Set(expire_time); timer.callback = std::bind(&ToolTip::Hide, this);
	}
	void Hide() {
		timer.Stop();
		if (HasParent()) { desktop.RemoveChild(*this); }
	}

public:
	void Track(Msg msg, Para para) {
		if (msg == Msg::MouseEnter) { timer.Set(delay_time); timer.callback = std::bind(&ToolTip::Show, this); }
		if (msg == Msg::MouseLeave) { Hide(); }
	}

private:
	virtual bool Handler(Msg msg, Para para) override {
		if (msg == Msg::LoseFocus) { Hide(); }
		return true;
	}
};


inline ToolTip& GetToolTip() { return ToolTip::Get(); }


END_NAMESPACE(WndDesign)