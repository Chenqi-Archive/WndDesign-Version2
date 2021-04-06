#include "../wnd/WndObject.h"
#include "../WndDesign.h"
#include "../../WndDesignCore/system/init.h"

#include <Windows.h>


#pragma comment(lib, "WndDesignCore.lib")


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


using namespace WndDesign;

extern int main();


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WndDesignCoreInitialize();
	int ret = main();
	desktop.Terminate(); // destroy all windows and clear their d2d resources
	WndDesignCoreUninitialize();
	return ret;
}