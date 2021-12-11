/*
This is the initial version that uses multi-inheritance to implement style calculation.
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

struct WndStyle {
	int size = 0;
	int getSize() { 
		return size; 
	}
};

#define Use_WndStyle_Size_Calculators \
	virtual int getSize() override { return WndStyle::getSize(); }


struct MyWnd : OverlapWnd, WndStyle {
	MyWnd() {
		size = 3;
	}
	Use_WndStyle_Size_Calculators;
};


int main() {
	MyWnd myWnd;
	WndObject& object = myWnd;
	int size = object.getSize();
}