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
	LeaveRedrawQueue();
}

void DesktopWndFrame::JoinRedrawQueue() { 
	if (!_redraw_queue_index.valid()) { RedrawQueue::Get().AddDesktopWnd(*this); } 
}

void DesktopWndFrame::LeaveRedrawQueue() { 
	if (_redraw_queue_index.valid()) { RedrawQueue::Get().RemoveDesktopWnd(*this); } 
}

void DesktopWndFrame::UpdateInvalidRegion() {
	auto [bounding_region, regions] = _wnd._invalid_region.GetRect();

	// A little tricky here. 
	// Figures are drawn in desktop's coordinates, but the target is in window's coordinates, so first
	//   push the group as the desktop relative to the target.
	FigureQueue figure_queue;
	Vector offset_from_desktop = _wnd.OffsetFromParent();
	uint group_index = figure_queue.BeginGroup(vector_zero - offset_from_desktop, Rect(point_zero, GetDesktopSize()));
	_wnd.Composite(figure_queue, bounding_region - offset_from_desktop);
	figure_queue.EndGroup(group_index);

	Target& target = _resource.GetTarget();
	for (auto& region : regions) {
		target.DrawFigureQueue(figure_queue, vector_zero, region);
	}

	// The invalid region will still be used at present time, and will be cleared after presentation, see below.

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


WNDDESIGNCORE_API DesktopObject& DesktopObject::Get() {
    static DesktopObjectImpl desktop_object;
    return desktop_object;
}

DesktopObjectImpl::DesktopObjectImpl() : DesktopObject(std::make_unique<DesktopBase>(*this)) {
	// Initialize the size of desktop.
	CalculateRegion(size_min);
}

const Rect DesktopObjectImpl::CalculateRegionOnParent(Size parent_size) {
	return Rect(point_zero, GetDesktopSize());
}

void DesktopObjectImpl::AddChild(WndObject& child) {
	RegisterChild(child);
	// DesktopBase will call AddChild again with child's WndBase information.
}

void DesktopObjectImpl::OnChildDetach(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	HANDLE hwnd = frame._hwnd;
	_child_wnds.erase(frame._desktop_index);
	Win32::DestroyWnd(hwnd);
}

void DesktopObjectImpl::OnChildRegionUpdate(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Rect child_region = UpdateChildRegion(child, GetSize());
	Win32::MoveWnd(frame._hwnd, child_region);
}

void DesktopObjectImpl::AddChild(WndBase& child, WndObject& child_object) {
	Rect child_region = child_object.CalculateRegion(GetSize());
	HANDLE hwnd = Win32::CreateWnd(child_region, child_object.GetTitle());
	DesktopWndFrame& frame = _child_wnds.emplace_front(child, child_object, hwnd);
	frame._desktop_index = _child_wnds.begin();
	SetChildRegion(child_object, child_region);
	SetChildFrame(child_object, frame);
	frame.Invalidate(region_infinite);
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

std::pair<HANDLE, const Point> DesktopObjectImpl::ConvertPointToDesktopWndPoint(WndObject& wnd, Point point) const {
	auto child = &wnd;
	auto parent = wnd.GetParent();
	while (parent != this) {
		if (parent == nullptr) { 
			return std::make_pair(nullptr, point_zero); 
		}
		point = child->ConvertPointToParentPoint(point);
		child = parent;
		parent = child->GetParent();
	}
	DesktopWndFrame& frame = GetChildFrame(*child);
	return std::make_pair(frame._hwnd, point);
}


DesktopBase::DesktopBase(DesktopObjectImpl& desktop_object) : WndBase(desktop_object) {
	SetDepth(0);
}

const Rect DesktopBase::GetCachedRegion() const {
	return Rect(point_zero, GetDesktopSize()); 
}

void DesktopBase::AddChild(IWndBase& child_wnd) {
	WndBase::AddChild(child_wnd);
	WndBase& child = static_cast<WndBase&>(child_wnd);
	GetObject().AddChild(child, child._object);
}

void DesktopBase::ReleaseCapture() {
	Win32::ReleaseCapture();
}

void DesktopBase::ReleaseFocus() {
	Win32::ReleaseFocus();
}


END_NAMESPACE(WndDesign)