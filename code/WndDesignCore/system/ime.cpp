#include "ime.h"
#include "win32.h"
#include "win32_ime_input.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGNCORE_API void EnableIME(WndObject& wnd) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(wnd));
	if (hWnd == NULL) { return; }
	ImeInput::Get().EnableIME(hWnd);
}

WNDDESIGNCORE_API void DisableIME(WndObject& wnd) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(wnd));
	if (hWnd == NULL) { return; }
	ImeInput::Get().DisableIME(hWnd);
}

WNDDESIGNCORE_API void CancelIME(WndObject& wnd) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(wnd));
	if (hWnd == NULL) { return; }
	ImeInput::Get().CancelIME(hWnd);
}

WNDDESIGNCORE_API void MoveImeWindow(WndObject& wnd, Rect caret_region) {
	auto pair = ConvertPointToDesktopWndPoint(wnd, caret_region.point);
	HWND hWnd = static_cast<HWND>(pair.first); caret_region.point = pair.second;
	if (hWnd == NULL) { return; }
	ImeInput::Get().UpdateCaretRect(hWnd, caret_region);
}


END_NAMESPACE(WndDesign)