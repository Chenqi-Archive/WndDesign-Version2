#include "wnd_base.h"
#include "reflow_queue.h"
#include "redraw_queue.h"
#include "WndObject.h"
#include "../layer/layer.h"
#include "../layer/background_types.h"
#include "../geometry/geometry_helper.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGNCORE_API unique_ptr<IWndBase> IWndBase::Create(WndObject& object) {
	return std::make_unique<WndBase>(object);
}


WndBase::WndBase(WndObject& object) :
	_object(object),
	_parent(nullptr),
	_index_on_parent(),
	_depth(-1),
	_child_wnds(),

	_accessible_region(region_empty),
	_display_offset(point_zero),
	_region_on_parent(region_empty),
	_visible_region(region_empty),

	_reflow_queue_index(),
	_background(NullBackground::Get()),
	_layer(),

	_redraw_queue_index(),
	_invalid_region(),

	_capture_child(nullptr),
	_focus_child(nullptr),
	_last_tracked_child(nullptr) {
}

WndBase::~WndBase() {
	LeaveReflowQueue();
	LeaveRedrawQueue();
	DetachFromParent();
}

void WndBase::SetParent(ref_ptr<WndBase> parent, list<ref_ptr<WndBase>>::iterator index_on_parent) {
	DetachFromParent();
	_parent = parent; _index_on_parent = index_on_parent;
}

void WndBase::ClearParent() {
	_parent = nullptr; _index_on_parent = {};
}

void WndBase::DetachFromParent() {
	if (HasParent()) {
		_parent->_object.RemoveChild(_object);
	}
}

void WndBase::SetDepth(uint depth) {
	if (_depth == depth) { return; }

	if (depth > max_wnd_depth && depth != -1) {
		throw std::invalid_argument("window hierarchy too deep");
	}

	// Depth will be reset, leave redraw queue and reflow queue.
	LeaveRedrawQueue();
	LeaveReflowQueue();

	_depth = depth;

	// Set depth for child windows.
	for (auto child : _child_wnds) { child->SetDepth(GetChildDepth()); }

	// If depth != -1, join redraw queue and reflow queue if has invalid region or invalid layout.
	if (IsDepthValid()) {
		if (!_invalid_region.IsEmpty()) { JoinRedrawQueue(); }
		if (_object.HasInvalidLayout()) { JoinReflowQueue(); }
	}
}

void WndBase::AddChild(IWndBase& child_wnd) {
	WndBase& child = static_cast<WndBase&>(child_wnd);
	assert(child._parent != this);
	_child_wnds.push_front(&child);
	child.SetParent(this, _child_wnds.begin());
	child.SetDepth(GetChildDepth());
	child._object.OnAttachToParent();
}

void WndBase::RemoveChild(IWndBase& child_wnd) {
	WndBase& child = static_cast<WndBase&>(child_wnd);
	assert(child._parent == this);
	_child_wnds.erase(child._index_on_parent);
	child.ClearParent();
	// Child's depth will be reset at UpdateInvalidRegion() or UpdateLayout().
	// Child's visible region remains unchanged until attached to a parent window next time.
	if (_capture_child == &child) { _capture_child = nullptr; }
	if (_focus_child == &child) { _focus_child = nullptr; }
	if (_last_tracked_child == &child) { ChildLoseTrack(); }
}

void WndBase::SetAccessibleRegion(Rect accessible_region) {
	if (_accessible_region == accessible_region) { return; }
	_accessible_region = accessible_region;
	if (HasLayer()) { _layer->ResetTileSize(_accessible_region.size); }
	if (SetDisplayOffset(point_zero + GetDisplayOffset()) == vector_zero) {
		ResetVisibleRegion();
	}
}

const Vector WndBase::SetDisplayOffset(Point display_offset) {
	display_offset = BoundRectInRegion(Rect(display_offset, _region_on_parent.size), _accessible_region).point;
	Vector vector = display_offset - _display_offset;
	if (vector == vector_zero) { return vector; }
	_display_offset = display_offset;
	ResetVisibleRegion();
	_object.OnDisplayRegionChange(_accessible_region, GetDisplayRegion());
	return vector;
}

void WndBase::SetRegionOnParent(Rect region_on_parent) {
	Rect display_region = BoundRectInRegion(Rect(_display_offset, region_on_parent.size), _accessible_region);
	bool display_region_changed = display_region != GetDisplayRegion();
	if (!display_region_changed && _region_on_parent.point == region_on_parent.point) { return; }
	_region_on_parent = Rect(region_on_parent.point, display_region.size);
	_display_offset = display_region.point;
	ResetVisibleRegion();
	if (!display_region_changed) { return; }
	_object.OnDisplayRegionChange(_accessible_region, display_region);
}

