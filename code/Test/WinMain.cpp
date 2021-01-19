#include <Windows.h>

extern int main();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	if (FAILED(CoInitialize(NULL))) { return 0; }
	return main();
}