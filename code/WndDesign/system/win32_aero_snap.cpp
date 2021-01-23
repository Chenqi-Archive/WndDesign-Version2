#include "win32_aero_snap.h"
#include "../system/win32.h"

#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)


void AeroSnapDraggingEffect(WndObject& wnd, Point point) {
	auto [hwnd, new_point] = ConvertPointToDesktopWndPoint(wnd, point);
	if (hwnd == nullptr) { return; }
	SendMessageW((HWND)hwnd, WM_NCLBUTTONDOWN, HTCAPTION, ((short)new_point.y << 16) | (short)new_point.x);
}

void AeroSnapBorderResizingEffect(WndObject& wnd, Point point, BorderPosition border_position) {
	auto [hwnd, new_point] = ConvertPointToDesktopWndPoint(wnd, point);
	if (hwnd == nullptr) { return; }
	SendMessage((HWND)hwnd, WM_NCLBUTTONDOWN, (WPARAM)border_position, ((short)new_point.y << 16) | (short)new_point.x);
}


END_NAMESPACE(WndDesign)