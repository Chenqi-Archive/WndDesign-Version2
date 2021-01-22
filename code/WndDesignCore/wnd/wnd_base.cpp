#include "wnd_base.h"
#include "redraw_queue.h"
#include "WndObject.h"
#include "../layer/layer.h"
#include "../layer/background_types.h"
#include "../geometry/geometry_helper.h"
#include "../message/message.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGNCORE_API unique_ptr<IWndBase> IWndBase::Create(WndObject& object) {
	return std::make_unique<WndBase>(object);
}


WndBase::WndBase(WndObject& object) :
	_object(object),
	_parent(nullptr), 
	_index_on_parent(),
	_child_wnds(),

	_accessible_region(region_empty), 
	_display_offset(point_zero),
	_region_on_parent(region_empty),
	_visible_region(region_empty),

	_background(NullBackground::Get()),
	_layer(),

	_depth(-1),
	_redraw_queue_index(),
	_invalid_region(),

	_capture_child(nullptr),
	_focus_child(nullptr),
	_last_tracked_child(nullptr) {
}

WndBase::~WndBase() {
	LeaveRedrawQueue();
	DetachFromParent();
}

void WndBase::SetParent(ref_ptr<WndBase> parent, list<ref_ptr<WndBase>>::iterator index_on_parent) {
	DetachFromParent();
	_parent = parent; _index_on_parent = index_on_parent;
	if (!_invalid_region.IsEmpty()) { JoinRedrawQueue(); }
}

void WndBase::ClearParent() { 
	_parent = nullptr; _index_on_parent = {}; 
}

void WndBase::DetachFromParent() {
	if (HasParent()) { 
		_parent->RemoveChild(*this); 
		// Send child detached message to notify parent window object.
		_parent->_object.OnChildDetach(_object);
	}
}

void WndBase::AddChild(IWndBase& child_wnd) {
	WndBase& child = static_cast<WndBase&>(child_wnd);
	if (child._parent == this) { return; }

	_child_wnds.push_front(&child);
	child.SetParent(this, _child_wnds.begin());
	child.SetDepth(GetChildDepth());
	child.ResetVisibleRegion();

	// Child's accessible region and display region will be calculated and set by parent window later.
}

void WndBase::RemoveChild(IWndBase& child_wnd) {
	WndBase& child = static_cast<WndBase&>(child_wnd);
	if (child._parent != this) { return; }
	_child_wnds.erase(child._index_on_parent);
	child.ClearParent();
	// Child's depth will be reset at UpdateInvalidRegion, because the child window may soon be added as a child.
	// Child's visible region remains unchanged until attached to a parent window next time.

	if (_capture_child == &child) { _capture_child = nullptr; }
	if (_focus_child == &child) { _focus_child = nullptr; }
	if (_last_tracked_child == &child) { ChildLoseTrack(); }
}

void WndBase::SetDepth(uint depth) {
	if (_depth == depth) { return; }

	if (depth != -1 && depth > max_wnd_depth) { 
		throw std::invalid_argument("window hierarchy too deep"); 
	}

	// Depth will be changed, leave redraw queue.
	LeaveRedrawQueue();

	_depth = depth;

	// Set depth for child windows.
	for (auto child : _child_wnds) { child->SetDepth(GetChildDepth()); }

	// Join redraw queue, if invalid region is not empty && depth != -1(checked in JoinRedrawQueue) .
	if (!_invalid_region.IsEmpty()) { JoinRedrawQueue(); }
}

void WndBase::SetAccessibleRegion(Rect accessible_region) {
	if (_accessible_region == accessible_region) { return; }
	_accessible_region = accessible_region;
	if (HasLayer()) { 
		_layer->ResetTileSize(_accessible_region.size); 
		// If tile size changed, cached region has been cleared, 
		//   and will be reset when SetDisplayRegion is called next.
	}
	_object.OnSizeChange(_accessible_region);
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

void WndBase::AllocateLayer() {
	if (HasLayer()) { return; }
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
		if (!old_cached_region.Contains(_visible_region)) {
		#pragma message(Remark"It's arbitrary to decide when to reset cached region.")
			_layer->SetCachedRegion(_accessible_region, _visible_region);
			// Invalidate new cached region.
			Region& new_cached_region = Region::Temp(_layer->GetCachedRegion().Intersect(_accessible_region));
			new_cached_region.Sub(old_cached_region);
			if (!new_cached_region.IsEmpty()) {
				_invalid_region.Union(new_cached_region);
				JoinRedrawQueue();
			}
		}
	}

	// Set visible region for child windows.
	for (auto child : _child_wnds) { child->SetVisibleRegion(GetCachedRegion()); }

	// For object's lazy loading.
	_object.OnCachedRegionChange(_accessible_region, GetCachedRegion());
}

