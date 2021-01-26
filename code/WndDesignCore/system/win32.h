#pragma once

#include "../geometry/geometry.h"
#include <utility>


BEGIN_NAMESPACE(WndDesign)


using HANDLE = void*;

class WndObject;


// Convert the point of a window to the point on its win32 desktop window.
// Returns the HWND and the point. If HWND is nullptr, the window is not the descendant of Desktop.
WNDDESIGNCORE_API std::pair<HANDLE, const Point> ConvertPointToDesktopWndPoint(WndObject& wnd, Point point);

// Get the win32 HWND of a window.
// Returns nullptr if wnd is not the descendant of Desktop.
// You can convert it to HWND using static_cast.
inline HANDLE GetObjectHWND(WndObject& wnd) {
	return ConvertPointToDesktopWndPoint(wnd, point_zero).first;
}


END_NAMESPACE(WndDesign)