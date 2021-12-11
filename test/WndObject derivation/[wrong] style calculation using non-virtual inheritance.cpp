/*
Error! WndObject was constructed twice!
*/
#include <memory>

using namespace std;


struct WndObject {
	unique_ptr<int> wnd;
	WndObject(): wnd(new int(10)) {

	}
	virtual int getSize() { return 0; }
	virtual void removeChild(WndObject& child) {}
};

struct OverlapWnd : WndObject {
	int has_child;
	virtual void removeChild(WndObject& child) {
		has_child = false;
	}
};

struct WndStyle : WndObject {
	int size = 0;
	virtual int getSize() override { 
		return size; 
	}
};

struct MyWnd : OverlapWnd, WndStyle {
	MyWnd() {
		size = 3;
	}
};

int main() {
	MyWnd myWnd;
	WndObject& object = (OverlapWnd&)myWnd;
	int size = object.getSize();
}