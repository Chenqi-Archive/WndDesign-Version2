#include <memory>
#include <iostream>

using namespace std;


struct WndObject {
	unique_ptr<int> wnd;
	WndObject(): wnd(new int(10)) {

	}
	virtual int getSize() { return 0; }
};

struct OverlapWnd : virtual WndObject {
	
};

struct WndStyle : virtual WndObject {
	int size = 0;
	virtual int getSize() override { return size; }
};

struct MyWnd : OverlapWnd, WndStyle {
	MyWnd() {
		size = 3;
	}
};

int main() {
	MyWnd myWnd;
	int size = myWnd.getSize();
}