void WndBase::JoinReflowQueue() {
	if (IsDepthValid() && !_reflow_queue_index.valid()) {
		GetReflowQueue().AddWnd(*this);
	}
}

void WndBase::LeaveReflowQueue() {
	if (_reflow_queue_index.valid()) {
		GetReflowQueue().RemoveWnd(*this);
	}
}

void WndBase::MayRegionOnParentChange() {
	if (_object.HasParent() && _object.MayRegionOnParentChange()) {
		_object.GetParent()->ChildRegionMayChange(_object);
	}
}

void WndBase::UpdateInvalidLayout() {
	// If has no parent window, clear depth and skip.
	if (!HasParent()) { SetDepth(-1); return; }
	_object.UpdateLayout();
}

void WndBase::AllocateLayer() {
	if (HasLayer()) { return; }
#pragma message(Remark"May support user-defined layer tiling policy.")
	_layer = std::make_unique<Layer>();
	_layer->ResetTileSize(_accessible_region.size);
	ResetVisibleRegion();
}

const Rect WndBase::GetCachedRegion() const { 
	return HasLayer() ? _accessible_region.Intersect(_layer->GetCachedRegion()) : _visible_region;
}

void WndBase::SetVisibleRegion(Rect parent_cached_region) {
	Rect visible_region = (parent_cached_region.Intersect(_region_on_parent) + OffsetFromParent()).Intersect(_accessible_region);
	if (_visible_region == visible_region) { return; }
	_visible_region = visible_region;

	if (HasLayer()) {
		Rect old_cached_region = _layer->GetCachedRegion();
	#pragma message(Remark"May support user-defined layer caching policy")
		if (!old_cached_region.Contains(_visible_region)) {
			_layer->SetCachedRegion(_accessible_region, _visible_region);
			// Invalidate new cached region.
			Region& new_cached_region = Region::Temp(_layer->GetCachedRegion().Intersect(_accessible_region));
			new_cached_region.Sub(old_cached_region);
			if (!new_cached_region.IsEmpty()) {
				_invalid_region.Union(new_cached_region);
				JoinRedrawQueue();
			}
			// For object's lazy loading.
			_object.OnCachedRegionChange(_accessible_region, GetCachedRegion());
		}
	}

	// Set visible region for child windows.
	for (auto child : _child_wnds) { child->SetVisibleRegion(GetCachedRegion()); }
}

void WndBase::JoinRedrawQueue() {
	if (IsDepthValid() && !_redraw_queue_index.valid()) {
		GetRedrawQueue().AddWnd(*this);
	}
}

void WndBase::LeaveRedrawQueue() {
	if (_redraw_queue_index.valid()) {
		GetRedrawQueue().RemoveWnd(*this);
	}
}

void WndBase::InvalidateChild(WndBase& child, Region& child_invalid_region) {
	child_invalid_region.Translate(child._region_on_parent.point - point_zero);
	child_invalid_region.Intersect(child._region_on_parent.Intersect(GetCachedRegion()));
	if (!child_invalid_region.IsEmpty()) {
		_invalid_region.Union(child_invalid_region);
		JoinRedrawQueue();
	}
}

void WndBase::InvalidateChild(IWndBase& child, Rect child_invalid_region) {
	InvalidateChild(static_cast<WndBase&>(child), Region::Temp(child_invalid_region));
}

void WndBase::Invalidate(Rect region) {
	region = region.Intersect(GetCachedRegion());
	if (!region.IsEmpty()) {
		_invalid_region.Union(region);
		JoinRedrawQueue();
	}
}

void WndBase::UpdateInvalidRegion() {
	// If has no parent window, clear depth and skip, but not erase the invalid region.
	if (!HasParent()) { SetDepth(-1); return; }

	// Clip invalid region inside cached region.
	_invalid_region.Intersect(GetCachedRegion());
	if (_invalid_region.IsEmpty()) { return; }

	// Draw figure queue to layer.
	if (HasLayer()) {
		auto [bounding_region, regions] = _invalid_region.GetRect();
		FigureQueue figure_queue;

		uint group_index = figure_queue.BeginGroup(vector_zero, bounding_region);
		figure_queue.Append(bounding_region.point, new BackgroundFigure(_background, bounding_region, true));
		_object.OnPaint(figure_queue, _accessible_region, bounding_region);
		figure_queue.EndGroup(group_index);

	#pragma message(Remark"Or just draw the bounding region once ?")
		for (auto& region : regions) {
			_layer->DrawFigureQueue(figure_queue, region);
		}
	}

	// Invalidate parent window, my invalid region will be used by parent window.
	_invalid_region.Translate(vector_zero - GetDisplayOffset());
	_parent->InvalidateChild(*this, _invalid_region);
	_invalid_region.Clear();
}

