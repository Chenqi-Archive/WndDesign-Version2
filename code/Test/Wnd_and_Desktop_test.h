#include "../WndDesign/WndDesign.h"
#include "../WndDesign/wnd/Wnd.h"


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
			width.setFixed(70pct);
			height.setFixed(70pct);
			position.setHorizontalCenter().setVerticalCenter();
			border.width(5).color(ColorSet::DarkGreen);
			background.setColor(ColorSet::LightGray);
			title.set(L"Wnd and Desktop test");
		}
	};
public:
	MainWnd() : Wnd(std::make_unique<Style>()) {}
	~MainWnd() {}
};

int main() {
	MainWnd main_wnd;
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}