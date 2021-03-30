#include "../WndDesign/WndDesign.h"
#include "../WndDesign/wnd/Wnd.h"
#include "../WndDesign/system/win32_aero_snap.h"
#include "../WndDesign/widget/ToolTip.h"
#include "../WndDesign/widget/MessageBox.h"


using namespace WndDesign;


class MainWnd : public Wnd {
private:
	struct Style : Wnd::Style {
		Style() {
			width.normal(800px).max(100pct);
			height.normal(500px).max(100pct);
			position.setHorizontalCenter().setVerticalCenter();
			border.width(3).color(ColorSet::DarkGreen);
			background.setColor(ColorSet::LightGray);
		}
	};
public:
	MainWnd() : Wnd(std::make_unique<Style>()) {}
	~MainWnd() {}

private:
	virtual void NonClientHandler(Msg msg, Para para) override {
		Wnd::NonClientHandler(msg, para);
		if (msg == Msg::LeftDown) { AeroSnapDraggingEffect(*this, GetMouseMsg(para).point); }
		if (msg == Msg::RightDown) { ShowMessage(L"This is Main Window"); }
		if (msg == Msg::MouseEnter) { GetToolTip().OnMouseEnter(L"Main Window"); }
		if (msg == Msg::MouseLeave) { GetToolTip().OnMouseLeave(); }
	}
};

int main() {
	MainWnd main_wnd;
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}