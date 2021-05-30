#include "desktop.h"
#include "reflow_queue.h"
#include "redraw_queue.h"
#include "../layer/layer.h"
#include "../system/win32_api.h"
#include "../system/metrics.h"


BEGIN_NAMESPACE(WndDesign)

extern ref_ptr<DesktopWndFrame> mouse_tracked_frame;


DesktopWndFrame::DesktopWndFrame(WndBase& wnd, WndObject& wnd_object, HANDLE hwnd, Size size) :
	_wnd(wnd), _wnd_object(wnd_object), _hwnd(hwnd), _resource(_hwnd, size) {
	// Store the pointer of the attached frame as the user data.
	Win32::SetWndUserData(_hwnd, this);
}

DesktopWndFrame::~DesktopWndFrame() {
	OnMouseLeave();
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
	GetDesktop().SetChildRegionStyle(_wnd_object, region);
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
	_invalid_region.Intersect(Rect(point_zero, _wnd.GetRegionOnParent().size));
	if (!_invalid_region.IsEmpty()) {
		JoinRedrawQueue();
	}
}

void DesktopWndFrame::Invalidate(Rect region) {
	Invalidate(Region::Temp(region));
}

void DesktopWndFrame::UpdateInvalidRegion(FigureQueue& figure_queue) {
	_invalid_region.Intersect(Rect(point_zero, _wnd.GetRegionOnParent().size));
	if (_invalid_region.IsEmpty()) { return; }

	auto [bounding_region, regions] = _invalid_region.GetRect();

	// A little tricky here. 
	// Figures are drawn in desktop's coordinates, but the target is in window's coordinates, so first
	//   push the group as the desktop relative to the target.
	Vector offset_from_desktop = point_zero - _wnd.GetRegionOnParent().point;
	uint group_begin = figure_queue.BeginGroup(offset_from_desktop, bounding_region - offset_from_desktop);
	figure_queue.Append(point_zero, new ClearCommand());
	_wnd.Composite(figure_queue, bounding_region - offset_from_desktop, CompositeEffect{});
	figure_queue.EndGroup(group_begin);

	Target& target = _resource.GetTarget();
	for (auto& region : regions) {
		target.DrawFigureQueue(figure_queue, vector_zero, region);
	}

	// The invalid region will still be used at present time, and will be cleared after presentation, see below.
}

void DesktopWndFrame::Present() { 
	auto [bounding_region, regions] = _invalid_region.GetRect();
	if (bounding_region.IsEmpty()) { return; }
	_resource.Present(std::move(regions));
	_invalid_region.Clear();
}

void DesktopWndFrame::RefreshLayer() {
#pragma message(Remark"May use a unique_ptr to manage WindowResource, but this method also works.")
	_resource.~WindowResource();
	new(&_resource)WindowResource(_hwnd, _wnd.GetRegionOnParent().size);
	Invalidate(region_infinite);
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

void DesktopWndFrame::OnMouseLeave() {
	if (mouse_tracked_frame == this) {
		mouse_tracked_frame = nullptr;
		_wnd_object.NonClientHandler(Msg::MouseLeave, nullmsg);
	}
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

void DesktopWndFrame::OnDestroy() {
	desktop.RemoveChild(_wnd_object);
}


WNDDESIGNCORE_API DesktopObject& DesktopObject::Get() {
    static DesktopObjectImpl desktop_object;
    return desktop_object;
}

DesktopObjectImpl::DesktopObjectImpl() :
	DesktopObject(std::make_unique<DesktopBase>(*this)) {
}

DesktopObjectImpl::~DesktopObjectImpl() {}

void DesktopObjectImpl::AddChild(WndObject& child, std::function<void(HANDLE)> callback) {
	Rect region = UpdateChildRegion(child, GetSize());
	RegisterChild(child);
	HANDLE hwnd = Win32::CreateWnd(region, child.GetTitle(), child.GetCompositeEffect(), callback);
	DesktopWndFrame& frame = _child_wnds.emplace_front(static_cast<WndBase&>(*child.wnd), child, hwnd, region.size);
	frame._desktop_index = _child_wnds.begin();
	SetChildFrame(child, frame);
}

void DesktopObjectImpl::OnChildDetach(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	HANDLE hwnd = frame._hwnd;
	_child_wnds.erase(frame._desktop_index);
	Win32::DestroyWnd(hwnd);
	if (message_loop_entered && _child_wnds.empty()) { Win32::ExitMessageLoop(); }
}

void DesktopObjectImpl::SetChildRegionStyle(WndObject& child, Rect parent_specified_region) {
	WndObject::SetChildRegionStyle(child, parent_specified_region, GetSize());
}

void DesktopObjectImpl::OnChildTitleChange(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Win32::SetWndTitle(frame._hwnd, child.GetTitle());
}

inline const Size CheckWndSize(Size size) {
	static Size max_window_size = []() { Size size = GetDesktopSize(); size.width *= 2; size.height *= 2; return size; }();
	if (size.width > max_window_size.width) { size.width = max_window_size.width; }
	if (size.height > max_window_size.height) { size.height = max_window_size.height; }
	return size;
}

void DesktopObjectImpl::OnChildRegionUpdate(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Rect region = UpdateChildRegion(child, GetSize());
	region.size = CheckWndSize(region.size);
	if (region != frame._wnd.GetRegionOnParent()) {
		frame.OnRegionChange(region);
		SetChildRegion(child, region);
		Win32::MoveWnd(frame._hwnd, region);
	}
}

void DesktopObjectImpl::OnChildCompositeEffectChange(WndObject& child) {
	DesktopWndFrame& frame = GetChildFrame(child);
	Win32::SetWndCompositeEffect(frame._hwnd, child.GetCompositeEffect());
}

void DesktopObjectImpl::CommitReflowQueue() {
	static ReflowQueue& reflow_queue = GetReflowQueue();
	reflow_queue.Commit();
}

void DesktopObjectImpl::CommitRedrawQueue() {
	static RedrawQueue& redraw_queue = GetRedrawQueue();
	redraw_queue.Commit();
}

void DesktopObjectImpl::RefreshLayer() {
	for (DesktopWndFrame& frame : _child_wnds) {
		frame.RefreshLayer();
		frame._wnd.RefreshLayer();
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
	if (_child_wnds.empty()) { return; }
	message_loop_entered = true;
	Win32::MessageLoop();
	message_loop_entered = false;
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


void WndBase::SetCapture() { GetDesktop().SetCapture(_object); }
void WndBase::ReleaseCapture() { GetDesktop().ReleaseCapture(); }
void WndBase::SetFocus() { GetDesktop().SetFocus(_object); }
void WndBase::NotifyDesktopWhenDetached() { GetDesktop().OnWndDetach(_object); }


END_NAMESPACE(WndDesign)