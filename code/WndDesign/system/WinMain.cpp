#include "../wnd/WndObject.h"
#include "../WndDesign.h"

#include <Windows.h>

using std::vector;
using std::wstring;


#pragma comment(lib, "WndDesignCore.lib")


extern int main();


BEGIN_NAMESPACE(WndDesign)


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

END_NAMESPACE(WndDesign)


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	if (FAILED(CoInitialize(NULL))) { return 0; }
	int ret = main();
	CoUninitialize();
	return ret;
}