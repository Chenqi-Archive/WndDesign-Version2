#include "desktop.h"
#include "redraw_queue.h"
#include "../layer/figure_queue.h"
#include "../layer/background_types.h"
#include "../system/win32_api.h"
#include "../system/metrics.h"


BEGIN_NAMESPACE(WndDesign)


DesktopWndFrame::DesktopWndFrame(WndBase& wnd, WndObject& wnd_object, HANDLE hwnd) :
	_wnd(wnd), _wnd_object(wnd_object), _hwnd(hwnd), _resource(_hwnd) {
	// Store the pointer of the attached frame as the user data.
	Win32::SetWndUserData(_hwnd, this);
}

DesktopWndFrame::~DesktopWndFrame() {
	Win32::SetWndUserData(_hwnd, nullptr);
	Win32::DestroyWnd(_hwnd);
	LeaveRedrawQueue();
}

void DesktopWndFrame::JoinRedrawQueue() { 
	if (!HasRedrawQueueIndex()) { RedrawQueue::Get().AddDesktopWnd(*this); } 
}

void DesktopWndFrame::LeaveRedrawQueue() { 
	if (HasRedrawQueueIndex()) { RedrawQueue::Get().RemoveDesktopWnd(*this); } 
}

void DesktopWndFrame::UpdateInvalidRegion() {
	auto [bounding_region, regions] = _wnd._invalid_region.GetRect();

	FigureQueue figure_queue;
	uint group_index = figure_queue.BeginGroup(vector_zero, bounding_region);
	figure_queue.Append(bounding_region.point - point_zero, new BackgroundFigure(NullBackground::Get(), bounding_region, true));
	_wnd.Composite(figure_queue, bounding_region - _wnd.OffsetFromParent());
	figure_queue.EndGroup(group_index);

	Target& target = _resource.GetTarget();
	for (auto& region : regions) {
		target.DrawFigureQueue(figure_queue, vector_zero, region);
	}

	JoinRedrawQueue();
}

void DesktopWndFrame::Present() { 
	_resource.Present(_wnd._invalid_region.GetRect().second);
	_wnd._invalid_region.Clear();
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

void DesktopWndFrame::SetRegion(Rect region) {
	Rect old_region = _wnd.GetRegionOnParent();
	if (old_region.size != region.size) {
		_resource.OnResize(region.size);
		_wnd.Invalidate(region_infinite);
	}
	_wnd.SetRegionOnParent(region);
}

const pair<Size, Size> DesktopWndFrame::CalculateMinMaxSize() {
	return _wnd_object.CalculateMinMaxSize(GetDesktopSize());
}


DesktopObject& DesktopObject::Get() {
    static DesktopObjectImpl desktop_object;
    return desktop_object;
}

void DesktopObjectImpl::AddChild(WndObject& child) {
	RegisterChild(child);
}

void DesktopObjectImpl::RemoveChild(WndObject& child) {
	UnregisterChild(child);
	OnChildDetach(child);
}

void DesktopObjectImpl::OnChildDetach(WndObject& child) {
	_child_wnds.erase(GetChildFrame(child)._desktop_index);
}

void DesktopObjectImpl::OnChildRegionChange(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Rect child_region = child.CalculateRegion(GetSize());
	Win32::MoveWnd(frame._hwnd, child_region);
}

void DesktopObjectImpl::AddChild(WndBase& child, WndObject& child_object) {
	Rect child_region = child_object.CalculateRegion(GetSize());
	HANDLE hwnd = Win32::CreateWnd(child_region, child_object.GetTitle());
	DesktopWndFrame& frame = _child_wnds.emplace_front(child, child_object, hwnd);
	frame._desktop_index = _child_wnds.begin();
	SetChildRegion(child_object, child_region);
	SetChildFrame(child_object, frame);
}

void DesktopObjectImpl::MessageLoop() {
	Win32::MessageLoop();
}

void DesktopObjectImpl::Terminate() {
#pragma message(Remark"Use PostQuitMessage() instead.")
	while (!_child_wnds.empty()) {
		RemoveChild(_child_wnds.back()._wnd_object);
	}
}


void DesktopBase::ReleaseCapture() {
	Win32::ReleaseCapture();
}

void DesktopBase::ReleaseFocus() {
	Win32::ReleaseFocus();
}


END_NAMESPACE(WndDesign)