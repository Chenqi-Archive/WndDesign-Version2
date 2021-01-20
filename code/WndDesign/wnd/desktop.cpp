#include "desktop.h"
#include "WndObject.h"
#include "redraw_queue.h"
#include "../system/win32_api.h"
#include "../system/metrics.h"


BEGIN_NAMESPACE(WndDesign)


void DesktopWndFrame::SetRegion(Rect region) { 
	Win32::MoveWnd(_hwnd, region); 
}
void DesktopWndFrame::SetCapture() {
	Win32::SetCapture(_hwnd);
}
void DesktopWndFrame::ReleaseCapture() {
	Win32::ReleaseCapture();
}
void DesktopWndFrame::SetFocus() {
	Win32::SetFocus(_hwnd);
}
void DesktopWndFrame::ReleaseFocus() {
	Win32::ReleaseFocus();
}

void DesktopWndFrame::RegionUpdated(Rect region) {
	_wnd.Invalidate(region);
	GetRedrawQueue().Commit();
}

void DesktopWndFrame::SetRegion(Rect region) { 
	// If size changed, resize d2d resource, and invalidate the entire region.
	_resource.OnResize(region.size); 

	// Set display region.

	// 
}

void DesktopWndFrame::ReceiveMessage(Msg msg, Para para) const {
	_wnd.DispatchMessage(msg, para); 
	GetRedrawQueue().Commit();
}

const pair<Size, Size> DesktopWndFrame::CalculateMinMaxSize() {
	return _wnd._object.CalculateMinMaxSize(GetDesktopSize());
}


DesktopObject::DesktopObject() : WndObject(std::make_unique<Desktop>(*this)) {}

DesktopObject::~DesktopObject() {}

DesktopObject& DesktopObject::Get() {
    static DesktopObject desktop_object;
    return desktop_object;
}

void DesktopObject::AddChild(WndObject& child) {
	RegisterChild(child);
	Rect child_region = child.CalculateRegion(GetSize());
	Win32::CreateWnd()
	SetChildRegion(child, child_region);
}


END_NAMESPACE(WndDesign)