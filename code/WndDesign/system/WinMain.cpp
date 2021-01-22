#include "../wnd/WndObject.h"
#include "../WndDesign.h"

#include <Windows.h>

using std::vector;
using std::wstring;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesignCore.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesignCore.lib")
#endif // _DEBUG


extern int main();


BEGIN_NAMESPACE(WndDesign)

DesktopObject& desktop = DesktopObject::Get();

END_NAMESPACE(WndDesign)


vector<wstring> GetCommandLineArgs() {
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	vector<wstring> args;
	for (int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	LocalFree(argv);
	return args;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	if (FAILED(CoInitialize(NULL))) { return 0; }
	int ret = main();
	CoUninitialize();
	return ret;
}