#include "desktop.h"
#include "redraw_queue.h"
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

void DesktopWndFrame::OnRegionUpdate(Rect region) {
	Rect old_region = _wnd.GetRegionOnParent();
	if (old_region.size != region.size) {
		_resource.OnResize(region.size);
		Invalidate(Rect(point_zero, region.size));
	}
}

void DesktopWndFrame::Invalidate(Region& region) {
	_invalid_region.Union(region);
	if (!_invalid_region.IsEmpty()) {
		JoinRedrawQueue();
	}
}

void DesktopWndFrame::Invalidate(Rect region) {
	Invalidate(Region::Temp(region));
}

void DesktopWndFrame::UpdateInvalidRegion() {
	_invalid_region.Intersect(Rect(point_zero, _wnd.GetRegionOnParent().size));
	if (_invalid_region.IsEmpty()) { return; }

	auto [bounding_region, regions] = _invalid_region.GetRect();

	// A little tricky here. 
	// Figures are drawn in desktop's coordinates, but the target is in window's coordinates, so first
	//   push the group as the desktop relative to the target.
	FigureQueue figure_queue;
	Vector offset_from_desktop = _wnd.OffsetFromParent();
	uint group_begin = figure_queue.BeginGroup(offset_from_desktop, _wnd.GetRegionOnParent());
	_wnd.Composite(figure_queue, bounding_region - offset_from_desktop);
	figure_queue.EndGroup(group_begin);

	Target& target = _resource.GetTarget();
	for (auto& region : regions) {
		target.DrawFigureQueue(figure_queue, vector_zero, region);
	}

	// The invalid region will still be used at present time, and will be cleared after presentation, see below.
}

void DesktopWndFrame::Present() { 
	_resource.Present(_invalid_region.GetRect().second);
	_invalid_region.Clear();
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
	static_cast<DesktopObjectImpl&>(DesktopObject::Get()).SetChildRegionStyle(_wnd_object, region);
}

const pair<Size, Size> DesktopWndFrame::CalculateMinMaxSize() {
	return _wnd_object.CalculateMinMaxSize(GetDesktopSize());
}


WNDDESIGNCORE_API DesktopObject& DesktopObject::Get() {
    static DesktopObjectImpl desktop_object;
    return desktop_object;
}

DesktopObjectImpl::DesktopObjectImpl() : DesktopObject(std::make_unique<DesktopBase>(*this)) {}

void DesktopObjectImpl::AddChild(WndBase& child, WndObject& child_object) {
	HANDLE hwnd = Win32::CreateWnd(region_empty, child_object.GetTitle());
	DesktopWndFrame& frame = _child_wnds.emplace_front(child, child_object, hwnd);
	frame._desktop_index = _child_wnds.begin();
	SetChildFrame(child_object, frame);
}

void DesktopObjectImpl::SetChildRegionStyle(WndObject& child, Rect parent_specified_region) {
	SetChildRegionStyle(child, parent_specified_region);
}

void DesktopObjectImpl::OnChildDetach(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	HANDLE hwnd = frame._hwnd;
	_child_wnds.erase(frame._desktop_index);
	Win32::DestroyWnd(hwnd);
}

void DesktopObjectImpl::OnChildRegionUpdate(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Rect region = UpdateChildRegion(child, GetSize());
	frame.OnRegionUpdate(region);
	Win32::MoveWnd(frame._hwnd, region);
	SetChildRegion(child, region);
}

void DesktopObjectImpl::OnChildTitleChange(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Win32::SetWndTitle(frame._hwnd, child.GetTitle());
}

void DesktopObjectImpl::MessageLoop() {
	Win32::MessageLoop();
}

void DesktopObjectImpl::Terminate() {
#pragma message(Remark"May use PostQuitMessage() instead.")
	while (!_child_wnds.empty()) {
		RemoveChild(_child_wnds.back()._wnd_object);
	}
}

std::pair<HANDLE, const Point> DesktopObjectImpl::ConvertNonClientPointToDesktopPoint(WndObject& wnd, Point point) const {
	auto child = &wnd;
	auto parent = wnd.GetParent();
	while (parent != this) {
		if (parent == nullptr) { 
			return std::make_pair(nullptr, point_zero); 
		}
		point = child->ConvertNonClientPointToParentPoint(point);
		point = parent->ConvertPointToNonClientPoint(point);
		child = parent;
		parent = parent->GetParent();
	}
	DesktopWndFrame& frame = GetChildFrame(*child);
	return std::make_pair(frame._hwnd, point);
}


DesktopBase::DesktopBase(DesktopObjectImpl& desktop_object) : WndBase(desktop_object) {
	SetAccessibleRegion(Rect(point_zero, GetDesktopSize()));
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