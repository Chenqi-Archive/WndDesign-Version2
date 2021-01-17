#include "wnd_base.h"
#include "redraw_queue.h"
#include "WndObject.h"
#include "../layer/layer.h"
#include "../layer/background.h"
#include "../geometry/geometry_helper.h"


BEGIN_NAMESPACE(WndDesign)


WndBase::WndBase(WndObject& object) :
	_object(object),
	_parent(nullptr), _index_on_parent(),
	_child_wnds(),

	_accessible_region(region_empty), 
	_display_offset(point_zero),
	_region_on_parent(region_empty),
	_visible_region(region_empty),

	_background(_object.GetBackground()),
	_layer(),

	_depth(0),
	_redraw_queue_index(),
	_invalid_region() 

	/* capture */
{
}

WndBase::~WndBase() {
	LeaveRedrawQueue();
	DetachFromParent();
}

void WndBase::SetParent(ref_ptr<WndBase> parent, list<ref_ptr<WndBase>>::iterator index_on_parent) {
	DetachFromParent();
	_parent = parent; _index_on_parent = index_on_parent;

	// Set depth.

	// If has invalid region, join figure queue.

	// Set visible region.
}

void WndBase::ClearParent() {
	_parent = nullptr;
	_index_on_parent = {};
}

void WndBase::DetachFromParent() {
	if (HasParent()) { _parent->RemoveChild(*this); }

	// reset depth

}

void WndBase::AddChild(WndBase& child_wnd) {

	// set child depth, display region and visible region.

}

void WndBase::RemoveChild(WndBase& child_wnd) {
	if (child_wnd._parent != this) { return; }
	_child_wnds.erase(child_wnd._index_on_parent);
	child_wnd.ClearParent();

	// clear child depth and visible region.

	// Clear message tracked windows.
}

void WndBase::SetDepth(uint depth) {
	if (_depth == depth) { return; }
	// Depth has changed, re-enter redraw queue.
	LeaveRedrawQueue();
	_depth = depth;

	// Set depth for child windows.

	// Enter redraw queue, if depth > 0 && invalid region is not empty.
	if (_depth > 0 && !_invalid_region.IsEmpty()) {

	}
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
		#pragma message("It's arbitrary to decide when to reset cached region.")
			_layer->SetCachedRegion(_accessible_region, _visible_region);
			Region& new_cached_region = Region::Temp(_layer->GetCachedRegion());
			new_cached_region.Sub(old_cached_region);
			Invalidate(std::move(new_cached_region));
		}
	}

	// Set visible region for child windows.
	for (auto child : _child_wnds) { child->SetVisibleRegion(GetCachedRegion()); }

	// For object's lazy loading.
	_object.OnVisibleRegionChange(_accessible_region, _visible_region);
}

void WndBase::Invalidate(Region&& region) {
	region.Intersect(GetCachedRegion());
	if (!region.IsEmpty()) {
		_invalid_region.Union(region);
		JoinRedrawQueue();
	}
}

void WndBase::Invalidate(Rect region) {
	region = region.Intersect(GetCachedRegion());
	if (!region.IsEmpty()) {
		_invalid_region.Union(region);
		JoinRedrawQueue();
	}
}

void WndBase::JoinRedrawQueue() {
	if (_depth > 0 && !HasRedrawQueueIndex()) {
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
	if (!HasParent()) { SetDepth(0); return; }

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
		for (auto& region : regions) {
			_layer->DrawFigureQueue(figure_queue, region);
		}
	}

	// Invalidate region for parent window.
	_invalid_region.Translate(_display_offset - _region_on_parent.point);
	_invalid_region.Intersect(_region_on_parent);
	_parent->Invalidate(std::move(_invalid_region));

	// Clear invalid region.
	_invalid_region.Clear();
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
		uint group_index = figure_queue.BeginGroup(coordinate_offset, invalid_region);
		figure_queue.Append(invalid_region.point - point_zero, new BackgroundFigure(_background, invalid_region, false));
		_object.OnPaint(figure_queue, _accessible_region, invalid_region);
		figure_queue.EndGroup(group_index);
	}
}


END_NAMESPACE(WndDesign)