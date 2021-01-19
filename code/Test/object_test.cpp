#include "../WndDesign/wnd/WndObject.h"

using namespace WndDesign;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesignCore.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesignCore.lib")
#endif // _DEBUG


class Wnd : public WndObject {

};

class DesktopObject : public WndObject {

	void AddChild(WndObject& wnd, const wstring& title) {  // title is used for win32 displaying.
		RegisterChild(wnd);


	}
};


DesktopObject desktop;


int main() {

}