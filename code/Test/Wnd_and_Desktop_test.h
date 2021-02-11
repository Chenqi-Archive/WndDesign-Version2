#include "../WndDesign/WndDesign.h"
#include "../WndDesign/wnd/Wnd.h"
#include "../WndDesign/system/win32_aero_snap.h"


using namespace WndDesign;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesign.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesign.lib")
#endif // _DEBUG


class MainWnd : public Wnd {
private:
	struct Style : Wnd::Style {
		Style() {
			width.normal(800px).max(100pct);
			height.normal(500px).max(100pct);
			position.setHorizontalCenter().setVerticalCenter();
			border.width(30).color(ColorSet::DarkGreen);
			background.setColor(ColorSet::LightGray);
			title.set(L"Wnd and Desktop test");
		}
	};
public:
	MainWnd() : Wnd(std::make_unique<Style>()) {}
	~MainWnd() {}

private:
	virtual bool NonClientHandler(Msg msg, Para para) override {
		if (msg == Msg::LeftDown) {
			AeroSnapDraggingEffect(*this, GetMouseMsg(para).point);
		}
		return true;
	}
};

int main() {
	MainWnd main_wnd;
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}