void WndBase::Invalidate(WndBase& child) {
	Region& child_invalid_region = child._invalid_region;
	child_invalid_region.Translate(vector_zero - child.OffsetFromParent());
	child_invalid_region.Intersect(child._region_on_parent.Intersect(GetCachedRegion()));
	if (!child_invalid_region.IsEmpty()) {
		_invalid_region.Union(child_invalid_region);
		JoinRedrawQueue();
	}
	child_invalid_region.Clear();
}

void WndBase::Invalidate(Rect region) {
	region = region.Intersect(GetCachedRegion());
	if (!region.IsEmpty()) {
		_invalid_region.Union(region);
		JoinRedrawQueue();
	}
}

void WndBase::JoinRedrawQueue() {
	if (_depth != -1 && !HasRedrawQueueIndex()) {
		RedrawQueue::Get().AddWnd(*this);
	}
}

void WndBase::LeaveRedrawQueue() {
	if (HasRedrawQueueIndex()) {
		RedrawQueue::Get().RemoveWnd(*this);
	}
}

void WndBase::UpdateInvalidRegion() {
	// First check if has parent window, reset depth when detached from parent.
	// But not erase the invalid region.
	if (!HasParent()) { SetDepth(-1); return; }

	// Clip invalid region inside cached region.
	_invalid_region.Intersect(GetCachedRegion());
	if (_invalid_region.IsEmpty()) { return; }

	// Draw figure queue to layer.
	if (HasLayer()) {
		auto [bounding_region, regions] = _invalid_region.GetRect();
		FigureQueue figure_queue;

		uint group_index = figure_queue.BeginGroup(vector_zero, bounding_region);
		figure_queue.Append(bounding_region.point - point_zero, new BackgroundFigure(_background, bounding_region, true));
		_object.OnPaint(figure_queue, _accessible_region, bounding_region);
		figure_queue.EndGroup(group_index);

	#pragma message(Remark"Or just draw the bounding region once ?")
		for (auto& region : regions) {
			_layer->DrawFigureQueue(figure_queue, region);
		}
	}

	// Invalidate parent window, my invalid region will be cleared by parent window.
	_parent->Invalidate(*this);
}

void WndBase::Composite(FigureQueue& figure_queue, Rect parent_invalid_region) const {
	// Convert parent invalid region to my invalid region.
	parent_invalid_region = parent_invalid_region.Intersect(_region_on_parent);
	Vector coordinate_offset = OffsetFromParent();
	Rect invalid_region = parent_invalid_region + coordinate_offset;
	if (HasLayer()) {
		// Draw layer directly in parent's coordinates, no need to create figure group.
		figure_queue.Append(parent_invalid_region.point - point_zero, new LayerFigure(*_layer, _background, invalid_region, {}));
	} else {
		// figure in my coordinates - coordinate_offset = figure in parent's coordinates.
		uint group_index = figure_queue.BeginGroup(vector_zero - coordinate_offset, invalid_region);
		figure_queue.Append(invalid_region.point - point_zero, new BackgroundFigure(_background, invalid_region, false));
		_object.OnPaint(figure_queue, _accessible_region, invalid_region);
		figure_queue.EndGroup(group_index);
	}
}

void WndBase::ChildLoseCapture() {
	if (_capture_child != nullptr) {
		_capture_child == this ? HandleMessage(Msg::LoseCapture, nullmsg) : _capture_child->ChildLoseCapture();
		_capture_child = nullptr;
	}
}

void WndBase::ChildLoseFocus() {
	if (_focus_child != nullptr) {
		_focus_child == this ? HandleMessage(Msg::LoseFocus, nullmsg) : _focus_child->ChildLoseFocus();
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

void WndBase::HandleMessage(Msg msg, Para para) {
#pragma message(Remark"May use the return value to implement message bubbling.")
	_object.Handler(msg, para);
}

void WndBase::DispatchMessage(Msg msg, Para para) {
	if (IsMouseMsg(msg)) {
		MouseMsg mouse_msg = GetMouseMsg(para);
		ref_ptr<WndBase> child = _capture_child;
		if (child == nullptr) { child = static_cast<WndBase*>(_object.HitTestChild(mouse_msg.point).wnd.get()); }
		if (child == this) { return HandleMessage(msg, para); }
		if (child != _last_tracked_child) {
			ChildLoseTrack();
			_last_tracked_child = child;
			child->HandleMessage(Msg::MouseEnter, nullmsg);
		}
		mouse_msg.point = mouse_msg.point + child->OffsetFromParent();
		return child->DispatchMessage(msg, mouse_msg);
	}
	if (IsKeyboardMsg(msg) && _focus_child != nullptr && _focus_child != this) {
		return _focus_child->DispatchMessage(msg, para);
	}
	if (msg == Msg::MouseLeave) { ChildLoseTrack(); }
	return HandleMessage(msg, para);
}


END_NAMESPACE(WndDesign)