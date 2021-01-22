#include "../WndDesign/WndDesign.h"
//#include "../WndDesign/wnd/OverlapView.h"

using namespace WndDesign;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesign.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesign.lib")
#endif // _DEBUG


class MainWnd : public WndObject {
public:
	MainWnd() {}
	virtual const Rect CalculateRegionOnParent(Size parent_size) { 
		return Rect(100, 100, 800, 500); 
	}
};


int main() {
	MainWnd main_wnd;
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}