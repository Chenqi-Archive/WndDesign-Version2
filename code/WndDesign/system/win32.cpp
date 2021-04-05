#include "win32.h"

#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)


void ShowWnd(HANDLE hwnd) {
	ShowWindow((HWND)hwnd, SW_SHOW);
}

void HideWnd(HANDLE hwnd) {
	ShowWindow((HWND)hwnd, SW_HIDE);
}

void AddToolWindow(WndObject& wnd) {
	desktop.AddChild(wnd, WS_EX_TOOLWINDOW);
}


END_NAMESPACE(WndDesign)