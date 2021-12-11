#include <functional>



class A {
public:
	A() {

	}
	~A() {

	}
};

A& f() {
	static A a;
	return a;
}

using HANDLE = void*;


int AddChild(int a, std::function<void(HANDLE hWnd)> callback = nullptr) {
	void* hWnd = 0; // CreateWindow;
	if (callback != nullptr) {
		callback(hWnd);  // called between CreateWindow and ShowWindow.
	}
	return 0;
}


int main() {
	A& a = f();

	int b;

	AddChild(0);
	AddChild(0, [&](HANDLE hWnd) {hWnd = 0; });
}


int
main() {
	int
		a = 1;

}