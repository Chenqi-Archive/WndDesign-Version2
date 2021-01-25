#include "win32.h"
#include "../wnd/desktop.h"


BEGIN_NAMESPACE(WndDesign)


// May use a Union-Find to quickly get the cooresponding window.
WNDDESIGNCORE_API std::pair<HANDLE, const Point> ConvertPointToDesktopWndPoint(WndObjectBase& wnd, Point point) {
	return static_cast<DesktopObjectImpl&>(DesktopObject::Get()).ConvertPointToDesktopWndPoint(wnd, point);
}


END_NAMESPACE(WndDesign)