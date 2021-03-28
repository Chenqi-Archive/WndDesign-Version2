#pragma once

#include "../WndDesign/widget/ToolTip.h"


using namespace WndDesign;


class MainWnd : public Wnd {
private:
	struct Style : Wnd::Style {
		Style() {
			width.setFixed(800px);
			height.setFixed(500px);
			position.setHorizontalCenter().setVerticalCenter();
			border.width(1).color(ColorSet::Black);
			background.setColor(ColorSet::LightGray);
			cursor.set(Cursor::Help);
		}
	};
public:
	MainWnd() : Wnd(std::make_unique<Style>()) {}
	~MainWnd() {}

private:
	ToolTip tool_tip = ToolTip(L"ToolTip test");

private:
	virtual bool NonClientHandler(Msg msg, Para para) override {
		Wnd::NonClientHandler(msg, para);
		tool_tip.Track(msg, para);
		return true;
	}
};


int main() {
	MainWnd main_wnd;
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}