#include "win32.h"
#include "../wnd/desktop.h"


BEGIN_NAMESPACE(WndDesign)


// May use a Union-Find to quickly get the cooresponding window.
WNDDESIGNCORE_API std::pair<HANDLE, const Point> ConvertPointToDesktopWndPoint(WndObject& wnd, Point point) {
	return GetDesktop().ConvertWndNonClientPointToDesktopWindowPoint(wnd, point);
}

// Get the win32 HWND of a window.
// Returns nullptr if wnd is not the descendant of Desktop.
// You can convert it to HWND using static_cast.
WNDDESIGNCORE_API HANDLE GetWndHandle(WndObject& wnd) {
	return GetDesktop().GetWndHandle(wnd);
}


END_NAMESPACE(WndDesign)