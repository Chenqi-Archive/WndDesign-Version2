#pragma once

#include "../wnd/WndObject.h"


BEGIN_NAMESPACE(WndDesign)


using HANDLE = void*;


// Get the win32 HWND of a window. (You can safely convert it to HWND by static_cast.)
// Returns nullptr if wnd is not the descendant of Desktop.
WNDDESIGNCORE_API HANDLE GetWndHandle(WndObject& wnd);

// Convert the point of a window to the point on its win32 desktop window.
// Returns the HWND and the point. If HWND is nullptr, the window is not the descendant of Desktop.
WNDDESIGNCORE_API pair<HANDLE, const Point> ConvertPointToDesktopWndPoint(WndObject& wnd, Point point);


END_NAMESPACE(WndDesign)