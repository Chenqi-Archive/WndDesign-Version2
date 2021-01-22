#include "../wnd/WndObject.h"
#include "../WndDesign.h"

#include <Windows.h>

using namespace WndDesign;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesignCore.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesignCore.lib")
#endif // _DEBUG


extern int main();


DesktopObject& desktop = DesktopObject::Get();


vector<wstring> GetCommandLineArgs() {
	int argc;
	const wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	vector<wstring> args;
	for (int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	LocalFree(argv);
	return args;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	if (FAILED(CoInitialize(NULL))) { return 0; }
	int ret = main();
	CoUninitialize();
	return ret;
}