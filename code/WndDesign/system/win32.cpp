#include "win32.h"

#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)


void ShowWnd(HANDLE hwnd) {
	ShowWindow((HWND)hwnd, SW_SHOW);
}

void HideWnd(HANDLE hwnd) {
	ShowWindow((HWND)hwnd, SW_HIDE);
}

void HideWndFromTaskbar(HANDLE hwnd) {
	SetWindowLong((HWND)hwnd, GWL_EXSTYLE, GetWindowLong((HWND)hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
}


END_NAMESPACE(WndDesign)