void WndBase::Composite(FigureQueue& figure_queue, Vector client_offset, Rect parent_invalid_region) const {
	//parent_invalid_region = parent_invalid_region.Intersect(_region_on_parent);
	assert(_region_on_parent.Contains(parent_invalid_region)); // intersection should have been done by parent.

	// Composite client region.
	// Convert parent invalid region to my invalid region.
	Vector coordinate_offset = OffsetFromParent();
	Rect invalid_region = parent_invalid_region + coordinate_offset;
	if (HasLayer()) {
		// Draw layer directly in parent's coordinates, no need to create figure group.
		figure_queue.Append(parent_invalid_region.point - client_offset, new LayerFigure(*_layer, _background, invalid_region, {}));
	} else {
		// figure in my coordinates - coordinate_offset = figure in parent's coordinates.
		uint group_begin = figure_queue.BeginGroup(vector_zero - coordinate_offset - client_offset, invalid_region);
		figure_queue.Append(invalid_region.point, new BackgroundFigure(_background, invalid_region, false));
		_object.OnPaint(figure_queue, _accessible_region, invalid_region);
		figure_queue.EndGroup(group_begin);
	}

	// Composite non-client region.
	Vector display_region_offset = _region_on_parent.point - point_zero;
	uint group_begin = figure_queue.BeginGroup(display_region_offset - client_offset, parent_invalid_region - display_region_offset);
	_object.OnComposite(figure_queue, _region_on_parent.size, parent_invalid_region - display_region_offset);
	figure_queue.EndGroup(group_begin);
}

void WndBase::ChildLoseCapture() {
	if (_capture_child != nullptr) {
		if (_capture_child == this) { 
			HandleMessage(Msg::LoseCapture, nullmsg); 
		} else { 
			_capture_child->ChildLoseCapture();
		}
		_capture_child = nullptr;
	}
}

void WndBase::ChildLoseFocus() {
	if (_focus_child != nullptr) {
		if (_focus_child == this) {
			HandleMessage(Msg::LoseFocus, nullmsg);
		} else {
			_focus_child->ChildLoseFocus();
		}
		_focus_child = nullptr;
	}
}

void WndBase::ChildLoseTrack() {
	if (_last_tracked_child != nullptr) {
		_last_tracked_child->DispatchMessage(Msg::MouseLeave, nullmsg);
		_last_tracked_child = nullptr;
	}
}

void WndBase::SetChildCapture(WndBase& child) {
	if (_capture_child == &child) { return; }
	ChildLoseCapture();
	_capture_child = &child;
	if (HasParent()) { _parent->SetChildCapture(*this); }
}

void WndBase::SetChildFocus(WndBase& child) {
	if (_focus_child == &child) { return; }
	ChildLoseFocus();
	_focus_child = &child;
	if (HasParent()) { _parent->SetChildFocus(*this); }
}

void WndBase::ReleaseCapture() { 
	ChildLoseCapture(); 
	if (HasParent()) { _parent->ReleaseCapture(); } 
}

void WndBase::ReleaseFocus() { 
	ChildLoseFocus(); 
	if (HasParent()) { _parent->ReleaseFocus(); } 
}

bool WndBase::SendChildMessage(IWndBase& child_wnd, Msg msg, Para para) {
	WndBase* child = &static_cast<WndBase&>(child_wnd);
	if (IsMouseMsg(msg)) {
		if (child != _last_tracked_child) {
			ChildLoseTrack();
			_last_tracked_child = child;
			child->HandleMessage(Msg::MouseEnter, nullmsg);
		}
		MouseMsg& mouse_msg = GetMouseMsg(para);
		mouse_msg.point = mouse_msg.point + child->OffsetFromParent();
	}
	return child->DispatchMessage(msg, para);
}

bool WndBase::HandleMessage(Msg msg, Para para) {
	return _object.NonClientHandler(msg, para);
}

bool WndBase::DispatchMessage(Msg msg, Para para) {
	if (IsMouseMsg(msg)) {
		ref_ptr<WndBase> child = _capture_child;
		if (child == nullptr || child == this) { 
			return HandleMessage(msg, para);
		}
		return SendChildMessage(*child, msg, para);
	}
	if (IsKeyboardMsg(msg) && _focus_child != nullptr && _focus_child != this) {
		return _focus_child->DispatchMessage(msg, para);
	}
	if (msg == Msg::MouseLeave) { ChildLoseTrack(); }
	return HandleMessage(msg, para);
}


END_NAMESPACE(WndDesign)