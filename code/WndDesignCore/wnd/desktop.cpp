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

void DesktopWndFrame::OnRegionChange(Rect region) {
	Rect old_region = _wnd.GetRegionOnParent();
	if (old_region.size != region.size) {
		_resource.OnResize(region.size);
		Invalidate(Rect(point_zero, region.size));
	}
}

void DesktopWndFrame::SetRegion(Rect region) {
	if (_wnd.GetRegionOnParent() == region) { return; }
	static_cast<DesktopObjectImpl&>(DesktopObject::Get()).SetChildRegionStyle(_wnd_object, region);
}

const pair<Size, Size> DesktopWndFrame::CalculateMinMaxSize() {
	return _wnd_object.CalculateMinMaxSize(GetDesktopSize());
}

void DesktopWndFrame::JoinRedrawQueue() { 
	if (!_redraw_queue_index.valid()) { RedrawQueue::Get().AddDesktopWnd(*this); } 
}

void DesktopWndFrame::LeaveRedrawQueue() { 
	if (_redraw_queue_index.valid()) { RedrawQueue::Get().RemoveDesktopWnd(*this); } 
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
	Vector offset_from_desktop = point_zero - _wnd.GetRegionOnParent().point;
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

void DesktopWndFrame::OnWndDetach(WndObject& wnd) {
	if (_capture_wnd == &wnd) { LoseCapture(); }
	if (_focus_wnd == &wnd) { LoseFocus(); }
}

void DesktopWndFrame::SetCapture(WndObject& wnd, Vector offset) {
	_capture_wnd_offset_from_desktop = _wnd.GetRegionOnParent().point + offset;
	if (_capture_wnd == &wnd) { return; }
	if (_capture_wnd == nullptr) { 
		Win32::SetCapture(_hwnd);
	} else {
		LoseCapture();
	}
	_capture_wnd = &wnd;
}

void DesktopWndFrame::SetFocus(WndObject& wnd) {
	if (_focus_wnd == &wnd) { return; }
	if (_focus_wnd == nullptr) {
		Win32::SetFocus(_hwnd);
	} else {
		LoseFocus();
	}
	_focus_wnd = &wnd;
}

void DesktopWndFrame::LoseCapture() {
	if (_capture_wnd == nullptr) { return; }
	_capture_wnd->NonClientHandler(Msg::LoseCapture, nullmsg);
	_capture_wnd = nullptr;
}

void DesktopWndFrame::LoseFocus() {
	if (_focus_wnd == nullptr) { return; }
	_focus_wnd->NonClientHandler(Msg::LoseFocus, nullmsg);
	_focus_wnd = nullptr;
}

void DesktopWndFrame::ReceiveMessage(Msg msg, Para para) const {
	if (IsMouseMsg(msg) && _capture_wnd != nullptr) {
		GetMouseMsg(para).point += _wnd.GetRegionOnParent().point - _capture_wnd_offset_from_desktop;
		_capture_wnd->NonClientHandler(msg, para);
		return;
	}
	if (IsKeyboardMsg(msg) && _focus_wnd != nullptr) {
		_focus_wnd->NonClientHandler(msg, para);
		return;
	}
	_wnd_object.NonClientHandler(msg, para);
}


WNDDESIGNCORE_API DesktopObject& DesktopObject::Get() {
    static DesktopObjectImpl desktop_object;
    return desktop_object;
}

DesktopObjectImpl::DesktopObjectImpl() :
	DesktopObject(std::make_unique<DesktopBase>(*this)) {
}

DesktopObjectImpl::~DesktopObjectImpl() {}

void DesktopObjectImpl::AddChild(WndBase& child, WndObject& child_object) {
	HANDLE hwnd = Win32::CreateWnd(region_empty, child_object.GetTitle());
	DesktopWndFrame& frame = _child_wnds.emplace_front(child, child_object, hwnd);
	frame._desktop_index = _child_wnds.begin();
	SetChildFrame(child_object, frame);
}

void DesktopObjectImpl::OnChildDetach(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	HANDLE hwnd = frame._hwnd;
	_child_wnds.erase(frame._desktop_index);
	Win32::DestroyWnd(hwnd);
}

void DesktopObjectImpl::SetChildRegionStyle(WndObject& child, Rect parent_specified_region) {
	WndObject::SetChildRegionStyle(child, parent_specified_region, GetSize());
}

void DesktopObjectImpl::OnChildTitleChange(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Win32::SetWndTitle(frame._hwnd, child.GetTitle());
}

void DesktopObjectImpl::OnChildRegionUpdate(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Rect region = UpdateChildRegion(child, GetSize());
	if (region != frame._wnd.GetRegionOnParent()) {
		frame.OnRegionChange(region);
		SetChildRegion(child, region);
		Win32::MoveWnd(frame._hwnd, region);
	}
}

void DesktopObjectImpl::OnWndDetach(WndObject& wnd) {
	if (ref_ptr<DesktopWndFrame> frame = GetWndFrame(wnd); frame != nullptr) {
		frame->OnWndDetach(wnd);
	}
}

void DesktopObjectImpl::SetCapture(WndObject& wnd) {
	if (auto [frame, point_on_frame] = ConvertWndNonClientPointToFramePoint(wnd, point_zero); frame != nullptr) {
		frame->SetCapture(wnd, point_on_frame - point_zero);
	}
}

void DesktopObjectImpl::ReleaseCapture() {
	Win32::ReleaseCapture();
}

void DesktopObjectImpl::SetFocus(WndObject& wnd) {
	if (ref_ptr<DesktopWndFrame> frame = GetWndFrame(wnd); frame != nullptr) {
		frame->SetFocus(wnd);
	}
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

ref_ptr<DesktopWndFrame> DesktopObjectImpl::GetWndFrame(WndObject& wnd) const {
	auto child = &wnd;
	auto parent = wnd.GetParent();
	while (parent != this) {
		if (parent == nullptr) { return nullptr; }
		child = parent;
		parent = parent->GetParent();
	}
	return &GetChildFrame(*child);
}

const std::pair<ref_ptr<DesktopWndFrame>, Point> DesktopObjectImpl::ConvertWndNonClientPointToFramePoint(WndObject& wnd, Point point) const {
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
	return { &GetChildFrame(*child), point };
}


DesktopBase::DesktopBase(DesktopObjectImpl& desktop_object) : WndBase(desktop_object) {
	_depth = 0;
	_accessible_region = _cached_region = Rect(point_zero, GetDesktopSize());
}

DesktopBase::~DesktopBase() {}

void DesktopBase::AddChild(IWndBase& child_wnd) {
	WndBase::AddChild(child_wnd);
	WndBase& child = static_cast<WndBase&>(child_wnd);
	GetObject().AddChild(child, child._object);
}


END_NAMESPACE(WndDesign)